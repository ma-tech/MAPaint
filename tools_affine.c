#pragma ident "MRC HGU $Id$"
/*****************************************************************************
* Copyright   :    1994 Medical Research Council, UK.                        *
*                  All rights reserved.                                      *
******************************************************************************
* Address     :    MRC Human Genetics Unit,                                  *
*                  Western General Hospital,                                 *
*                  Edinburgh, EH4 2XU, UK.                                   *
******************************************************************************
* Project     :    Mouse Atlas Project					     *
* File        :    tools_affine.c					     *
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Fri Oct  4 19:04:44 1996				     *
* Synopsis    : 							     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

typedef enum {
  HGU_XTRANSMODE_TRANSLATE,
  HGU_XTRANSMODE_ROTATE,
  HGU_XTRANSMODE_SCALE,
  HGU_XTRANSMODE_SHEAR,
  HGU_XTRANSMODE_LAST
} HGU_XTransModeType;

static Widget			translate_w, rotate_w, scale_w, shear_w;
static WlzObject		*origObj=NULL, *origBoundary, *lastBoundary;
static DomainSelection		origDomain;
static int			origX, origY;
static HGU_XTransModeType	transMode;
static GC			affineGc=NULL;
static WlzAffineTransform	*scaleTrans, *lastTrans;
static   int			transformAllFlg;


static void nextTransMode(
  Display	*dpy,
  Window	win)
{
  Boolean		mode_set; 

  transMode = (transMode + 1) % HGU_XTRANSMODE_LAST;
  switch( transMode ){

  case HGU_XTRANSMODE_ROTATE:
    XtVaGetValues(rotate_w, XmNset, &mode_set, NULL);
    if( mode_set ){
      HGU_XSetCursor( dpy, win, HGU_XCURSOR_ROTATE );
      break;
    }
    transMode = (transMode + 1) % HGU_XTRANSMODE_LAST;

  case HGU_XTRANSMODE_SCALE:
    XtVaGetValues(scale_w, XmNset, &mode_set, NULL);
    if( mode_set ){
      HGU_XSetCursor( dpy, win, HGU_XCURSOR_SCALE );
      break;
    }
    transMode = (transMode + 1) % HGU_XTRANSMODE_LAST;

  case HGU_XTRANSMODE_SHEAR:
    XtVaGetValues(shear_w, XmNset, &mode_set, NULL);
    if( mode_set ){
      HGU_XSetCursor( dpy, win, HGU_XCURSOR_SHEAR );
      break;
    }
    transMode = (transMode + 1) % HGU_XTRANSMODE_LAST;

  default:
  case HGU_XTRANSMODE_TRANSLATE:
    HGU_XSetCursor( dpy, win, HGU_XCURSOR_TRANSLATE );
    break;

  }
}

void MAPaintAffine2DInit(
  ThreeDViewStruct	*view_struct)
{
  /* check original object */
  if( origObj ){
    WlzFreeObj(origObj);
    origObj = NULL;
  }

  /* set the start mode */
  transMode = HGU_XTRANSMODE_TRANSLATE;
  transformAllFlg = 0;

  /* set the cursor */
  HGU_XSetCursor(XtDisplay(view_struct->canvas),
		 XtWindow(view_struct->canvas),
		 HGU_XCURSOR_TRANSLATE);

  /* get the graphics context */
  if( !affineGc ){
    affineGc = HGU_XGetGC(XtDisplay(view_struct->canvas),
			  XtWindow(view_struct->canvas),
			  HGU_XGC_INTERACT);
  }

  return;
}

void MAPaintAffine2DQuit(
  ThreeDViewStruct	*view_struct)
{
  /* unset the cursor */
  XUndefineCursor(XtDisplay(view_struct->canvas),
		  XtWindow(view_struct->canvas));

  /* free the original object if left loafing */
  if( origObj ){
    WlzFreeObj(origObj);
    origObj = NULL;
  }
  transformAllFlg = 0;

  return;
}

#define MAXXPOINTS	1024
static XPoint	xpoints[MAXXPOINTS];

void DisplayPoly(
  Display		*dpy,
  Window		win,
  GC			gc,
  WlzPolygonDomain	*poly)
{
  WlzIVertex2	*ivtxs;
  int		i;

  /* check the polygon */
  if( poly == NULL ){
    return;
  }

  /* copy to an X line list */
  ivtxs = poly->vtx;
  for(i=0; (i < poly->nvertices) && (i < MAXXPOINTS); i++, ivtxs++){
    xpoints[i].x = ivtxs->vtX;
    xpoints[i].y = ivtxs->vtY;
  }

  /* display the polyline */
  XDrawLines(dpy, win, gc, xpoints, i, CoordModeOrigin);
  XFlush( dpy );
  return;
}

void DisplayBound(
  Display	*dpy,
  Window	win,
  GC		gc,
  WlzBoundList	*bound)
{
  /* check boundary */
  if( bound == NULL ){
    return;
  }

  /* display the polyline */
  DisplayPoly(dpy, win, gc, bound->poly);

  /* display the next bound elements */
  DisplayBound(dpy, win, gc, bound->next);

  /* display the down bound elements */
  DisplayBound(dpy, win, gc, bound->down);

  return;
}

void MAPaintAffine2DCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			delX, delY;
  WlzObject		*obj, *boundary;
  Display		*dpy=XtDisplay(w);
  Window		win=XtWindow(w);
  WlzAffineTransform	*trans, *trans1;
  double		theta, scale, tx, ty;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
    case Button2:
      /* do nothing if shift pressed - magnify option */
      if((cbs->event->xbutton.state & ShiftMask)){
	break;
      }

      /* do nothing if control pressed - toggle affine mode 
	 note if alt also pressed it implies select all domains */
      if((cbs->event->xbutton.state & ControlMask) &&
	 !(cbs->event->xbutton.state & (Mod1Mask|Mod2Mask)) )
      {
	nextTransMode(dpy, win);
	break;
      }

      /* save the original coordinates */
      origX = cbs->event->xbutton.x;
      origY = cbs->event->xbutton.y;

      /* select domain logic:
	 button press within a domain: select that part of the domain
	 <alt> button press within a domain: select whole domain
	 button press outside of a domain: no selection
	 <alt> button press outside of a domain: select all of current_domain
	 <shift><alt> button 1 press: select ALL domains for transform, as
	 above for selection of regions to provide transform feedback.
	 <alt> button 1 == button 2
	 */
	 
      /* check if outside of any domain */
      if((origDomain = getSelectedDomainType(origX, origY, view_struct)) < 1){
	origDomain = globals.current_domain;
	if((cbs->event->xbutton.button == Button2) ||
	   (cbs->event->xbutton.state & (Mod1Mask|Mod2Mask))){
	  origObj = get_domain_from_object(view_struct->painted_object,
					 origDomain);
	}
	else {
	  origObj = NULL;
	}
      }
      else {	/* must be inside a domain */
	/* get the selected part of the domain - note need to mark
	   how to get the whole domain.
	   This object acts as the trigger for the button motion */
	if((cbs->event->xbutton.button == Button2) ||
	   (cbs->event->xbutton.state & (Mod1Mask|Mod2Mask))){
	  origObj = get_domain_from_object(view_struct->painted_object,
					   origDomain);
	}
	else {
	  origObj = getSelectedRegion(origX, origY, view_struct);
	}
      }

      /* check this again for good measure! */
      if( origObj == NULL){
	break;
      }

      /* test if ALL domains are to be transformed */
      if((cbs->event->xbutton.state & (Mod1Mask|Mod2Mask)) &&
	 (cbs->event->xbutton.state & ControlMask)){
	transformAllFlg = 1;
      }

      origObj = WlzAssignObject(origObj, NULL);
      if( (obj = WlzObjToBoundary(origObj, 1, &errNum)) == NULL ){
	WlzFreeObj(origObj);
	origObj = NULL;
	break;
      }
      origBoundary = 
	WlzShiftObject(obj,
		       -(view_struct->painted_object->domain.i->kol1),
		       -(view_struct->painted_object->domain.i->line1),
		       0, &errNum);
      WlzFreeObj(obj);
      if( origBoundary ){
	origBoundary = WlzAssignObject(origBoundary, NULL);
	if(scaleTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
						   0.0, 0.0, 0.0,
						   wlzViewStr->scale,
						   0.0, 0.0, 0.0, 0.0, 0.0,
						   0, &errNum)){
	  lastTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
						 0.0, 0.0, 0.0, 1.0,
						 0.0, 0.0, 0.0, 0.0, 0.0,
						 0, &errNum);
	}
	if( errNum == WLZ_ERR_NONE ){
	  lastBoundary = WlzAffineTransformObj(origBoundary, scaleTrans,
					       WLZ_INTERPOLATION_NEAREST,
					       &errNum);
	}
	else {
	  lastBoundary = NULL;
	}
	if( lastBoundary ){
	  DisplayBound(dpy, win, affineGc, lastBoundary->domain.b);
	}
      }
      break;

    default:
      break;

    }
    break;

  case ButtonRelease:
    if( origObj ){
      /* transform the original object and increment the domain */
      DisplayBound(dpy, win, affineGc, lastBoundary->domain.b);
      trans = WlzAffineTransformFromPrimVal
	(WLZ_TRANSFORM_2D_AFFINE,
	 (double) -(view_struct->painted_object->domain.i->kol1),
	 (double) -(view_struct->painted_object->domain.i->line1),
	 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, NULL);
      trans1 = WlzAffineTransformProduct(trans, lastTrans, NULL);
      WlzFreeAffineTransform(lastTrans);
      lastTrans = WlzAffineTransformInverse(trans, NULL);
      WlzFreeAffineTransform(trans);
      trans = WlzAffineTransformProduct(trans1, lastTrans, NULL);
      
      if( transformAllFlg ){
	/* transform the painted object, extract the domains and 
	   increment each one */
	if( obj = WlzAffineTransformObj(view_struct->painted_object,
					trans, WLZ_INTERPOLATION_NEAREST,
					&errNum) ){
	  int		numOverlays, i;
	  WlzObject	*tmpObj;

	  obj = WlzAssignObject(obj, NULL);
	  setGreyValuesFromObject(view_struct->painted_object,
				  obj);

	  /* push onto undo stack and increment*/
	  pushUndoDomains(view_struct);

	  /* clear painted object to greys */
	  if( view_struct->view_object == NULL ){
	    if( tmpObj = WlzGetSectionFromObject(globals.orig_obj,
						 view_struct->wlzViewStr,
						 &errNum) ){
	      view_struct->view_object = WlzAssignObject(tmpObj, NULL);
	    }
	    else {
	      break;
	    }
	  }
	  setGreyValuesFromObject(view_struct->painted_object,
				  view_struct->view_object);

	  /* get domains one by one and set current */
	  numOverlays = globals.cmapstruct->num_overlays +
	    globals.cmapstruct->num_solid_overlays;
	  for(i=1; i <= numOverlays; i++){
	    if(tmpObj = get_domain_from_object(obj,
					       (DomainSelection) i)){
	      if((view_struct->curr_domain[i]) &&
		 (view_struct->curr_domain[i]->type != WLZ_EMPTY_OBJ)){
		WlzFreeObj(view_struct->curr_domain[i]);
		view_struct->curr_domain[i] = WlzAssignObject(tmpObj, NULL);
	      }
	      else {
		continue;
	      }

	      /* set the new domain */
	      if( i > globals.cmapstruct->num_overlays )
	      {
		set_grey_values_from_domain(view_struct->curr_domain[i],
					    view_struct->painted_object,
					    globals.cmapstruct->ovly_cols[i],
					    255);
	      }
	      else
	      {
		set_grey_values_from_domain(view_struct->curr_domain[i],
					    view_struct->painted_object,
					    globals.cmapstruct->ovly_cols[i],
					    globals.cmapstruct->ovly_planes);
	      }
	    }
	  }
	  
	  /* re-display */
	  redisplay_view_cb(view_struct->canvas, (XtPointer) view_struct,
			    NULL);
	  transformAllFlg = 0;
	  WlzFreeObj(obj);
	  obj = NULL;
	}
      }
      else {
	if( obj = WlzAffineTransformObj(origObj, trans,
					WLZ_INTERPOLATION_NEAREST, &errNum) ){
	  /* push onto undo stack and increment*/
	  pushUndoDomains(view_struct);
	  setDomainIncrement(origObj, view_struct, origDomain, 1);
	  setDomainIncrement(obj, view_struct, origDomain, 0);
	  WlzFreeObj(obj);
	  obj = NULL;
	}
      }

      /* free the various saved objects */
      if(origObj){WlzFreeObj(origObj);}
      origObj = NULL;
      if(origBoundary){WlzFreeObj(origBoundary);}
      if(lastBoundary){WlzFreeObj(lastBoundary);}
      if(trans){WlzFreeAffineTransform(trans);}
      if(trans1){WlzFreeAffineTransform(trans1);}
      if(lastTrans){WlzFreeAffineTransform(lastTrans);}
      if(scaleTrans){WlzFreeAffineTransform(scaleTrans);}
    }
    break;

  case MotionNotify:
    /* note the origObj acts as a trigger
       and either button 1 or 2 pressed */
    if((origObj) &&
       (cbs->event->xmotion.state & (Button1Mask|Button2Mask)) ){
      switch( transMode ){

      default:
      case HGU_XTRANSMODE_TRANSLATE:
	delX = cbs->event->xmotion.x - origX;
	delY = cbs->event->xmotion.y - origY;
	delX /= wlzViewStr->scale;
	delY /= wlzViewStr->scale;
	WlzFreeAffineTransform(lastTrans);
	lastTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
					       (double) delX, (double) delY,
					       0.0,
					       1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
					       0, &errNum);
	break;

      case HGU_XTRANSMODE_ROTATE:
	delX = cbs->event->xmotion.x - origX;
	delY = cbs->event->xmotion.y - origY;
	theta = atan2( (double) delY, (double) delX );
	WlzFreeAffineTransform(lastTrans);
	lastTrans =
	  WlzAffineTransformFromSpin((double) origX / wlzViewStr->scale,
				     (double) origY / wlzViewStr->scale,
				     theta, &errNum);
	break;

      case HGU_XTRANSMODE_SCALE:
	delX = cbs->event->xmotion.x - origX;
	scale = 1.0 + (double) abs(delX) / 10.0;
	if( delX < 0 ){
	  scale = 1.0 / scale;
	}
	theta = 0.0;
	lastTrans =
	  WlzAffineTransformFromSpinSqueeze
	  ((double) origX / wlzViewStr->scale,
	   (double) origY / wlzViewStr->scale,
	   theta, scale, scale, &errNum);
	break;

      case HGU_XTRANSMODE_SHEAR:
	delX = cbs->event->xmotion.x - origX;
	delY = cbs->event->xmotion.y - origY;
	scale = sqrt( (double) (delX*delX + delY*delY) );
	scale /= 50.0;
	if( scale < 0.02 ){
	  scale = 0.0;
	  theta = 0.0;
	}
	else {
	  theta = atan2((double) delY, (double) delX);
	}

	tx = (scale*sin(theta)*cos(theta)*origX -
	      scale*cos(theta)*cos(theta)*origY)/wlzViewStr->scale;
	ty = (scale*sin(theta)*sin(theta)*origX -
	      scale*sin(theta)*cos(theta)*origY)/wlzViewStr->scale;
	lastTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
					       tx, ty, 0.0,
					       1.0, 0.0, 0.0,
					       scale, theta, 0.0,
					       0, &errNum);
	break;
      }

      /* un display the last boundary, calculate and 
	 display the new boundary */
      if( errNum == WLZ_ERR_NONE ){
	if( trans1 = WlzAffineTransformProduct(lastTrans, scaleTrans, &errNum) ){
	  if( boundary = WlzAffineTransformObj(origBoundary, trans1,
					       WLZ_INTERPOLATION_NEAREST,
					       &errNum) ){
	    DisplayBound(dpy, win, affineGc, lastBoundary->domain.b);
	    DisplayBound(dpy, win, affineGc, boundary->domain.b);
	    WlzFreeObj(lastBoundary);
	    lastBoundary = boundary;
	  }
	  WlzFreeAffineTransform(trans1);
	}
      }
    }
    break;

  case KeyPress:
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      /* if <alt> or <alt gr> pressed then change tool option */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	nextTransMode(dpy, win);
      }
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_b:
      /* if <alt> or <alt gr> pressed then change tool option */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	nextTransMode(dpy, win);
      }
      break;

    case XK_Down:
    case XK_n:
      break;
    }
    break;

  default:
    break;
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "MAPaintAffine2DCb", errNum);
  }
  return;
}

Widget	CreateAffineControls(
  Widget	parent)
{
  Widget	form, form1, label, frame, widget;

  /* create a parent form to hold all the tracking controls */
  form = XtVaCreateWidget("affine_controls_form", xmFormWidgetClass,
			  parent,
			  XmNtopAttachment,     XmATTACH_FORM,
			  XmNbottomAttachment,	XmATTACH_FORM,
			  XmNleftAttachment,    XmATTACH_FORM,
			  XmNrightAttachment,  	XmATTACH_FORM,
			  NULL);

  /* create frame, form and label for the tracking parameters */
  frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass,
				  form,
				  XmNtopAttachment,     XmATTACH_FORM,
				  XmNleftAttachment,    XmATTACH_FORM,
				  XmNrightAttachment,  	XmATTACH_FORM,
				  NULL);

  form1 = XtVaCreateWidget("affine_rc", xmRowColumnWidgetClass,
			   frame,
			   XmNtopAttachment,    XmATTACH_FORM,
			   XmNbottomAttachment,	XmATTACH_FORM,
			   XmNleftAttachment,   XmATTACH_FORM,
			   XmNrightAttachment,  XmATTACH_FORM,
			   XmNpacking,		XmPACK_COLUMN,
			   XmNnumColumns,	2,
			   NULL);

  label = XtVaCreateManagedWidget("affine_options", xmLabelWidgetClass,
				  frame,
				  XmNborderWidth,	0,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  NULL);

  /* create toggles for the allowed affine transformations */
  translate_w = XtVaCreateManagedWidget("translate", xmToggleButtonGadgetClass,
					form1,
					XmNindicatorOn,	True,
					XmNindicatorType,	XmN_OF_MANY,
					XmNvisibleWhenOff,	True,
					XmNset,		True,
					XmNsensitive,	False,
					NULL);

  rotate_w = XtVaCreateManagedWidget("rotate", xmToggleButtonGadgetClass,
				     form1,
				     XmNindicatorOn,	True,
				     XmNindicatorType,	XmN_OF_MANY,
				     XmNvisibleWhenOff,	True,
				     XmNset,		True,
				     NULL);

  scale_w = XtVaCreateManagedWidget("scale", xmToggleButtonGadgetClass,
				    form1,
				    XmNindicatorOn,	True,
				    XmNindicatorType,	XmN_OF_MANY,
				    XmNvisibleWhenOff,	True,
				    XmNset,		False,
				    NULL);

  shear_w = XtVaCreateManagedWidget("shear", xmToggleButtonGadgetClass,
				    form1,
				    XmNindicatorOn,	True,
				    XmNindicatorType,	XmN_OF_MANY,
				    XmNvisibleWhenOff,	True,
				    XmNset,		False,
				    NULL);

  XtManageChild( form1 );

  return( form );
}
