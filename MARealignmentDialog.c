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
*   File       :   MARealignmentDialog.c				*
* $Revision$
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Sun Mar 14 16:43:06 1999				*
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

/* #include <Reconstruct.h>*/

extern Widget create_view_window_dialog(Widget topl,
					double theta,
					double phi,
					WlzDVertex3 *fixed);

extern DisplayBound(Display	*dpy,
		    Window	win,
		    GC		gc,
		    WlzBoundList	*bound);

void realignSetImage(ThreeDViewStruct *view_struct);
void realignDisplayPolysCb(Widget w, XtPointer client_data,
			   XtPointer call_data);
void realignDisplayOverlayCb(Widget w, XtPointer client_data,
			   XtPointer call_data);

/* realignment globals */
typedef enum {
  MAREALIGN_BOUNDARY_TYPE	= 0,
  MAREALIGN_DITHERED_TYPE,
  MAREALIGN_EDGE_TYPE
} MARealignOverlayType;

static int			alignBufferSize=100;
static XPoint			*srcPoly=NULL, *dstPoly=NULL, *tmpPoly=NULL;
static WlzObject		*origPaintedObject=NULL;
static WlzObject		*origRefObj=NULL, *origObj=NULL;
static WlzObject		*transformsObj=NULL;
static WlzObject		*overlayObj=NULL, *ovlyObj=NULL;
static MARealignOverlayType	overlayType=MAREALIGN_BOUNDARY_TYPE;
static Widget			realign_read_ovly_dialog = NULL;
static Widget			realign_write_ovly_dialog = NULL;
static int			x_shift=0, y_shift=0;
static double			x_scale=1.0, y_scale=1.0;
static double			theta=0.0;

void setOverlayTypeCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	canvas;

  if( client_data ){
    overlayType = (MARealignOverlayType) client_data;
  }

  if( paint_key->canvas ){
    XtCallCallbacks(paint_key->canvas, XmNexposeCallback, NULL);
  }

  return;
}

void realignDisplayOverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win = XtWindow(view_struct->canvas);
  GC			gc = globals.gc_set;
  WlzObject		*obj;
  WlzAffineTransform	*trans;

  /* work on the boundary only for now */
  /* the coordinates should be as required for the canvas window */
  if(view_struct && ovlyObj &&
     (ovlyObj->type == WLZ_BOUNDLIST)){
    if( trans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
					      0.0, 0.0, 0.0,
					      wlzViewStr->scale,
					      0.0, 0.0, 0.0, 0.0, 0.0,
					      0, NULL) ){
      if( obj = WlzAffineTransformObj(ovlyObj, trans,
				      WLZ_INTERPOLATION_NEAREST, NULL) ){
	
	(void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				     "blue");
	DisplayBound(dpy, win, gc, obj->domain.b);
	WlzFreeObj(obj);
      }
      WlzFreeAffineTransform(trans);
    }
  }

  return;
}

void realignSetOverlay(
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  int			dst_height, src_height, tx, ty;
  double		scale;
  WlzObject		*obj1=NULL, *obj2;
  WlzAffineTransform	*trans;
  double		spX, spY;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* regenerate the image to be displayed */
  /* rescale the original */
  if( overlayObj ){
    /* if the object is a domain object then re-scale and align
       if it is a boundary/polygon then don't bother - this is
       to allow the saved overlay to be read back in */
    switch( overlayObj->type ){
    case WLZ_2D_DOMAINOBJ:
      dst_height = view_struct->ximage->height;
      src_height = overlayObj->domain.i->lastln -
	overlayObj->domain.i->line1 + 1;
      scale = ((double) dst_height) / src_height;
      spX = (overlayObj->domain.i->lastkl + overlayObj->domain.i->kol1) / 2;
      spY = (overlayObj->domain.i->lastln + overlayObj->domain.i->line1) / 2;
      if( trans = WlzAffineTransformFromSpinSqueeze(spX, spY, 0.0,
						    scale, scale, &errNum) ){
	obj1 = WlzAffineTransformObj(overlayObj, trans,
				     WLZ_INTERPOLATION_NEAREST, &errNum);
	WlzFreeAffineTransform(trans);
      }

      /* translate to the centre */
      if( (errNum == WLZ_ERR_NONE) && obj1 ){
	obj1 = WlzAssignObject(obj1, NULL);
	tx = (view_struct->ximage->width - obj1->domain.i->lastkl -
	      obj1->domain.i->kol1) / 2 + x_shift;
	ty = (view_struct->ximage->height - obj1->domain.i->lastln -
	      obj1->domain.i->line1) / 2 + y_shift;
	if( obj2 = WlzShiftObject(obj1, tx, ty, 0, &errNum) ){
	  WlzFreeObj(obj1);
	  obj1 = WlzAssignObject(obj2, NULL);
	}
	else {
	  WlzFreeObj(obj1);
	  obj1 = NULL;
	}
      }

      /* convert to required type */
      if( (errNum == WLZ_ERR_NONE) && obj1 ){
	switch( overlayType ){
	default:
	case MAREALIGN_DITHERED_TYPE:
	case MAREALIGN_EDGE_TYPE:
	case MAREALIGN_BOUNDARY_TYPE:
	  obj2 = WlzObjToBoundary(obj1, 1, &errNum);
	  obj1 = WlzAssignObject(obj2, NULL);
	  break;
	}
      }
      break;

    case WLZ_2D_POLYGON:
    case WLZ_BOUNDLIST:
      obj1 = WlzAssignObject(overlayObj, NULL);
      break;

    }
  }

  /* generate the object to be displayed  - apply local scale,
     rotate and shift*/
  if( (errNum == WLZ_ERR_NONE) && obj1 ){
    /* first apply scaling and rotation around the view centre */
    spX = view_struct->ximage->width / 2;
    spY = view_struct->ximage->height / 2;
    if( trans = WlzAffineTransformFromSpinSqueeze(spX, spY, theta,
						  x_scale, y_scale,
						  &errNum) ){
      if( obj2 = WlzAffineTransformObj(obj1, trans,
				       WLZ_INTERPOLATION_NEAREST, &errNum)){
	WlzFreeAffineTransform(trans);
	WlzFreeObj(obj1);

	/* now apply the shift */
	obj1 = WlzAssignObject(WlzShiftObject(obj2, x_shift, y_shift,
					      0, &errNum), NULL);
	WlzFreeObj(obj2);
      }
    }
    if( ovlyObj ){
      WlzFreeObj(ovlyObj);
      ovlyObj = NULL;
    }
    if( (errNum == WLZ_ERR_NONE) && obj1 ){
      ovlyObj = WlzAssignObject(obj1, NULL);
      WlzFreeObj(obj1);
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "realignSetOverlay", errNum);
  }
  return;
}

void realignShiftLROverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  int	incr=0;

  switch( cbs->event->type ){
  case ButtonPress:
  case ButtonRelease:
    if( cbs->event->xbutton.state&ShiftMask ){
      incr = 5;
    }
    else {
      incr = 1;
    }
    if( cbs->event->xbutton.state&(Mod1Mask|Mod2Mask) ){
      incr = -incr;
    }
    break;
  case KeyPress:
  case KeyRelease:
    break;
  }

  x_shift += incr;

  realignSetImage(view_struct);
  realignDisplayPolysCb(w, client_data, call_data);
  realignSetOverlay(view_struct);
  realignDisplayOverlayCb(w, client_data, call_data);

  return;
}

void realignShiftUDOverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  int	incr=0;

  switch( cbs->event->type ){
  case ButtonPress:
  case ButtonRelease:
    if( cbs->event->xbutton.state&ShiftMask ){
      incr = 5;
    }
    else {
      incr = 1;
    }
    if( cbs->event->xbutton.state&(Mod1Mask|Mod2Mask) ){
      incr = -incr;
    }
    break;
  case KeyPress:
  case KeyRelease:
    break;
  }

  y_shift += incr;

  realignSetImage(view_struct);
  realignDisplayPolysCb(w, client_data, call_data);
  realignSetOverlay(view_struct);
  realignDisplayOverlayCb(w, client_data, call_data);

  return;
}

void realignScaleLROverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  double	incr=1.0;

  switch( cbs->event->type ){
  case ButtonPress:
  case ButtonRelease:
    if( cbs->event->xbutton.state&ShiftMask ){
      incr = 1.1;
    }
    else {
      incr = 1.01;
    }
    if( cbs->event->xbutton.state&(Mod1Mask|Mod2Mask) ){
      incr = 1.0 / incr;
    }
    break;
  case KeyPress:
  case KeyRelease:
    break;
  }

  x_scale *= incr;

  realignSetImage(view_struct);
  realignDisplayPolysCb(w, client_data, call_data);
  realignSetOverlay(view_struct);
  realignDisplayOverlayCb(w, client_data, call_data);

  return;
}

void realignScaleUDOverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  double	incr=1.0;

  switch( cbs->event->type ){
  case ButtonPress:
  case ButtonRelease:
    if( cbs->event->xbutton.state&ShiftMask ){
      incr = 1.1;
    }
    else {
      incr = 1.01;
    }
    if( cbs->event->xbutton.state&(Mod1Mask|Mod2Mask) ){
      incr = 1.0 / incr;
    }
    break;
  case KeyPress:
  case KeyRelease:
    break;
  }

  y_scale *= incr;

  realignSetImage(view_struct);
  realignDisplayPolysCb(w, client_data, call_data);
  realignSetOverlay(view_struct);
  realignDisplayOverlayCb(w, client_data, call_data);

  return;
}

void realignRotateOverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  double	incr=0.0;

  switch( cbs->event->type ){
  case ButtonPress:
  case ButtonRelease:
    if( cbs->event->xbutton.state&ShiftMask ){
      incr = 5.0;
    }
    else {
      incr = 1.0;
    }
    if( cbs->event->xbutton.state&(Mod1Mask|Mod2Mask) ){
      incr = -incr;
    }
    break;
  case KeyPress:
  case KeyRelease:
    break;
  }
  incr *= WLZ_M_PI / 180.0;

  theta += incr;

  realignSetImage(view_struct);
  realignDisplayPolysCb(w, client_data, call_data);
  realignSetOverlay(view_struct);
  realignDisplayOverlayCb(w, client_data, call_data);

  return;
}

void realignResetOverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;

  x_shift = 0;
  y_shift = 0;
  x_scale = 1.0;
  y_scale = 1.0;
  theta = 0.0;

  realignSetImage(view_struct);
  realignDisplayPolysCb(w, client_data, call_data);
  realignSetOverlay(view_struct);
  realignDisplayOverlayCb(w, client_data, call_data);

  return;
}

void realignDeleteOverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{

  if( overlayObj ){
    WlzFreeObj(overlayObj);
    overlayObj = NULL;
  }
  if( ovlyObj ){
    WlzFreeObj(ovlyObj);
    ovlyObj = NULL;
  }

  realignSetImage((ThreeDViewStruct *) client_data);
  realignDisplayPolysCb(w, client_data, call_data);

  return;
}

void realignReadOverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  String		fileStr;
  FILE			*fp;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  WlzObject		*obj;

  /* check we can open the file and save the filename */
  if( (fp = HGU_XmGetFilePointer(view_struct->dialog, cbs->value,
				 cbs->dir, "r")) == NULL ){
    return;
  }

  /* read the new source object - ready to extend to other types */
  obj = WlzEffReadObj(fp, NULL, WLZEFF_FORMAT_WLZ, 0, &errNum);
  fclose(fp);

  /* check the object */
  if( errNum == WLZ_ERR_NONE ){
    if( obj ){
      switch( obj->type ){
      case WLZ_2D_DOMAINOBJ:
      case WLZ_2D_POLYGON:
      case WLZ_BOUNDLIST:
	/* set the overlay object */
	if( overlayObj ){
	  WlzFreeObj(overlayObj);
	}
	overlayObj = WlzAssignObject(obj, &errNum);
	realignSetImage(view_struct);
	realignDisplayPolysCb(w, client_data, call_data);
	realignSetOverlay(view_struct);
	realignDisplayOverlayCb(w, client_data, call_data);
	/* don't reset on re-read */
/*	realignResetOverlayCb(w, client_data, call_data);*/
	break;

      default:
	WlzFreeObj(obj);
	HGU_XmUserError(globals.topl,
			"Read Realign Overlay Object:\n"
			"    Wrong object type, please select\n"
			"    another image file.",
			XmDIALOG_FULL_APPLICATION_MODAL);
	break;
      }
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "realignReadOverlayCb", errNum);
  }
  return;
}

void realignReadOverlayPopupCb(
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

  if( realign_read_ovly_dialog == NULL ){
    realign_read_ovly_dialog =
      XmCreateFileSelectionDialog(view_struct->dialog,
				  "realign_read_ovly_dialog", arg, 1);

    XtAddCallback(realign_read_ovly_dialog, XmNokCallback,
		  realignReadOverlayCb, client_data);
    XtAddCallback(realign_read_ovly_dialog, XmNokCallback,
		  PopdownCallback, NULL);
    XtAddCallback(realign_read_ovly_dialog, XmNcancelCallback, 
		  PopdownCallback, NULL);
    XtAddCallback(realign_read_ovly_dialog, XmNmapCallback,
		  FSBPopupCallback, NULL);
  }

  XtManageChild(realign_read_ovly_dialog);
  PopupCallback(w, (XtPointer) XtParent(realign_read_ovly_dialog), NULL);

  return;
}

void realignWriteOverlayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  String		fileStr;
  FILE			*fp;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  WlzObject		*obj;

  /* check we can open the file and save the filename */
  if( (fp = HGU_XmGetFilePointer(view_struct->dialog, cbs->value,
				 cbs->dir, "w")) == NULL ){
    return;
  }

  /* write the overlay object */
  if( ovlyObj ){
    errNum = WlzWriteObj(fp, ovlyObj);
  }
  fclose(fp);

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "realignWriteOverlayCb", errNum);
  }
  return;
}

void realignWriteOverlayPopupCb(
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

  if( realign_write_ovly_dialog == NULL ){
    realign_write_ovly_dialog =
      XmCreateFileSelectionDialog(view_struct->dialog,
				  "realign_write_ovly_dialog", arg, 1);

    XtAddCallback(realign_write_ovly_dialog, XmNokCallback,
		  realignWriteOverlayCb, client_data);
    XtAddCallback(realign_write_ovly_dialog, XmNokCallback,
		  PopdownCallback, NULL);
    XtAddCallback(realign_write_ovly_dialog, XmNcancelCallback, 
		  PopdownCallback, NULL);
    XtAddCallback(realign_write_ovly_dialog, XmNmapCallback,
		  FSBPopupCallback, NULL);
  }

  XtManageChild(realign_write_ovly_dialog);
  PopupCallback(w, (XtPointer) XtParent(realign_write_ovly_dialog), NULL);

  return;
}

void realignmentCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	dialog, widget;

  if( dialog = createRealignmentDialog(globals.topl) ){
    if( widget = XtNameToWidget(globals.topl, "*options_menu*realignment") ){
      XtSetSensitive(widget, False);
    }
    XtManageChild(dialog);
  }

  return;
}

void realignmentDestroyCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget		widget;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  if( origRefObj ){
    WlzFreeObj(origRefObj);
    origRefObj = NULL;
  }
  if( origObj ){
    WlzFreeObj(origObj);
    origObj = NULL;
  }
  if( origPaintedObject ){
    WlzFreeObj(origPaintedObject);
    origPaintedObject = NULL;
  }

  if( srcPoly ){
    AlcFree((void *) srcPoly);
    srcPoly = NULL;
  }
  if( dstPoly ){
    AlcFree((void *) dstPoly);
    dstPoly = NULL;
  }

  if( transformsObj ){
    WlzFreeObj(transformsObj);
    transformsObj = NULL;
  }

  if( widget = XtNameToWidget(globals.topl, "*options_menu*realignment") ){
    XtSetSensitive(widget, True);
  }
  
  if( paint_key == view_struct ){
    paint_key = NULL;
  }
  
  return;
}

void realignSetImage(
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  int		width, height, oldWidth;
  int		i, offset;

  /* check offsets for each line and set the image */
  width = view_struct->ximage->width;
  oldWidth = origPaintedObject->values.r->width;
  height = view_struct->ximage->height;
  memset((void *) view_struct->ximage->data, 0, width*height);
  for(i=0; i < height; i++){
    offset = alignBufferSize + dstPoly[i].x - srcPoly[i].x;
    memcpy((void *) (view_struct->ximage->data + i * width + offset),
	   (void *) (origPaintedObject->values.r->values.ubp + i * oldWidth),
	   WLZ_MIN(oldWidth, width - offset));
  }

  return;
}

void realignDisplayPolysCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win = XtWindow(view_struct->canvas);
  GC			gc = globals.gc_set;
  int			i;

  /* re-display the image */
  redisplay_view_cb(w, client_data, call_data);

  /* need to check for scaling */
  if( wlzViewStr->scale > 1.0 ){
    for(i=0; i < view_struct->ximage->height; i++){
      tmpPoly[i].x = srcPoly[i].x * wlzViewStr->scale;
      tmpPoly[i].y = srcPoly[i].y * wlzViewStr->scale;
    }
    (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				 "red");
    XDrawLines(dpy, win, gc, tmpPoly, view_struct->ximage->height,
	       CoordModeOrigin);

    for(i=0; i < view_struct->ximage->height; i++){
      tmpPoly[i].x = dstPoly[i].x * wlzViewStr->scale;
      tmpPoly[i].y = dstPoly[i].y * wlzViewStr->scale;
    }
    (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				 "green");
    XDrawLines(dpy, win, gc, tmpPoly, view_struct->ximage->height,
	       CoordModeOrigin);
  }
  else {
    (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				 "red");
    XDrawLines(dpy, win, gc, srcPoly, view_struct->ximage->height,
	       CoordModeOrigin);

    (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				 "green");
    XDrawLines(dpy, win, gc, dstPoly, view_struct->ximage->height,
	       CoordModeOrigin);
  }

  XFlush(dpy);
  
  return;
}

void resetRealignPolyCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  Widget		widget;
  Boolean		srcPolySet;
  int			i;

  if( widget = XtNameToWidget(view_struct->dialog,
			      "*.src_dest_select.src_poly" ) ){
    XtVaGetValues(widget, XmNset, &srcPolySet, NULL);
  }
  else {
    return;
  }

  if( srcPolySet ){
    for(i=0; i < view_struct->ximage->height; i++){
      srcPoly[i].x = view_struct->ximage->width/2;
      dstPoly[i].x = srcPoly[i].x;
    }
  }
  else {
    for(i=0; i < view_struct->ximage->height; i++){
      dstPoly[i].x = srcPoly[i].x;
    }
  }

  realignSetImage(view_struct);
  realignDisplayPolysCb(w, client_data, call_data);
  realignDisplayOverlayCb(w, client_data, call_data);
  return;
}

WlzObject *WlzAffineTransformObj3D(
  WlzObject	*obj,
  WlzObject	*transObj,
  WlzErrorNum	*dstErr)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*rtnObj=NULL;
  WlzDomain	domain;
  WlzValues	values;
  int		p, nPlanes;
  WlzObject	*obj1, *obj2;

  /* check the objects */
  if( (obj == NULL) || (transObj == NULL) ){
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  if((obj->type != WLZ_3D_DOMAINOBJ) || 
     (transObj->type != WLZ_3D_DOMAINOBJ)){
    errNum = WLZ_ERR_OBJECT_TYPE;
  }
  if((obj->domain.p == NULL) ||
     (transObj->domain.p == NULL)){
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  if((obj->domain.p->type != WLZ_PLANEDOMAIN_DOMAIN) ||
     (transObj->domain.p->type != WLZ_PLANEDOMAIN_AFFINE)){
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }

  /* make the new object */
  if( errNum == WLZ_ERR_NONE ){
    if( domain.p = WlzMakePlaneDomain(obj->domain.p->type,
				      obj->domain.p->plane1,
				      obj->domain.p->lastpl,
				      obj->domain.p->line1,
				      obj->domain.p->lastln,
				      obj->domain.p->kol1,
				      obj->domain.p->lastkl,
				      &errNum) ){
      for(p=0; p < 3; p++){
	domain.p->voxel_size[p] = obj->domain.p->voxel_size[p];
      }

      if( obj->values.core ){
	values.vox = WlzMakeVoxelValueTb(obj->values.vox->type,
					 obj->values.vox->plane1,
					 obj->values.vox->lastpl,
					 obj->values.vox->bckgrnd,
					 NULL, &errNum);
      }
      else {
	values.vox = NULL;
      }
      rtnObj = WlzMakeMain(origRefObj->type, domain, values,
			 NULL, NULL, &errNum);
    }
  }

  /* apply the transform plane by plane */
  if( errNum == WLZ_ERR_NONE ){
    nPlanes = obj->domain.p->lastpl - obj->domain.p->plane1 + 1;
    for(p=0; p < nPlanes; p++){
      /* check if the reference object has empty domains */
      if((obj->domain.p->domains[p].core == NULL) ||
	 (obj->domain.p->domains[p].core->type == WLZ_EMPTY_OBJ) ||
	 (obj->domain.p->domains[p].core->type == WLZ_EMPTY_DOMAIN)){
	rtnObj->domain.p->domains[p].core = NULL;
	continue;
      }
      
      /* get the plane to be transformed */
      if( obj->values.core ){
	obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			   obj->domain.p->domains[p],
			   obj->values.vox->values[p],
			   NULL, NULL, NULL);
      }
      else {
	obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			   obj->domain.p->domains[p],
			   obj->values,
			   NULL, NULL, NULL);
      }
      obj1 = WlzAssignObject(obj1, NULL);

      /* now transform it */
      if(((p + obj->domain.p->plane1) >= transObj->domain.p->plane1) &&
	 ((p + obj->domain.p->plane1) <= transObj->domain.p->lastpl) &&
	 (transObj->domain.p->domains[p]).t ){
	obj2 = WlzAffineTransformObj(obj1,
				     (transObj->domain.p->domains[p]).t,
				     WLZ_INTERPOLATION_NEAREST, NULL);
	rtnObj->domain.p->domains[p] = WlzAssignDomain(obj2->domain, NULL);
	rtnObj->values.vox->values[p] = WlzAssignValues(obj2->values, NULL);
	WlzFreeObj(obj2);
      }
      else {
	rtnObj->domain.p->domains[p] = WlzAssignDomain(obj1->domain, NULL);
	rtnObj->values.vox->values[p] = WlzAssignValues(obj1->values, NULL);
      }
      WlzFreeObj(obj1);
    }
  }

  /* standardise the plane domain and voxel table */
  if( errNum == WLZ_ERR_NONE ){
    WlzStandardPlaneDomain(rtnObj->domain.p, rtnObj->values.vox);
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

WlzObject *WlzMakeTransformObj3D(
  WlzObject	*obj,
  WlzErrorNum	*dstErr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  WlzDomain	domain;
  WlzValues	values;
  WlzObject	*rtnObj=NULL;
  int		i;

  if( domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_AFFINE,
				    obj->domain.p->plane1,
				    obj->domain.p->lastpl,
				    obj->domain.p->line1,
				    obj->domain.p->lastln,
				    obj->domain.p->kol1,
				    obj->domain.p->lastkl,
				    &errNum) ){
    for(i=0; i < 3; i++){
      domain.p->voxel_size[i] = obj->domain.p->voxel_size[i];
    }
    values.core = NULL;
    if( !(rtnObj = WlzMakeMain(WLZ_3D_DOMAINOBJ,
			       domain, values, NULL, NULL, &errNum)) ){
      WlzFreeDomain(domain);
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

static void realignResetTransformsObj(void)
{
  int		i, p;

  /* set all transforms to the unit transform */
  if( transformsObj ){
    for(p=transformsObj->domain.p->plane1, i=0;
	p <=  transformsObj->domain.p->lastpl;
	p++, i++){
      if( (transformsObj->domain.p->domains[i]).t ){
	WlzFreeAffineTransform((transformsObj->domain.p->domains[i]).t);
      }
      (transformsObj->domain.p->domains[i]).t = 
	WlzAssignAffineTransform(
	  WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
					0.0, 0.0, 0.0, 1.0,
					0.0, 0.0, 0.0, 0.0, 0.0,
					0, NULL),
	  NULL);
    }
  }
  
  return;
}

static void realignUpdateTransformsObj(
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  int		i, p, nPlanes = view_struct->ximage->height;
  int		xp, yp;
  int		origWidth;
  double	x, y;
  double	kol1, kol2, line1, line2;
  int		plane1, plane2;
  WlzAffineTransform	*newTrans, *tmpTrans;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* fill transforms as required */
  for(i=0; i < nPlanes; i++){
    /* get true coordinates and build the new transform */
    origWidth = WLZ_NINT(wlzViewStr->maxvals.vtX) -
      WLZ_NINT(wlzViewStr->minvals.vtX) + 1;
    x = srcPoly[i].x - alignBufferSize;
    xp = WLZ_NINT(x);
    xp = WLZ_MAX(0, xp);
    xp = WLZ_MIN(xp, origWidth);
    yp = i;
    kol1 = wlzViewStr->xp_to_x[xp] + wlzViewStr->yp_to_x[yp];
    line1 = wlzViewStr->xp_to_y[xp] + wlzViewStr->yp_to_y[yp];
    x = wlzViewStr->xp_to_z[xp] + wlzViewStr->yp_to_z[yp];
    plane1 = WLZ_NINT(x);

    x = dstPoly[i].x - alignBufferSize;
    xp = WLZ_NINT(x);
    xp = WLZ_MAX(0, xp);
    xp = WLZ_MIN(xp, origWidth);
    kol2 = wlzViewStr->xp_to_x[xp] + wlzViewStr->yp_to_x[yp];
    line2 = wlzViewStr->xp_to_y[xp] + wlzViewStr->yp_to_y[yp];
    x = wlzViewStr->xp_to_z[xp] + wlzViewStr->yp_to_z[yp];
    plane2 = WLZ_NINT(x);

    if( plane1 != plane2 ){
      HGU_XmUserError(view_struct->dialog,
		      "Something wrong here, original planes\n"
		      "not coming out equal. Please check the\n"
		      "pitch angle which should be 90 degrees",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      return;
    }
    if( newTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
					      kol2 - kol1,
					      line2 - line1,
					      0.0, 1.0, 0.0, 0.0,
					      0.0, 0.0, 0.0, 0, &errNum)){

      /* apply to existing transform */
      p = plane1 - transformsObj->domain.p->plane1;
      if( (transformsObj->domain.p->domains[p]).t ){
	if( tmpTrans = WlzAffineTransformProduct((transformsObj->
						  domain.p->domains[p]).t,
						 newTrans, &errNum) ){

	  WlzFreeAffineTransform((transformsObj->domain.p->domains[p]).t);
	  WlzFreeAffineTransform(newTrans);
	  (transformsObj->domain.p->domains[p]).t =
	    WlzAssignAffineTransform(tmpTrans, NULL);
	}
	else {
	  break;
	}
      }
      else {
	(transformsObj->domain.p->domains[p]).t =
	  WlzAssignAffineTransform(newTrans, NULL);
      }
    }
    else {
      break;
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "realignUpdateTransformsObj", errNum);
  }
  return;
}

static int transformsUpdatedFlg=0;

void applyRealignTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmPushButtonCallbackStruct	*cbs = 
    (XmPushButtonCallbackStruct *) call_data;
  WlzObject		*newOrigObj, *newObj;
  Widget		widget;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check for transforms obj */
  if( transformsObj == NULL ){
    HGU_XmUserError(view_struct->dialog,
		    "Something wrong here. Please\n"
		    "dismiss this realignment dialog\n"
		    "and try again.",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* check we really want to do this */
  if( !HGU_XmUserConfirm(view_struct->dialog,
			 "Do you really want to transform\n"
			 "the whole 3D stack?", "Yes", "No", 1) ){
    return;
  }

  /* check the view is valid for this operation: pitch == 90 */
  if( fabs(wlzViewStr->phi - WLZ_M_PI_2) > 1.0e-2 ){
    HGU_XmUserError(view_struct->dialog,
		    "Invalid pitch value. For this\n"
		    "operation please set pitch = 90.0\n"
		    "and redefine the alignment",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* set hour glass */
  HGU_XmSetHourGlassCursor(globals.topl);
  HGU_XmSetHourGlassCursor(XtParent(view_struct->dialog));

  /* set up a transform object and update with current */
  if( !transformsUpdatedFlg ){
    realignUpdateTransformsObj(view_struct);
  }

  /* make a new reference object and painted volume */
  if( newOrigObj = WlzAffineTransformObj3D(origRefObj, transformsObj, &errNum) ){
    if( !(newObj = WlzAffineTransformObj3D(origObj, transformsObj, &errNum)) ){
      WlzFreeObj(newOrigObj);
      newOrigObj = NULL;
    }
  }

  /* install the new reference object and painted image */
  if( errNum == WLZ_ERR_NONE ){
    if( globals.orig_obj != NULL ){
      WlzFreeObj( globals.orig_obj );
    }
    globals.orig_obj = WlzAssignObject(newOrigObj, NULL);

    if( globals.obj != NULL ){
      WlzFreeObj( globals.obj );
    }
    globals.obj = WlzAssignObject(newObj, NULL);
  
    /* reset  displays */
    setup_ref_display_list_cb(NULL, NULL, NULL);
    setup_obj_props_cb(NULL, NULL, NULL);
    
    /* reset realignment controls */
    if( widget = XtNameToWidget(view_struct->dialog,
				"*.realignment_frame_title") ){
      XmToggleButtonCallbackStruct tmpcbs;

      XtVaSetValues(widget, XmNset, False, NULL);
      tmpcbs.set = False;
      tmpcbs.event = cbs->event;
      XtCallCallbacks(widget, XmNvalueChangedCallback, (XtPointer) &tmpcbs);
    }
  }

  /* unset hour glass */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "applyRealignTransCb", errNum);
  }
  return;
}

void undoRealignTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget		widget;
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmPushButtonCallbackStruct	*cbs = 
    (XmPushButtonCallbackStruct *) call_data;

  /* install the new reference object and painted image */
  if( globals.orig_obj != NULL ){
    WlzFreeObj( globals.orig_obj );
  }
  globals.orig_obj = WlzAssignObject(origRefObj, NULL);

  if( globals.obj != NULL ){
    WlzFreeObj( globals.obj );
  }
  globals.obj = WlzAssignObject(origObj, NULL);
  
  /* reset  displays */
  setup_ref_display_list_cb(NULL, NULL, NULL);
  setup_obj_props_cb(NULL, NULL, NULL);

  /* reset realignment controls */
  if( widget = XtNameToWidget(view_struct->dialog,
			      "*.realignment_frame_title") ){
    XmToggleButtonCallbackStruct tmpcbs;

    XtVaSetValues(widget, XmNset, False, NULL);
    tmpcbs.set = False;
    tmpcbs.event = cbs->event;
    XtCallCallbacks(widget, XmNvalueChangedCallback, (XtPointer) &tmpcbs);
  }

  /* reset the transform object */
  realignResetTransformsObj();

  return;
}

void writeRealignTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  String		fileStr;
  FILE			*fp;
  int			i, p;
  BibFileRecord	*record = NULL;
  BibFileField	*field0 = NULL,
		*field1 = NULL,
		*field2 = NULL;
  char		tmpBuf[256], *eMsg;
  char		*tmpS,
		*idxS = NULL,
		*dateS = NULL,
		*hostS = NULL,
		*userS = NULL,
                *fileS = NULL;
  char 		tTypeS[32],
		tTxS[32],
		tTyS[32],
		tTzS[32],
		tScaleS[32],
		tThetaS[32],
		tPhiS[32],
		tAlphaS[32],
		tPsiS[32],
		tXsiS[32],
		tInvertS[32];
  static char	unknownS[] = "unknown";
  time_t	tmpTime;

  /* check the view is valid for this operation: pitch == 90 */
  if( fabs(wlzViewStr->phi - WLZ_M_PI_2) > 1.0e-2 ){
    HGU_XmUserError(view_struct->dialog,
		    "Invalid pitch value. For this\n"
		    "operation please set pitch = 90.0\n"
		    "and redefine the alignment",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* get a bib-file filename */
  if( fileStr =
     HGU_XmUserGetFilename(globals.topl,
			   "Please type in a filename\n"
			   "for the transforms bib-file\n",
			   "OK", "cancel", "MAPaintRealign.bib",
			   NULL, "*.bib") ){
    if( (fp = fopen(fileStr, "w")) == NULL ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to open the bib-file. Please\n"
		      "check the filename and permissions.\n",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      AlcFree((void *) fileStr);
      return;
    }
  }
  else {
    return;
  }
  AlcFree((void *) fileStr);

  /* set hour glass */
  HGU_XmSetHourGlassCursor(globals.topl);
  HGU_XmSetHourGlassCursor(XtParent(view_struct->dialog));

  /* set up a transform object and update with current */
  if( transformsObj == NULL ){
    if( !(transformsObj =
	  WlzAssignObject(WlzMakeTransformObj3D(origObj, NULL), NULL)) ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to allocate memory for this\n"
		      "operation. Probably you need to restart\n"
		      "MAPaint and try again or make a smaller\n"
		      "reference image",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      return;
    }
  }
  if( !transformsUpdatedFlg ){
    realignUpdateTransformsObj(view_struct);
  }

  /* run through the transform list writing a bibfile for each
     plane. Use correct plane numbers but do not define a
     file name (since its not known)
     Note the relative transform from the previous section is used.
  */

  /* an identifier record - who, when, how */
  if((((field0 = BibFileFieldMakeVa("Text", "MAPaint realignment section file",
				    "Version", "1",
				    "TransformType", "absolute",
				    NULL)) == NULL) ||
      ((record = BibFileRecordMake("Ident", "0", field0)) == NULL)))
  {
    HGU_XmUnsetHourGlassCursor(globals.topl);
    HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));
    fclose(fp);
    return;
  }
  BibFileRecordWrite(fp, &eMsg, record);
  BibFileRecordFree(&record);

  tmpS = getenv("USER");
  (void )sprintf(tmpBuf, "User: %s", tmpS?tmpS:unknownS);
  userS = AlcStrDup(tmpBuf);

  tmpTime = time(NULL);
  tmpS = ctime(&tmpTime);
  *(tmpS + strlen(tmpS) - 1) = '\0';
  (void )sprintf(tmpBuf, "Date: %s", tmpS?tmpS:unknownS);
  dateS = AlcStrDup(tmpBuf);

  tmpS = getenv("HOST");
  (void )sprintf(tmpBuf, "Host: %s", tmpS?tmpS:unknownS);
  hostS = AlcStrDup(tmpBuf);

  (void )sprintf(tmpBuf, "File: %s", globals.file?globals.file:unknownS);
  fileS = AlcStrDup(tmpBuf);

  if((((field0 = BibFileFieldMakeVa("Text", userS,
				    "Text", dateS,
				    "Text", hostS,
				    "Text", fileS,
				    NULL)) == NULL) ||
       ((record = BibFileRecordMake("Comment", "0", field0)) == NULL)))
  {
    HGU_XmUnsetHourGlassCursor(globals.topl);
    HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));
    fclose(fp);
    return;
  }
  BibFileRecordWrite(fp, &eMsg, record);
  BibFileRecordFree(&record);
  AlcFree(dateS);
  AlcFree(hostS);
  AlcFree(userS);

  /* now the section records */
  if( transformsObj ){
    for(p=transformsObj->domain.p->plane1, i=0;
	p <= transformsObj->domain.p->lastpl; p++, i++){
      WlzAffineTransform    *transf=transformsObj->domain.p->domains[i].t;
      WlzAffineTransformPrim	tPrim;

      if( transf == NULL ){
	continue;
      }
      if( WlzAffineTransformPrimGet(transf, &tPrim) != WLZ_ERR_NONE ){
	continue;
      }
      sprintf(tmpBuf, "%d", p);
      idxS = AlcStrDup(tmpBuf);

      (void )sprintf(tTypeS, "%d", transf->type);
      (void )sprintf(tTxS, "%g", tPrim.tx);
      (void )sprintf(tTyS, "%g", tPrim.ty);
      (void )sprintf(tTzS, "%g", tPrim.tz);
      (void )sprintf(tScaleS, "%g", tPrim.scale);
      (void )sprintf(tThetaS, "%g", tPrim.theta);
      (void )sprintf(tPhiS, "%g", tPrim.phi);
      (void )sprintf(tAlphaS, "%g", tPrim.alpha);
      (void )sprintf(tPsiS, "%g", tPrim.psi);
      (void )sprintf(tXsiS, "%g", tPrim.xsi);
      (void )sprintf(tInvertS, "%d", tPrim.invert);
      field0 = BibFileFieldMakeVa(
	"TransformType", tTypeS,
	"TransformTx", tTxS,
	"TransformTy", tTyS,
	"TransformTz", tTzS,
	"TransformScale", tScaleS,
	"TransformTheta", tThetaS,
	"TransformPhi", tPhiS,
	"TransformAlpha", tAlphaS,
	"TransformPsi", tPsiS,
	"TransformXsi", tXsiS,
	"TransformInvert", tInvertS,
	NULL);

      record = BibFileRecordMake("Section", idxS, field0);
      BibFileRecordWrite(fp, &eMsg, record);
      BibFileRecordFree(&record);
      AlcFree(idxS);
    }
  }
  fclose(fp);


  /* unset hour glass */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));

  /* check for apply transform */
  if( HGU_XmUserConfirm(view_struct->dialog,
			"Apply this transform?\n",
			"Yes", "No", 1) ){
    transformsUpdatedFlg = 1;
    applyRealignTransCb(w, client_data, call_data);
    transformsUpdatedFlg = 0;
  }
  else {
    realignResetTransformsObj();
  }

  return;
}

void readRealignTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmPushButtonCallbackStruct	*cbs = 
    (XmPushButtonCallbackStruct *) call_data;
  String		fileStr;
  FILE			*fp;
  int			i, p;
  char			*errMsg;
  int			numParsedFields=0;
  BibFileRecord		*bibfileRecord;
  BibFileField		*bibfileField;
  BibFileError		bibFileErr=BIBFILE_ER_NONE;
  double		tx=0.0, ty=0.0, theta=0.0;
  WlzAffineTransform	*inTrans, *tmpTrans;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* get a bib-file filename */
  if( fileStr =
     HGU_XmUserGetFilename(globals.topl,
			   "Please type in a filename\n"
			   "for the transforms bib-file\n",
			   "OK", "cancel", "MAPaintRealign.bib",
			   NULL, "*.bib") ){
    if( (fp = fopen(fileStr, "r")) == NULL ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to open the bib-file. Please\n"
		      "check the filename and permissions.\n",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      AlcFree((void *) fileStr);
      return;
    }
  }
  else {
    return;
  }
  AlcFree((void *) fileStr);

  /* set hour glass */
  HGU_XmSetHourGlassCursor(globals.topl);
  HGU_XmSetHourGlassCursor(XtParent(view_struct->dialog));

  /* set up a transform object and update with current */
  if( transformsObj == NULL ){
    if( !(transformsObj =
	  WlzAssignObject(WlzMakeTransformObj3D(origObj, NULL), NULL)) ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to allocate memory for this\n"
		      "operation. Probably you need to restart\n"
		      "MAPaint and try again or make a smaller\n"
		      "reference image",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      return;
    }
  }

  /* check for transform on top of current or to replace it */
  if( HGU_XmUserConfirm(view_struct->dialog,
			"Do you want the transforms\n"
			"read in to overwrite the current\n"
			"transform (i.e. the current line"
			"offsets) or to extend the current\n"
			"transforms?",
			"Overwrite", "Extend", 1) ){
    realignResetTransformsObj();
  }
  else {
    realignUpdateTransformsObj(view_struct);
  }

  /* now read the bibfile applying any transform with a matching
     plane number to the current transform.
     In this version assume the bibfile is from MAPaint therefore a 
     set of absolute transforms */
  bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
  while((errNum == WLZ_ERR_NONE) && (bibFileErr == BIBFILE_ER_NONE)) 
  {
    /* should check what type of bibfile here */
    if( strncmp(bibfileRecord->name, "Section", 7) ){
      BibFileRecordFree(&bibfileRecord);
    }
    else {
      /* check if this corresponds to a plane in the reference object */
      p = atoi(bibfileRecord->id);
      if((p < transformsObj->domain.p->plane1) ||
	 (p > transformsObj->domain.p->lastpl)){
	BibFileRecordFree(&bibfileRecord);
	BibFileRecordRead(&bibfileRecord, &errMsg, fp);
	continue;
      }
      p -= transformsObj->domain.p->plane1;

      /* parse the record */
      numParsedFields = BibFileFieldParseFmt
	(bibfileRecord->field,
	 (void *) &tx, "%lg", "TransformTx",
	 (void *) &ty, "%lg", "TransformTy",
	 (void *) &theta, "%lg", "TransformTheta",
	 NULL);

      /* make the transform for this record */
      if( inTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
					       tx, ty, 0.0, 1.0,
					       theta, 0.0, 0.0, 0.0, 0.0, 0,
					       &errNum) ){

	/* if concatenate with the existing transforms */
	if( transformsObj->domain.p->domains[p].t ){
	  if( tmpTrans =
	     WlzAffineTransformProduct(transformsObj->domain.p->domains[p].t,
				       inTrans, &errNum) ){
	    WlzFreeAffineTransform(transformsObj->domain.p->domains[p].t);
	    WlzFreeAffineTransform(inTrans);
	    transformsObj->domain.p->domains[p].t =
	      WlzAssignAffineTransform(tmpTrans, NULL);
	  }
	}	
	else {
	  transformsObj->domain.p->domains[p].t =
	    WlzAssignAffineTransform(inTrans, NULL);
	}
      }
      BibFileRecordFree(&bibfileRecord);
    }
    bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
  }
  fclose(fp);

  /* now apply the transform read in - this is the only way to get the
     image to change since resetting the source and destination lines might
     be too painful because each transform would have to projected into
     translations othogonal and parallel to the current view. */
  if( errNum == WLZ_ERR_NONE ){
    HGU_XmUnsetHourGlassCursor(globals.topl);
    HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));
    transformsUpdatedFlg = 1;
    applyRealignTransCb(w, client_data, call_data);
    transformsUpdatedFlg = 0;
  }
  else {
    MAPaintReportWlzError(globals.topl, "readRealignTransCb", errNum);
  }
  return;
}

static int	lastX, lastY;
static int	lineModeFlg;
static int	setSrcPolyFlg;

void realignment_input_cb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y, origWidth, domain;
  int			i;
  Widget		widget;
  Boolean		toggleSet;
  Display		*dpy=XtDisplay(w);
  Window		win=XtWindow(w);

  /* switch on event type */
  switch( cbs->event->type ){

   case ButtonPress:
     switch( cbs->event->xbutton.button ){

      case Button1:
	x = cbs->event->xbutton.x / wlzViewStr->scale;
	y = cbs->event->xbutton.y / wlzViewStr->scale;
	if( (y < 0) || (y >= view_struct->ximage->height) ){
	  break;
	}

	/* establish the modes */
	lineModeFlg = 0;
	if( widget = XtNameToWidget(view_struct->dialog,
				    "*.straight_line") ){
	  XtVaGetValues(widget, XmNset, &toggleSet, NULL);
	  if( toggleSet ){
	    lineModeFlg = 1;
	  }
	}
	setSrcPolyFlg = 0;
	if( widget = XtNameToWidget(view_struct->dialog,
				    "*.src_poly") ){
	  XtVaGetValues(widget, XmNset, &toggleSet, NULL);
	  if( toggleSet ){
	    setSrcPolyFlg = 1;
	  }
	}

	dstPoly[y].x = x;
	if( setSrcPolyFlg ){
	  srcPoly[y].x = x;
	}
	realignSetImage(view_struct);
	realignDisplayPolysCb(w, client_data, call_data);
	realignDisplayOverlayCb(w, client_data, call_data);
	lastX = x;
	lastY = y;
	break;

      case Button2:
	break;

      case Button3:		/* unused */
      default:
	break;

     }
     break;

   case ButtonRelease:
     switch( cbs->event->xbutton.button ){

      case Button1:
	break;

      case Button2:
	x = cbs->event->xbutton.x / wlzViewStr->scale;
	y = cbs->event->xbutton.y / wlzViewStr->scale;
	domain = imageValueToDomain
	  ((unsigned int) *((UBYTE *) (view_struct->ximage->data + x +
				       y * view_struct->ximage->bytes_per_line)));
	origWidth = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
	x -= alignBufferSize;
	x = WLZ_MAX(0, x);
	x = WLZ_MIN(x, origWidth);
	display_pointer_feedback_informationV(view_struct, x, y, domain);
	break;

      default:
	break;

     }
     break;

  case MotionNotify:
    if( cbs->event->xmotion.state & Button1Mask ){
      x = cbs->event->xmotion.x / wlzViewStr->scale;
      y = cbs->event->xmotion.y / wlzViewStr->scale;
      if( y == lastY ){
	dstPoly[y].x = x;
	if( setSrcPolyFlg ){
	  srcPoly[y].x = dstPoly[y].x;
	}
      }
      else {
	if( (y < 0) || (y >= view_struct->ximage->height) ){
	  break;
	}
	i=lastY;
	while( i != y ){
	  i += (lastY < y) ? 1 : -1;
	  dstPoly[i].x = ((y-i)*lastX*1024 + (i-lastY)*x*1024)/(y-lastY)/1024;
	  if( setSrcPolyFlg ){
	    srcPoly[i].x = dstPoly[i].x;
	  }
	}
      }
      realignSetImage(view_struct);
      realignDisplayPolysCb(w, client_data, call_data);
      realignDisplayOverlayCb(w, client_data, call_data);
      if( !lineModeFlg ){
	lastX = x;
	lastY = y;
      }
    }

    if( cbs->event->xmotion.state & Button2Mask )
     {
	x = cbs->event->xmotion.x / wlzViewStr->scale;
	y = cbs->event->xmotion.y / wlzViewStr->scale;
	domain = imageValueToDomain
	  ((unsigned int) *((UBYTE *) (view_struct->ximage->data + x +
				       y * view_struct->ximage->bytes_per_line)));
	origWidth = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
	x -= alignBufferSize;
	x = WLZ_MAX(0, x);
	x = WLZ_MIN(x, origWidth);
	display_pointer_feedback_informationV(view_struct, x, y, domain);
     }
     break;

  case KeyPress:
    x = cbs->event->xkey.x;
    y = cbs->event->xkey.y;
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      XWarpPointer(dpy, win, win, x, y, 1, 1, x+1, y);
      x += 1;
      break;

    case XK_Up:
    case XK_p:
      XWarpPointer(dpy, win, win, x, y, 1, 1, x, y-1);
      y -= 1;
      break;

    case XK_Left:
    case XK_b:
      XWarpPointer(dpy, win, win, x, y, 1, 1, x-1, y);
      x -= 1;
      break;

    case XK_Down:
    case XK_n:
      XWarpPointer(dpy, win, win, x, y, 1, 1, x, y+1);
      y += 1;
      break;

    }
    cbs->event->type = MotionNotify;
    cbs->event->xmotion.state = Button1Mask;
    cbs->event->xmotion.x = x;
    cbs->event->xmotion.y = y;
    realignment_input_cb(w, client_data, call_data);
    break;

  default:
    break;
  }

  return;
}

static void realignment_setup_cb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmToggleButtonCallbackStruct 
    *cbs=(XmToggleButtonCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  WlzObject	*obj;
  WlzIBox2	newSize;
  int		width, height;
  int		i;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check if painting */
  if( paint_key && (paint_key != view_struct) ){
    return;
  }

  /* check the view is valid for this operation: pitch == 90 */
  if( fabs(wlzViewStr->phi - WLZ_M_PI_2) > 1.0e-2 ){
    return;
  }

  if( cbs->set == True ){
    /* install a new painted_image and redisplay */
    newSize.xMin = view_struct->painted_object->domain.i->kol1 -
      alignBufferSize;
    newSize.xMax = view_struct->painted_object->domain.i->lastkl +
      alignBufferSize;
    newSize.yMin = view_struct->painted_object->domain.i->line1;
    newSize.yMax = view_struct->painted_object->domain.i->lastln;
    width = newSize.xMax - newSize.xMin + 1;
    height = newSize.yMax - newSize.yMin + 1;
    obj = WlzCutObjToBox2D(view_struct->painted_object, newSize,
			   WLZ_GREY_UBYTE, 0, 0.0, 0.0, &errNum);
    origPaintedObject = WlzAssignObject(view_struct->painted_object, NULL);
    WlzFreeObj(view_struct->painted_object);
    view_struct->painted_object = WlzAssignObject(obj, NULL);

    view_struct->ximage->width = width;
    view_struct->ximage->height = height;
    view_struct->ximage->bytes_per_line = width;
    view_struct->ximage->data = (char *) obj->values.r->values.ubp;
    XtVaSetValues(view_struct->canvas,
		  XmNwidth, (Dimension) (width*wlzViewStr->scale),
		  XmNheight, (Dimension) (height*wlzViewStr->scale),
		  NULL);

    /* set default source polyline */
    srcPoly = (XPoint *) AlcMalloc(sizeof(XPoint) * height);
    for(i=0; i < height; i++){
      srcPoly[i].x = width/2;
      srcPoly[i].y = i;
    }

    /* set default destination polyline */
    dstPoly = (XPoint *) AlcMalloc(sizeof(XPoint) * height);
    for(i=0; i < height; i++){
      dstPoly[i].x = width/2;
      dstPoly[i].y = i;
    }
    tmpPoly = (XPoint *) AlcMalloc(sizeof(XPoint) * height);

    /* set the image, display the polyline and redisplay */
    realignSetImage(view_struct);
    realignDisplayPolysCb(view_struct->canvas, client_data, call_data);
    realignSetOverlay(view_struct);
    realignDisplayOverlayCb(view_struct->canvas, client_data, call_data);

    XtAddCallback(view_struct->canvas, XmNexposeCallback,
		  realignDisplayPolysCb, view_struct);
    XtAddCallback(view_struct->canvas, XmNexposeCallback,
		  realignDisplayOverlayCb, view_struct);
  }
  else {
    /* clear stored data */
    XtRemoveCallback(view_struct->canvas, XmNexposeCallback,
		     realignDisplayPolysCb, view_struct);
    XtRemoveCallback(view_struct->canvas, XmNexposeCallback,
		     realignDisplayOverlayCb, view_struct);
    reset_view_struct(view_struct);
    display_view_cb(widget, client_data, call_data);
    AlcFree((void *) srcPoly);
    AlcFree((void *) dstPoly);
    AlcFree((void *) tmpPoly);
    srcPoly = dstPoly = tmpPoly = NULL;
    WlzFreeObj(origPaintedObject);
    origPaintedObject = NULL;
  }
  return;
}

static void realignment_controls_cb(
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
  cntrlFrame = XtNameToWidget(dialog, "*.realignment_frame");
  cntrlForm = XtNameToWidget(dialog, "*.realignment_form");
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

    /* check the view is valid for this operation: pitch == 90 */
    if( fabs(wlzViewStr->phi - WLZ_M_PI_2) > 1.0e-2 ){
      HGU_XmUserError(view_struct->dialog,
		      "Invalid pitch value. For this\n"
		      "operation please set pitch = 90.0\n"
		      "and redefine the alignment",
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
    XtAddCallback(view_struct->canvas, XmNinputCallback, realignment_input_cb,
		  client_data);

    /* get the paint key */
    paint_key = view_struct;
  }
  else {
    /* swap the callbacks to normal input mode */
    XtRemoveCallback(view_struct->canvas, XmNinputCallback, realignment_input_cb,
		     client_data);
    XtAddCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		  client_data);

    /* release the paint key */
    paint_key = NULL;
  }

  return;
}

static ActionAreaItem   realign_controls_actions[] = {
{"reset",	NULL,		NULL},
{"apply",	NULL,		NULL},
{"undo",	NULL,		NULL},
{"read",	NULL,           NULL},
{"write",	NULL,           NULL},
};

static MenuItem poly_menu_itemsP[] = {		/* poly_menu items */
  {"1", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
/*  {"2", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"3", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"4", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},*/
  NULL,
};

static MenuItem overlay_type_itemsP[] = {		/* poly_menu items */
  {"boundary", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   setOverlayTypeCb, (XtPointer) MAREALIGN_BOUNDARY_TYPE,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"dithered", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   setOverlayTypeCb, (XtPointer) MAREALIGN_DITHERED_TYPE,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"edge", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   setOverlayTypeCb, (XtPointer) MAREALIGN_EDGE_TYPE,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

Widget createRealignmentDialog(
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
  dialog = create_view_window_dialog(topl, 0.0, WLZ_M_PI/2, &fixed);

  /* prevent use for painting */
  XtVaGetValues(dialog, XmNuserData, &view_struct, NULL);
  view_struct->noPaintingFlag = 1;
  view_struct->titleStr = "Realigment dialog";

  /* reset the dialog title */
  set_view_dialog_title(dialog, 0.0, WLZ_M_PI/2);

  /* get controls form and controls frame to add realignment controls */
  control = XtNameToWidget( dialog, "*.control" );
  controls_frame = XtNameToWidget( control, "*.controls_frame");

  /* add a new frame */
  frame = XtVaCreateManagedWidget("realignment_frame",
				  xmFrameWidgetClass, control,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  XmNbottomAttachment,	XmATTACH_FORM,
				  NULL);
  form = XtVaCreateWidget("realignment_form",
			  xmFormWidgetClass, 	frame,
			  XmNleftAttachment,	XmATTACH_FORM,
			  XmNrightAttachment,	XmATTACH_FORM,
			  XmNtopAttachment,	XmATTACH_FORM,
			  XmNchildType, XmFRAME_WORKAREA_CHILD,
			  NULL);  
  title = XtVaCreateManagedWidget("realignment_frame_title",
				  xmToggleButtonGadgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  XtAddCallback(title, XmNvalueChangedCallback, realignment_controls_cb,
		view_struct);
  XtAddCallback(title, XmNvalueChangedCallback, realignment_setup_cb,
		view_struct);

  /* now the polyline controls */
  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "poly_select", 0,
				XmTEAR_OFF_DISABLED, poly_menu_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  widget = option_menu;
  XtManageChild(widget);

  radio_box = XmCreateRadioBox(form, "src_dest_select", NULL, 0);
  XtVaSetValues(radio_box,
		XmNorientation, XmHORIZONTAL,
		XmNtopAttachment,  XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, widget,
		XmNspacing, 0,
		XmNborderWidth,	0,
		XmNmarginHeight, 0,
		XmNmarginWidth, 0,
		XmNpacking, XmPACK_TIGHT,
		NULL);
  button = XtVaCreateManagedWidget("src_poly",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   NULL);
  button = XtVaCreateManagedWidget("dst_poly",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   XmNset, True,
				   NULL);
  XtVaSetValues(radio_box, XmNmenuHistory, button, NULL);
  widget = radio_box;
  XtManageChild(widget);

  label = XtVaCreateManagedWidget("poly_mode_select_label",
				  xmLabelWidgetClass, form,
				  XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
				  XmNtopWidget, widget,
				  XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
				  XmNbottomWidget, widget,
				  XmNleftAttachment, XmATTACH_WIDGET,
				  XmNleftWidget, widget,
				  XmNborderWidth,	0,
				  NULL);
  widget = label;

  radio_box = XmCreateRadioBox(form, "poly_mode_select", NULL, 0);
  XtVaSetValues(radio_box,
		XmNorientation, XmHORIZONTAL,
		XmNtopAttachment,  XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, widget,
		XmNspacing, 0,
		XmNborderWidth,	0,
		XmNmarginHeight, 0,
		XmNmarginWidth, 0,
		XmNpacking, XmPACK_TIGHT,
		NULL);
  button = XtVaCreateManagedWidget("polyline",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   XmNset, True,
				   NULL);
  XtVaSetValues(radio_box, XmNmenuHistory, button, NULL);
  button = XtVaCreateManagedWidget("straight_line",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   NULL);
  widget = radio_box;
  XtManageChild(widget);

  /* now the overlay controls */
  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "overlay_select", 0,
				XmTEAR_OFF_DISABLED, overlay_type_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);

  button = XtVaCreateManagedWidget("overlay_input",
				   xmPushButtonWidgetClass, form,
				   XmNtopAttachment,XmATTACH_OPPOSITE_WIDGET,
				   XmNtopWidget,	option_menu,
				   XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
				   XmNbottomWidget,	option_menu,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	option_menu,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignReadOverlayPopupCb,
		view_struct);

  button = XtVaCreateManagedWidget("overlay_write",
				   xmPushButtonWidgetClass, form,
				   XmNtopAttachment,XmATTACH_OPPOSITE_WIDGET,
				   XmNtopWidget,	option_menu,
				   XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
				   XmNbottomWidget,	option_menu,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	button,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignWriteOverlayPopupCb,
		view_struct);
  widget = option_menu;

  /* buttons to adjust the overlay */
  radio_box = XmCreateRowColumn(form, "overlay_adjust_rc", NULL, 0);
  XtVaSetValues(radio_box,
		XmNorientation, XmHORIZONTAL,
		XmNtopAttachment,  XmATTACH_WIDGET,
		XmNtopWidget, widget,
		XmNleftAttachment, XmATTACH_FORM,
		XmNpacking, XmPACK_TIGHT,
		NULL);
  label = XtVaCreateManagedWidget("overlay_adjust_label",
				  xmLabelWidgetClass, radio_box,
				  XmNborderWidth,	0,
				  NULL);
  button = XtVaCreateManagedWidget("shift_left_right",
				   xmPushButtonWidgetClass, radio_box,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignShiftLROverlayCb,
		view_struct);
  button = XtVaCreateManagedWidget("shift_up_down",
				   xmPushButtonWidgetClass, radio_box,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignShiftUDOverlayCb,
		view_struct);
  button = XtVaCreateManagedWidget("scale_left_right",
				   xmPushButtonWidgetClass, radio_box,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignScaleLROverlayCb,
		view_struct);
  button = XtVaCreateManagedWidget("scale_up_down",
				   xmPushButtonWidgetClass, radio_box,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignScaleUDOverlayCb,
		view_struct);
  button = XtVaCreateManagedWidget("rotate",
				   xmPushButtonWidgetClass, radio_box,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignRotateOverlayCb,
		view_struct);
  button = XtVaCreateManagedWidget("overlay_reset",
				   xmPushButtonWidgetClass, radio_box,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignResetOverlayCb,
		view_struct);
  button = XtVaCreateManagedWidget("overlay_delete",
				   xmPushButtonWidgetClass, radio_box,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, realignDeleteOverlayCb,
		view_struct);
  XtManageChild(radio_box);
  widget = radio_box;

  /* now some buttons */
  realign_controls_actions[0].callback = resetRealignPolyCb;
  realign_controls_actions[0].client_data = view_struct;
  realign_controls_actions[1].callback = applyRealignTransCb;
  realign_controls_actions[1].client_data = view_struct;
  realign_controls_actions[2].callback = undoRealignTransCb;
  realign_controls_actions[2].client_data = view_struct;
  realign_controls_actions[3].callback = readRealignTransCb;
  realign_controls_actions[3].client_data = view_struct;
  realign_controls_actions[4].callback = writeRealignTransCb;
  realign_controls_actions[4].client_data = view_struct;
  buttons = HGU_XmCreateWActionArea(form,
				    realign_controls_actions,
				    XtNumber(realign_controls_actions),
				    xmDrawnButtonWidgetClass);

  /* set the buttons attachments */
  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  /* modify attachments for the orientation controls frame */
  XtVaSetValues(controls_frame,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,	frame,
		NULL);

  /* copy the original reference object and painted volume
     and add a callback to the destroy callback list */
  origRefObj = WlzAssignObject(globals.orig_obj, NULL);
  origObj = WlzAssignObject(globals.obj, NULL);
  XtAddCallback(dialog, XmNdestroyCallback, realignmentDestroyCb,
		view_struct);

  /* create a transforms object with the identity transform
     - this is the current product of all alignment operations */
  if( !(transformsObj =
	WlzAssignObject(WlzMakeTransformObj3D(origObj, NULL), NULL)) ){
    HGU_XmUserError(view_struct->dialog,
		    "Failed to allocate memory for this\n"
		    "operation. Probably you need to restart\n"
		    "MAPaint and try again or make a smaller\n"
		    "reference image",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    realignmentDestroyCb(dialog, (XtPointer) view_struct, NULL);
    return NULL;
  }
  realignResetTransformsObj();
  transformsUpdatedFlg = 0;

  return( dialog );
}
