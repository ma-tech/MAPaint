#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAPaintSocket_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAPaintSocket.c
* \author	Richard Baldock
* \date		April 2009
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be
* useful but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301, USA.
* \ingroup      MAPaint
* \brief        
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef DARWIN
#include <netdb.h>
#endif /* DARWIN */

#include <MAPaint.h>

/* default MAPaint port number for IP socket */
#define	MAPAINT_PORT 2392

/* status enums */
typedef enum {
  MAPAINT_OK	= 0,
  MAPAINT_ERR	= 1
} MAPaintSocketStatus;

/* error enums plus error strings - note the enum table and string
   table are in lockstep order so that MAPaintSocketErrorStr[enum]
   yields the corect string so CHANGE WITH CARE!!! */
typedef enum {
  MAPAINT_COM_SUCCESS,
  MAPAINT_COM_FAIL,
  MAPAINT_COM_INVALID,
  MAPAINT_SBJ_INVALID,
  MAPAINT_MSG_CORRUPT,
  MAPAINT_MSG_INCOMPLETE,
  MAPAINT_DOM_INVALID,
  MAPAINT_FILE_INVALID,
  MAPAINT_STAGE_INVALID
} MAPaintSocketError;

static char *MAPaintSocketErrorStr[] = {
  "command success",
  "command failure",
  "command invalid",
  "subject invalid",
  "message corrupt",
  "message incomplete",
  "invalid domain",
  "invalid file",
  "invalid stage",
};

/* command enums and matching strings */
typedef enum {
  MAPAINT_CMD_SET,
  MAPAINT_CMD_GET,
  /* the next enum is a dummy to provide an error return
     and the number of valid enums
     add all new enums before this line */
  MAPAINT_CMD_DUMMY
} MAPaintSocketCommand;

static char *MAPaintSocketCmdStr[] = {
  "set",
  "get",
};

/* subject enums and matching strings */
typedef enum {
  MAPAINT_SBJ_DOMAIN,
  MAPAINT_SBJ_VIEW,
  MAPAINT_SBJ_STAGE,
  /* the next enum is a dummy to provide an error return
     and the number of valid enums
     add all new enums before this line */
  MAPAINT_SBJ_DUMMY
} MAPaintSocketSubject;

static char *MAPaintSocketSbjStr[] = {
  "domain",
  "view",
  "stage",
};

static MAPaintSocketCommand socketGetCommand(
  char	*msgBuf,
  char	**msgRemPtr)
{
  MAPaintSocketCommand rtnCmd=MAPAINT_CMD_DUMMY;
  char 	cmdStr[16];
  
  if( sscanf(msgBuf, "%8s", cmdStr) == 1 ){

    if( !strncmp(cmdStr, MAPaintSocketCmdStr[MAPAINT_CMD_SET],
		 strlen(MAPaintSocketCmdStr[MAPAINT_CMD_SET])) ){
      rtnCmd = MAPAINT_CMD_SET;
      *msgRemPtr = strstr(msgBuf, cmdStr) +
	strlen(MAPaintSocketCmdStr[MAPAINT_CMD_SET]);
    }

    if( !strncmp(cmdStr, MAPaintSocketCmdStr[MAPAINT_CMD_GET],
		 strlen(MAPaintSocketCmdStr[MAPAINT_CMD_GET])) ){
      rtnCmd = MAPAINT_CMD_SET;
      *msgRemPtr = strstr(msgBuf, cmdStr) +
	strlen(MAPaintSocketCmdStr[MAPAINT_CMD_GET]);
    }

  }

  return rtnCmd;
}

static MAPaintSocketSubject socketGetSubject(
  char	*msgBuf,
  char	**msgRemPtr)
{
  MAPaintSocketSubject rtnSbj=MAPAINT_SBJ_DUMMY;
  char 	sbjStr[16];
  
  if( sscanf(msgBuf, "%8s", sbjStr) == 1 ){

    if( !strncmp(sbjStr, MAPaintSocketSbjStr[MAPAINT_SBJ_DOMAIN],
		 strlen(MAPaintSocketSbjStr[MAPAINT_SBJ_DOMAIN])) ){
      rtnSbj = MAPAINT_SBJ_DOMAIN;
      *msgRemPtr = strstr(msgBuf, sbjStr) +
	strlen(MAPaintSocketSbjStr[MAPAINT_SBJ_DOMAIN]);
    }

    if( !strncmp(sbjStr, MAPaintSocketSbjStr[MAPAINT_SBJ_VIEW],
		 strlen(MAPaintSocketSbjStr[MAPAINT_SBJ_VIEW])) ){
      rtnSbj = MAPAINT_SBJ_VIEW;
      *msgRemPtr = strstr(msgBuf, sbjStr) +
	strlen(MAPaintSocketSbjStr[MAPAINT_SBJ_VIEW]);
    }

    if( !strncmp(sbjStr, MAPaintSocketSbjStr[MAPAINT_SBJ_STAGE],
		 strlen(MAPaintSocketSbjStr[MAPAINT_SBJ_STAGE])) ){
      rtnSbj = MAPAINT_SBJ_STAGE;
      *msgRemPtr = strstr(msgBuf, sbjStr) +
	strlen(MAPaintSocketSbjStr[MAPAINT_SBJ_STAGE]);
    }

  }

  return rtnSbj;
}

static int socketGetDomainDetail(
  char	*msgBuf,
  int	*domRtn,
  char	*fileStrRtn)
{
  if( sscanf(msgBuf, "%d %s", domRtn, fileStrRtn) < 2 ){
    return 1;
  }
  return 0;
}

static int socketGetViewDetail(
  char	*msgBuf,
  char	*fileStrRtn)
{
  if( sscanf(msgBuf, "%s", fileStrRtn) < 1 ){
    return 1;
  }
  return 0;
}

static int socketGetStageDetail(
  char	*msgBuf,
  char	*stageRtn)
{
  if( sscanf(msgBuf, "%s", stageRtn) < 1 ){
    return 1;
  }
  return 0;
}

static char *socketBuildRequestReturn(
  char			*msgBuf,
  MAPaintSocketStatus	stat,
  MAPaintSocketError	err,
  char			*rtnDetail)
{
  int	msgLen;

  /* we assume that the message buffer has sufficient room
     for the message return. The return length will be 24
     plus the length of the error string */
  msgLen = 24 + (int) strlen(MAPaintSocketErrorStr[err]);
  if( rtnDetail ){
    msgLen += 3 + strlen("detail:") + strlen(rtnDetail);
    sprintf(msgBuf, "%9.9d status %1.1d %4.4d %s; %s %s",
	    msgLen, stat, err, MAPaintSocketErrorStr[err],
	    "detail:", rtnDetail);
  }
  else {
    sprintf(msgBuf, "%9.9d status %1.1d %4.4d %s",
	    msgLen, stat, err, MAPaintSocketErrorStr[err]);
  }

  return msgBuf;
}

static ThreeDViewStruct		*listeningView=NULL;
static int			sock=-1;
static struct sockaddr_in 	server;
static XtInputId		sockId;

static void socketInputReadCb(
  XtPointer	client_data,
  int		*source,
  XtInputId	*inputId)
{
  ThreeDViewStruct *viewStruct=(ThreeDViewStruct *) client_data;
  char	msgBuf[1024], *msgRemPtr, *rtnDetail=NULL;
  int	rtnVal, msgSock, numBytes;
  MAPaintSocketStatus	stat=MAPAINT_ERR;
  MAPaintSocketError	err=MAPAINT_MSG_CORRUPT;
  int		dom;
  char		fileStr[256], stage[16];
  FILE		*fp;
  WlzObject	*obj;

  /* attempt to read from the socket */
  msgSock = accept(*source, NULL, NULL);
  memset(msgBuf, 0, sizeof(msgBuf));
  if( (rtnVal = (int) read(msgSock, msgBuf, 1024)) == -1 ){
    perror("reading stream message");
  }
  else if( rtnVal == 0 ){
    fprintf(stderr, "socketInput, no bytes read\n");
  }
  else {
    /* first check number of bytes */
    if( (rtnVal < 10) || (sscanf(msgBuf, "%d", &numBytes) < 1) ){
      stat = MAPAINT_ERR;
      err = MAPAINT_MSG_CORRUPT;
    }
    else if( rtnVal < numBytes ){
      stat = MAPAINT_ERR;
      err = MAPAINT_MSG_INCOMPLETE;
    }
    else {
      /* parse the message */
      msgRemPtr = &msgBuf[10];
      switch(socketGetCommand(msgRemPtr, &msgRemPtr)){
      case MAPAINT_CMD_SET:
	switch(socketGetSubject(msgRemPtr, &msgRemPtr)){
	case MAPAINT_SBJ_DOMAIN:
	  if( socketGetDomainDetail(msgRemPtr, &dom, fileStr) ){
	    stat = MAPAINT_ERR;
	    err = MAPAINT_DOM_INVALID;
	  }
	  else {
	    /* read the object and set the domain */
	    if( strncmp(fileStr, "Nil", 3) == 0 ){
	      if( clearDomain(dom) ){
		stat = MAPAINT_ERR;
		err = MAPAINT_DOM_INVALID;
	      }
	      else {
		display_all_views_cb(globals.topl, client_data, NULL);
		stat = MAPAINT_OK;
		err = MAPAINT_COM_SUCCESS;
	      }
	    }
	    else {
	      if((fp = fopen(fileStr, "rb"))){
		if((obj = WlzReadObj(fp, NULL))){
		  if( setDomain(obj, dom, fileStr) ){
		    stat = MAPAINT_ERR;
		    err = MAPAINT_DOM_INVALID;
		  }
		  else {
		    display_all_views_cb(globals.topl, client_data, NULL);
		    stat = MAPAINT_OK;
		    err = MAPAINT_COM_SUCCESS;
		  }
		}
		else {
		  stat = MAPAINT_ERR;
		  err = MAPAINT_DOM_INVALID;
		}
		fclose(fp);
	      }
	      else {
		stat = MAPAINT_ERR;
		err = MAPAINT_FILE_INVALID;
	      }
	    }
	  }
	  break;

	case MAPAINT_SBJ_VIEW:
	  socketGetViewDetail(msgRemPtr, fileStr);
	  if( controls_io_quiet_read(fileStr, viewStruct) ){
	    stat = MAPAINT_ERR;
	    err = MAPAINT_COM_FAIL;
	  }
	  else {
	    stat = MAPAINT_OK;
	    err = MAPAINT_COM_SUCCESS;
	  }
	  break;

	case MAPAINT_SBJ_STAGE:
	  if( socketGetStageDetail(msgRemPtr, stage) ){
	    stat = MAPAINT_ERR;
	    err = MAPAINT_STAGE_INVALID;
	  }
	  else {
	    set_theiler_stage_cb(viewStruct->canvas, stage, NULL);
	    stat = MAPAINT_OK;
	    err = MAPAINT_COM_SUCCESS;
	  }
	  break;

	default:
	  stat = MAPAINT_ERR;
	  err = MAPAINT_SBJ_INVALID;
	}
	break;

      case MAPAINT_CMD_GET:
	switch(socketGetSubject(msgRemPtr, &msgRemPtr)){
	case MAPAINT_SBJ_DOMAIN:
	case MAPAINT_SBJ_VIEW:
	  stat = MAPAINT_ERR;
	  err = MAPAINT_SBJ_INVALID;
	  break;

	case MAPAINT_SBJ_STAGE:
	  if( globals.theiler_stage ){
	    rtnDetail = globals.theiler_stage;
	    stat = MAPAINT_OK;
	    err = MAPAINT_COM_SUCCESS;
	  }
	  else {
	    stat = MAPAINT_ERR;
	    err = MAPAINT_STAGE_INVALID;
	  }
	  break;

	default:
	  stat = MAPAINT_ERR;
	  err = MAPAINT_SBJ_INVALID;
	}
	break;

      default:
	stat = MAPAINT_ERR;
	err = MAPAINT_COM_INVALID;
      }

      /* return the status on pending request */
      (void) socketBuildRequestReturn(msgBuf, stat, err, rtnDetail);
      write(msgSock, msgBuf, strlen(msgBuf));
    }
  }

  /* close the socket */
  close(msgSock);

  return;
}


void sockListenCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct *viewStruct=(ThreeDViewStruct *) client_data;
  Widget	button;
  XmString	xmstr;

  /* check if already listening somewhere acquire lock */
  if( listeningView ){
    /* say something in X11 */
    HGU_XmUserError(viewStruct->dialog,
		    "Can't start listening with this view\n"
		    "because another view is already listening\n"
		    "for input. If you want this view to\n"
		    "respond to input then disconnect or\n"
		    "dismiss the other view",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* create the socket */
  if( (sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
    /* failed - should say something in X11 */
    perror("opening stream socket");
    return;
  }

  /* bind the socket */
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(MAPAINT_PORT);
  if( (bind(sock, (struct sockaddr *) &server, sizeof(server))) == -1 ){
    /* failed - say something in X */
    perror("binding stream socket");
    return;
  }

  /* listen and put callback on the input stream */
  listen(sock, 5);
  sockId = XtAppAddInput(globals.app_con, sock, (XtPointer) XtInputReadMask,
			 socketInputReadCb, client_data);

  /* reset the listen button to say "disconnect" and reset the callback */
  if((button = XtNameToWidget(viewStruct->dialog, "*.listen"))){
    xmstr = XmStringCreateSimple("Disconnect");
    XtVaSetValues(button, XmNlabelString, xmstr, NULL);
    XmStringFree(xmstr);
    XtRemoveCallback(button, XmNactivateCallback, sockListenCb,
		     client_data);
    XtAddCallback(button, XmNactivateCallback, sockCloseCb, client_data);
  }

  listeningView = viewStruct;
  return;
}


void sockCloseCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct *viewStruct=(ThreeDViewStruct *) client_data;
  Widget	button;
  XmString	xmstr;

  /* check lock */
  if( listeningView != viewStruct ){
    return;
  }

  /* remove the input callback */
  XtRemoveInput(sockId);

  /* close the socket */
  close(sock);

  /* reset the button label and reset callbacks */
  if((button = XtNameToWidget(viewStruct->dialog, "*.listen"))){
    xmstr = XmStringCreateSimple("Listen");
    XtVaSetValues(button, XmNlabelString, xmstr, NULL);
    XmStringFree(xmstr);
    XtRemoveCallback(button, XmNactivateCallback, sockCloseCb, client_data);
    XtAddCallback(button, XmNactivateCallback, sockListenCb, client_data);
  }

  /* release the listening lock */
  listeningView = NULL;

  return;
}
