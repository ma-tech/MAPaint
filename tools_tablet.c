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
* File        :    tools_tablet.c					     *
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Wed May  8 17:09:47 1996				     *
* Synopsis    : 							     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <MAPaint.h>
#include <HGU_Tablet.h>

/* define tablet events to be different to any
   X11 event byt using LASTEvent which is bigger than any true X11 event */
#define TabletButtonPress	(ButtonPress+LASTEvent)
#define TabletButtonRelease	(ButtonRelease+LASTEvent)
#define TabletMotionNotify	(MotionNotify+LASTEvent)

static Widget tablet_controls=NULL;
static HGUTablet *tablet=NULL;
static XtInputId tabletInputId;

/* this converts the tablet event into a pretend X-event
   what we really need is to sense the keyboard state so
   that the modifier key masks can be set */
static void tabletInputProc(
  XtPointer	client_data,
  int		*fd,
  XtInputId	*inputId)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  TabletEvent		event;
  static TabletEvent	lastEvent;
  XEvent		xevent;
  int			state, button;
  static int		laststate;
  XmDrawingAreaCallbackStruct	cbs;

  /* get the next tablet event and convert to an X-event,
     use the previous event to detect button press and release events
     three events possible: ButtonPress, ButtonRelease, MotionNotify */
  if( TabletNextEvent(tablet, &event) == TABLET_ERR_NONE ){
    if( event.eventNum == 1 ){
      lastEvent = event;
      laststate = 0;
    }

    /* check event type from buttonPressed and buttons values */
    if((lastEvent.buttonPressed < event.buttonPressed) ||
       (lastEvent.buttons < event.buttons)){
      xevent.type = TabletButtonPress;
    }
    else if((lastEvent.buttonPressed > event.buttonPressed) ||
            (lastEvent.buttons > event.buttons)){
      xevent.type = TabletButtonRelease;
    }
    else {
      xevent.type = TabletMotionNotify;
    }

    /* attempt to work out buttons state */
    switch( event.device ){
    default:
    case TABLET_DEVICE_STYLUS:
      switch( event.buttons ){
      default:
      case 0:
	state = 0;
	break;
      case 1:
	state = Button1Mask;
	break;
      case 2:
	state = Button3Mask;
	break;
      case 3:
	state = Button1Mask|Button3Mask;
	break;
      case 4:
	state = Button2Mask;
	break;
      }
      break;
	
    case TABLET_DEVICE_4BUTTON_CURSOR:
      switch( event.buttons ){
      default:
      case 0:
	state = 0;
	break;
      case 1:
	state = Button2Mask;
	break;
      case 2:
	state = Button1Mask;
	break;
      case 3:
	switch( xevent.type ){
	case TabletMotionNotify: /* note this is ambiguous if eventNum = 1 */
	  if( laststate ){
	    state = laststate;
	  }
	  else {
	    state = Button4Mask;
	  }
	  break;
	case TabletButtonPress:
	  if( laststate&Button1Mask ){
	    state = Button1Mask|Button2Mask;
	  }
	  else if( laststate&Button2Mask ){
	    state = Button1Mask|Button2Mask;
	  }
	  else {
	    state = Button4Mask;
	  }
	  break;
	case TabletButtonRelease:
	  state = Button4Mask;
	}
	break;
      case 4:
	switch( xevent.type ){
	case TabletMotionNotify: /* note this is ambiguous if eventNum = 1 */
	  if( laststate ){
	    state = laststate;
	  }
	  else {
	    state = Button3Mask;
	  }
	  break;
	case TabletButtonPress:
	  if( laststate&(Button2Mask|Button4Mask) ){
	    state = Button2Mask|Button4Mask;
	  }
	  else {
	    state = Button3Mask;
	  }
	  break;
	case TabletButtonRelease: 
	  state = Button4Mask;
	}
	break;
      case 5:
	switch( xevent.type ){
	case TabletButtonPress:
	  if( (laststate&(Button1Mask|Button4Mask)) ){
	    state = Button1Mask|Button4Mask;
	  }
	  else if( laststate&(Button2Mask|Button3Mask) ){
	    state = Button2Mask|Button3Mask;
	  }
	  else {
	    state = Button3Mask;
	  }
	  break;
	case TabletMotionNotify: /* not possible */
	  state = laststate;
	  break;
	case TabletButtonRelease: /* not possible */
	  state = Button5Mask;
	}
	break;
      case 6:
	switch( xevent.type ){
	case TabletButtonPress:
	  if( laststate&Button1Mask ){
	    state = Button1Mask|Button3Mask;
	  }
	  else if( laststate&Button4Mask ){
	    state = Button3Mask|Button4Mask;
	  }
	  else { /* ambiguous */
	    state = Button5Mask;
	  }
	  break;
	case TabletMotionNotify: /* not possible */
	  state = laststate;
	  break;
	case TabletButtonRelease: /* not possible */
	  state = Button5Mask;
	}
	break;
      }
      break;
    }

    /* set which button */
    switch( xevent.type ){
    case TabletMotionNotify:
      xevent.xmotion.state = state;
      break;
    case TabletButtonPress:
      xevent.xbutton.state = laststate;
      switch( state & (~laststate) ){
      case Button1Mask:
	xevent.xbutton.button = Button1;
	break;
      case Button2Mask:
	xevent.xbutton.button = Button2;
	break;
      case Button3Mask:
	xevent.xbutton.button = Button3;
	break;
      case Button4Mask:
	xevent.xbutton.button = Button4;
	break;
      default: /* this indicates ambiguity or error */
	xevent.xbutton.button = Button5;
	break;
      }
      break;
    case TabletButtonRelease:
      xevent.xbutton.state = laststate;
      switch( (~state) & laststate ){
      case Button1Mask:
	xevent.xbutton.button = Button1;
	break;
      case Button2Mask:
	xevent.xbutton.button = Button2;
	break;
      case Button3Mask:
	xevent.xbutton.button = Button3;
	break;
      case Button4Mask:
	xevent.xbutton.button = Button4;
	break;
      default: /* this indicates ambiguity or error */
	xevent.xbutton.button = Button5;
	break;
      }
      break;
    }

    /* set coordinate values - x,y are converted, x_root, y_root originals */
    switch( xevent.type ){
    case TabletMotionNotify:
      xevent.xmotion.x = WLZ_NINT(event.X);
      xevent.xmotion.y = WLZ_NINT(event.Y);
      xevent.xmotion.x_root = event.x;
      xevent.xmotion.y_root = event.y;
      break;
    case TabletButtonPress:
    case TabletButtonRelease:
      xevent.xbutton.x = WLZ_NINT(event.X);
      xevent.xbutton.y = WLZ_NINT(event.Y);
      xevent.xbutton.x_root = event.x;
      xevent.xbutton.y_root = event.y;
      break;
    }

    /* set other bits and bobs */
    xevent.xany.serial = event.eventNum;
    xevent.xany.send_event = False;
    xevent.xany.display = XtDisplay(view_struct->canvas);
    xevent.xany.window = XtWindow(view_struct->canvas);
    switch( xevent.type ){
    case TabletMotionNotify:
      xevent.xmotion.root = (Window) 0;
      xevent.xmotion.subwindow = (Window) 0;
      xevent.xmotion.time = (Time) 0;
      xevent.xmotion.is_hint = NotifyNormal;
      xevent.xmotion.same_screen = True;
      break;
    case TabletButtonPress:
    case TabletButtonRelease:
      xevent.xbutton.root = (Window) 0;
      xevent.xbutton.subwindow = (Window) 0;
      xevent.xbutton.time = (Time) 0;
      xevent.xbutton.same_screen = True;
      break;
    }

    lastEvent = event;
    laststate = state;

    /* all above should be in convert event,
       now call the tablet callback function */
    cbs.reason = XmCR_INPUT;
    cbs.event = &xevent;
    cbs.window = xevent.xany.window;
    MAPaintTabletCb(view_struct->canvas, client_data, (XtPointer) &cbs);
  }
  return;
}
    

static unsigned int 	modifiers=0;
static int		tabletDrawingFlag=0;
static DomainSelection	currDomain;
static int		delFlag;
static int		quitDrawingTrigger=0;
static WlzPolygonDomain	*tabletPoly=NULL;

void MAPaintTabletInit(
  ThreeDViewStruct	*view_struct)
{
  TabletErrNum	errNum;
  TabletEvent	event;
  int		quitFlag;
  Widget	dialog;
  XmString	text, title;
  WlzDVertex2	tabVtxs[2], wlzVtxs[2];
  Widget	slider;
  WlzAffineTransform	*trans;

  /* ignore if already open */
  if( tablet ){
    return;
  }

  /* open the tablet */
  if( (tablet = TabletOpen(WACOM_IV_TABLET_TYPE,
			   "/dev/term/b", NULL)) == NULL ){
    HGU_XmUserError(globals.topl,
		    "Open Wacom Tablet:\n"
		    "    Failed to open the tablet, please check\n"
		    "    the tablet is connected to serial line b\n"
		    "    and switched on, then try again.",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* set tilt mode and start the tablet */
  TabletSetMode(tablet, TABLET_TILTMODE_MASK);
  tablet->cntrlMode2 |= TABLET_CORRECT_TILT_MASK;
  tablet->cntrlMode2 |= TABLET_TRANSFORM_MASK;
  TabletStart(tablet);

  /* if the tablet needs the coordinates set then do so here */
  if( !view_struct->tablet_initialised ){
    /* get first coordinate */
    /* put up an application modal dialog to request first
       coordinate */
    dialog = XmCreateMessageDialog(view_struct->canvas,
				   "MAPaint Message", NULL, 0);
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON));
    XtSetSensitive(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
		   False);
    text  = XmStringCreateLtoR("Input tablet reference point 1 please",
			       XmSTRING_DEFAULT_CHARSET);
    title = XmStringCreateSimple("MAPaint Tablet Message");

    XtVaSetValues(dialog,
		  XmNmessageString,	text,
		  XmNdialogTitle,	title,
		  XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL);

    XmStringFree( text );
    XmStringFree( title );

    /* popup widget and process events */
    XtManageChild( dialog );
    XSync(XtDisplay(view_struct->canvas), False);
    quitFlag = 0;
    while( quitFlag < 100 ){
      if((XtAppPending(globals.app_con) > 0) ){
	XtAppProcessEvent(globals.app_con, XtIMAll);
      }
      else {
	XSync(XtDisplay(view_struct->canvas), False);
	usleep(1000);
	quitFlag++;
      }
    }
    
    quitFlag = 0;
    while( !quitFlag ){
      if( TabletNextEvent(tablet, &event) == TABLET_ERR_NONE ){
	if(event.buttonPressed && 
	   ((event.buttons == 1) || (event.buttons == 3))){
	  tabVtxs[0].vtX = event.x;
	  tabVtxs[0].vtY = event.y;
	  quitFlag = 1;
	  XBell(XtDisplay(view_struct->canvas), 100);
	  XFlush(XtDisplay(view_struct->canvas));
	}
      }
      else {
	TabletClose(tablet);
	tablet = NULL;
	XtDestroyWidget(dialog);
	return;
      }
    }

    /* get second coordinate */
    text  = XmStringCreateLtoR("Input tablet reference point 2 please",
			       XmSTRING_DEFAULT_CHARSET);

    XtVaSetValues(dialog,
		  XmNmessageString,	text,
		  NULL);

    XmStringFree( text );

    /* popup widget and process events */
    XtManageChild( dialog );
    XSync(XtDisplay(view_struct->canvas), False);
    while( (XtAppPending(globals.app_con) > 0) ){
      XtAppProcessEvent(globals.app_con, XtIMAll);
      XSync(XtDisplay(view_struct->canvas), False);
    }
    
    quitFlag = 0;
    sleep(1);
    TabletClearEvents(tablet);
    while( !quitFlag ){
      if( TabletNextEvent(tablet, &event) == TABLET_ERR_NONE ){
	if(event.buttonPressed && 
	   ((event.buttons == 1) || (event.buttons == 3))){
	  tabVtxs[1].vtX = event.x;
	  tabVtxs[1].vtY = event.y;
	  quitFlag = 1;
	  XBell(XtDisplay(view_struct->canvas), 100);
	  XFlush(XtDisplay(view_struct->canvas));
	}
      }
      else {
	TabletClose(tablet);
	XtDestroyWidget(dialog);
	tablet = NULL;
	return;
      }
    }
    TabletStop(tablet);
    XtDestroyWidget(dialog);
    usleep(10000);
    TabletClearEvents(tablet);

    /* set the reference coordinates */
    view_struct->ref1.vtX = tabVtxs[0].vtX;
    view_struct->ref1.vtY = tabVtxs[0].vtY;
    view_struct->ref2.vtX = tabVtxs[1].vtX;
    view_struct->ref2.vtY = tabVtxs[1].vtY;
  }
  else {
    tabVtxs[0].vtX = view_struct->ref1.vtX;
    tabVtxs[0].vtY = view_struct->ref1.vtY;
    tabVtxs[1].vtX = view_struct->ref2.vtX;
    tabVtxs[1].vtY = view_struct->ref2.vtY;
  }

  /* set tablet transform parameters */
  if( slider = XtNameToWidget(tablet_controls, "*.x1") )
  {
    wlzVtxs[0].vtX = HGU_XmGetSliderValue(slider);
  }
  if( slider = XtNameToWidget(tablet_controls, "*.y1") )
  {
    wlzVtxs[0].vtY = HGU_XmGetSliderValue(slider);
  }
  if( slider = XtNameToWidget(tablet_controls, "*.x2") )
  {
    wlzVtxs[1].vtX = HGU_XmGetSliderValue(slider);
  }
  if( slider = XtNameToWidget(tablet_controls, "*.y2") )
  {
    wlzVtxs[1].vtY = HGU_XmGetSliderValue(slider);
  }
  trans = WlzAffineTransformLSq(2, tabVtxs, 2, wlzVtxs,
				WLZ_TRANSFORM_2D_NOSHEAR, NULL);
  tablet->xTrans[0] = trans->mat[0][0];
  tablet->xTrans[1] = trans->mat[0][1];
  tablet->xTrans[2] = trans->mat[0][2];
  tablet->yTrans[0] = trans->mat[1][0];
  tablet->yTrans[1] = trans->mat[1][1];
  tablet->yTrans[2] = trans->mat[1][2];
  WlzFreeAffineTransform(trans);
  view_struct->tablet_initialised = 1;

  /* initialise the globals */
  modifiers = 0;
  tabletDrawingFlag = 0;
  quitDrawingTrigger = 0;
  if( tabletPoly ){
    WlzFreePolyDmn(tabletPoly);
    tabletPoly = NULL;
  }

  /* now add the tablet stream as an event input source */
  TabletStart(tablet);
  TabletClearEvents(tablet);
  tabletInputId = XtAppAddInput(globals.app_con, tablet->fd,
				(XtPointer) XtInputReadMask,
				tabletInputProc, (XtPointer) view_struct);

  return;
}

void MAPaintTabletQuit(
  ThreeDViewStruct	*view_struct)
{
  /* do nothing if tablet NULL */
  if( !tablet ){
    return;
  }

  /* stop the tablet */
  TabletStop(tablet);

  /* remove it as an input source */
  XtRemoveInput(tabletInputId);

  /* close it */
  TabletClose(tablet);
  tablet = NULL;

  /* clear the global polyline */
  if( tabletPoly ){
    WlzFreePolyDmn(tabletPoly);
    tabletPoly = NULL;
  }

  return;
}

static void TabletPolyNextPoint(
  int	x,
  int	y)
{
  int	lastVtx;
  WlzPolygonDomain	*tmpPoly;

  /* create the polygon if required */
  if( tabletPoly == NULL ){
    tabletPoly = WlzMakePolyDmn(WLZ_POLYGON_INT, NULL, 0, 256, 1, NULL);
    tabletPoly->vtx[0].vtX = x;
    tabletPoly->vtx[0].vtY = y;
    tabletPoly->nvertices = 1;
    return;
  }

  /* check if this point differs from the last */
  lastVtx = tabletPoly->nvertices;
  if((tabletPoly->vtx[lastVtx-1].vtX == x) &&
     (tabletPoly->vtx[lastVtx-1].vtY == y)){
    return;
  }

  /* check if more points needed */
  if( lastVtx >= tabletPoly->maxvertices ){
    tmpPoly = WlzMakePolyDmn(WLZ_POLYGON_INT, tabletPoly->vtx, lastVtx,
			     lastVtx + 256, 1, NULL);
    WlzFreePolyDmn(tabletPoly);
    tabletPoly = tmpPoly;
  }

  /* add the next point */
  tabletPoly->vtx[lastVtx].vtX = x;
  tabletPoly->vtx[lastVtx].vtY = y;
  tabletPoly->nvertices++;

  return;
}

void MAPaintTabletCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  WlzObject		*obj, *obj1;

  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){
    case Button1:
    case Button2:
      /* do nothing if shift pressed - magnify option */
      if( cbs->event->xbutton.state & ButtonStateIgnoreMask ){
	break;
      }
      break;

    default:
      break;

    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){
    case Button1:
    case Button2:
      break;

    case Button3:
      if( tabletDrawingFlag ){
	if( quitDrawingTrigger ){
	  tabletDrawingFlag = 0;
	  quitDrawingTrigger = 0;
	}
	/* install the new domain */
	if( tabletPoly ){
	  obj = WlzPolyToObj(tabletPoly, WLZ_SIMPLE_FILL, NULL);
	  /* reset the painted object and redisplay */
	  if( obj ){
	    obj1 =
	      WlzShiftObject(obj,
			     view_struct->painted_object->domain.i->kol1,
			     view_struct->painted_object->domain.i->line1,
			     0, NULL);
	    setDomainIncrement(obj1, view_struct, currDomain, delFlag);
	    WlzFreeObj( obj );
	    WlzFreeObj( obj1 );
	  }
	  WlzFreePolyDmn(tabletPoly);
	  tabletPoly = NULL;
	}	  
      }
      break;
    }
    break;

  case MotionNotify:
    break;

  case KeyPress:
    /* check for modifier key events */
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
    case XK_Shift_L:
    case XK_Shift_R:
      modifiers |= ShiftMask;
      break;
    case XK_Control_L:
    case XK_Control_R:
      modifiers |= ControlMask;
      break;
    case XK_Meta_L:
    case XK_Meta_R:
      modifiers |= Mod4Mask;
      break;
    case XK_Alt_L:
    case XK_Alt_R:
      modifiers |= Mod1Mask;
      break;
    }
    break;

  case KeyRelease:
    /* check for modifier key events */
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
    case XK_Shift_L:
    case XK_Shift_R:
      modifiers &= ~ShiftMask;
      break;
    case XK_Control_L:
    case XK_Control_R:
      modifiers &= ~ControlMask;
      break;
    case XK_Meta_L:
    case XK_Meta_R:
      modifiers &= ~Mod4Mask;
      break;
    case XK_Alt_L:
    case XK_Alt_R:
      modifiers &= ~Mod1Mask;
      break;
    }
    break;

  case TabletButtonPress:
    switch( cbs->event->xbutton.button ){
    case Button1:
    case Button2:
      /* set the drawing flag */
      if( !tabletDrawingFlag ){
	/* save the current domain selection and meta button state */
	currDomain = globals.current_domain;
	delFlag = ((modifiers & Mod1Mask) ||
		   (cbs->event->xbutton.button == Button2));
      
	tabletDrawingFlag = 1;
      }
      /* set the next point */
      TabletPolyNextPoint(cbs->event->xbutton.x, cbs->event->xbutton.y);
      break;

    case Button3:
      if( tabletDrawingFlag ){
	quitDrawingTrigger = 1;
      }
      else {
	canvas_2D_painting_cb(w, client_data, call_data);
      }
      break;
    }
    break;

  case TabletButtonRelease:
    switch( cbs->event->xbutton.button ){
    case Button1:
    case Button2:
      break;

    case Button3:
      if( tabletDrawingFlag ){
	if( quitDrawingTrigger ){
	  tabletDrawingFlag = 0;
	  quitDrawingTrigger = 0;
	}
	/* install the new domain */
	if( tabletPoly ){
	  obj = WlzPolyToObj(tabletPoly, WLZ_SIMPLE_FILL, NULL);
	  /* reset the painted object and redisplay */
	  if( obj ){
	    obj1 =
	      WlzShiftObject(obj,
			     view_struct->painted_object->domain.i->kol1,
			     view_struct->painted_object->domain.i->line1,
			     0, NULL);
	    setDomainIncrement(obj1, view_struct, currDomain, delFlag);
	    WlzFreeObj( obj );
	    WlzFreeObj( obj1 );
	  }
	  WlzFreePolyDmn(tabletPoly);
	  tabletPoly = NULL;
	}	  
      }
      else {
	canvas_2D_painting_cb(w, client_data, call_data);
      }
      break;
    }
    break;

  case TabletMotionNotify:
    if( tabletDrawingFlag ){
      if( cbs->event->xmotion.state & (Button1Mask|Button2Mask) ){
	/* if buttons 1 or 2 pressed then set next point */
	TabletPolyNextPoint(cbs->event->xmotion.x, cbs->event->xmotion.y);
      }
      else {
	/* rubber band from last point */
	/* don't need this since the user is looking at the tablet */
      }
    }
    else {
      canvas_2D_painting_cb(w, client_data, call_data);
    }
    break;

  default:
    break;
  }

  return;
}

Widget	CreateTabletControls(
  Widget	parent)
{
  Widget	form, form1, label, slider, frame, widget;

  /* create a parent form to hold all the tracking controls */
  form = XtVaCreateWidget("tablet_controls_form", xmFormWidgetClass,
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

  form1 = XtVaCreateWidget("tablet_form1", xmFormWidgetClass,
			  frame,
			  XmNtopAttachment,     XmATTACH_FORM,
			  XmNbottomAttachment,	XmATTACH_FORM,
			  XmNleftAttachment,    XmATTACH_FORM,
			  XmNrightAttachment,  	XmATTACH_FORM,
			  NULL);

  label = XtVaCreateManagedWidget("tablet_ref_points", xmLabelWidgetClass,
				  frame,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  XmNborderWidth,	0,
				  NULL);

  /* create sliders for the reference coordinates */
  slider = HGU_XmCreateHorizontalSlider("x1", form1,
					0.0, -1000.0, 2000.0, 0,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  slider = HGU_XmCreateHorizontalSlider("y1", form1,
					0.0, -1000.0, 2000.0, 0,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  slider = HGU_XmCreateHorizontalSlider("x2", form1,
					150.0, -1000.0, 2000.0, 0,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  slider = HGU_XmCreateHorizontalSlider("y2", form1,
					100.0, -1000.0, 2000.0, 0,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  XtManageChild( form1 );

  tablet_controls = form;

  return( form );
}
