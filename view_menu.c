#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	view_menu.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#ifndef __ppc
#include <values.h>
#endif

#include <MAPaint.h>

/* menu item structures */

static MenuItem view_menu_itemsP[] = {		/* file_menu items */
  {"x_y_view", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   view_cb, (XtPointer) WLZ_X_Y_VIEW,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"y_z_view", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   view_cb, (XtPointer) WLZ_Y_Z_VIEW,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"z_x_view", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   view_cb, (XtPointer) WLZ_Z_X_VIEW,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem scale_menu_itemsP[] = {		/* scale_menu items */
  {"1", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"2", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"3", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"4", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"6", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"8", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"12", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"16", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"0_5", &xmPushButtonWidgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"0_25", &xmPushButtonWidgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem view_mode_menu_itemsP[] = {	/* view_mode_menu items */
  {"up-is-up", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) WLZ_UP_IS_UP_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"statue", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) WLZ_STATUE_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"absolute", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) WLZ_ZETA_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem feedback_mode_menu_itemsP[] = {	/* 3d feedback menu items */
  {"poly", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) MAPAINT_POLY_VIEWFB_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"solid", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) MAPAINT_SOLID_VIEWFB_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"mask", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) MAPAINT_MASK_VIEWFB_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"template", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) MAPAINT_TEMPLATE_VIEWFB_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"switch", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) MAPAINT_SWITCH_VIEWFB_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"image", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) MAPAINT_IMAGE_VIEWFB_MODE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};


MenuItem	*view_menu_items = &(view_menu_itemsP[0]);
static unsigned long    borderColor;

ViewListEntry	*global_view_list=NULL;
ThreeDViewStruct    *paint_key = NULL;
unsigned int	ButtonStateIgnoreMask=ShiftMask|ControlMask;

static ActionAreaItem   view_dialog_controls_actions[] = {
  {"fixed_1",		NULL,		NULL},
  {"fixed_2",     	NULL,           NULL},
  {"up_vector",        	NULL,           NULL},
  {"save_settings",     NULL,           NULL},
};

static ActionAreaItem   view_dialog_actions[] = {
  {"save_section",	NULL,		NULL},
  {"dismiss",     	NULL,           NULL},
  {"listen",     	NULL,           NULL},
  {"help",        	NULL,           NULL},
};

static String translations_table =
"<FocusIn>: 	ViewFeedback()\n\
<FocusOut>:	ViewFeedback()\n\
<MapNotify>: 	ViewFeedback()";

void setControlButtonsSensitive(
  ThreeDViewStruct        *view_struct,
  Boolean	sensitive)
{
  Widget	button, popup;

  /* get button and popup pairs, remove the callback, make insensitive */
  if( button = XtNameToWidget(view_struct->control_buttons,
			      "*.fixed_1") ){
    if( sensitive == False ){
      XtRemoveCallback(button, XmNactivateCallback,
		       fixed_1_cb, view_struct);
    }
    else {
      XtAddCallback(button, XmNactivateCallback,
		    fixed_1_cb, view_struct);
    }
    if( popup = XtNameToWidget(view_struct->control_buttons,
			       "*.fixed_1_popup") ){
      XtSetSensitive(popup, sensitive);
    }
  }

  if( button = XtNameToWidget(view_struct->control_buttons,
			      "*.fixed_2") ){
    if( sensitive == False ){
      XtRemoveCallback(button, XmNactivateCallback,
		       fixed_2_cb, view_struct);
    }
    else {
      XtAddCallback(button, XmNactivateCallback,
		    fixed_2_cb, view_struct);
    }
    if( popup = XtNameToWidget(view_struct->control_buttons,
			       "*.fixed_2_popup") ){
      XtSetSensitive(popup, sensitive);
    }
  }

  if( button = XtNameToWidget(view_struct->control_buttons,
			      "*.up_vector") ){
    if( sensitive == False ){
      XtRemoveCallback(button, XmNactivateCallback,
		       up_vector_cb, view_struct);
    }
    else {
      XtAddCallback(button, XmNactivateCallback,
		    up_vector_cb, view_struct);
    }
    if( popup = XtNameToWidget(view_struct->control_buttons,
			       "*.up_vector_popup") ){
      XtSetSensitive(popup, sensitive);
    }
  }

  if( button = XtNameToWidget(view_struct->control_buttons,
			      "*.save_settings") ){
    if( sensitive == False ){
      XtRemoveCallback(button, XmNactivateCallback,
		       controls_io_write_cb, view_struct);
    }
    else {
      XtAddCallback(button, XmNactivateCallback,
		    controls_io_write_cb, view_struct);
    }
    if( popup = XtNameToWidget(view_struct->control_buttons,
			       "*.save_settings_popup") ){
      XtSetSensitive(popup, sensitive);
    }
  }

  return;
}

void set_view_dialog_title(
  Widget	dialog,
  double	theta,
  double	phi)
{
  XmString	xmstr;
  char		str_buff[64];
  ThreeDViewStruct	*view_struct;

  
  XtVaGetValues(dialog, XmNuserData, &view_struct, NULL);

  theta *= 180.0 / WLZ_M_PI;
  phi *= 180.0 / WLZ_M_PI;
  (void) sprintf(str_buff, "%s: (%.2f, %.2f)",
		 view_struct->titleStr, phi, theta);
  xmstr = XmStringCreateLocalized( str_buff );
  XtVaSetValues(dialog, XmNdialogTitle, xmstr, NULL);
  XmStringFree( xmstr );

  return;
}


static void add_feedback_callbacks(
  Widget                  scale,
  ThreeDViewStruct        *view_struct)
{
  /*XtAddCallback(scale, XmNdragCallback, redisplay_all_views_cb,
    view_struct);*/
  XtAddCallback(scale, XmNdragCallback, view_feedback_cb, view_struct);
  /*XtAddCallback(scale, XmNvalueChangedCallback, redisplay_all_views_cb,
    view_struct);*/
  XtAddCallback(scale, XmNvalueChangedCallback, view_feedback_cb,
		view_struct);
  return;
}

void ViewFeedback(
  Widget	w,
  XEvent	*event,
  String	*params,
  Cardinal	*num_params)
{
  ViewListEntry	*vl = global_view_list;
  Widget		dialog;

  /* get this view */
  if( (dialog = XtNameToWidget(w, "*view_dialog")) == NULL )
    return;;
  while( vl != NULL ){
    if( vl->view_struct->dialog == dialog )
      break;
    vl = vl->next;
  }
  if( vl == NULL )
    return;

  switch( event->type ){
  case FocusIn:
    vl->view_struct->controlFlag |= MAPAINT_HIGHLIGHT_SECTION;
    view_feedback_cb(w, (XtPointer) vl->view_struct, NULL);
    break;

  case FocusOut:
    if( event->xfocus.detail != NotifyInferior ){
      vl->view_struct->controlFlag &= ~MAPAINT_HIGHLIGHT_SECTION;
      view_feedback_cb(w, (XtPointer) vl->view_struct, NULL);
    }
    break;

  case MapNotify:
    if( vl->view_struct->wlzViewStr->initialised ){
      redisplay_view_cb(w, vl->view_struct, NULL);
    }
    else {
      display_view_cb(w, vl->view_struct, NULL);
    }
    break;

  default:
    break;
  }

  return;
}

void display_pointer_feedback_information(
  ThreeDViewStruct	*view_struct,
  int			x, 
  int			y)
{
  int			sel_domain;

  sel_domain = imageValueToDomain
    ((unsigned int) *((UBYTE *) (view_struct->ximage->data + x +
				 y * view_struct->ximage->bytes_per_line)));
  display_pointer_feedback_informationV(view_struct, x, y, sel_domain);
  return;
}
static char spacesBuf[256];

void display_pointer_feedback_informationV(
  ThreeDViewStruct	*view_struct,
  int			x, 
  int			y,
  int			sel_domain)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  int			widthp, heightp;
  WlzGreyValueWSpace	*gVWSp = NULL;
  int			kol, line, plane;
  char			str_buf[512];
  String		domainName;
  short			i, numCols, numSpaces;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
  if( (x < 0) || (x >= widthp) || (y < 0) || (y >= heightp) )
  {
    return;
  }

  kol = (int) (wlzViewStr->xp_to_x[x] + wlzViewStr->yp_to_x[y]);
  line = (int) (wlzViewStr->xp_to_y[x] + wlzViewStr->yp_to_y[y]);
  plane = (int) (wlzViewStr->xp_to_z[x] + wlzViewStr->yp_to_z[y]);

  /* get the selected domain */
  domainName = getAnatFullNameFromCoord(kol, line, plane);
  sel_domain = WLZ_MAX(sel_domain, 0);

  /* calculate the number of spaces */
  XtVaGetValues(view_struct->text, XmNcolumns, &numCols, NULL);
  numSpaces = strlen(domainName) - numCols - 1;
  if( numSpaces = WLZ_MAX(numSpaces, 0) ){
    for(i=0; i < numSpaces; i++){
      spacesBuf[i] = ' ';
    }
  }
  spacesBuf[numSpaces] = '\0';

  /* get the image grey-value */
  if( gVWSp = WlzGreyValueMakeWSp(globals.orig_obj, &errNum) ){
    WlzGreyValueGet(gVWSp, (double) plane, (double) line, (double) kol);
    switch(gVWSp->gType){
    case WLZ_GREY_INT:
      sprintf(str_buf, "%s(%d,%d,%d) %d: %s\n%s", spacesBuf, kol, line, plane,
	      gVWSp->gVal[0].inv, globals.domain_name[sel_domain], domainName);
      break;
    case WLZ_GREY_SHORT:
      sprintf(str_buf, "%s(%d,%d,%d) %d: %s\n%s", spacesBuf, kol, line, plane,
	      gVWSp->gVal[0].shv, globals.domain_name[sel_domain], domainName);
      break;
    case WLZ_GREY_UBYTE:
      sprintf(str_buf, "%s(%d,%d,%d) %d: %s\n%s", spacesBuf, kol, line, plane,
	      gVWSp->gVal[0].ubv, globals.domain_name[sel_domain], domainName);
      break;
    case WLZ_GREY_FLOAT:
      sprintf(str_buf, "%s(%d,%d,%d) %f: %s\n%s", spacesBuf, kol, line, plane,
	      gVWSp->gVal[0].flv, globals.domain_name[sel_domain], domainName);
      break;
    case WLZ_GREY_DOUBLE:
      sprintf(str_buf, "%s(%d,%d,%d) %f: %s\n%s", spacesBuf, kol, line, plane,
	      gVWSp->gVal[0].dbv, globals.domain_name[sel_domain], domainName);
      break;
    }
    WlzGreyValueFreeWSp(gVWSp);
  }
  else {
    MAPaintReportWlzError(globals.topl,
			  "display_pointer_feedback_informationV", errNum);
    sprintf(str_buf, "Something gone wrong see error message");
  }
  XtVaSetValues(view_struct->text,
		XmNvalue, 		str_buf,
		NULL);
  XmTextSetCursorPosition(view_struct->text, strlen(str_buf));

  return;
}
  

void view_canvas_highlight(
ThreeDViewStruct	*view_struct,
Boolean			highlight)
{
    Widget	widget1, widget2;
    Pixel	pixel;

    widget1 = XtNameToWidget(view_struct->dialog, "*.section_frame");
    widget2 = XtNameToWidget(view_struct->dialog, "*.scrolled_window");

    /* get the current domain colour */
    if( globals.current_domain > globals.cmapstruct->num_overlays ){
      pixel = globals.cmapstruct->ovly_cols[globals.current_domain];
    }
    else {
      pixel = globals.cmapstruct->ovly_cols[globals.current_domain] +
	globals.cmapstruct->gmax - globals.cmapstruct->gmin;
    }
    pixel = HGU_XGetColorPixel(globals.dpy, globals.cmap,
			       (float) globals.colormap[0][pixel]/255.0,
			       (float) globals.colormap[1][pixel]/255.0,
			       (float) globals.colormap[2][pixel]/255.0);

    if( highlight == True ){
	XtVaSetValues(widget1, XmNshadowType, XmSHADOW_IN, NULL);
	XtVaGetValues(widget2, XmNborderColor, &borderColor, NULL);
	XtVaSetValues(widget2, XmNborderColor, pixel, NULL);
    } else {
	XtVaSetValues(widget2, XmNborderColor, borderColor, NULL);
	XtVaSetValues(widget1, XmNshadowType, XmSHADOW_ETCHED_IN, NULL);
    }

    return;
}

static int paintingTrigger=0;

void canvas_input_cb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;

  /* switch on event type */
  switch( cbs->event->type ){

   case ButtonPress:
     switch( cbs->event->xbutton.button ){

      case Button1:
	if(cbs->event->xbutton.state & Mod1Mask){
	  x = cbs->event->xbutton.x / wlzViewStr->scale;
	  y = cbs->event->xbutton.y / wlzViewStr->scale;
	  display_pointer_feedback_information(view_struct, x, y);
	}
	else if( (!view_struct->noPaintingFlag) && (!globals.sectViewFlg) ){
	  paintingTrigger = 1;
	}
	break;

      case Button2:		/* domain and coordinate feedback */
	x = cbs->event->xbutton.x / wlzViewStr->scale;
	y = cbs->event->xbutton.y / wlzViewStr->scale;
	display_pointer_feedback_information(view_struct, x, y);
	break;

      case Button3:		/* unused */
      default:
	break;

     }
     break;

   case ButtonRelease:
     switch( cbs->event->xbutton.button ){

      case Button1:
	/* check if triggered */
	if( !paintingTrigger ){
	  break;
	}
	paintingTrigger = 0;
	/* get the paint selection */
	if( paint_key != NULL ){
	  break;
	}
	paint_key = view_struct;

	/* pause the 3D feedback display */
	HGUglwCanvasTbPause( globals.canvas );
	XtSetSensitive( globals.canvas, False );

	/* make controls insensitive */
	XtSetSensitive(view_struct->controls, False);
	XtSetSensitive(view_struct->slider, False);
	setControlButtonsSensitive(view_struct, False);

	/* highlight this canvas */
	view_canvas_highlight( view_struct, True );

	/* swap the callbacks to 2D painting mode and set the domains */
	XtRemoveCallback(w, XmNinputCallback, canvas_input_cb,
			 client_data);
	XtAddCallback(w, XmNinputCallback, canvas_2D_painting_cb,
		      client_data);
	installCurrentPaintTool(w, view_struct);

	getViewDomains(view_struct);

	break;

      case Button2:		/* domain and coordinate feedback */
/*	XtVaSetValues(view_struct->text, XmNvalue, "", NULL);*/
	break;

      default:
	break;

     }
     break;

   case MotionNotify:

     if((cbs->event->xmotion.state & Button2Mask) ||
        ((cbs->event->xmotion.state & Button1Mask) &&
	 (cbs->event->xmotion.state & Mod1Mask)))
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
      /* in this mode - next section */
      if( wlzViewStr->dist <= (wlzViewStr->maxvals.vtZ - 1.0) ){
	wlzViewStr->dist += 1.0;
	reset_view_struct( view_struct );
	display_view_cb(w, (XtPointer) view_struct, call_data);
	view_feedback_cb(w, (XtPointer) view_struct, NULL);
      }
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_b:
      /* in this mode - previous section */
      if( wlzViewStr->dist >= (wlzViewStr->minvals.vtZ + 1.0) ){
	wlzViewStr->dist -= 1.0;
	reset_view_struct( view_struct );
	display_view_cb(w, (XtPointer) view_struct, call_data);
	view_feedback_cb(w, (XtPointer) view_struct, NULL);
      }
      break;

    case XK_Down:
    case XK_n:
      break;

    case XK_w:
      /* get a filename for the view image */
      break;

    }
    break;

  default:
    break;
  }

  return;
}
 
void CanvasMotionEventHandler(
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

void CanvasButtonEventHandler(
  Widget        w,
  XtPointer     client_data,
  XEvent        *event,
  Boolean       *continue_to_dispatch)
{
  XmDrawingAreaCallbackStruct cbs;
  unsigned int	modMask=(ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|
			 Mod4Mask|Mod5Mask);

  /* call the canvas input callbacks for the special case of
     Ctrl<Btn1Down> which is currently trapped by something unknown */
  if((event->type == ButtonPress) &&
     (event->xbutton.button == Button1) &&
     ((event->xbutton.state & modMask) == ControlMask)){
    cbs.reason = XmCR_INPUT;
    cbs.event = event;
    cbs.window = XtWindow(w);
    XtCallCallbacks(w, XmNinputCallback, (XtPointer) &cbs);
  }

  return;
}

static void solid_FB_toggle_cb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmToggleButtonCallbackStruct	*tbCbStruct = 
    (XmToggleButtonCallbackStruct *) call_data;

  if( tbCbStruct->set ){
    view_struct->controlFlag |= MAPAINT_SHOW_SOLID_SECTION;
  }
  else {
    view_struct->controlFlag &= ~MAPAINT_SHOW_SOLID_SECTION;
  }
  view_feedback_cb(widget, (XtPointer) view_struct, NULL);

  return;
}

static void view_feedback_mode_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;

  /* clear feedback options */
  view_struct->controlFlag &= ~(MAPAINT_POLY_VIEWFB_MODE|
				MAPAINT_SOLID_VIEWFB_MODE|
				MAPAINT_MASK_VIEWFB_MODE|
				MAPAINT_TEMPLATE_VIEWFB_MODE|
				MAPAINT_SWITCH_VIEWFB_MODE|
				MAPAINT_IMAGE_VIEWFB_MODE);

  /* get current selection */
  if( strcmp(XtName(w), "poly") == 0 ){
    view_struct->controlFlag |= MAPAINT_POLY_VIEWFB_MODE;
  }
  else if( strcmp(XtName(w), "solid") == 0 ){
    view_struct->controlFlag |= MAPAINT_SOLID_VIEWFB_MODE;
  }
  else if( strcmp(XtName(w), "mask") == 0 ){
    view_struct->controlFlag |= MAPAINT_MASK_VIEWFB_MODE;
  }
  else if( strcmp(XtName(w), "template") == 0 ){
    view_struct->controlFlag |= MAPAINT_TEMPLATE_VIEWFB_MODE;
  }
  else if( strcmp(XtName(w), "switch") == 0 ){
    view_struct->controlFlag |= MAPAINT_SWITCH_VIEWFB_MODE;
  }
  else if( strcmp(XtName(w), "image") == 0 ){
    view_struct->controlFlag |= MAPAINT_IMAGE_VIEWFB_MODE;
  }
  else {
    view_struct->controlFlag |= MAPAINT_POLY_VIEWFB_MODE;
  }
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  return;
}

static void saveSectionCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  String		fileStr;
  FILE			*fp;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* get the view object */
  if( view_struct->view_object == NULL ){
    WlzObject	*tmpObj;
    if( tmpObj = WlzGetSectionFromObject(globals.orig_obj,
					 wlzViewStr, &errNum) ){
      view_struct->view_object = WlzAssignObject(tmpObj, NULL);
    }
    else {
      MAPaintReportWlzError(globals.topl, "saveSectionCb", errNum);
      return;
    }
  }

  /* get a filename for the section object */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a filename\n"
				      "for the section image which\n"
				      "be saved as a woolz object",
				      "OK", "cancel", "MAPaintSectObj.wlz",
				      NULL, "*.wlz") ){
    if( fp = fopen(fileStr, "w") ){
      if( WlzWriteObj(fp, view_struct->view_object) != WLZ_ERR_NONE ){
	HGU_XmUserError(globals.topl,
			"Save Section Image:\n"
			"    Incomplete write, please\n"
			"    check disk space or quotas.\n"
			"    Section image not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }
      if( fclose(fp) == EOF ){
	HGU_XmUserError(globals.topl,
			"Save Section Image:\n"
			"    File close error, please\n"
			"    check disk space or quotas.\n"
			"    Section image not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }

    }
    else {
      HGU_XmUserError(globals.topl,
		      "Save Section Image:\n"
		      "    Couldn't open the file for\n"
		      "    writing, please check\n"
		      "    permissions.\n"
		      "    Section image not saved",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }
    AlcFree(fileStr);
  }

  return;
}

static void view_direction_toggle_cb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmToggleButtonCallbackStruct	*tbCbStruct = 
    (XmToggleButtonCallbackStruct *) call_data;

  if( tbCbStruct->set ){
    view_struct->controlFlag |= MAPAINT_SHOW_VIEW_DIRECTION;
  }
  else {
    view_struct->controlFlag &= ~MAPAINT_SHOW_VIEW_DIRECTION;
  }
  view_feedback_cb(widget, (XtPointer) view_struct, NULL);

  return;
}

static void view_controls_cb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Widget		shell, dialog, cntrlFrame, cntrlForm;
  int			wasManaged;
  Dimension		shellHeight, cntrlFormHeight;

  /* get the section viewer frames and unmanage everything */
  dialog = view_struct->dialog;
  shell = XtParent(dialog);
  cntrlFrame = XtNameToWidget(dialog, "*.controls_frame");
  cntrlForm = XtNameToWidget(dialog, "*.top_controls_form");
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
  if( wasManaged == False ){
    XtManageChild(cntrlForm);
  }
  XtVaSetValues(shell, XmNheight, shellHeight, NULL);
  XtManageChild(cntrlFrame);

  return;
}

static void viewLogXScrollBarCB(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  /* log the scrollbar change */
  if( globals.logfileFp ){
    char strBuf[16];
    int code = 10;
    int	value;

    XtVaGetValues(widget, XmNvalue, &value, NULL);
    sprintf(strBuf, "%d", value);
    MAPaintLogData("xScrollBar", strBuf, code, view_struct->dialog);
  }

  return;
}

static void viewLogYScrollBarCB(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  /* log the scrollbar change */
  if( globals.logfileFp ){
    char strBuf[16];
    int code = 10;
    int	value;

    XtVaGetValues(widget, XmNvalue, &value, NULL);
    sprintf(strBuf, "%d", value);
    MAPaintLogData("yScrollBar", strBuf, code, view_struct->dialog);
  }

  return;
}

static String canvas_translations_table =
"<BtnMotion>: 	DrawingAreaInput()\n\
Ctrl<BtnDown>:	DrawingAreaInput()\n\
Ctrl<BtnUp>: 	DrawingAreaInput()";

static XtTranslations	translations=NULL;
static XtTranslations	canvas_translations=NULL;

Widget create_view_window_dialog(
  Widget	topl,
  double	theta,
  double        phi,
  WlzDVertex3	*fixed)
{
  Widget		dialog, control, frame, canvas, slider;
  Widget		scale, text, title_form;
  Widget		widget, form, scrolled_window, title, option_menu;
  Widget		row_col, button;
  Widget		top_controls_form, buttons;
  ThreeDViewStruct	*view_struct;
  WlzThreeDViewStruct	*wlzViewStr;
  ViewListEntry		*new_view_list;
  int			i, width, height;
  Atom			WM_DELETE_WINDOW;

  dialog = HGU_XmCreateStdDialog(topl, "view_dialog",
				 xmFormWidgetClass,
				 view_dialog_actions,
				 XtNumber(view_dialog_actions));
  XtAddCallback(dialog, XmNhelpCallback, HGU_XmHelpStandardCb,
		"paint/paint.html#view_dialog");

  control = XtNameToWidget( dialog, "*.control" );

  /* create and initialise the view structure */
  view_struct = (ThreeDViewStruct *) AlcMalloc(sizeof(ThreeDViewStruct));
  view_struct->dialog      = dialog;
  view_struct->controlFlag = 0;
  view_struct->controls    = NULL;
  view_struct->ximage      = NULL;
  view_struct->tablet_initialised = 0;
  view_struct->viewLockedFlag = 0;
  view_struct->display_list = (GLuint) 0;
  wlzViewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, NULL);
  view_struct->wlzViewStr = wlzViewStr;
  wlzViewStr->initialised = 0;
  wlzViewStr->scale       = 1.0;
  wlzViewStr->theta       = theta;
  wlzViewStr->phi         = phi;
  wlzViewStr->fixed       = *fixed;
  wlzViewStr->dist        = 0.0;
  wlzViewStr->up.vtX	  = 0.0;
  wlzViewStr->up.vtY	  = 0.0;
  wlzViewStr->up.vtZ	  = -1.0;
  wlzViewStr->view_mode   = WLZ_UP_IS_UP_MODE;

  for(i=0; i < 33; i++){
    view_struct->curr_domain[i] = NULL;
    view_struct->prev_domain[i] = NULL;
  }
  view_struct->prev_view_obj = NULL;
  view_struct->prev_dist = 0.0;
  view_struct->view_object = NULL;
  view_struct->painted_object = NULL;
  view_struct->masked_object = NULL;
  view_struct->noPaintingFlag = 0;
  view_struct->titleStr = "Section view";

  /* check logging */
  if( globals.logfileFp ){
    char strBuf[48];
    sprintf(strBuf, "(%f,%f)", wlzViewStr->dist, wlzViewStr->dist);
    MAPaintLogData("Distance", strBuf, 0, view_struct->dialog);
    sprintf(strBuf, "(%f,%f)", wlzViewStr->theta, wlzViewStr->theta);
    MAPaintLogData("Theta", strBuf, 0, view_struct->dialog);
    sprintf(strBuf, "(%f,%f)", wlzViewStr->phi, wlzViewStr->phi);
    MAPaintLogData("Phi", strBuf, 0, view_struct->dialog);
    sprintf(strBuf, "(%f,%f,%f)", wlzViewStr->fixed.vtX,
	    wlzViewStr->fixed.vtY, wlzViewStr->fixed.vtZ);
    MAPaintLogData("FixedPoint", strBuf, 0, view_struct->dialog);
    sprintf(strBuf, "%f", 1.0);
    MAPaintLogData("Scale", strBuf, 0, view_struct->dialog);
  }

  /* the section is within a frame with a text feed-back area plus the
     distance slider */
  frame = XtVaCreateManagedWidget("section_frame",
				  xmFrameWidgetClass, control,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  NULL);
  form = XtVaCreateManagedWidget("section_form",
				 xmFormWidgetClass, frame,
				 XmNchildType, XmFRAME_WORKAREA_CHILD,
				 NULL);
  title_form = XtVaCreateManagedWidget("section_title_form",
				       xmFormWidgetClass, frame,
				       XmNchildType, XmFRAME_TITLE_CHILD,
				       NULL);
  title = XtVaCreateManagedWidget("section_frame_title",
				  xmLabelWidgetClass, title_form,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNbottomAttachment,	XmATTACH_FORM,
				  NULL);

  for(i=0; feedback_mode_menu_itemsP[i].name != NULL; i++){
    feedback_mode_menu_itemsP[i].callback = view_feedback_mode_cb;
    feedback_mode_menu_itemsP[i].callback_data = view_struct;
  }
  option_menu = HGU_XmBuildMenu(title_form, XmMENU_OPTION,
				"view_feedback_mode", 0,
				XmTEAR_OFF_DISABLED,
				feedback_mode_menu_itemsP);
  XtVaSetValues(option_menu,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,	title,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);
  widget = option_menu;

  /*widget = XtVaCreateManagedWidget("section_frame_FB_toggle",
				   xmToggleButtonGadgetClass, title_form,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	title,
				   NULL);
  XtAddCallback(widget, XmNvalueChangedCallback, solid_FB_toggle_cb,
  view_struct);*/

  widget = XtVaCreateManagedWidget("section_frame_direction_toggle",
				   xmToggleButtonGadgetClass, title_form,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	widget,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNbottomAttachment,	XmATTACH_FORM,
				   NULL);
  XtAddCallback(widget, XmNvalueChangedCallback, view_direction_toggle_cb,
		view_struct);

  /* create a widget for text feedback from the view */
  text = XtVaCreateManagedWidget("text", xmTextWidgetClass, form,
				 XmNeditable,		False,
				 XmNeditMode,		XmMULTI_LINE_EDIT,
				 XmNrows,		2,
				 XmNcursorPositionVisible,	True,
				 XmNautoShowCursorPosition,	True,
				 XmNtopAttachment,	XmATTACH_FORM,
				 XmNleftAttachment,	XmATTACH_FORM,
				 XmNrightAttachment,	XmATTACH_FORM,
				 NULL);
  view_struct->text = text;

  /* create the distance slider */
  slider = HGU_XmCreateHorizontalSlider("distance_slider", form,
					0.0, -1000.0, 1000.0, 0,
					distance_cb, (XtPointer) view_struct);
  XtVaSetValues(slider,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		NULL);

  view_struct->slider = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, distance_cb, view_struct);
  add_feedback_callbacks(scale, view_struct);

  /* create the painting function buttons */
  row_col = XtVaCreateManagedWidget("paint_function_row_col",
				    xmRowColumnWidgetClass, form,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget,     text,
				    XmNbottomAttachment, XmATTACH_WIDGET,
				    XmNbottomWidget,     slider,
				    XmNleftAttachment,	XmATTACH_FORM,
				    XtNborderWidth,     0,
				    XmNpacking,	        XmPACK_COLUMN,
				    XmNnumColumns,      1,
				    XmNorientation,     XmVERTICAL,
				    XmNrowColumnType,   XmWORK_AREA,
				    NULL);
  button = XtVaCreateManagedWidget("undo", xmPushButtonWidgetClass,
				   row_col, NULL);
  XtAddCallback(button, XmNactivateCallback, undoCb, view_struct);

  button = XtVaCreateManagedWidget("redo", xmPushButtonWidgetClass,
				   row_col, NULL);
  XtAddCallback(button, XmNactivateCallback, redoCb, view_struct);

  button = XtVaCreateManagedWidget("mag+", xmPushButtonWidgetClass,
				   row_col, NULL);
  XtAddCallback(button, XmNactivateCallback, canvasMagPlusCb, view_struct);

  button = XtVaCreateManagedWidget("mag-", xmPushButtonWidgetClass,
				   row_col, NULL);
  XtAddCallback(button, XmNactivateCallback, canvasMagMinusCb, view_struct);

  /* create scrolled view */
  width = globals.obj->domain.p->lastkl - globals.obj->domain.p->kol1 + 1;
  height = globals.obj->domain.p->lastln - globals.obj->domain.p->line1 + 1;
  scrolled_window =
    XtVaCreateManagedWidget("scrolled_window",
			    xmScrolledWindowWidgetClass, form,
			    XmNscrollingPolicy, 	XmAUTOMATIC,
			    XmNwidth,  		width+6,
			    XmNheight, 		height+6,
			    XmNtopAttachment,		XmATTACH_WIDGET,
			    XmNtopWidget,		text,
			    XmNbottomAttachment,	XmATTACH_WIDGET,
			    XmNbottomWidget,		slider,
			    XmNleftAttachment,	XmATTACH_WIDGET,
			    XmNleftWidget,	row_col,
			    XmNrightAttachment,	XmATTACH_FORM,
			    NULL);

  /* add callbacks if logging */
  if(  globals.logfileFp ){
    Widget scrollBar;

    XtVaGetValues(scrolled_window, XmNhorizontalScrollBar, &scrollBar, NULL);
    if( scrollBar ){
      XtAddCallback(scrollBar, XmNdragCallback, viewLogXScrollBarCB, view_struct);
    }

    XtVaGetValues(scrolled_window, XmNverticalScrollBar, &scrollBar, NULL);
    if( scrollBar ){
      XtAddCallback(scrollBar, XmNdragCallback, viewLogYScrollBarCB, view_struct);
    }
  }
  canvas = XtVaCreateManagedWidget("canvas", hgu_DrawingAreaWidgetClass,
				   scrolled_window,
				   XmNwidth,  width,
				   XmNheight, height,
				   NULL);
  view_struct->canvas = canvas;

  /*if( !canvas_translations ){
    canvas_translations = XtParseTranslationTable( canvas_translations_table );
  }
  XtAugmentTranslations( canvas, canvas_translations );*/
  if( !translations ){
    translations = XtParseTranslationTable( translations_table );
  }
  XtAugmentTranslations( XtParent(dialog), translations );

  /* add the view struct to the dialog via the user-data */
  XtVaSetValues(dialog, XmNuserData, (XtPointer) view_struct, NULL);
  XtAddCallback(canvas, XmNexposeCallback, redisplay_view_cb, view_struct);
  XtAddCallback(canvas, XmNinputCallback, canvas_input_cb, view_struct);
  XtAddEventHandler(canvas, ButtonMotionMask|EnterWindowMask|LeaveWindowMask,
		    False, CanvasMotionEventHandler, view_struct);
  XtAddEventHandler(canvas, ButtonPressMask,
		    False, CanvasButtonEventHandler, view_struct);

  /* create the controls frame */
  frame = XtVaCreateManagedWidget("controls_frame",
				  xmFrameWidgetClass, control,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  XmNbottomAttachment,	XmATTACH_FORM,
				  NULL);
  top_controls_form = XtVaCreateWidget("top_controls_form",
			  xmFormWidgetClass, frame,
			  XmNchildType, XmFRAME_WORKAREA_CHILD,
			  NULL);
  form = XtVaCreateManagedWidget("controls_form",
				 xmFormWidgetClass, top_controls_form,
				 XmNleftAttachment,	XmATTACH_FORM,
				 XmNrightAttachment,	XmATTACH_FORM,
				 XmNtopAttachment,	XmATTACH_FORM,
				 NULL);
  view_struct->controls = form;
  title = XtVaCreateManagedWidget("controls_frame_title",
				  xmToggleButtonGadgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  XtAddCallback(title, XmNvalueChangedCallback, view_controls_cb,
		view_struct);

  /* add scale and view mode menus */
  for(i=0; scale_menu_itemsP[i].name != NULL; i++){
    scale_menu_itemsP[i].callback = scale_cb;
    scale_menu_itemsP[i].callback_data = view_struct;
  }
  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "view_scale", 0,
				XmTEAR_OFF_DISABLED, scale_menu_itemsP);

  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  widget = option_menu;
  XtManageChild(option_menu);

  for(i=0; view_mode_menu_itemsP[i].name != NULL; i++){
    view_mode_menu_itemsP[i].callback = view_mode_cb;
    view_mode_menu_itemsP[i].callback_data = view_struct;
  }
  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "view_mode", 0,
				XmTEAR_OFF_DISABLED,
				view_mode_menu_itemsP);

  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		widget,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);

  /* now the orientation sliders */
  slider = HGU_XmCreateHorizontalSlider("phi_slider", form,
					phi*180/WLZ_M_PI, 0.0, 180, 1,
					phi_cb, view_struct);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, phi_cb, view_struct);
  add_feedback_callbacks(scale, view_struct);

  slider = HGU_XmCreateHorizontalSlider("theta_slider", form,
					theta*180/WLZ_M_PI, 0.0, 360, 1,
					theta_cb, view_struct);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, theta_cb, view_struct);
  add_feedback_callbacks(scale, view_struct);

  slider = HGU_XmCreateHorizontalSlider("zeta_slider", form,
					0.0, 0.0, 360, 1,
					zeta_cb, view_struct);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  XtSetSensitive(slider, False);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, zeta_cb, view_struct);
  add_feedback_callbacks(scale, view_struct);

  slider = HGU_XmCreateHorizontalSlider("psi_slider", form,
					0.0, -180.0, 180.0, 1,
					psi_cb, view_struct);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, psi_cb, view_struct);
  add_feedback_callbacks(scale, view_struct);
  XtSetSensitive(slider, False);
  widget = slider;

  /* now the fixed point and line options */
  buttons = HGU_XmCreateWActionArea(top_controls_form,
				    view_dialog_controls_actions,
				    XtNumber(view_dialog_controls_actions),
				    xmDrawnButtonWidgetClass);

  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		view_struct->controls,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  view_struct->control_buttons = buttons;
			
  /* attach a time-delay (now right button)
     pulldown menu to the fixed-point buttons
     each set of menu items needs the callback and data set */
  widget = XtNameToWidget(buttons, "*.fixed_1");
  setupFixed_1_Menu(widget, view_struct);
  
  widget = XtNameToWidget(buttons, "*.fixed_2");
  setupFixed_2_Menu(widget, view_struct);
  
  widget = XtNameToWidget(buttons, "*.up_vector");
  setupUpVectorMenu(widget, view_struct);
  
  widget = XtNameToWidget(buttons, "*.save_settings");
  setupUpIOMenu(widget, view_struct);
  
  /* set the constraints for the section frame */
  frame = XtNameToWidget(dialog, "*.controls_frame");
  widget = XtNameToWidget(dialog, "*.section_frame");
  XtVaSetValues(widget,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget,     frame,
		NULL);

  /* add callbacks to the action buttons */
  if( (widget = XtNameToWidget(dialog, "*.save_section")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, saveSectionCb,
		  view_struct);
  }

  if( (widget = XtNameToWidget(dialog, "*.dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, sockCloseCb,
		  view_struct);
    XtAddCallback(widget, XmNactivateCallback, destroy_view_cb,
		  view_struct);
    XtAddCallback(widget, XmNactivateCallback, destroy_cb,
		  XtParent(dialog));
  }

  if( (widget = XtNameToWidget(dialog, "*.listen")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, sockListenCb,
		  view_struct);
  }

  if( (widget = XtNameToWidget(dialog, "*.help")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, HGU_XmHelpStandardCb,
		  "paint/paint.html#view_dialog");
  }

  /* set the dialog title */
  set_view_dialog_title(dialog, theta, phi);

  /* add dialog and view_structure to the view list */
  new_view_list = (ViewListEntry *) AlcMalloc(sizeof(ViewListEntry));
  new_view_list->dialog = dialog;
  new_view_list->view_struct = view_struct;
  new_view_list->next = global_view_list;
  global_view_list = new_view_list;

  /* add callbacks to the delete window protocol callback */
  WM_DELETE_WINDOW = XmInternAtom(XtDisplay(globals.topl),
				  "WM_DELETE_WINDOW", False);
  XmAddWMProtocols(XtParent(dialog), &WM_DELETE_WINDOW, 1);
  XmAddWMProtocolCallback(XtParent(dialog), WM_DELETE_WINDOW,
			  sockCloseCb, (XtPointer) view_struct);
  XmAddWMProtocolCallback(XtParent(dialog), WM_DELETE_WINDOW,
			  destroy_view_cb, (XtPointer) view_struct);
  XmAddWMProtocolCallback(XtParent(dialog), WM_DELETE_WINDOW,
			  destroy_cb, XtParent(dialog));
  

  return( dialog );
}


/* action and callback procedures */
void view_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzThreeDStdViews	type = (WlzThreeDStdViews) client_data;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  Widget		widget;
  WlzDVertex3		fixed;
  XColor		colorcell_def;
  XWindowAttributes	win_att;
  double		theta, phi;

  if( globals.obj == NULL )
    return;

  /* should call reset fixed point for this */
  fixed.vtX = (globals.obj->domain.p->kol1 + globals.obj->domain.p->lastkl)/2;
  fixed.vtY = (globals.obj->domain.p->line1 + globals.obj->domain.p->lastln)/2;
  fixed.vtZ = (globals.obj->domain.p->plane1 + globals.obj->domain.p->lastpl)/2;

  switch( type ){
  case WLZ_X_Y_VIEW:
    theta = 0.0;
    phi = 0.0;
    break;
  case WLZ_Y_Z_VIEW:
    theta = 0.0;
    phi = WLZ_M_PI/2;
    break;
  case WLZ_Z_X_VIEW:
    theta = WLZ_M_PI/2;
    phi = WLZ_M_PI/2;
    break;
  case WLZ_ARBITRARY_VIEW:
    theta = WLZ_M_PI/4;
    phi = WLZ_M_PI/4;
    break;
  default:
    return;
  }
  widget = create_view_window_dialog(globals.topl, theta, phi, &fixed);
  XtManageChild( widget );
  XtPopup(XtParent(widget), XtGrabNone);

  return;
}

int view_menu_init(
  Widget	topl)
{
  Display	*dpy = XtDisplay(topl);
  Window	win = XtWindow(topl);
  XGCValues	gc_values;
  char		*hostName;
  int		i, j;

  globals.gc_set = HGU_XCreateGC(dpy, win);
  (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
			       "green");
  gc_values.plane_mask = 255;
  XChangeGC(dpy, globals.gc_set, GCPlaneMask, &gc_values);

  /* initialise the undo list */
  initUndoList(0);

  return( 0 );
}
