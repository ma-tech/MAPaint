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
*   File       :   view2DPaintingCb.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Thu Mar  5 09:30:54 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>
#include <HGU_Tablet.h>

/* define tablet events to be different to any
   X11 event byt using LASTEvent which is bigger than any true X11 event */
#define TabletButtonPress	(ButtonPress+LASTEvent)
#define TabletButtonRelease	(ButtonRelease+LASTEvent)
#define TabletMotionNotify	(MotionNotify+LASTEvent)

extern void view_canvas_highlight(ThreeDViewStruct	*view_struct,
				  Boolean		highlight);

extern void display_pointer_feedback_information(
  ThreeDViewStruct	*view_struct,
  int			x, 
  int			y);

static int quitPaintingTrigger=0;

void installCurrentPaintTool(
  Widget		w,
  ThreeDViewStruct	*view_struct)
{
  /* initialise the paint tool */
  (*(globals.currentPaintActionInitFunc))(view_struct);

  /* install the callback */
  XtAddCallback(w, XmNinputCallback, globals.currentPaintActionCbFunc,
		(XtPointer) view_struct);

  return;
}

void removeCurrentPaintTool(
  Widget		w,
  ThreeDViewStruct	*view_struct)
{
  /* remove the callback */
  XtRemoveCallback(w, XmNinputCallback, globals.currentPaintActionCbFunc,
		   (XtPointer) view_struct);

  /* call the paint tool quit function */
  (*(globals.currentPaintActionQuitFunc))(view_struct);

  return;
}

void setViewScale(
  ThreeDViewStruct	*viewStruct,
  double		newScale,
  int			fixedX,
  int			fixedY)
{
  WlzThreeDViewStruct	*wlzViewStr=viewStruct->wlzViewStr;
  Widget		menu, widget, scrolledWindow, xScrollBar, yScrollBar;
  int			oldXSize, oldXMin, oldXMax, oldXVal;
  int			oldYSize, oldYMin, oldYMax, oldYVal;
  int			newSize, newMin, newMax, newVal;
  int			increment, pageIncrement;
  int			xp;
  double		a, b;
  double		mag;
  Dimension		width, height, widthp, heightp;

  /* check scale value and get the widget - should use the scale menu options */
  if((menu = XtNameToWidget(viewStruct->controls, "*view_scale"))
     == NULL){
    return;
  }
  if( newScale > 12.0 ){
    newScale = 16.0;
    widget = XtNameToWidget(menu, "*16");
  }
  else if( newScale > 8.0 ){
    newScale = 12.0;
    widget = XtNameToWidget(menu, "*12");
  }
  else if( newScale > 6.0 ){
    newScale = 8.0;
    widget = XtNameToWidget(menu, "*8");
  }
  else if( newScale > 4.0 ){
    newScale = 6.0;
    widget = XtNameToWidget(menu, "*6");
  }
  else if( newScale > 3.0 ){
    newScale = 4.0;
    widget = XtNameToWidget(menu, "*4");
  }
  else if( newScale > 2.0 ){
    newScale = 3.0;
    widget = XtNameToWidget(menu, "*3");
  }
  else if( newScale > 1.0 ){
    newScale = 2.0;
    widget = XtNameToWidget(menu, "*2");
  }
  else if( newScale < 0.4 ){
    newScale = 0.25;
    widget = XtNameToWidget(menu, "*0_25");
  }
  else if( newScale < 0.9 ){
    newScale = 0.5;
    widget = XtNameToWidget(menu, "*0_5");
  }
  else {
    newScale = 1.0;
    widget = XtNameToWidget(menu, "*1");
  }

  /* if no change then do nothing ! */
  if( fabs(newScale - wlzViewStr->scale) < 0.1 ){
    return;
  }  
  if( globals.logfileFp ){
    char strBuf[48];
    sprintf(strBuf, "%f", newScale);
    MAPaintLogData("Scale", strBuf, 0, viewStruct->dialog);
  }
  mag = newScale / wlzViewStr->scale;

  /* get old slider and window info */
  scrolledWindow = XtNameToWidget(viewStruct->dialog, "*scrolled_window");
  XtVaGetValues(scrolledWindow,
		XmNhorizontalScrollBar, &xScrollBar,
		XmNverticalScrollBar, &yScrollBar,
		NULL);
  XtVaGetValues(xScrollBar,
		XmNsliderSize, &oldXSize,
		XmNminimum, &oldXMin,
		XmNmaximum, &oldXMax,
		XmNvalue, &oldXVal,
		NULL);
  XtVaGetValues(yScrollBar,
		XmNsliderSize, &oldYSize,
		XmNminimum, &oldYMin,
		XmNmaximum, &oldYMax,
		XmNvalue, &oldYVal,
		NULL);
  XtVaGetValues(viewStruct->canvas,
		XmNwidth, &width,
		XmNheight, &height,
		NULL);

  /* test if the fixed point is to be set */
  if( fixedX < 0 ){
    fixedX = (oldXVal + oldXSize/2) * width / (oldXMax - oldXMin);
  }
  if( fixedY < 0 ){
    fixedY = (oldYVal + oldYSize/2) * height / (oldYMax - oldYMin);
  }

  /* reset the scale */
/*  XtCallCallbacks(widget, XmNactivateCallback, NULL);*/
  wlzViewStr->scale = newScale;
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
  widthp *= wlzViewStr->scale;
  heightp *= wlzViewStr->scale;
  XtVaSetValues(viewStruct->canvas,
		XmNwidth, widthp,
		XmNheight, heightp,
		NULL);
  
  XtVaSetValues(menu, XmNmenuHistory, widget, NULL);

  /* reset the new sliders */
  XtVaGetValues(xScrollBar,
		XmNsliderSize, &newSize,
		XmNminimum, &newMin,
		XmNmaximum, &newMax,
		XmNincrement, &increment,
		XmNpageIncrement, &pageIncrement,
		NULL);

  a = - ((double) width) / (oldXMax - oldXMin);
  b = -a * oldXMin;
  xp = fixedX + a * oldXVal + b;

  a = - ((double) width) * mag / (newMax - newMin);
  b = -a * newMin;
  newVal = (xp - mag * fixedX - b) / a;

  newVal = WLZ_MIN(newVal, newMax - newSize);
  newVal = WLZ_MAX(newVal, newMin);

  XmScrollBarSetValues(xScrollBar, newVal, newSize, increment,
		       pageIncrement, True);

  XtVaGetValues(yScrollBar,
		XmNsliderSize, &newSize,
		XmNminimum, &newMin,
		XmNmaximum, &newMax,
		XmNincrement, &increment,
		XmNpageIncrement, &pageIncrement,
		NULL);

  a = - ((double) height) / (oldYMax - oldYMin);
  b = -a * oldYMin;
  xp = fixedY + a * oldYVal + b;

  a = - ((double) height) * mag / (newMax - newMin);
  b = -a * newMin;
  newVal = (xp - mag * fixedY - b) / a;

  newVal = WLZ_MIN(newVal, newMax - newSize);
  newVal = WLZ_MAX(newVal, newMin);

  XmScrollBarSetValues(yScrollBar, newVal, newSize, increment,
		       pageIncrement, True);


  /* redisplay the section */
  redisplay_view_cb(viewStruct->canvas, (XtPointer) viewStruct, NULL);

  return;
}

void setViewMode(
  ThreeDViewStruct	*viewStruct,
  WlzThreeDViewMode	newMode)
{
  WlzThreeDViewStruct	*wlzViewStr=viewStruct->wlzViewStr;
  Widget		menu, widget;

  /* check mode value and get the widget -
     should use the mode menu options */
  if((menu = XtNameToWidget(viewStruct->controls, "*view_mode"))
     == NULL){
    return;
  }

  switch( newMode ){
  default:
  case WLZ_UP_IS_UP_MODE:
    widget = XtNameToWidget(menu, "*up-is-up");
    break;

  case WLZ_STATUE_MODE:
    widget = XtNameToWidget(menu, "*statue");
    break;

  case WLZ_ZETA_MODE:
    widget = XtNameToWidget(menu, "*absolute");
    break;
  }

  /* if no change then do nothing ! */
/*  if( newMode == wlzViewStr->view_mode ){
    return;
  }  */

  /* reset the mode */
  XtCallCallbacks(widget, XmNactivateCallback, NULL);
  XtVaSetValues(menu, XmNmenuHistory, widget, NULL);

  return;
}

static int		paintLiftFlag=0;
static DomainSelection	paintLiftDomain;
static WlzObject	*paintLiftObj=NULL;

void canvasMagPlusCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  int			x, y;

  setViewScale(view_struct, wlzViewStr->scale * 2.0, -1, -1);
  return;
}
  
void canvasMagMinusCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  int			x, y;

  setViewScale(view_struct, wlzViewStr->scale / 2.0, -1, -1);
  return;
}
  
void canvas_2D_painting_cb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;

  /* check for lock */
  if( view_struct->viewLockedFlag ){
    return;
  }

  /* switch on event type */
  switch( cbs->event->type ){

  case ButtonPress:
  case TabletButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
      /* if shift is pressed then increase the magnification
	 the scrolled window must be set to keep the pointer
	 position fixed */
      switch(cbs->event->xbutton.state & (ShiftMask|ControlMask|Mod1Mask)){

      case ShiftMask: /* magnify */
	setViewScale(view_struct, wlzViewStr->scale * 2.0,
		     cbs->event->xbutton.x, cbs->event->xbutton.y);
	break;

      case ShiftMask|Mod1Mask: /* reduce */
	setViewScale(view_struct, wlzViewStr->scale / 2.0,
		     cbs->event->xbutton.x, cbs->event->xbutton.y);
	break;

      case ControlMask: /* lift paint */
	if( paintLiftFlag ){
	  break;
	}
	paintLiftFlag = 1;
	if( (paintLiftDomain =
	     getSelectedDomainType(cbs->event->xbutton.x,
				   cbs->event->xbutton.y,
				   view_struct)) < 1 ){
	  break;
	}
	paintLiftObj = getSelectedRegion(cbs->event->xbutton.x,
					 cbs->event->xbutton.y,
					 view_struct);
	paintLiftObj = WlzAssignObject(paintLiftObj, NULL);

	/* clear the selected paint object object and redisplay */
	if( paintLiftObj ){
	  setDomainIncrement(paintLiftObj, view_struct, paintLiftDomain, 1);
	}
	break;

      default:
	break;
      }
	
      break;

    case Button2:		/* domain and coordinate feedback */
      /* if shift is pressed then decrease the magnification
	 the scrolled window must be set to keep the pointer
	 position fixed */
      if( cbs->event->xbutton.state & ShiftMask ){
	setViewScale(view_struct, wlzViewStr->scale / 2.0,
		     cbs->event->xbutton.x, cbs->event->xbutton.y);
      }
      else {
	x = cbs->event->xbutton.x / wlzViewStr->scale;
	y = cbs->event->xbutton.y / wlzViewStr->scale;
	display_pointer_feedback_information(view_struct, x, y);
      }
      break;

    case Button3:
      quitPaintingTrigger = 1;
      break;

    default:
      break;

    }
    break;

  case ButtonRelease:
  case TabletButtonRelease:
    switch( cbs->event->xbutton.button ){

    case Button1:
      if( paintLiftFlag ){
	paintLiftFlag = 0;
	if( paintLiftObj ){
	  setDomainIncrement(paintLiftObj, view_struct,
			     paintLiftDomain, 0);
	  WlzFreeObj(paintLiftObj);
	  paintLiftObj = NULL;
	}
      }

    case Button2:		/* domain and coordinate feedback */
      XtVaSetValues(view_struct->text, XmNvalue, "", NULL);
      break;

    case Button3: /* release paint selection - autoincrement
		     if required */
      /* check the trigger */
      if( !quitPaintingTrigger ){
	break;
      }
      quitPaintingTrigger = 0;
      removeCurrentPaintTool(w, view_struct);
      paint_key = NULL;

      /* clear the undo domains */
      clearUndoDomains();

      /* clear the section view image */
      if( view_struct->view_object ){
	WlzFreeObj(view_struct->view_object);
	view_struct->view_object = NULL;
      }

      /* install new domains, update all views */
      installViewDomains(view_struct);

      /* check for autoincrement here - reclaim the paint key */
      if( globals.auto_increment ){
	paint_key = view_struct;
	wlzViewStr->dist += 1.0;
	reset_view_struct( view_struct );
	display_view_cb(w, (XtPointer) view_struct, call_data);
	view_feedback_cb(w, (XtPointer) view_struct, NULL);
	getViewDomains(view_struct);
	installCurrentPaintTool(w, view_struct);
	break;
      }

      /* restart the 3D feedback display */
      HGUglwCanvasTbAnimate( globals.canvas );
      XtSetSensitive( globals.canvas, True );

      /* make controls sensitive */
      XtSetSensitive(view_struct->controls, True);
      XtSetSensitive(view_struct->slider, True);
      setControlButtonsSensitive(view_struct, True);

      /* unhighlight this canvas */
      view_canvas_highlight( view_struct, False );

      /* swap the callbacks to standard input mode */
      XtRemoveCallback(w, XmNinputCallback, canvas_2D_painting_cb,
		       client_data);
      XtAddCallback(w, XmNinputCallback, canvas_input_cb,
		    client_data);

      break;

    case EnterNotify:
      fprintf(stderr, "Got EnterNotify on view canvas\n");
      break;

    case LeaveNotify:
      fprintf(stderr, "Got LeaveNotify on view canvas\n");
      break;

    default:
      break;

    }
    break;

  case MotionNotify:
  case TabletMotionNotify:

    if( cbs->event->xmotion.state & Button2Mask )
    {
      x = cbs->event->xmotion.x / wlzViewStr->scale;
      y = cbs->event->xmotion.y / wlzViewStr->scale;
      display_pointer_feedback_information(view_struct, x, y);
    }
    break;

  case KeyPress:
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      /* in this mode - next section installing and resetting domains
	 if <alt> or <alt gr> pressed then change tool option
	 (tool dependent) */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	break;
      }
      else {
	installViewDomains(view_struct);
	clearUndoDomains();
	wlzViewStr->dist += 1.0;
	reset_view_struct( view_struct );
	display_view_cb(w, (XtPointer) view_struct, call_data);
	view_feedback_cb(w, (XtPointer) view_struct, NULL);
	getViewDomains(view_struct);
      }
      break;

    case XK_Up:
    case XK_p:
      /* next domain or next paint tool if <alt> (which seems
	 to be Mod1Mask) pressed */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	selectNext2DTool(0);
      }
      else {
	selectNextDomain(0);
      }
      break;

    case XK_Left:
    case XK_b:
      /* in this mode - next section installing and resetting domains 
	 if <alt> pressed then change tool option (tool dependent) */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	break;
      }
      else {
	installViewDomains(view_struct);
	clearUndoDomains();
	wlzViewStr->dist -= 1.0;
	reset_view_struct( view_struct );
	display_view_cb(w, (XtPointer) view_struct, call_data);
	view_feedback_cb(w, (XtPointer) view_struct, NULL);
	getViewDomains(view_struct);
      }
      break;

    case XK_Down:
    case XK_n:
      /* previous paint tool or previous domain if <alt> pressed */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	selectNext2DTool(1);
      }
      else {
	selectNextDomain(1);
      }
      break;

    case XK_w:
      /* get a filename for the view image */
      break;

    case XK_u:	/* undo */
    case XK_U:
      if( !(cbs->event->xkey.state & ControlMask) ){
	break;
      }
      UndoDomains(view_struct);
      break;

    case XK_r:	/* redo */
    case XK_R:
      if( !(cbs->event->xkey.state & ControlMask) ){
	break;
      }
      RedoDomains(view_struct);
      break;

    case XK_Undo:
    case XK_F14:
      UndoDomains(view_struct);
      break;

    case XK_Redo:
    case XK_F12:
      RedoDomains(view_struct);
      break;

    }
    break;

  default:
    break;
  }

  return;
}
 
