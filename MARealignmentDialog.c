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

#include <MAPaint.h>

extern Widget create_view_window_dialog(Widget topl,
					double theta,
					double phi,
					WlzDVertex3 *fixed);

static int		alignBufferSize=100;
static XPoint		*srcPoly, *dstPoly, *tmpPoly;
static WlzObject	*origPaintedObject;

void realignmentCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	dialog;

  dialog = createRealignmentDialog(globals.topl);
  XtManageChild(dialog);

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
	  break;
	}
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
	realignSetImage(view_struct);
	realignDisplayPolysCb(w, client_data, call_data);
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
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_b:
      break;

    case XK_Down:
    case XK_n:
      break;

    }
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
			   WLZ_GREY_UBYTE, 0, 0.0, 0.0, NULL);
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

    XtAddCallback(view_struct->canvas, XmNexposeCallback,
		  realignDisplayPolysCb, view_struct);
  }
  else {
    /* clear stored data */
    reset_view_struct(view_struct);
    display_view_cb(widget, client_data, call_data);
    AlcFree((void *) srcPoly);
    AlcFree((void *) dstPoly);
    AlcFree((void *) tmpPoly);
    WlzFreeObj(origPaintedObject);
    XtRemoveCallback(view_struct->canvas, XmNexposeCallback,
		     realignDisplayPolysCb, view_struct);
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
  }
  else {
    /* swap the callbacks to normal input mode */
    XtRemoveCallback(view_struct->canvas, XmNinputCallback, realignment_input_cb,
		     client_data);
    XtAddCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		  client_data);
  }

  return;
}

static ActionAreaItem   realign_controls_actions[] = {
{"reset",	NULL,		NULL},
{"I/O",        NULL,           NULL},
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

Widget createRealignmentDialog(
  Widget	topl)
{
  Widget	dialog=NULL;
  WlzDVertex3		fixed;
  ThreeDViewStruct	*view_struct;
  Widget	control, frame, form, title, controls_frame;
  Widget	option_menu, button, buttons, radio_box, label, widget;

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
				  xmToggleButtonWidgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  XtAddCallback(title, XmNvalueChangedCallback, realignment_controls_cb,
		view_struct);
  XtAddCallback(title, XmNvalueChangedCallback, realignment_setup_cb,
		view_struct);

  /* now the controls */
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
				   xmToggleButtonWidgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   NULL);
  button = XtVaCreateManagedWidget("dst_poly",
				   xmToggleButtonWidgetClass, radio_box,
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
				   xmToggleButtonWidgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   XmNset, True,
				   NULL);
  XtVaSetValues(radio_box, XmNmenuHistory, button, NULL);
  button = XtVaCreateManagedWidget("straight_line",
				   xmToggleButtonWidgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   NULL);
  widget = radio_box;
  XtManageChild(widget);

  /* now some buttons */
  realign_controls_actions[0].callback = resetRealignPolyCb;
  realign_controls_actions[0].client_data = view_struct;
  buttons = HGU_XmCreateWActionArea(form,
				    realign_controls_actions,
				    XtNumber(realign_controls_actions),
				    xmPushButtonWidgetClass);
  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		option_menu,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  /* modify attachments for the orientation controls frame */
  XtVaSetValues(controls_frame,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,	frame,
		NULL);

  return( dialog );
}
