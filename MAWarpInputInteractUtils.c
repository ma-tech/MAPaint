#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Woolz Library					*
*   File       :   MAWarpInputInteractUtils.c				*
*************************************************************************
* This module has been copied from the original woolz library and       *
* modified for the public domain distribution. The original authors of  *
* the code and the original file headers and comments are in the        *
* HISTORY file.                                                         *
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Nov 29 14:17:52 1999				*
*   $Revision$							*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <MAPaint.h>
#include <MAWarp.h>

WlzDVertex2 warpDisplayTransBack(
  WlzDVertex2	vtx,
  MAPaintImageWinStruct	*winStruct)
{
  WlzDVertex2	rtnVtx;
  float		x, y;
  int		width, height;	
  WlzCompoundArray	*cobj;

  if( winStruct->obj == NULL ){
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }

  switch( winStruct->obj->type ){
  case WLZ_2D_DOMAINOBJ:
    width = winStruct->obj->domain.i->lastkl -
      winStruct->obj->domain.i->kol1 + 1;
    height = winStruct->obj->domain.i->lastln -
      winStruct->obj->domain.i->line1 + 1;
    break;

  case WLZ_COMPOUND_ARR_1:
  case WLZ_COMPOUND_ARR_2:
    cobj = (WlzCompoundArray *) winStruct->obj;
    if( cobj->n > 0 ){
      width = (cobj->o[0])->domain.i->lastkl -
	(cobj->o[0])->domain.i->kol1 + 1;
      height = (cobj->o[0])->domain.i->lastln -
	(cobj->o[0])->domain.i->line1 + 1;
    }
    else {
      rtnVtx.vtX = -1;
      rtnVtx.vtY = -1;
      return rtnVtx;
    }
    break;

  default:
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }

  /* the back transform - floating point fashion */
  x = vtx.vtX / winStruct->mag;
  y = vtx.vtY / winStruct->mag;

  switch( winStruct->rot ){
  case 0:
    rtnVtx.vtX = x;
    rtnVtx.vtY = y;
    break;

  case 1:
    rtnVtx.vtX = width - y - 1;
    rtnVtx.vtY = x;
    break;

  case 2:
    rtnVtx.vtX = width - x - 1;
    rtnVtx.vtY = height - y - 1;
    break;

  case 3:
    rtnVtx.vtX = y;
    rtnVtx.vtY = height - x - 1;
    break;
  }

  if( winStruct->transpose ){
    rtnVtx.vtX = width - rtnVtx.vtX - 1;
  }

  return rtnVtx;
}
  
WlzDVertex2 warpDisplayTransFore(
  WlzDVertex2	vtx,
  MAPaintImageWinStruct	*winStruct)
{
  WlzDVertex2	rtnVtx;
  float		x, y;
  int		width, height;
  WlzCompoundArray	*cobj;

  if( winStruct->obj == NULL ){
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }

  switch( winStruct->obj->type ){
  case WLZ_2D_DOMAINOBJ:
    width = winStruct->obj->domain.i->lastkl -
      winStruct->obj->domain.i->kol1 + 1;
    height = winStruct->obj->domain.i->lastln -
      winStruct->obj->domain.i->line1 + 1;
    break;

  case WLZ_COMPOUND_ARR_1:
  case WLZ_COMPOUND_ARR_2:
    cobj = (WlzCompoundArray *) winStruct->obj;
    if( cobj->n > 0 ){
      width = (cobj->o[0])->domain.i->lastkl -
	(cobj->o[0])->domain.i->kol1 + 1;
      height = (cobj->o[0])->domain.i->lastln -
	(cobj->o[0])->domain.i->line1 + 1;
    }
    else {
      rtnVtx.vtX = -1;
      rtnVtx.vtY = -1;
      return rtnVtx;
    }
    break;

  default:
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }
  width *= winStruct->mag;
  height *= winStruct->mag;

  /* the foreward transform - floating point fashion */
  x = vtx.vtX * winStruct->mag;
  y = vtx.vtY * winStruct->mag;

  if( winStruct->transpose ){
    x = width - x - winStruct->mag;
  }

  switch( winStruct->rot ){
  case 0:
    rtnVtx.vtX = x;
    rtnVtx.vtY = y;
    break;

  case 3:
    rtnVtx.vtX = height - y - winStruct->mag;
    rtnVtx.vtY = x;
    break;

  case 2:
    rtnVtx.vtX = width - x - winStruct->mag;
    rtnVtx.vtY = height - y - winStruct->mag;
    break;

  case 1:
    rtnVtx.vtX = y;
    rtnVtx.vtY = width - x - winStruct->mag;
    break;
  }

  return rtnVtx;
}
  
int warpCloseDstVtx(
  int	x,
  int	y)
{
  int	dx, dy, i;
  WlzDVertex2	vtx1, vtx2;
  int	delta;

  vtx1.vtX = x;
  vtx1.vtY = y;
  vtx2 = 
    warpDisplayTransBack(vtx1, &(warpGlobals.dst));
  delta = 4 / warpGlobals.dst.mag;

  for(i=0; i < warpGlobals.num_vtxs; i++){
    dx = (int) (warpGlobals.dst_vtxs[i].vtX - vtx2.vtX);
    dy = (int) (warpGlobals.dst_vtxs[i].vtY - vtx2.vtY);
    dx = (dx < 0)?-dx:dx;
    dy = (dy < 0)?-dy:dy;
    if( (dx < delta) && (dy < delta) ){
      return i;
    }
  }

  return -1;
}

int warpCloseSrcVtx(
  int	x,
  int	y)
{
  int	dx, dy, i;
  WlzDVertex2	vtx1, vtx2;
  int	delta;

  vtx1.vtX = x;
  vtx1.vtY = y;
  vtx2 = 
    warpDisplayTransBack(vtx1, &(warpGlobals.src));
  delta = 4 / warpGlobals.src.mag;

  for(i=0; i < warpGlobals.num_vtxs; i++){
    dx = (int) warpGlobals.src_vtxs[i].vtX - vtx2.vtX;
    dy = (int) warpGlobals.src_vtxs[i].vtY - vtx2.vtY;
    dx = (dx < 0)?-dx:dx;
    dy = (dy < 0)?-dy:dy;
    if( (dx < delta) && (dy < delta) ){
      return i;
    }
  }

  return -1;
}

void warpIncrGammaCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  winStruct->gamma *= 1.1;
  if( winStruct->gamma > 10.0 ){
    winStruct->gamma = 10.0;
  }

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpDecrGammaCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  winStruct->gamma /= 1.1;
  if( winStruct->gamma < 0.1 ){
    winStruct->gamma = 0.1;
  }

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpMixRatioCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmScaleCallbackStruct
    *cbs = (XmScaleCallbackStruct *) call_data;

  winStruct->mixRatio = cbs->value;

  warpSetOvlyXImage(winStruct);
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, NULL);

  return;
}

void warpSetOvlyMethodCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = &(warpGlobals.ovly);
  XmToggleButtonCallbackStruct
    *cbs = (XmToggleButtonCallbackStruct *) call_data;

  if( cbs->set ){
    winStruct->mixType = (MAOverlayMixType) client_data;

    warpSetOvlyXImage(winStruct);
    XtCallCallbacks(winStruct->canvas, XmNexposeCallback, NULL);
  }

  return;
}

void warpInvertGreyCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  /* invert the grey-level image - note this just sets a flag so that the
     ximage is the inverse and the original object is unchanged */
  winStruct->grey_invert = !winStruct->grey_invert;

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasMagCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  if( cbs->event->xbutton.state & (Mod1Mask|Mod2Mask|ControlMask|ShiftMask) ){
    if( winStruct->mag > 0.25 ){
      winStruct->mag /= 2.0;
    }
  }
  else {
    if( winStruct->mag < 8.0 ){
      winStruct->mag *= 2.0;
    }
  }	

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasMagPlusCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  if( winStruct->mag < 8.0 ){
    winStruct->mag *= 2.0;
  }

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasMagMinusCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  if( winStruct->mag > 0.25 ){
    winStruct->mag /= 2.0;
  }

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasRotCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  if( cbs->event->xbutton.state & Mod1Mask ){
    winStruct->rot -= 1;
  }
  else {
    winStruct->rot += 1;
  }
  while( winStruct->rot < 0 ){
    winStruct->rot += 4;
  }
  winStruct->rot %= 4;

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasFlipCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  winStruct->transpose = (winStruct->transpose)?0:1;

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasResetCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  winStruct->rot = 0;
  winStruct->transpose = 0;
  winStruct->mag = 1.0;

  if( winStruct == &(warpGlobals.ovly) ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  else {
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}


void warpCanvasMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct;
  XmToggleButtonCallbackStruct
    *cbs = (XmToggleButtonCallbackStruct *) call_data;
  int		winId = (int) client_data;

  switch( winId ){
  case 0 :
    winStruct = &(warpGlobals.dst);
    if( cbs->set == True ){
      XtAddCallback(winStruct->canvas, XmNexposeCallback,
		    warpDisplayDstMeshCb, (XtPointer) winStruct);
      warpDisplayDstMeshCb(winStruct->canvas,
			   (XtPointer) winStruct, call_data);
    }
    else {
      XtRemoveCallback(winStruct->canvas, XmNexposeCallback,
		       warpDisplayDstMeshCb, (XtPointer) winStruct);
      warpCanvasExposeCb(w, (XtPointer) winStruct, call_data);
    }
    break;
  case 1 :
    winStruct = &(warpGlobals.src);
    if( cbs->set == True ){
      XtAddCallback(winStruct->canvas, XmNexposeCallback,
		    warpDisplaySrcMeshCb, (XtPointer) winStruct);
      warpDisplaySrcMeshCb(winStruct->canvas,
			   (XtPointer) winStruct, call_data);
    }
    else {
      XtRemoveCallback(winStruct->canvas, XmNexposeCallback,
		       warpDisplaySrcMeshCb, (XtPointer) winStruct);
      warpCanvasExposeCb(w, (XtPointer) winStruct, call_data);
    }
    break;
  case 2 :
    winStruct = &(warpGlobals.ovly);
    break;
  }

  return;
}

static int	resetOvlyFlg=0;

void warpDstCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  int		vtxIdx;
  WlzDVertex2	vtx;
  Widget	toggle;
  Boolean	autoUpdateFlg;
  int		i;
  UINT		modMask=ShiftMask|ControlMask|LockMask|Mod1Mask|Mod2Mask|
    Mod3Mask|Mod4Mask;

  /* check there is an image */
  if( winStruct->ximage == NULL ){
    return;
  }

  switch( cbs->event->type ){
  case ButtonPress:
    /* remap the event */
    if( MAPaintEventRemap(MAPAINT_WARP_2D_CONTEXT,
			  MAPAINT_WARP_MODE, cbs->event) != WLZ_ERR_NONE ){
      break;
    }

    vtx.vtX = cbs->event->xbutton.x;
    vtx.vtY = cbs->event->xbutton.y;

    /* check for any modifiers */
    if( cbs->event->xbutton.state & modMask ){
      /* check for control only */
      if( !(cbs->event->xbutton.state & (modMask & ~ControlMask)) ){
	WlzFVertex2	start, *rect;

	switch( cbs->event->xbutton.button ){
	case Button1:
	  /* get a rectangle - this will capture the release
	     event */
	  start.vtX = cbs->event->xbutton.x;
	  start.vtY = cbs->event->xbutton.y;
	  if( rect = HGU_XGetRect(XtDisplay(widget),
				  XtWindow(widget),
				  HGU_XNoConfirmMask,
				  NULL, &start) ){
	    /* loop through source vertices to check for selection */
	    rect[1].vtX += rect[0].vtX;
	    rect[1].vtY += rect[0].vtY;
	    for(i=0; i < warpGlobals.num_vtxs; i++){
	      vtx = warpDisplayTransFore(warpGlobals.dst_vtxs[i],
					 &(warpGlobals.dst));
	      if((vtx.vtX >= rect[0].vtX) &&
		 (vtx.vtX <= rect[1].vtX) &&
		 (vtx.vtY >= rect[0].vtY) &&
		 (vtx.vtY <= rect[1].vtY)){
		warpGlobals.sel_vtxs[i] = 1;
	      }
	      else {
		warpGlobals.sel_vtxs[i] = 0;
	      }
	    }
	    AlcFree(rect);
	  }
	  break;

	default:
	  break;
	}
	warpDisplayTiePoints();
      }
    }
    else {
      /* clear any selected vertices */
      for(i=0; i < warpGlobals.num_vtxs; i++){
	warpGlobals.sel_vtxs[i] = 0;
      }

      switch( cbs->event->xbutton.button ){
      case Button1:
	switch( warpGlobals.tp_state ){
	case TP_INACTIVE:
	  /* set the dst vertex */
	  if( warpGlobals.num_vtxs < WARP_MAX_NUM_VTXS ){
	    warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	      warpDisplayTransBack(vtx, &(warpGlobals.dst));
	    warpGlobals.tp_state = TP_DST_DEFINED;
	  }
	  break;

	case TP_DST_DEFINED:
	  /* reset vertex */
	  warpUndisplayVtx(&(warpGlobals.dst),
			   warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
	  warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	    warpDisplayTransBack(vtx, &(warpGlobals.dst));
	  break;

	case TP_SRC_DEFINED:
	  /* set vertex, increment number of tie-points */
	  warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	    warpDisplayTransBack(vtx, &(warpGlobals.dst));
	  warpGlobals.tp_state = TP_SELECTED;
	  warpGlobals.sel_vtx = warpGlobals.num_vtxs;
	  warpGlobals.num_vtxs++;
	  resetOvlyFlg = 1;
	  break;

	case TP_SELECTED:
	  /* reset vertex */
	  warpUndisplayVtx(&(warpGlobals.dst),
			   warpGlobals.dst_vtxs[warpGlobals.sel_vtx]);
	  warpGlobals.dst_vtxs[warpGlobals.sel_vtx] =
	    warpDisplayTransBack(vtx, &(warpGlobals.dst));
	  resetOvlyFlg = 1;
	  break;
	}
	warpDisplayTiePoints();
	break;

      case Button2:
	switch( warpGlobals.tp_state ){
	case TP_INACTIVE:
	  break;

	case TP_DST_DEFINED:
	  /* delete vertex */
	  warpUndisplayVtx(&(warpGlobals.dst),
			   warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
	  warpGlobals.tp_state = TP_INACTIVE;
	  break;

	case TP_SRC_DEFINED:
	  break;

	case TP_SELECTED:
	  /* delete vertex */
	  warpUndisplayVtx(&(warpGlobals.dst),
			   warpGlobals.dst_vtxs[warpGlobals.sel_vtx]);
	  warpUndisplayVtx(&(warpGlobals.src),
			   warpGlobals.src_vtxs[warpGlobals.sel_vtx]);
	  warpGlobals.num_vtxs--;
	  while( warpGlobals.sel_vtx < warpGlobals.num_vtxs ){
	    warpGlobals.dst_vtxs[warpGlobals.sel_vtx] = 
	      warpGlobals.dst_vtxs[warpGlobals.sel_vtx + 1];
	    warpGlobals.src_vtxs[warpGlobals.sel_vtx] = 
	      warpGlobals.src_vtxs[warpGlobals.sel_vtx + 1];
	    warpGlobals.sel_vtx++;
	  }
	  warpGlobals.tp_state = TP_INACTIVE;
	  resetOvlyFlg = 1;
	  break;
	}
	warpDisplayTiePoints();
	break;

      case Button3:
/*	if( warpGlobals.dst.popup ){
	  XmMenuPosition(warpGlobals.dst.popup, &(cbs->event->xbutton));
	  XtManageChild(warpGlobals.dst.popup);
	  }*/
	break;
      }
    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){
    case Button1:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	break;
      case TP_DST_DEFINED:
	break;
      case TP_SRC_DEFINED:
	break;
      case TP_SELECTED:
	break;
      }
      warpDisplayTiePoints();
      break;

    case Button2:
      break;

    case Button3:
      break;
    }
    if( toggle = XtNameToWidget(warpGlobals.warp2DInteractDialog,
				"*.autoUpdate") ){
      XtVaGetValues(toggle, XmNset, &autoUpdateFlg, NULL);
    }
    else {
      autoUpdateFlg = True;
    }
    if( (autoUpdateFlg == True) && resetOvlyFlg ){
      /* update the overlay image and display */
      warpSetOvlyObject();
      warpSetOvlyXImages(&(warpGlobals.ovly), 1);
      warpSetOvlyXImage(&(warpGlobals.ovly));
      warpCanvasExposeCb(warpGlobals.ovly.canvas,
			 (XtPointer) &(warpGlobals.ovly),
			 call_data);
      XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		      call_data);
    }
    resetOvlyFlg = 0;
    break;

  case EnterNotify:
    break;
  case LeaveNotify:
    break;

  case MotionNotify:
    vtx.vtX = cbs->event->xmotion.x;
    vtx.vtY = cbs->event->xmotion.y;

    if( cbs->event->xmotion.state & (Button1Mask) ){
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
      break;

      case TP_DST_DEFINED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.dst),
			 warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
	warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	warpDisplayTiePoints();
	break;

      case TP_SRC_DEFINED:
	break;

      case TP_SELECTED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.dst),
		      warpGlobals.dst_vtxs[warpGlobals.sel_vtx]);
	warpGlobals.dst_vtxs[warpGlobals.sel_vtx] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	warpDisplayTiePoints();
	resetOvlyFlg = 1;
	break;
      }
    }
    else {
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	if( (vtxIdx = warpCloseDstVtx(cbs->event->xmotion.x,
				      cbs->event->xmotion.y)) > -1 ){
	  warpGlobals.sel_vtx = vtxIdx;
	  warpGlobals.tp_state = TP_SELECTED;
	  warpDisplayTiePoints();
	}
	break;

      case TP_DST_DEFINED:
	break;
      case TP_SRC_DEFINED:
	break;
      case TP_SELECTED:
	if( (vtxIdx = warpCloseDstVtx(cbs->event->xmotion.x,
				      cbs->event->xmotion.y)) > -1 ){
	  warpGlobals.sel_vtx = vtxIdx;
	  warpGlobals.tp_state = TP_SELECTED;
	}
	else {
	  warpGlobals.tp_state = TP_INACTIVE;
	}
	warpDisplayTiePoints();
	break;
      }
    }
    break;
  case KeyPress:
    break;
  case KeyRelease:
    break;
  default:
    break;
  }

  return;
}

void warpSrcCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  int		vtxIdx;
  WlzDVertex2	vtx;
  Widget	toggle;
  Boolean	autoUpdateFlg;
  int		i;
  UINT		modMask=ShiftMask|ControlMask|LockMask|Mod1Mask|Mod2Mask|
    Mod3Mask|Mod4Mask;

  /* check there is and image */
  if( winStruct->ximage == NULL ){
    return;
  }

  switch( cbs->event->type ){
  case ButtonPress:
    /* remap the event */
    if( MAPaintEventRemap(MAPAINT_WARP_2D_CONTEXT,
			  MAPAINT_WARP_MODE, cbs->event) != WLZ_ERR_NONE ){
      break;
    }

    vtx.vtX = cbs->event->xbutton.x;
    vtx.vtY = cbs->event->xbutton.y;

    /* check for any modifiers */
    if( cbs->event->xbutton.state & modMask ){
      /* check for shift only */
      if( !(cbs->event->xbutton.state & (modMask & ~ShiftMask)) ){
	WlzFVertex2	start, *rect;

	switch( cbs->event->xbutton.button ){
	case Button1:
	  /* get a rectangle - this will capture the release
	     event */
	  start.vtX = cbs->event->xbutton.x;
	  start.vtY = cbs->event->xbutton.y;
	  if( rect = HGU_XGetRect(XtDisplay(widget),
				  XtWindow(widget),
				  HGU_XNoConfirmMask,
				  NULL, &start) ){
	    /* loop through source vertices to check for selection */
	    rect[1].vtX += rect[0].vtX;
	    rect[1].vtY += rect[0].vtY;
	    for(i=0; i < warpGlobals.num_vtxs; i++){
	      vtx = warpDisplayTransFore(warpGlobals.src_vtxs[i],
					 &(warpGlobals.src));
	      if((vtx.vtX >= rect[0].vtX) &&
		 (vtx.vtX <= rect[1].vtX) &&
		 (vtx.vtY >= rect[0].vtY) &&
		 (vtx.vtY <= rect[1].vtY)){
		warpGlobals.sel_vtxs[i] = 1;
	      }
	      else {
		warpGlobals.sel_vtxs[i] = 0;
	      }
	    }
	    AlcFree(rect);
	  }
	  break;

	default:
	  break;
	}
	warpDisplayTiePoints();
      }
    }
    else {
      /* clear any selected vertices */
      for(i=0; i < warpGlobals.num_vtxs; i++){
	warpGlobals.sel_vtxs[i] = 0;
      }

      switch( cbs->event->xbutton.button ){
      case Button1:
	switch( warpGlobals.tp_state ){
	case TP_INACTIVE:
	  /* set the src vertex */
	  if( warpGlobals.num_vtxs < WARP_MAX_NUM_VTXS ){
	    warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	      warpDisplayTransBack(vtx, &(warpGlobals.src));
	    warpGlobals.tp_state = TP_SRC_DEFINED;
	  }
	  break;

	case TP_SRC_DEFINED:
	  /* reset vertex */
	  warpUndisplayVtx(&(warpGlobals.src),
			   warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
	  warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	    warpDisplayTransBack(vtx, &(warpGlobals.src));
	  break;

	case TP_DST_DEFINED:
	  /* set vertex, increment number of tie-points */
	  warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	    warpDisplayTransBack(vtx, &(warpGlobals.src));
	  warpGlobals.tp_state = TP_SELECTED;
	  warpGlobals.sel_vtx = warpGlobals.num_vtxs;
	  warpGlobals.num_vtxs++;
	  resetOvlyFlg = 1;
	  break;

	case TP_SELECTED:
	  /* reset vertex */
	  warpUndisplayVtx(&(warpGlobals.src),
			   warpGlobals.src_vtxs[warpGlobals.sel_vtx]);
	  warpGlobals.src_vtxs[warpGlobals.sel_vtx] =
	    warpDisplayTransBack(vtx, &(warpGlobals.src));
	  resetOvlyFlg = 1;
	  break;
	}
	warpDisplayTiePoints();
	break;

      case Button2:
	switch( warpGlobals.tp_state ){
	case TP_INACTIVE:
	  break;

	case TP_SRC_DEFINED:
	  /* delete vertex */
	  warpUndisplayVtx(&(warpGlobals.src),
			   warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
	  warpGlobals.tp_state = TP_INACTIVE;
	  break;

	case TP_DST_DEFINED:
	  break;

	case TP_SELECTED:
	  /* delete vertex */
	  warpUndisplayVtx(&(warpGlobals.dst),
			   warpGlobals.dst_vtxs[warpGlobals.sel_vtx]);
	  warpUndisplayVtx(&(warpGlobals.src),
			   warpGlobals.src_vtxs[warpGlobals.sel_vtx]);
	  warpGlobals.num_vtxs--;
	  while( warpGlobals.sel_vtx < warpGlobals.num_vtxs ){
	    warpGlobals.dst_vtxs[warpGlobals.sel_vtx] = 
	      warpGlobals.dst_vtxs[warpGlobals.sel_vtx + 1];
	    warpGlobals.src_vtxs[warpGlobals.sel_vtx] = 
	      warpGlobals.src_vtxs[warpGlobals.sel_vtx + 1];
	    warpGlobals.sel_vtx++;
	  }
	  warpGlobals.tp_state = TP_INACTIVE;
	  resetOvlyFlg = 1;
	  break;
	}
	warpDisplayTiePoints();
	break;

      case Button3:
/*	if( warpGlobals.src.popup ){
	  XmMenuPosition(warpGlobals.src.popup, &(cbs->event->xbutton));
	  XtManageChild(warpGlobals.src.popup);
	  }*/
	break;
      }
    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){
    case Button1:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	break;
      case TP_DST_DEFINED:
	break;
      case TP_SRC_DEFINED:
	break;
      case TP_SELECTED:
	break;
      }
      warpDisplayTiePoints();
      break;

    case Button2:
    case Button3:
      break;
    }
    if( toggle = XtNameToWidget(warpGlobals.warp2DInteractDialog,
				"*.autoUpdate") ){
      XtVaGetValues(toggle, XmNset, &autoUpdateFlg, NULL);
    }
    else {
      autoUpdateFlg = True;
    }
    if( (autoUpdateFlg == True) && resetOvlyFlg ){
      /* update the overlay image and display */
      warpSetOvlyObject();
      warpSetOvlyXImages(&(warpGlobals.ovly), 1);
      warpSetOvlyXImage(&(warpGlobals.ovly));
      warpCanvasExposeCb(warpGlobals.ovly.canvas,
			 (XtPointer) &(warpGlobals.ovly),
			 call_data);
      XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		      call_data);
    }
    resetOvlyFlg = 0;
    break;

  case EnterNotify:
    break;
  case LeaveNotify:
    break;

  case MotionNotify:
    vtx.vtX = cbs->event->xmotion.x;
    vtx.vtY = cbs->event->xmotion.y;

    if( cbs->event->xmotion.state & (Button1Mask) ){
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
      break;

      case TP_SRC_DEFINED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.src),
			 warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
	warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	warpDisplayTiePoints();
	break;

      case TP_DST_DEFINED:
	break;

      case TP_SELECTED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.src),
			 warpGlobals.src_vtxs[warpGlobals.sel_vtx]);
	warpGlobals.src_vtxs[warpGlobals.sel_vtx] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	warpDisplayTiePoints();
	resetOvlyFlg = 1;
	break;
      }
    }
    else {
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	if( (vtxIdx = warpCloseSrcVtx(cbs->event->xmotion.x,
				      cbs->event->xmotion.y)) > -1 ){
	  warpGlobals.sel_vtx = vtxIdx;
	  warpGlobals.tp_state = TP_SELECTED;
	  warpDisplayTiePoints();
	}
	break;

      case TP_DST_DEFINED:
	break;
      case TP_SRC_DEFINED:
	break;
      case TP_SELECTED:
	if( (vtxIdx = warpCloseSrcVtx(cbs->event->xmotion.x,
				      cbs->event->xmotion.y)) > -1 ){
	  warpGlobals.sel_vtx = vtxIdx;
	  warpGlobals.tp_state = TP_SELECTED;
	}
	else {
	  warpGlobals.tp_state = TP_INACTIVE;
	}
	warpDisplayTiePoints();
	break;
      }
    }
    break;

  case KeyPress:
    break;
  case KeyRelease:
    break;
  default:
    break;
  }

  return;
}

void warpOvlyCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  int		vtxIdx;
  WlzDVertex2	vtx;

  /* check there is an image */
  if( winStruct->ximage == NULL ){
    return;
  }

  switch( cbs->event->type ){
  case ButtonPress:
    /* remap the event */
    if( MAPaintEventRemap(MAPAINT_WARP_2D_CONTEXT,
			  MAPAINT_WARP_MODE, cbs->event) != WLZ_ERR_NONE ){
      break;
    }

    vtx.vtX = cbs->event->xbutton.x;
    vtx.vtY = cbs->event->xbutton.y;

    switch( cbs->event->xbutton.button ){

    case Button3:
/*      if( winStruct->popup ){
	XmMenuPosition(winStruct->popup, &(cbs->event->xbutton));
	XtManageChild(winStruct->popup);
	}*/
      break;

    default:
      break;
    }
    break;

  case KeyPress:
    break;
  case KeyRelease:
    break;
  default:
    break;
  }

  return;
}
