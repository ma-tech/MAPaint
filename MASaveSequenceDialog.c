#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MASaveSeque_ceDialog_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MASaveSequenceDialog.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:47:05 2009
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par Copyright:
* Copyright (C) 2005 Medical research Council, UK.
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
*               
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>
#include <X11/cursorfont.h>

Widget	save_seq_dialog=NULL;


static int	num_rows=0;
static Widget	rowWidgets[6];
static Widget	rowSliders[6];
static int	sequenceIndxs[6];
static XtIntervalId	timeoutIds[6];
static unsigned long	timeoutTime[6];
static char	*commandDefStr=
"import -crop 512x512+0+0";

static ActionAreaItem   autosave_dialog_actions[] = {
  {"Start",		NULL,		NULL},
  {"Stop",		NULL,		NULL},
  {"New",		NULL,		NULL},
  {"Delete",		NULL,		NULL},
  {"Dismiss",     	NULL,           NULL},
  {"Help",        	NULL,           NULL},
};

void save_seq_opts_cb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  /* put up the autosave dialog */
  PopupCallback(w, (XtPointer) XtParent(save_seq_dialog), NULL);

  return;
}

static Widget get_window_widget(Widget	w)
{
  if( XtIsShell( w ) || XtIsShell(XtParent(w)) )
    return( w );

  return( get_window_widget( XtParent(w) ) );
}

Window GetWMWindow(
  Display		*dpy,
  Window		win)
{
  unsigned int		nchildren;
  XWindowAttributes	win_att;
  Window		root, parent, *children;

  /* get attributes */
  if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
    return( 1 );

  if( win == win_att.root ){ /* already too far*/
    return win;
  }

  /* get the parent window */
  XQueryTree(dpy, win, &root, &parent, &children, &nchildren);
  XFree( children );

  /* check parent */
  if( parent == win_att.root ){
    return win;
  }
  else {
    return GetWMWindow(dpy, parent);
  }
}

void saveSeqSliderCb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  int		rowIndx = (int) client_data;
  char		cmdStr[256], fileBuf[128];
  char		*fileStr, *commandStr, *winStr;
  Widget	widget;
  int		i,  j;

  /* get the data strings */
  if((widget = XtNameToWidget(rowWidgets[rowIndx], "*.command"))){
    XtVaGetValues(widget, XmNvalue, &commandStr, NULL);
  }
  else {
    commandStr = "import";
  }
  if((widget = XtNameToWidget(rowWidgets[rowIndx], "*.window"))){
    XtVaGetValues(widget, XmNvalue, &winStr, NULL);
  }
  else {
    winStr = "root";
  }
  if((widget = XtNameToWidget(rowWidgets[rowIndx], "*.filename"))){
    XtVaGetValues(widget, XmNvalue, &fileStr, NULL);
    strcpy(fileBuf, fileStr);
    for(i=0, j=strlen(fileStr); i < strlen(fileStr); i++){
      if(fileStr[i] == '.'){
	j = i;
      }
    }
    fileBuf[j] = '\0';
  }
  else {
    fileStr = "MASaveSeq_";
  }

  /* generate the system command */
  sprintf(cmdStr, "%s -window %s %s%05d.jpg", commandStr, winStr, fileBuf,
	  sequenceIndxs[rowIndx]);
  fprintf(stderr, "%s\n", cmdStr);
  if( system(cmdStr) < 0 ){
    HGU_XmUserError(save_seq_dialog,
		    "Can't execute the save command:\n"
		    "    Please stop the save sequence process\n"
		    "    check the program \"import\" is\n"
		    "    available or check and reset the command\n"
		    "    string",
		    XmDIALOG_FULL_APPLICATION_MODAL);
  }

  sequenceIndxs[rowIndx]++;

  return;
}

static void saveSeqTimeOutProc(
  XtPointer	client_data,
  XtIntervalId	*id)
{
  int	rowIndx = (int) client_data;

  /* sanity check */
  if( *id != timeoutIds[rowIndx] ){
    return;
  }

  /* call the callback to write the file */
  saveSeqSliderCb(save_seq_dialog, client_data, NULL);

  /* reset the timeout */
  timeoutIds[rowIndx] = XtAppAddTimeOut(globals.app_con,
					timeoutTime[rowIndx],
					saveSeqTimeOutProc,
					(XtPointer) rowIndx);

  return;
}

void saveSeqStartCb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  int		i;
  Widget	toggle, scale, widget;
  XtCallbackList	dragList;
  Boolean		set;
  int			time;
  char			*timeStr;

  /* make the controls insensitive */
  if((widget = XtNameToWidget(save_seq_dialog, "*.control"))){
    XtSetSensitive(widget, False);
  }
  if((widget = XtNameToWidget(save_seq_dialog, "*.Start"))){
    XtSetSensitive(widget, False);
  }
  if((widget = XtNameToWidget(save_seq_dialog, "*.New"))){
    XtSetSensitive(widget, False);
  }
  if((widget = XtNameToWidget(save_seq_dialog, "*.Delete"))){
    XtSetSensitive(widget, False);
  }
  if((widget = XtNameToWidget(save_seq_dialog, "*.Dismiss"))){
    XtSetSensitive(widget, False);
  }

  /* set up timers and callbacks */
  for(i=1; i < num_rows; i++){

    /* check the toggle */
    set = False;
    if((toggle = XtNameToWidget(rowWidgets[i], "*.toggle"))){
      XtVaGetValues(toggle, XmNset, &set, NULL);
    }
    if( set == False ){
      continue;
    }

    /* add the slider callbacks */
    if( rowSliders[i] ){
      sequenceIndxs[i] = 0;
      if((scale = XtNameToWidget(rowSliders[i], "*.scale"))){
	XtAddCallback(scale, XmNvalueChangedCallback,
		      saveSeqSliderCb, (XtPointer) i);
	XtVaGetValues(scale, XmNdragCallback, &dragList, NULL);
	if( dragList ){
	  XtAddCallback(scale, XmNdragCallback,
			saveSeqSliderCb, (XtPointer) i);
	}
      }
    }
    /* else add the timer callbacks */
    else {
      sequenceIndxs[i] = 0;
      timeoutIds[i] = 0;

      /* get the time interval */
      if((widget = XtNameToWidget(rowWidgets[i], "*.interval"))){
	XtVaGetValues(widget, XmNvalue, &timeStr, NULL);
      }
      else {
	timeStr = "3";
      }
      time = atoi(timeStr);
      if( (time < 1) || (time > 1000) ){
	continue;
      }
      timeoutTime[i] = time*1000;
      timeoutIds[i] = XtAppAddTimeOut(globals.app_con,
				      timeoutTime[i],
				      saveSeqTimeOutProc,
				      (XtPointer) i);
    }

  }
  
  /* make the stop button sensitive */
  if((widget = XtNameToWidget(save_seq_dialog, "*.Stop"))){
    XtSetSensitive(widget, True);
  }
  return;
}

void saveSeqStopCb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  int		i;
  Widget	toggle, scale, widget;
  XtCallbackList	dragList;
  Boolean		set;

  /* make the stop button insensitive */
  if((widget = XtNameToWidget(save_seq_dialog, "*.Stop"))){
    XtSetSensitive(widget, False);
  }

  /* clear timers and callbacks */
  for(i=1; i < num_rows; i++){

    /* check the toggle */
    set = False;
    if((toggle = XtNameToWidget(rowWidgets[i], "*.toggle"))){
      XtVaGetValues(toggle, XmNset, &set, NULL);
    }
    if( set == False ){
      continue;
    }

    /* add the slider callbacks */
    if( rowSliders[i] ){
      sequenceIndxs[i] = 0;
      if((scale = XtNameToWidget(rowSliders[i], "*.scale"))){
	XtRemoveCallback(scale, XmNvalueChangedCallback,
			 saveSeqSliderCb, (XtPointer) i);
	XtVaGetValues(scale, XmNdragCallback, &dragList, NULL);
	if( dragList ){
	  XtRemoveCallback(scale, XmNdragCallback,
			   saveSeqSliderCb, (XtPointer) i);
	}
      }
    }
    /* else remove the timer callbacks */
    else {
      if( timeoutIds[i] ){
	XtRemoveTimeOut(timeoutIds[i]);
	timeoutIds[i] = 0;
      }
    }
  }
  
  /* make the controls insensitive */
  if((widget = XtNameToWidget(save_seq_dialog, "*.control"))){
    XtSetSensitive(widget, True);
  }
  if((widget = XtNameToWidget(save_seq_dialog, "*.Start"))){
    XtSetSensitive(widget, True);
  }
  if((widget = XtNameToWidget(save_seq_dialog, "*.New"))){
    XtSetSensitive(widget, True);
  }
  if((widget = XtNameToWidget(save_seq_dialog, "*.Delete"))){
    XtSetSensitive(widget, True);
  }
  if((widget = XtNameToWidget(save_seq_dialog, "*.Dismiss"))){
    XtSetSensitive(widget, True);
  }

  return;
}

void saveSeqNewCb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	control = (Widget) client_data;
  Widget	row_rc, text, toggle;
  Widget	slider;
  char		*commandStr, *fileStr;
  char		timeStr[8], windowStr[16];
  Window	window=0;
  Cursor		cursor;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;

  /* check max number of rows */
  if( num_rows >= 6 ){
    return;
  }

  /* get the type, command/slider,
     window and filename - by default off */
  cursor = XCreateFontCursor(XtDisplayOfObject(w), XC_hand2);
  sprintf(timeStr, "3");
  if( !(commandStr = HGU_XmUserGetstr(save_seq_dialog,
				    "Please type in the required command\n"
				    "to be executed at the specified time\n"
				    "interval. Select \"slider\" if you \n"
				    "want to attach the save-to-file to a\n"
				    "slider callback. For this option the\n"
				    "required slider must be visible\n",
				    "OK", "Slider", commandDefStr)) ){
    /* get the slider widget pointer */
    if((slider = XmTrackingEvent(globals.topl, cursor, False, cbs->event))){
      XSync(XtDisplayOfObject(w), False);
      if(strcmp(XtName(slider), "Scrollbar")){
	XFreeCursor(XtDisplayOfObject(w), cursor);
	return;
      }
      slider = XtParent(slider);
      if(strcmp(XtName(slider), "scale")){
	XFreeCursor(XtDisplayOfObject(w), cursor);
	return;
      }
      slider = XtParent(slider);
      if( XmIsForm(slider) ){
	commandStr = "import";
      }
      else {
	XFreeCursor(XtDisplayOfObject(w), cursor);
	return;
      }
      rowSliders[num_rows] = slider;
      sprintf(timeStr, "slider");
    }
    else {
      XFreeCursor(XtDisplayOfObject(w), cursor);
      return;
    }
  }
  else {
    rowSliders[num_rows] = NULL;
  }

  /* get the window */
  if( !(fileStr = HGU_XmUserGetstr(save_seq_dialog,
				   "Please type in the required window Id\n"
				   "which can be obtained using xwininfo,\n"
				   "or left as \"root\", or press \"Select\"\n"
				   "below then click the left button when\n"
				   "the cursor is over the required window\n",
				   "OK", "Select", "root")) ){
    if((slider = XmTrackingEvent(globals.topl, cursor, False, cbs->event))){
      window = XtWindow(get_window_widget(slider));
    }
    sprintf(windowStr, "0x%x", (unsigned int) GetWMWindow(XtDisplayOfObject(w), window));
  }
  else {
    sprintf(windowStr, "%s", fileStr);
    AlcFree(fileStr);
  }
  XFreeCursor(XtDisplayOfObject(w), cursor);

  /* get the filename stub */
  if( !(fileStr = HGU_XmUserGetFilename(save_seq_dialog,
					"Please type in a filename stub\n"
					"for the sequence images which will\n"
					"be save in jpeg format. Note the \n"
					"extension will be stripped off and\n"
					"replaced with <4-digit seq num>.jpg",
					"OK", "cancel", "MASaveSeq-.jpg",
					NULL, "*.jpg")) ){
    return;
  }
  

  /* add four text boxes plus one toggle */
  row_rc = XtVaCreateManagedWidget("row_row_column",
				   xmFormWidgetClass,
				   control,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget, rowWidgets[num_rows-1],
				   XmNleftAttachment,	XmATTACH_FORM,
				   XmNrightAttachment,	XmATTACH_FORM,
				   NULL);
  text = XtVaCreateManagedWidget("command", xmTextFieldWidgetClass,
				 row_rc,
				 XmNvalue,	commandStr,
				 XmNleftAttachment, XmATTACH_POSITION,
				 XmNleftPosition, 1,
				 XmNrightAttachment, XmATTACH_POSITION,
				 XmNrightPosition, 32,
				 NULL);
  text = XtVaCreateManagedWidget("interval", xmTextFieldWidgetClass,
				 row_rc,
				 XmNvalue,	timeStr,
				 XmNleftAttachment, XmATTACH_POSITION,
				 XmNleftPosition, 33,
				 XmNrightAttachment, XmATTACH_POSITION,
				 XmNrightPosition, 42,
				 NULL);
  if( rowSliders[num_rows] ){
    XtVaSetValues(text, XmNeditable, False, NULL);
  }
  text = XtVaCreateManagedWidget("window", xmTextFieldWidgetClass,
				 row_rc,
				 XmNvalue,	windowStr,
				 XmNleftAttachment, XmATTACH_POSITION,
				 XmNleftPosition, 43,
				 XmNrightAttachment, XmATTACH_POSITION,
				 XmNrightPosition, 59,
				 NULL);
  text = XtVaCreateManagedWidget("filename", xmTextFieldWidgetClass,
				 row_rc,
				 XmNvalue,	fileStr,
				 XmNleftAttachment, XmATTACH_POSITION,
				 XmNleftPosition, 60,
				 XmNrightAttachment, XmATTACH_POSITION,
				 XmNrightPosition, 89,
				 NULL);
  toggle = XtVaCreateManagedWidget("toggle", xmToggleButtonGadgetClass,
				   row_rc,
				   XmNvisibleWhenOff,	True,
				   XmNleftAttachment, XmATTACH_POSITION,
				   XmNleftPosition, 90,
				   XmNrightAttachment, XmATTACH_POSITION,
				   XmNrightPosition, 99,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNbottomAttachment, XmATTACH_WIDGET,
				   NULL);
  rowWidgets[num_rows] = row_rc;
  num_rows++;

  return;
}

Widget create_save_seq_dialog(
  Widget	topl)
{
  Widget	dialog, control, widget;
  Widget	row_rc;

  dialog = HGU_XmCreateStdDialog(topl, "save_seq_dialog",
				 xmFormWidgetClass,
				 autosave_dialog_actions, 6);

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  control = XtNameToWidget( dialog, "*.control" );

  /* add the title line */
  row_rc = XtVaCreateManagedWidget("title_row_column",
				   xmFormWidgetClass,
				   control,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_FORM,
				   XmNrightAttachment,	XmATTACH_FORM,
				   NULL);
  widget = XtVaCreateManagedWidget("Type", xmLabelWidgetClass,
				   row_rc,
				   XmNleftAttachment, XmATTACH_POSITION,
				   XmNleftPosition, 1,
				   XmNrightAttachment, XmATTACH_POSITION,
				   XmNrightPosition, 32,
				   XmNalignment, XmALIGNMENT_BEGINNING,
				   NULL);
  widget = XtVaCreateManagedWidget("Interval", xmLabelWidgetClass,
				   row_rc,
				   XmNleftAttachment, XmATTACH_POSITION,
				   XmNleftPosition, 33,
				   XmNrightAttachment, XmATTACH_POSITION,
				   XmNrightPosition, 42,
				   XmNalignment, XmALIGNMENT_BEGINNING,
				   NULL);
  widget = XtVaCreateManagedWidget("Window", xmLabelWidgetClass,
				   row_rc,
				   XmNleftAttachment, XmATTACH_POSITION,
				   XmNleftPosition, 43,
				   XmNrightAttachment, XmATTACH_POSITION,
				   XmNrightPosition, 59,
				   XmNalignment, XmALIGNMENT_BEGINNING,
				   NULL);
  widget = XtVaCreateManagedWidget("Filename", xmLabelWidgetClass,
				   row_rc,
				   XmNleftAttachment, XmATTACH_POSITION,
				   XmNleftPosition, 60,
				   XmNrightAttachment, XmATTACH_POSITION,
				   XmNrightPosition, 89,
				   XmNalignment, XmALIGNMENT_BEGINNING,
				   NULL);
  widget = XtVaCreateManagedWidget("On/Off", xmLabelWidgetClass,
				   row_rc,
				   XmNleftAttachment, XmATTACH_POSITION,
				   XmNleftPosition, 90,
				   XmNrightAttachment, XmATTACH_POSITION,
				   XmNrightPosition, 99,
				   NULL);
  rowWidgets[0] = row_rc;
  num_rows = 1;
 
  if( (widget = XtNameToWidget(dialog, "*.New")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, saveSeqNewCb,
		  (XtPointer) control);
  }

  if( (widget = XtNameToWidget(dialog, "*.Start")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, saveSeqStartCb,
		  (XtPointer) control);
  }

  if( (widget = XtNameToWidget(dialog, "*.Stop")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, saveSeqStopCb,
		  (XtPointer) control);
    XtSetSensitive(widget, False);
  }

  return( dialog );
}
