#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Mouse Atlas Project					*
*   File       :   MAWarpInput2DDialog.c				*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Jun 11 14:13:51 1999				*
*   $Revision$								*
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

#define WARP_MAX_NUM_VTXS 128

extern Widget create_view_window_dialog(Widget topl,
					double theta,
					double phi,
					WlzDVertex3 *fixed);

static Widget create2DWarpDialog(Widget parent,
				 ThreeDViewStruct *view_struct);
typedef struct {
  WlzObject		*obj;
  Widget		canvas;
  XImage		*ximage;
  GC			gc;
  double		mag;
  int			rot;
  int			transpose;
} MAPaintImageWinStruct;
  
typedef struct {
  ThreeDViewStruct	*view_struct;
  Widget		warp2DInteractDialog;

  /* destination canvas and affine transform */
  MAPaintImageWinStruct	dst;

  /* source canvas and affine transform */
  MAPaintImageWinStruct	src;

  /* overlay canvas and affine transform */
  MAPaintImageWinStruct	ovly;
  
  /* tie-points */
  WlzFVertex2	dst_vtxs[WARP_MAX_NUM_VTXS];
  WlzFVertex2	src_vtxs[WARP_MAX_NUM_VTXS];
  int		num_vtxs;
  int		sel_vtx;
  int		tp_state;
  GC		green_gc;
  GC		red_gc;
  WlzAffineTransform	*affine;
  WlzBasisFnTransform	*warp;

} MAPaintWarp2DStruct;

typedef enum
{
  TP_INACTIVE,
  TP_DST_DEFINED,
  TP_SRC_DEFINED,
  TP_SELECTED
} WarpTiePointState;

static MAPaintWarp2DStruct warpGlobals;
static Widget warp_read_src_dialog=NULL;

static WlzFVertex2 warpDisplayTransBack(
  WlzFVertex2	vtx,
  MAPaintImageWinStruct	*winStruct)
{
  WlzFVertex2	rtnVtx;
  float		x, y;
  int		width, height;

  if( winStruct->obj == NULL ){
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }
  width = winStruct->obj->domain.i->lastkl -
    winStruct->obj->domain.i->kol1 + 1;
  height = winStruct->obj->domain.i->lastln -
    winStruct->obj->domain.i->line1 + 1;

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
  
static WlzFVertex2 warpDisplayTransFore(
  WlzFVertex2	vtx,
  MAPaintImageWinStruct	*winStruct)
{
  WlzFVertex2	rtnVtx;
  float		x, y;
  int		width, height;

  if( winStruct->obj == NULL ){
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }
  width = winStruct->obj->domain.i->lastkl -
    winStruct->obj->domain.i->kol1 + 1;
  height = winStruct->obj->domain.i->lastln -
    winStruct->obj->domain.i->line1 + 1;
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
  
static void warpSetXImage(
  MAPaintImageWinStruct *winStruct)
{
  XWindowAttributes	win_att;
  Dimension		src_width, src_height, width, height;
  UBYTE			*data, *dst_data, *src_data;
  int			i, j, ip, jp;

  if( winStruct->ximage ){
    AlcFree(winStruct->ximage->data);
    winStruct->ximage->data = NULL;
    XDestroyImage(winStruct->ximage);
    winStruct->ximage = NULL;
  }

  if( !winStruct->obj ){
    return;
  }

  /* create the ximage using current mag and rotate */
  if( XGetWindowAttributes(XtDisplay(winStruct->canvas),
			   XtWindow(winStruct->canvas), &win_att) == 0 ){
    return;
  }
  src_width = winStruct->obj->domain.i->lastkl -
    winStruct->obj->domain.i->kol1 + 1; 
  src_height = winStruct->obj->domain.i->lastln -
    winStruct->obj->domain.i->line1 + 1;
  if( (winStruct->rot)%2 ){
    width = src_height*winStruct->mag;
    height = src_width*winStruct->mag;
  }
  else {
    width = src_width*winStruct->mag;
    height = src_height*winStruct->mag;
  }

  XtVaSetValues(winStruct->canvas,
		XmNwidth, width,
		XmNheight, height,
		NULL);

  src_data = winStruct->obj->values.r->values.ubp;
  dst_data = (UBYTE *) AlcMalloc(((win_att.depth == 8)?1:4)
			     *width*height*sizeof(char));
  data = dst_data;

  for(j=0; j < height; j++){
    for(i=0; i < width; i++, data++){
      WlzFVertex2 vtx1, vtx2;
      vtx1.vtX = i;
      vtx1.vtY = j;
      vtx2 = warpDisplayTransBack(vtx1, winStruct);
      ip = vtx2.vtX;
      jp = vtx2.vtY;
      *data = src_data[jp*src_width + ip];
      if( win_att.depth == 24 ){
	data[1] = data[0];
	data[2] = data[0];
	data[3] = data[0];
	data += 3;
      }
    }
  }

  winStruct->ximage = XCreateImage(XtDisplay(winStruct->canvas),
				   win_att.visual, win_att.depth,
				   ZPixmap, 0, (char *) dst_data,
				   width, height, 8, 0);

  return;
}

static int warpCloseDstVtx(
  int	x,
  int	y)
{
  int	dx, dy, i;
  WlzFVertex2	vtx1, vtx2;
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

static int warpCloseSrcVtx(
  int	x,
  int	y)
{
  int	dx, dy, i;
  WlzFVertex2	vtx1, vtx2;
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

static void warpUndisplayVtx(
  MAPaintImageWinStruct	*winStruct,
  WlzFVertex2	vtx)
{
  int	x, y, width, height;
  WlzFVertex2	vtx1;

  if( winStruct->ximage ){

    width = winStruct->ximage->width;
    height = winStruct->ximage->height;
    vtx1 = warpDisplayTransFore(vtx, winStruct);
    
    x = vtx1.vtX - 3;
    y = vtx1.vtY - 3;

    if( (x < -4) || (x >= width) || (y < -4) || (y >= height) ){
      return;
    }

    x = (x < 0) ? 0 : x;
    y = (y < 0) ? 0 : y;

    width = WLZ_MIN(width - x, 8);
    height = WLZ_MIN(height - y, 8);

    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage, x, y, x, y,
	      width, height);
    XFlush(XtDisplay(winStruct->canvas));
  }

  return;
}

static void warpDisplayTiePoints(void)
{
  Display	*dpy=XtDisplay(warpGlobals.dst.canvas);
  Window	dst_win = XtWindow(warpGlobals.dst.canvas);
  Window	src_win = XtWindow(warpGlobals.src.canvas);
  GC		gc;
  int		i, x, y;
  WlzFVertex2	vtx1, vtx2;

  /* check gc's */
  if( warpGlobals.red_gc == (GC) -1 ){
    XGCValues	values;

    values.foreground = 0xff;
    values.background = 0;
    warpGlobals.red_gc = XCreateGC(dpy, dst_win,
				   GCForeground|GCBackground,
				   &values);
    values.foreground = 0xff00;
    warpGlobals.green_gc = XCreateGC(dpy, dst_win,
				   GCForeground|GCBackground,
				   &values);
  }

  for(i=0; i < warpGlobals.num_vtxs; i++){
    if((warpGlobals.tp_state == TP_SELECTED) && (i == warpGlobals.sel_vtx)){
      gc = warpGlobals.red_gc;
    }
    else {
      gc = warpGlobals.green_gc;
    }
    vtx2 = warpDisplayTransFore(warpGlobals.dst_vtxs[i],
				&(warpGlobals.dst));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, dst_win, gc, x, y, 4, 4);

    vtx2 = warpDisplayTransFore(warpGlobals.src_vtxs[i],
				&(warpGlobals.src));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, src_win, gc, x, y, 4, 4);
  }

  if( warpGlobals.tp_state == TP_DST_DEFINED ){
    gc = warpGlobals.red_gc;

    vtx2 = warpDisplayTransFore(warpGlobals.dst_vtxs[warpGlobals.num_vtxs],
				&(warpGlobals.dst));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, dst_win, gc, x, y, 4, 4);
  }

  if( warpGlobals.tp_state == TP_SRC_DEFINED ){
    gc = warpGlobals.red_gc;
    vtx2 = warpDisplayTransFore(warpGlobals.src_vtxs[warpGlobals.num_vtxs],
				&(warpGlobals.src));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, src_win, gc, x, y, 4, 4);
  }

  XFlush(dpy);
  return;
}

void warpCanvasExposeCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;

  if( !winStruct->ximage ){
    warpSetXImage(winStruct);
    if( !winStruct->ximage ){
      return;
    }
  }

  /* check th graphics context */
  if( winStruct->gc == (GC) -1 ){
    XGCValues  gcvalues;
    winStruct->gc = XCreateGC(XtDisplay(winStruct->canvas),
			      XtWindow(winStruct->canvas),
			      0, &gcvalues);
  }

  /* display the image - if it is a real expose event just
     put the exposed rectangle */
  if( cbs && (cbs->event->type == Expose) ){
    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage,
	      cbs->event->xexpose.x, cbs->event->xexpose.y,
	      cbs->event->xexpose.x, cbs->event->xexpose.y,
	      cbs->event->xexpose.width, cbs->event->xexpose.height);
  }
  else {
    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage, 0, 0, 0, 0,
	      winStruct->ximage->width, winStruct->ximage->height);
  }
  XFlush(XtDisplay(winStruct->canvas));

  warpDisplayTiePoints();
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

  if( cbs->event->xbutton.state & Mod1Mask ){
    if( winStruct->mag > 0.25 ){
      winStruct->mag /= 2.0;
    }
  }
  else {
    if( winStruct->mag < 16.0 ){
      winStruct->mag *= 2.0;
    }
  }

  warpSetXImage(winStruct);
  warpCanvasExposeCb(w, client_data, call_data);

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

  warpSetXImage(winStruct);
  warpCanvasExposeCb(w, client_data, call_data);

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

  warpSetXImage(winStruct);
  warpCanvasExposeCb(w, client_data, call_data);

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

  warpSetXImage(winStruct);
  warpCanvasExposeCb(w, client_data, call_data);

  return;
}


void warpReadSourceCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzObject		*obj;
  FILE			*fp;
  WlzErrorNum		errNum;

  if( (fp = HGU_XmGetFilePointer(view_struct->dialog, cbs->value,
				 cbs->dir, "r")) == NULL ){
    return;
  }

  /* read the new source object - ready to extend to other types */
  obj = WlzEffReadObj(fp, NULL, WLZEFF_FORMAT_WLZ, &errNum);
  fclose(fp);

  /* check the object */
  if( errNum == WLZ_ERR_NONE ){
    if( obj ){
      switch( obj->type ){
      case WLZ_2D_DOMAINOBJ:
	if( warpGlobals.src.obj ){
	  WlzFreeObj(warpGlobals.src.obj);
	}
	warpGlobals.src.obj = WlzAssignObject(obj, &errNum);
	warpSetXImage(&(warpGlobals.src));
	warpCanvasExposeCb(warpGlobals.src.canvas,
			   (XtPointer) &(warpGlobals.src),
			   call_data);
	break;

      default:
	WlzFreeObj(obj);
	HGU_XmUserError(globals.topl,
			"Read Source Object:\n"
			"    Wrong object type, please select\n"
			"    another image file.",
			XmDIALOG_FULL_APPLICATION_MODAL);
	break;
      }
    }
  }

  return;
}

void warpReadSourcePopupCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Arg		arg[1];
  Visual	*visual;

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(globals.topl);
  XtSetArg(arg[0], XmNvisual, visual);

  if( warp_read_src_dialog == NULL ){
    warp_read_src_dialog =
      XmCreateFileSelectionDialog(view_struct->dialog,
				  "warp_read_src_dialog", arg, 1);

    XtAddCallback(warp_read_src_dialog, XmNokCallback,
		  warpReadSourceCb, client_data);
    XtAddCallback(warp_read_src_dialog, XmNokCallback,
		  PopdownCallback, NULL);
    XtAddCallback(warp_read_src_dialog, XmNcancelCallback, 
		  PopdownCallback, NULL);
  }

  XtManageChild(warp_read_src_dialog);
  PopupCallback(w, (XtPointer) XtParent(warp_read_src_dialog), NULL);

  return;
}

void warpInput2DCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	dialog, widget;

  if( dialog = createWarpInput2DDialog(globals.topl) ){
    if( widget = XtNameToWidget(globals.topl,
				"*options_menu*warp_input_2d") ){
      XtSetSensitive(widget, False);
    }
    XtManageChild(dialog);
  }

  return;
}

void warpInput2DDestroyCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget		widget;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  if( widget = XtNameToWidget(globals.topl, "*options_menu*warp_input_2d") ){
    XtSetSensitive(widget, True);
  }

  if( paint_key == view_struct ){
    paint_key = NULL;
  }

  if( warpGlobals.dst.obj ){
    WlzFreeObj(warpGlobals.dst.obj);
  }

  if( warpGlobals.src.obj ){
    WlzFreeObj(warpGlobals.src.obj);
  }
  
  return;
}

static void warpSetupCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmToggleButtonCallbackStruct
    *cbs = (XmToggleButtonCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Widget		dialog;

  dialog = view_struct->dialog;

  if( cbs->set == True ){
    if( paint_key == view_struct ){
      if( !warpGlobals.warp2DInteractDialog ){
	warpGlobals.warp2DInteractDialog =
	  create2DWarpDialog(dialog, view_struct);
      }
      XtManageChild(warpGlobals.warp2DInteractDialog);
    }
  }
  else {
    if( warpGlobals.warp2DInteractDialog ){
      XtUnmanageChild( warpGlobals.warp2DInteractDialog );
    }
  }

  return;
}

static void warpControlsCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Widget		shell, dialog, cntrlFrame, cntrlForm, magFncForm;
  int			wasManaged;
  Dimension		shellHeight, cntrlFormHeight;

  /* get the section viewer frames and unmanage everything */
  dialog = view_struct->dialog;
  shell = XtParent(dialog);
  cntrlFrame = XtNameToWidget(dialog, "*.warp_input_2d_frame");
  cntrlForm = XtNameToWidget(dialog, "*.warp_input_2d_form");
  XtVaGetValues(shell, XmNheight, &shellHeight, NULL);
  XtVaGetValues(cntrlForm, XmNheight, &cntrlFormHeight, NULL);
  XtVaSetValues(dialog, XmNdefaultPosition, False, NULL);
  if( XtIsManaged(cntrlForm) ){
    wasManaged = True;
    XtUnmanageChild(cntrlForm);
    XtUnmanageChild(cntrlFrame);
    shellHeight -= cntrlFormHeight;
  }
  else {
    /* check if painting */
    if( paint_key ){
      HGU_XmUserError(view_struct->dialog,
		      "One of the view windows has been\n"
		      "selected for painting. Please quit\n"
		      "painting and try again.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      XtVaSetValues(widget, XmNset, False, NULL);
      return;
    }
    wasManaged = False;
    shellHeight += cntrlFormHeight;
  }

  /* re-manage everything and re-map the widget */
  XtVaSetValues(shell, XmNheight, shellHeight, NULL);
  if( wasManaged == False ){
    XtManageChild(cntrlForm);
  }
  XtManageChild(cntrlFrame);

  /* reset input callback and orientation controls sensitivity */
  XtSetSensitive(view_struct->controls, wasManaged);
  XtSetSensitive(view_struct->slider, wasManaged );
  setControlButtonsSensitive(view_struct, wasManaged);
  if( magFncForm = XtNameToWidget(dialog, "*.paint_function_row_col") ){
    XtSetSensitive(magFncForm, wasManaged);
  }

  if( wasManaged == False ){
    /* swap the callbacks to realignment mode */
    XtRemoveCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		     client_data);

    /* get the paint key */
    paint_key = view_struct;

    /* set the destination object and clear the ximage */
    if( warpGlobals.dst.obj ){
      WlzFreeObj(warpGlobals.dst.obj);
    }
    warpGlobals.dst.obj =
      WlzGetSectionFromObject(globals.orig_obj,
			      view_struct->wlzViewStr, NULL);
    if( warpGlobals.dst.ximage ){
      AlcFree(warpGlobals.dst.ximage->data);
      warpGlobals.dst.ximage->data = NULL;
      XDestroyImage(warpGlobals.dst.ximage);
      warpGlobals.dst.ximage = NULL;
    }
  }
  else {
    /* swap the callbacks to normal input mode */
    XtAddCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		  client_data);

    /* release the paint key */
    paint_key = NULL;
  }

  return;
}

static  void warp2DInteractDismissCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	toggle;
  XmToggleButtonCallbackStruct	cbs;

  /* get the warp controls toggle */
  if( toggle = XtNameToWidget(globals.topl, "*warp_input_2d_frame_title") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
    cbs.set = False;
    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
  }

  return;
}

static void warpDstCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  int		vtxIdx;
  WlzFVertex2	vtx;

  /* check there is and image */
  if( winStruct->ximage == NULL ){
    return;
  }

  switch( cbs->event->type ){
  case ButtonPress:
    vtx.vtX = cbs->event->xbutton.x;
    vtx.vtY = cbs->event->xbutton.y;

    switch( cbs->event->xbutton.button ){
    case Button1:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	/* set the dst vertex */
	warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	warpGlobals.tp_state = TP_DST_DEFINED;
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
	break;

      case TP_SELECTED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.dst),
			 warpGlobals.dst_vtxs[warpGlobals.sel_vtx]);
	warpGlobals.dst_vtxs[warpGlobals.sel_vtx] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
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
	break;
      }
      warpDisplayTiePoints();
      break;

    case Button3:
      break;
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

static void warpSrcCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  int		vtxIdx;
  WlzFVertex2	vtx;

  /* check there is and image */
  if( winStruct->ximage == NULL ){
    return;
  }

  switch( cbs->event->type ){
  case ButtonPress:
    vtx.vtX = cbs->event->xbutton.x;
    vtx.vtY = cbs->event->xbutton.y;

    switch( cbs->event->xbutton.button ){
    case Button1:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	/* set the src vertex */
	warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	warpGlobals.tp_state = TP_SRC_DEFINED;
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
	break;

      case TP_SELECTED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.src),
			 warpGlobals.src_vtxs[warpGlobals.sel_vtx]);
	warpGlobals.src_vtxs[warpGlobals.sel_vtx] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
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
	break;
      }
      warpDisplayTiePoints();
      break;

    case Button3:
      break;
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

void warpCanvasMotionEventHandler(
  Widget        w,
  XtPointer     client_data,
  XEvent        *event,
  Boolean       *continue_to_dispatch)
{
  XmDrawingAreaCallbackStruct cbs;

  /* call the canvas input callbacks */
  cbs.reason = XmCR_INPUT;
  cbs.event = event;
  cbs.window = XtWindow(w);
  XtCallCallbacks(w, XmNinputCallback, (XtPointer) &cbs);

  return;
}

static Widget createWarpDisplayFrame(
  Widget	parent,
  String	name,
  Visual	*visual,
  int		depth)
{
  Widget	frame, title, scrolled_window, canvas;
  Widget	button;

  /* create the frame and frame children */
  frame = XtVaCreateManagedWidget(name, xmFrameWidgetClass,
				  parent, NULL);
  title = XtVaCreateManagedWidget("title", xmRowColumnWidgetClass, frame,
				  XmNchildType, 	XmFRAME_TITLE_CHILD,
				  XmNorientation,	XmHORIZONTAL,
				  NULL);
  scrolled_window =
    XtVaCreateManagedWidget("scrolled_window", xmScrolledWindowWidgetClass,
			    frame,
			    XmNchildType, XmFRAME_WORKAREA_CHILD,
			    XmNscrollingPolicy, 	XmAUTOMATIC,
			    XmNborderWidth, 0,
			    NULL);

  /* create the buttons */
  button = XtVaCreateManagedWidget("b_1", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateManagedWidget("b_2", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateManagedWidget("b_3", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateManagedWidget("b_4", xmPushButtonWidgetClass,
				   title, NULL);

  /* create the canvas */
  canvas = XtVaCreateManagedWidget("canvas", hgu_DrawingAreaWidgetClass,
				   scrolled_window,
				   XmNwidth, 512,
				   XmNheight, 512,
				   XmNshadowThickness, 0,
				   XtNdepth,	depth,
				   XtNvisual,	visual,
				   NULL);

  return frame;
}

static ActionAreaItem   warp_interact_actions[] = {
{"reset",	NULL,		NULL},
{"dismiss",	NULL,           NULL},
{"help",	NULL,           NULL},
};

static Widget create2DWarpDialog(
  Widget parent,
  ThreeDViewStruct *view_struct)
{
  Widget	dialog, control, child, button;
  Visual	*visual;

  /* create a dialog widget and get control form */
  dialog = HGU_XmCreateStdDialog(parent, "warp2DInteractDialog",
				 xmFormWidgetClass,
				 warp_interact_actions, 3);
  control = XtNameToWidget( dialog, "*.control" );
  if( !(visual = HGU_XGetVisual(globals.dpy, DefaultScreen(globals.dpy),
			      TrueColor, 24)) ){
    visual = HGU_XmWidgetToVisual(globals.topl);
  }
  button = XtNameToWidget( dialog, "*.dismiss" );
  XtAddCallback(button, XmNactivateCallback, warp2DInteractDismissCb, NULL);
		

  /* add children */
  child = createWarpDisplayFrame(control, "warp_dst_frame", visual, 24);
  XtVaSetValues(child,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	0,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	33,
		NULL);
  warpGlobals.dst.canvas = XtNameToWidget(child, "*canvas");
  warpGlobals.dst.ximage = NULL;
  warpGlobals.dst.mag = 1.0;
  warpGlobals.dst.rot = 0;
  warpGlobals.dst.transpose = 0;

  /* canvas callbacks and event handler */
  XtAddCallback(warpGlobals.dst.canvas, XmNexposeCallback,
		warpCanvasExposeCb, (XtPointer) &(warpGlobals.dst));
  XtAddCallback(warpGlobals.dst.canvas, XmNinputCallback,
		warpDstCanvasInputCb, (XtPointer) &(warpGlobals.dst));
  XtAddEventHandler(warpGlobals.dst.canvas, 
		    PointerMotionMask|EnterWindowMask|LeaveWindowMask,
		    False, warpCanvasMotionEventHandler, NULL);

  /* button callbacks */
  button = XtNameToWidget(child, "*b_1");
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_2");
  XtAddCallback(button, XmNactivateCallback, warpCanvasRotCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_3");
  XtAddCallback(button, XmNactivateCallback, warpCanvasFlipCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_4");
  XtAddCallback(button, XmNactivateCallback, warpCanvasResetCb,
		(XtPointer) &(warpGlobals.dst));

  child = createWarpDisplayFrame(control, "warp_src_frame", visual, 24);
  XtVaSetValues(child,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	33,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	67,
		NULL);
  warpGlobals.src.canvas = XtNameToWidget(child, "*canvas");
  warpGlobals.src.ximage = NULL;
  warpGlobals.src.mag = 1.0;
  warpGlobals.src.rot = 0;
  warpGlobals.src.transpose = 0;

  /* canvas callbacks and event handler */
  XtAddCallback(warpGlobals.src.canvas, XmNexposeCallback,
		warpCanvasExposeCb, (XtPointer) &(warpGlobals.src));
  XtAddCallback(warpGlobals.src.canvas, XmNinputCallback,
		warpSrcCanvasInputCb, (XtPointer) &(warpGlobals.src));
  XtAddEventHandler(warpGlobals.src.canvas, 
		    PointerMotionMask|EnterWindowMask|LeaveWindowMask,
		    False, warpCanvasMotionEventHandler, NULL);

  /* button callbacks */
  button = XtNameToWidget(child, "*b_1");
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_2");
  XtAddCallback(button, XmNactivateCallback, warpCanvasRotCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_3");
  XtAddCallback(button, XmNactivateCallback, warpCanvasFlipCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_4");
  XtAddCallback(button, XmNactivateCallback, warpCanvasResetCb,
		(XtPointer) &(warpGlobals.src));

  child = createWarpDisplayFrame(control, "warp_ovly_frame", visual, 24);
  XtVaSetValues(child,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	67,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	100,
		NULL);
  warpGlobals.ovly.canvas = XtNameToWidget(child, "*canvas");
  warpGlobals.ovly.ximage = NULL;
  warpGlobals.ovly.mag = 1.0;
  warpGlobals.ovly.rot = 0;
  warpGlobals.ovly.transpose = 0;

  return dialog;
}

static ActionAreaItem   warp_controls_actions[] = {
{"src_read",	NULL,		NULL},
{"src_transf",	NULL,		NULL},
{"i_o",		NULL,           NULL},
{"import",	NULL,           NULL},
};

Widget createWarpInput2DDialog(
  Widget	topl)
{
  Widget	dialog=NULL;
  WlzDVertex3		fixed;
  ThreeDViewStruct	*view_struct;
  Widget	control, frame, form, title, controls_frame;
  Widget	option_menu, button, buttons, radio_box, label, widget;

  /* check for reference object */
  if( !globals.obj ){
    return NULL;
  }

  /* should call reset fixed point for this */
  fixed.vtX = (globals.obj->domain.p->kol1 + globals.obj->domain.p->lastkl)/2;
  fixed.vtY = (globals.obj->domain.p->line1 + globals.obj->domain.p->lastln)/2;
  fixed.vtZ = (globals.obj->domain.p->plane1 + globals.obj->domain.p->lastpl)/2;
  dialog = create_view_window_dialog(topl, 0.0, 0.0, &fixed);

  /* can use for painting  - but switch off when warping */
  XtVaGetValues(dialog, XmNuserData, &view_struct, NULL);
  view_struct->titleStr = "2D warp input dialog";

  /* reset the dialog title */
  set_view_dialog_title(dialog, 0.0, 0.0);

  /* get controls form and controls frame to add warping controls */
  control = XtNameToWidget( dialog, "*.control" );
  controls_frame = XtNameToWidget( control, "*.controls_frame");

  /* add a new frame */
  frame = XtVaCreateManagedWidget("warp_input_2d_frame",
				  xmFrameWidgetClass, control,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  XmNbottomAttachment,	XmATTACH_FORM,
				  NULL);
  form = XtVaCreateWidget("warp_input_2d_form",
			  xmFormWidgetClass, 	frame,
			  XmNleftAttachment,	XmATTACH_FORM,
			  XmNrightAttachment,	XmATTACH_FORM,
			  XmNtopAttachment,	XmATTACH_FORM,
			  XmNchildType, XmFRAME_WORKAREA_CHILD,
			  NULL);  
  title = XtVaCreateManagedWidget("warp_input_2d_frame_title",
				  xmToggleButtonGadgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  XtAddCallback(title, XmNvalueChangedCallback, warpControlsCb,
		view_struct);
  XtAddCallback(title, XmNvalueChangedCallback, warpSetupCb,
		view_struct);
  /* now the controls */

  /* now some buttons */
  warp_controls_actions[0].callback = warpReadSourcePopupCb;
  warp_controls_actions[0].client_data = view_struct;

  buttons = HGU_XmCreateWActionArea(form,
				    warp_controls_actions,
				    XtNumber(warp_controls_actions),
				    xmPushButtonWidgetClass);

  /* set the buttons attachments */
  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);


  /* modify attachments for the orientation controls frame */
  XtVaSetValues(controls_frame,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,	frame,
		NULL);

  /* add a callback to the destroy callback list */
  XtAddCallback(dialog, XmNdestroyCallback, warpInput2DDestroyCb,
		view_struct);

  /* set the interact dioalog to NULL and set up structure */
  warpGlobals.view_struct = view_struct;
  warpGlobals.warp2DInteractDialog = NULL;
  warpGlobals.dst.obj = NULL;
  warpGlobals.src.obj = NULL;
  warpGlobals.ovly.obj = NULL;
  warpGlobals.dst.gc = (GC) -1;
  warpGlobals.src.gc = (GC) -1;
  warpGlobals.ovly.gc = (GC) -1;

  warpGlobals.num_vtxs = 0;
  warpGlobals.sel_vtx = 0;
  warpGlobals.tp_state = TP_INACTIVE;
  warpGlobals.green_gc = (GC) -1;
  warpGlobals.red_gc = (GC) -1;

  return( dialog );
}
