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
*   File       :   viewFixedPointUtils.c				*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Thu Feb 26 11:51:08 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>
#include <bibFile.h>

extern void special_conf_display_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data);

static MenuItem fixed_1_menu_itemsP[] = {  /* fixed point optional menu */
  {"graphical_f1", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"textual_f1", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"fiducial_f1", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Reset", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"display_f1", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem fixed_2_menu_itemsP[] = {  /* fixed point optional menu */
  {"graphical_f2", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"textual_f2", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"fiducial_f2", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Cancel", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"display_f2", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem up_vect_menu_itemsP[] = {  /* fixed point optional menu */
  {"graphical_uv", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"textual_uv", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"fiducial_uv", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Reset", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"display_uv", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem io_menu_itemsP[] = {  /* controls values io menu */
  {"controls_write", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"controls_read", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"special_coords_write", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"special_conf_display", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

void HGU_XmTimeDelayPostMenuTOCb(
  XtPointer	client_data,
  XtIntervalId	*timerId)
{
  /* post the menu */
  XtManageChild((Widget) client_data);

  return;
}

void HGU_XmTimeDelayDisarmCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XtIntervalId	timerId = (XtIntervalId) client_data;

  XtRemoveTimeOut(timerId);
  return;
}

void HGU_XmTimeDelayPulldownCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *) call_data;
  XtAppContext	appCon = XtWidgetToApplicationContext(widget);
  XtIntervalId	timerId;

  /* position the menu and set a time delay callback */
  XmMenuPosition((Widget) client_data, &(cbs->event->xbutton));
  timerId = XtAppAddTimeOut(appCon, 400, HGU_XmTimeDelayPostMenuTOCb,
			    client_data);

  /* add a disarm callback to remove the timeout */
  XtAddCallback(widget, XmNdisarmCallback, HGU_XmTimeDelayDisarmCb,
		(XtPointer) timerId);

  return;
}
  
void HGU_XmAttachTimeDelayPulldown(
  Widget	widget,
  unsigned char	tear_off_model,
  MenuItem	*menuItems)
{
  Widget	menu;
  String	name;

  /* create the menu */
  if( name = (String) AlcMalloc(strlen(XtName(widget)) + 12) ){
    sprintf(name, "%s_td_popup", XtName(widget));
    menu = HGU_XmBuildMenu(XtParent(widget), XmMENU_POPUP, name, '\0',
			   tear_off_model, menuItems);

    /* attach an arm callback with timeout to post the menu */
    XtAddCallback(widget, XmNarmCallback, HGU_XmTimeDelayPulldownCb,
		  (XtPointer) menu);
  }
  else {
    MAPaintReportWlzError(globals.topl, "HGU_XmAttachTimeDelayPulldown",
			  WLZ_ERR_MEM_ALLOC);
  }

  return;
}

void setViewSliderSensitivities(
  ThreeDViewStruct	*view_struct,
  Boolean		bool)
{
  Widget	widget;

  if( bool == True ){
    widget = XtNameToWidget(view_struct->dialog, "*.theta_slider");
    XtSetSensitive(widget, True);
    widget = XtNameToWidget(view_struct->dialog, "*.phi_slider");
    XtSetSensitive(widget, True);
    if( view_struct->wlzViewStr->view_mode == WLZ_ZETA_MODE ){
      widget = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
      XtSetSensitive(widget, True);
    }
    widget = XtNameToWidget(view_struct->dialog, "*.psi_slider");
    XtSetSensitive(widget, False);
    widget = XtNameToWidget(view_struct->dialog, "*.view_mode");
    XtSetSensitive(widget, True);
  }
  else {
    widget = XtNameToWidget(view_struct->dialog, "*.theta_slider");
    XtSetSensitive(widget, False);
    widget = XtNameToWidget(view_struct->dialog, "*.phi_slider");
    XtSetSensitive(widget, False);
    widget = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
    XtSetSensitive(widget, False);
    widget = XtNameToWidget(view_struct->dialog, "*.psi_slider");
    XtSetSensitive(widget, True);
    widget = XtNameToWidget(view_struct->dialog, "*.view_mode");
    XtSetSensitive(widget, False);
  }
  
  return;
}

void fixed_1_text_in_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  String		fixedPointStr, defFPStr;
  WlzDVertex3		vtx;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
 

  /* get a new fixed point and set the distance to zero */
  if( (defFPStr = (String) AlcMalloc(64)) == NULL ){
    MAPaintReportWlzError(globals.topl, "fixed_1_text_in_cb",
			  WLZ_ERR_MEM_ALLOC);
    return;
  }
  (void) sprintf(defFPStr, "%.1f, %.1f, %.1f", wlzViewStr->fixed.vtX,
		 wlzViewStr->fixed.vtY, wlzViewStr->fixed.vtZ);
  fixedPointStr = HGU_XmUserGetstr(globals.topl,
				   "Please type in the new fixed point\n"
				   "coordinates: x-val, yval, zval.\n"
				   "Three values are required.\n",
				   "OK", "cancel", defFPStr);
  AlcFree(defFPStr);
  if( fixedPointStr ){
    if( sscanf(fixedPointStr, "%lg ,%lg ,%lg",
	       &vtx.vtX, &vtx.vtY, &vtx.vtZ) < 3 ){
      AlcFree((void *) fixedPointStr);
      return;
    }
    wlzViewStr->fixed = vtx;
    AlcFree((void *) fixedPointStr);
  }
  wlzViewStr->dist = 0.0;
  wlzViewStr->view_mode = WLZ_UP_IS_UP_MODE;
  reset_view_struct( view_struct );

  /* normal viewing sliders active and unset fixed-line mode */
  setViewSliderSensitivities(view_struct, True);
  view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void fixed_1_reset_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Widget		menu, widget;

  /* reset the fixed point to the volume centre and the distance to zero */
  wlzViewStr->fixed.vtX = (globals.obj->domain.p->kol1 +
			   globals.obj->domain.p->lastkl)/2;
  wlzViewStr->fixed.vtY = (globals.obj->domain.p->line1 +
			   globals.obj->domain.p->lastln)/2;
  wlzViewStr->fixed.vtZ = (globals.obj->domain.p->plane1 +
			   globals.obj->domain.p->lastpl)/2;

  wlzViewStr->dist = 0.0;

  /* reset the mode to the last selected */
  if( menu = XtNameToWidget(view_struct->dialog, "*view_mode") ){
    XtVaGetValues(menu, XmNmenuHistory, &widget, NULL);
    if( strcmp(XtName(widget), "up-is-up") == 0 ){
      wlzViewStr->view_mode = WLZ_UP_IS_UP_MODE;
    }
    else if( strcmp(XtName(widget), "statue") == 0 ){
      wlzViewStr->view_mode = WLZ_STATUE_MODE;
    }
    else {
      wlzViewStr->view_mode = WLZ_ZETA_MODE;
    }
  }
    
  /* normal viewing sliders active and unset fixed-line mode */
  setViewSliderSensitivities(view_struct, True);
  view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

  reset_view_struct( view_struct );

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void fixed_2_cancel_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Widget		menu, widget;

  /* reset the mode to the last selected */
  if( menu = XtNameToWidget(view_struct->dialog, "*view_mode") ){
    XtVaGetValues(menu, XmNmenuHistory, &widget, NULL);
    if( strcmp(XtName(widget), "up-is-up") == 0 ){
      wlzViewStr->view_mode = WLZ_UP_IS_UP_MODE;
    }
    else if( strcmp(XtName(widget), "statue") == 0 ){
      wlzViewStr->view_mode = WLZ_STATUE_MODE;
    }
    else {
      wlzViewStr->view_mode = WLZ_ZETA_MODE;
    }
  }

  /* normal viewing sliders active and unset fixed-line mode */
  setViewSliderSensitivities(view_struct, True);
  view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void up_vector_reset_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;

  wlzViewStr->up.vtX = 0.0;
  wlzViewStr->up.vtY = 0.0;
  wlzViewStr->up.vtZ = -1.0;
  reset_view_struct( view_struct );

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void fixed_1_display_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmToggleButtonCallbackStruct	*tbCbStruct = 
    (XmToggleButtonCallbackStruct *) call_data;
  
  if( tbCbStruct->set ){
    view_struct->controlFlag |= MAPAINT_SHOW_FIXED_POINT;
  }
  else {
    view_struct->controlFlag &= ~MAPAINT_SHOW_FIXED_POINT;
  }

  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  return;
}

void fixed_2_display_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmToggleButtonCallbackStruct	*tbCbStruct = 
    (XmToggleButtonCallbackStruct *) call_data;
  
  if( tbCbStruct->set ){
    view_struct->controlFlag |= MAPAINT_SHOW_FIXED_LINE;
  }
  else {
    view_struct->controlFlag &= ~MAPAINT_SHOW_FIXED_LINE;
  }

  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  return;
}

void up_vector_display_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmToggleButtonCallbackStruct	*tbCbStruct = 
    (XmToggleButtonCallbackStruct *) call_data;
  
  if( tbCbStruct->set ){
    view_struct->controlFlag |= MAPAINT_SHOW_UP_VECTOR;
  }
  else {
    view_struct->controlFlag &= ~MAPAINT_SHOW_UP_VECTOR;
  }

  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  return;
}

void fixed_1_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win  = XtWindow(view_struct->canvas);
  Widget		menu, widget;
  WlzFVertex2		*vtx;
  int			xp, yp;
  unsigned int	widthp, heightp;
  HGU_XInteractCallbacks	callbacks;

  if( !wlzViewStr->initialised ){
    if( init_view_struct( view_struct ) ){
      return;
    }
  }

  /* get the new fixed point */
  callbacks.window_func = NULL;
  callbacks.window_data = NULL;
  callbacks.non_window_func = (HGU_XInteractCallback) InteractDispatchEvent;
  callbacks.non_window_data = NULL;
  callbacks.help_func = NULL;
  callbacks.help_data = NULL;
  if( (vtx = HGU_XGetVtx(dpy, win, 0, &callbacks, NULL)) == NULL ){
    return;
  }

  /* get coord limits */
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;

  xp = (int) (vtx->vtX / wlzViewStr->scale);
  xp = (xp < 0) ? 0 : xp; xp = (xp > widthp) ? widthp : xp;
  yp = (int) (vtx->vtY / wlzViewStr->scale);
  yp = (yp < 0) ? 0 : yp; yp = (yp > heightp) ? heightp : yp;
  wlzViewStr->fixed.vtX = wlzViewStr->xp_to_x[xp] +
    wlzViewStr->yp_to_x[yp];
  wlzViewStr->fixed.vtY = wlzViewStr->xp_to_y[xp] +
    wlzViewStr->yp_to_y[yp];
  wlzViewStr->fixed.vtZ = wlzViewStr->xp_to_z[xp] +
    wlzViewStr->yp_to_z[yp];
  wlzViewStr->dist = 0.0;

  /* reset the mode to the last selected */
  if( menu = XtNameToWidget(view_struct->dialog, "*view_mode") ){
    XtVaGetValues(menu, XmNmenuHistory, &widget, NULL);
    if( strcmp(XtName(widget), "up-is-up") == 0 ){
      wlzViewStr->view_mode = WLZ_UP_IS_UP_MODE;
    }
    else if( strcmp(XtName(widget), "statue") == 0 ){
      wlzViewStr->view_mode = WLZ_STATUE_MODE;
    }
    else {
      wlzViewStr->view_mode = WLZ_ZETA_MODE;
    }
  }

  /* normal viewing sliders active and unset fixed-line mode */
  setViewSliderSensitivities(view_struct, True);
  view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

  reset_view_struct( view_struct );

  /* redisplay the section */
  XClearWindow(dpy, win);
  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

static int quitAtTwoVerticesCb(
  Display	*dpy,
  Window	win,
  caddr_t 	client_data,
  caddr_t	call_data)
{
  HGU_XGetVtxsCallbackStruct *cb_struct = 
    (HGU_XGetVtxsCallbackStruct *) call_data;

  if( cb_struct->nvtxs >= 2 ){
    return 1;
  }
  return 0;
}

void fixed_2_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win  = XtWindow(view_struct->canvas);
  Widget		widget;
  WlzFVertex2		*vtx, fpVtx;
  int			xp, yp;
  unsigned int		widthp, heightp;
  float			x, y, z, s;
  WlzPolygonDomain	*startPoly, *poly;
  HGU_XInteractCallbacks	callbacks;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  if( !wlzViewStr->initialised ){
    if( init_view_struct( view_struct ) )
      return;
  }

  /* get the new fixed_2 point - get as a polygon with the first point
     equal to the existing fixed point if the distance is zero */
  if( WLZ_NINT(wlzViewStr->dist) == 0 ){
    if( view_struct->view_object == NULL ){
      WlzObject	*tmpObj;
      if( tmpObj = WlzGetSectionFromObject(globals.orig_obj,
					   wlzViewStr, &errNum) ){
	view_struct->view_object = WlzAssignObject(tmpObj, NULL);
      }
      else {
	MAPaintReportWlzError(globals.topl, "fixed_2_cb", errNum);
	return;
      }
    }
    fpVtx.vtX = (0 - view_struct->view_object->domain.i->kol1)
      * wlzViewStr->scale;
    fpVtx.vtY = (0 - view_struct->view_object->domain.i->line1)
      * wlzViewStr->scale;
    if( startPoly = WlzMakePolyDmn(WLZ_POLYGON_FLOAT,
				   (WlzIVertex2 *) &fpVtx,
				   1, 1, 1, &errNum) ){
      startPoly = WlzAssignPolygonDomain(startPoly, NULL);
    }
    else {
      MAPaintReportWlzError(globals.topl, "fixed_2_cb", errNum);
      return;
    }
  }
  else {
    startPoly = NULL;
  }
  callbacks.window_func = quitAtTwoVerticesCb;
  callbacks.window_data = NULL;
  callbacks.non_window_func = (HGU_XInteractCallback) InteractDispatchEvent;
  callbacks.non_window_data = NULL;
  callbacks.help_func = NULL;
  callbacks.help_data = NULL;
  if( (poly = HGU_XGetPolydmn(dpy, win, 0, &callbacks, startPoly)) == NULL ){
    if( startPoly ){
      WlzFreePolyDmn(startPoly);
    }
    return;
  }
  if( startPoly ){
    WlzFreePolyDmn(startPoly);
  }
  if( poly->nvertices < 2 ){
    WlzFreePolyDmn(poly);
    return;
  }

  /* thus far therefore we have two fixed points. */
  setViewSliderSensitivities(view_struct, False);
  view_struct->controlFlag |= MAPAINT_FIXED_LINE_SET;
  widget = XtNameToWidget(view_struct->dialog, "*.psi_slider");
  HGU_XmSetSliderValue(widget, (float) 0.0);

  /* get coord limits */
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
  vtx = (WlzFVertex2 *) (poly->vtx);

  /* set the new fixed point */
  xp = (int) (vtx[0].vtX / wlzViewStr->scale);
  xp = (xp < 0) ? 0 : xp; xp = (xp > widthp) ? widthp : xp;
  yp = (int) (vtx[0].vtY / wlzViewStr->scale);
  yp = (yp < 0) ? 0 : yp; yp = (yp > heightp) ? heightp : yp;
  wlzViewStr->fixed.vtX = wlzViewStr->xp_to_x[xp] + wlzViewStr->yp_to_x[yp];
  wlzViewStr->fixed.vtY = wlzViewStr->xp_to_y[xp] + wlzViewStr->yp_to_y[yp];
  wlzViewStr->fixed.vtZ = wlzViewStr->xp_to_z[xp] + wlzViewStr->yp_to_z[yp];
  wlzViewStr->dist = 0.0;

  /* set the second fixed point, the angle in the view and the view mode */
  xp = (int) (vtx[1].vtX / wlzViewStr->scale);
  xp = (xp < 0) ? 0 : xp; xp = (xp > widthp) ? widthp : xp;
  yp = (int) (vtx[1].vtY / wlzViewStr->scale);
  yp = (yp < 0) ? 0 : yp; yp = (yp > heightp) ? heightp : yp;
  wlzViewStr->fixed_2.vtX = wlzViewStr->xp_to_x[xp] + wlzViewStr->yp_to_x[yp];
  wlzViewStr->fixed_2.vtY = wlzViewStr->xp_to_y[xp] + wlzViewStr->yp_to_y[yp];
  wlzViewStr->fixed_2.vtZ = wlzViewStr->xp_to_z[xp] + wlzViewStr->yp_to_z[yp];
  wlzViewStr->fixed_line_angle = atan2((double) (vtx[1].vtY - vtx[0].vtY),
				       (double) (vtx[1].vtX - vtx[0].vtX));
  wlzViewStr->view_mode = WLZ_FIXED_LINE_MODE;

  /* can now free the input polyline */
  WlzFreePolyDmn(poly);

  /* define the two orthogonal axes
     first the normalised direction vector between the fixed points */
  x = wlzViewStr->fixed.vtX - wlzViewStr->fixed_2.vtX;
  y = wlzViewStr->fixed.vtY - wlzViewStr->fixed_2.vtY;
  z = wlzViewStr->fixed.vtZ - wlzViewStr->fixed_2.vtZ;
  s = sqrt( (double) x*x + y*y + z*z + 0.00001 );
  if( s <= .001 ){
    x = 1.0; y = z = 0.0;
  } else {
    x /= s; y /= s; z /= s;
  }
  /* now the viewing direction which is by definition perpendicular
     to the vector between the fixed points - assumes that the fixed
     points have been defined on the same visible section */
  view_struct->norm2.vtX = wlzViewStr->rotation[2][0];
  view_struct->norm2.vtY = wlzViewStr->rotation[2][1];
  view_struct->norm2.vtZ = wlzViewStr->rotation[2][2];

  view_struct->norm3.vtX = y*view_struct->norm2.vtZ -
    z*view_struct->norm2.vtY;
  view_struct->norm3.vtY = z*view_struct->norm2.vtX -
    x*view_struct->norm2.vtZ;
  view_struct->norm3.vtZ = x*view_struct->norm2.vtY -
    y*view_struct->norm2.vtX;

  reset_view_struct( view_struct );

  /* redisplay the section */
  XClearWindow(dpy, win);
  display_view_cb(w, (XtPointer) view_struct, call_data);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void up_vector_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win  = XtWindow(view_struct->canvas);
  WlzFVertex2		*vtx;
  int			xp, yp;
  unsigned int		widthp, heightp;
  double		s;
  WlzPolygonDomain	*poly;
  HGU_XInteractCallbacks	callbacks;

  if( !wlzViewStr->initialised ){
    if( init_view_struct( view_struct ) )
      return;
  }

  /* get the new up vector - two vertex polygon */
  callbacks.window_func = quitAtTwoVerticesCb;
  callbacks.window_data = NULL;
  callbacks.non_window_func = NULL;
  callbacks.non_window_data = NULL;
  callbacks.help_func = NULL;
  callbacks.help_data = NULL;
  if( (poly = HGU_XGetPolydmn(dpy, win, 0, &callbacks, NULL)) == NULL ){
    return;
  }
  if( poly->nvertices < 2 ){
    WlzFreePolyDmn(poly);
    return;
  }

  /* thus far therefore we have two fixed points. */
  /* get coord limits */
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
  vtx = (WlzFVertex2 *) (poly->vtx);

  /* set the up vector from 1st point to 2nd point and normalise */
  xp = (int) (vtx[1].vtX / wlzViewStr->scale);
  xp = (xp < 0) ? 0 : xp; xp = (xp > widthp) ? widthp : xp;
  yp = (int) (vtx[1].vtY / wlzViewStr->scale);
  yp = (yp < 0) ? 0 : yp; yp = (yp > heightp) ? heightp : yp;
  wlzViewStr->up.vtX = wlzViewStr->xp_to_x[xp] + wlzViewStr->yp_to_x[yp];
  wlzViewStr->up.vtY = wlzViewStr->xp_to_y[xp] + wlzViewStr->yp_to_y[yp];
  wlzViewStr->up.vtZ = wlzViewStr->xp_to_z[xp] + wlzViewStr->yp_to_z[yp];

  xp = (int) (vtx[0].vtX / wlzViewStr->scale);
  xp = (xp < 0) ? 0 : xp; xp = (xp > widthp) ? widthp : xp;
  yp = (int) (vtx[0].vtY / wlzViewStr->scale);
  yp = (yp < 0) ? 0 : yp; yp = (yp > heightp) ? heightp : yp;
  wlzViewStr->up.vtX -= wlzViewStr->xp_to_x[xp] + wlzViewStr->yp_to_x[yp];
  wlzViewStr->up.vtY -= wlzViewStr->xp_to_y[xp] + wlzViewStr->yp_to_y[yp];
  wlzViewStr->up.vtZ -= wlzViewStr->xp_to_z[xp] + wlzViewStr->yp_to_z[yp];
  s = (wlzViewStr->up.vtX * wlzViewStr->up.vtX +
       wlzViewStr->up.vtY * wlzViewStr->up.vtY +
       wlzViewStr->up.vtZ * wlzViewStr->up.vtZ);
  s = sqrt(s);
  wlzViewStr->up.vtX /= s;
  wlzViewStr->up.vtY /= s;
  wlzViewStr->up.vtZ /= s;

  /* can now free the input polyline */
  WlzFreePolyDmn(poly);
  reset_view_struct( view_struct );

  /* redisplay the section */
  XClearWindow(dpy, win);
  display_view_cb(w, (XtPointer) view_struct, call_data);
  view_feedback_cb(w, (XtPointer) view_struct, NULL);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void controls_io_write_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  String		fileStr;
  FILE			*fp;
  WlzErrorNum		errNum;

  /* call the disarm callbacks to remove any timeouts and avoid posting
     th emenu */
  XtCallCallbacks(w, XmNdisarmCallback, call_data);

  /* get a filename for the section parameters */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a filename\n"
				      "for the section view parameters\n"
				      "to be saved as plain text",
				      "OK", "cancel", "MAPaintSectParams.bib",
				      NULL, "*.bib") ){
    if( fp = fopen(fileStr, "w") ){
      if( WlzEffBibWrite3DSectionViewParamsRecord(fp, "Wlz3DSectionViewParams",
					      wlzViewStr) != WLZ_ERR_NONE ){
	HGU_XmUserError(globals.topl,
			"Save Section Parameters:\n"
			"    Error in writing the bibfile\n"
			"    Please check disk space or quotas\n"
			"    Section parameters not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }

      if( fclose(fp) == EOF ){
	HGU_XmUserError(globals.topl,
			"Save Section Parameters:\n"
			"    Error in closing the bibfile\n"
			"    Please check disk space or quotas\n"
			"    Section parameters not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }
    }
    else {
      HGU_XmUserError(globals.topl,
		      "Save Section Parameters:\n"
		      "    Couldn't open the file for\n"
		      "    writing, please check\n"
		      "    permissions.\n"
		      "    Section parameters not saved",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }
    AlcFree(fileStr);
  }

  return;
}

static FILE	*specialFile;
static int	numCoords;
static int	x1, x2, y11, y2, z1, z2;
static int	specialTriggered;

void special_input_cb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y, widthp, heightp, domain;
  int			kol, line, plane;
  double		distance;
  int			i;
  Widget		widget;
  Boolean		toggleSet;

  /* switch on event type */
  switch( cbs->event->type ){

   case ButtonPress:
     switch( cbs->event->xbutton.button ){

      case Button1:
	specialTriggered = 1;
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
	x = cbs->event->xbutton.x / wlzViewStr->scale;
	y = cbs->event->xbutton.y / wlzViewStr->scale;
	widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
	heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
	if( (x < 0) || (x >= widthp) || (y < 0) || (y >= heightp) )
	{
	  return;
	}
	kol = (int) (wlzViewStr->xp_to_x[x] + wlzViewStr->yp_to_x[y]);
	line = (int) (wlzViewStr->xp_to_y[x] + wlzViewStr->yp_to_y[y]);
	plane = (int) (wlzViewStr->xp_to_z[x] + wlzViewStr->yp_to_z[y]);

	if( specialTriggered ){
	  switch( numCoords ){
	  case 0:
	    x1 = kol;
	    y11 = line;
	    z1 = plane;
	    numCoords = 1;
	    break;

	  case 1:
	    if( plane == z1 ){
	      x2 = kol;
	      y2 = line;
	      z2 = plane;

	      distance = (x1-x2)*(x1-x2) + (y11-y2)*(y11-y2) +
		(z1-z2)*(z1-z2);
	      if( distance > 0.0 ){
		distance = sqrt(distance);
	      }

	      /* output coords and distance */
	      if( specialFile ){
		fprintf(specialFile,
			"r1 = (%d, %d, %d), "
			"r2 = (%d, %d, %d), "
			"distance = %f\n",
			x1, y11, z1, x2, y2, z2, distance);
	      }

	      /* reset */
	      numCoords = 0;
	    }
	    else {
	      x1 = kol;
	      y11 = line;
	      z1 = plane;
	      numCoords = 1;
	    }
	    break;
	  }
	}
	specialTriggered = 0;
	break;

     case Button2:
       x = cbs->event->xbutton.x / wlzViewStr->scale;
       y = cbs->event->xbutton.y / wlzViewStr->scale;
       domain = imageValueToDomain
	 ((unsigned int) *((UBYTE *) (view_struct->ximage->data + x +
				      y * view_struct->ximage->bytes_per_line)));
       widthp = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
       x = WLZ_MAX(0, x);
       x = WLZ_MIN(x, widthp);
       display_pointer_feedback_informationV(view_struct, x, y, domain);
       break;

     case Button3:
       /* close the output file */
       if( specialFile ){
	 fclose(specialFile);
	 specialFile = NULL;
       }

       /* revert to normal mode */
       view_struct->noPaintingFlag = 0;
       view_struct->titleStr = "Section view";
       set_view_dialog_title(view_struct->dialog,
			     wlzViewStr->theta,
			     wlzViewStr->phi);
       /* set sensitivities */
       XtSetSensitive(view_struct->controls, True);
       setControlButtonsSensitive(view_struct, True);

       /* swap the callbacks to special mode */
       XtRemoveCallback(view_struct->canvas, XmNinputCallback,
			special_input_cb, client_data);
       XtAddCallback(view_struct->canvas, XmNinputCallback,
		     canvas_input_cb, client_data);

       break;

     default:
       break;

     }
     break;

   case MotionNotify:
     if( cbs->event->xmotion.state & Button1Mask ){
     }

     if( cbs->event->xmotion.state & Button2Mask )
     {
	x = cbs->event->xmotion.x / wlzViewStr->scale;
	y = cbs->event->xmotion.y / wlzViewStr->scale;
	domain = imageValueToDomain
	  ((unsigned int) *((UBYTE *) (view_struct->ximage->data + x +
				       y * view_struct->ximage->bytes_per_line)));
	widthp = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
	x = WLZ_MAX(0, x);
	x = WLZ_MIN(x, widthp);
	display_pointer_feedback_informationV(view_struct, x, y, domain);
     }
     break;

  case KeyPress:
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      /* in this mode - next section */
      wlzViewStr->dist += 1.0;
      reset_view_struct( view_struct );
      display_view_cb(w, (XtPointer) view_struct, call_data);
      view_feedback_cb(w, (XtPointer) view_struct, NULL);
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_b:
      /* in this mode - previous section */
      wlzViewStr->dist -= 1.0;
      reset_view_struct( view_struct );
      display_view_cb(w, (XtPointer) view_struct, call_data);
      view_feedback_cb(w, (XtPointer) view_struct, NULL);
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

void special_coords_write_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  String		fileStr;
  FILE			*fp;

  /* call the disarm callbacks to remove any timeouts and avoid posting
     the menu */
  XtCallCallbacks(w, XmNdisarmCallback, call_data);

  /* get a filename for the section parameters */
  if( fileStr =
     HGU_XmUserGetFilename(globals.topl,
			   "Please type in a filename\n"
			   "for the coordinate parameters\n"
			   "to be saved as plain text",
			   "OK", "cancel", "MAPaint_special.txt",
			   NULL, "*.txt") ){

    /* if already painting do nothing */
    if( paint_key == view_struct ){
      return;
    }
    if( (fp = fopen(fileStr, "w")) == NULL ){
      return;
    }

    /* install a call back, don't allow painting */
    view_struct->noPaintingFlag = 1;
    view_struct->titleStr = "Coords Special dialog";
    set_view_dialog_title(view_struct->dialog,
			  wlzViewStr->theta,
			  wlzViewStr->phi);

    /* set sensitivities */
    XtSetSensitive(view_struct->controls, False);
    setControlButtonsSensitive(view_struct, False);

    /* swap the callbacks to special mode */
    XtRemoveCallback(view_struct->canvas, XmNinputCallback,
		     canvas_input_cb, client_data);
    XtAddCallback(view_struct->canvas, XmNinputCallback,
		  special_input_cb, client_data);

    /* set the control globals */
    specialFile = fp;
    numCoords = 0;
    specialTriggered = 0;
  }

  return;
}

int controls_io_quiet_read(
  String		fileStr,
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win  = XtWindow(view_struct->canvas);
  Widget		slider;
  FILE			*fp;
  double		oldScale, newScale;
  int			rtnVal=0;

  /* get a filename for the section parameters */
  if( fileStr == NULL ){
    rtnVal = 1;
  }
  else {
    if( fp = fopen(fileStr, "rb") ){
      BibFileRecord	*bibfileRecord;
      BibFileError	bibFileErr;
      char		*errMsg;

      /* read the bibfile - get the first section view entry */
      bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
      while((bibFileErr == BIBFILE_ER_NONE) &&
	    (strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22))){
	BibFileRecordFree(&bibfileRecord);
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
      }
      (void) fclose(fp);
      if( bibFileErr != BIBFILE_ER_NONE ){
	AlcFree((void *) errMsg);
	rtnVal = 1;
      }

      /* parse the record  - need to save the old scale */
      if( !rtnVal ){
	oldScale = wlzViewStr->scale;
	WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, wlzViewStr);
	BibFileRecordFree(&bibfileRecord);

	/* normal viewing sliders active and unset fixed-line mode */
	setViewSliderSensitivities(view_struct, True);
	view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

	/* reset the sliders and mode control */
	slider = XtNameToWidget(view_struct->dialog, "*.theta_slider");
	HGU_XmSetSliderValue(slider, (float) (wlzViewStr->theta * 180.0 /
					      WLZ_M_PI));
	slider = XtNameToWidget(view_struct->dialog, "*.phi_slider");
	HGU_XmSetSliderValue(slider, (float) (wlzViewStr->phi * 180.0 /
					      WLZ_M_PI));
	slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
	HGU_XmSetSliderValue(slider, (float) (wlzViewStr->zeta * 180.0 /
					      WLZ_M_PI));
	newScale = wlzViewStr->scale;
	wlzViewStr->scale = oldScale;
	setViewMode(view_struct, wlzViewStr->view_mode);
	setViewScale(view_struct, newScale, 0, 0);

	/* redisplay the section */
	reset_view_struct( view_struct );
	XClearWindow(dpy, win);
	display_view_cb(view_struct->canvas, (XtPointer) view_struct, NULL);
	view_feedback_cb(view_struct->canvas, (XtPointer) view_struct, NULL);

	/* clear previous domains */
	view_struct_clear_prev_obj( view_struct );

      }
      else {
	rtnVal = 1;
      }
    }
  }

  return rtnVal;
}

void controls_io_read_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win  = XtWindow(view_struct->canvas);
  Widget		slider;
  String		fileStr;
  FILE			*fp;
  double		oldScale, newScale;

  /* call the disarm callbacks to remove any timeouts and avoid posting
     th emenu */
  XtCallCallbacks(w, XmNdisarmCallback, call_data);

  /* get a filename for the section parameters */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a bibfile name\n"
				      "for the section view parameters\n"
				      "to be read in",
				      "OK", "cancel",
				      "MAPaintSectParams.bib",
				      NULL, "*.bib")
    ){
    if( fp = fopen(fileStr, "r") ){
      BibFileRecord	*bibfileRecord;
      BibFileError	bibFileErr;
      char		*errMsg;

      /* read the bibfile - get the first section view entry */
      bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
      while((bibFileErr == BIBFILE_ER_NONE) &&
	    (strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22))){
	BibFileRecordFree(&bibfileRecord);
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
      }
      (void) fclose(fp);
      if( bibFileErr != BIBFILE_ER_NONE ){
	HGU_XmUserError(globals.topl,
			"Read Section Parameters:\n"
			"    Error in reading the bibfile\n"
			"    Section parameters not set",
			XmDIALOG_FULL_APPLICATION_MODAL);
	AlcFree((void *) errMsg);
	return;
      }

      /* parse the record  - need to save the old scale */
      oldScale = wlzViewStr->scale;
      WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, wlzViewStr);
      BibFileRecordFree(&bibfileRecord);
      (void) fclose(fp);

      /* normal viewing sliders active and unset fixed-line mode */
      setViewSliderSensitivities(view_struct, True);
      view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

      /* reset the sliders and mode control */
      slider = XtNameToWidget(view_struct->dialog, "*.theta_slider");
      HGU_XmSetSliderValue(slider, (float) (wlzViewStr->theta * 180.0 /
					    WLZ_M_PI));
      slider = XtNameToWidget(view_struct->dialog, "*.phi_slider");
      HGU_XmSetSliderValue(slider, (float) (wlzViewStr->phi * 180.0 /
					    WLZ_M_PI));
      slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
      HGU_XmSetSliderValue(slider, (float) (wlzViewStr->zeta * 180.0 /
					    WLZ_M_PI));
      newScale = wlzViewStr->scale;
      wlzViewStr->scale = oldScale;
      setViewMode(view_struct, wlzViewStr->view_mode);
      setViewScale(view_struct, newScale, 0, 0);

      /* redisplay the section */
      reset_view_struct( view_struct );
      XClearWindow(dpy, win);
      display_view_cb(w, (XtPointer) view_struct, call_data);
      view_feedback_cb(w, (XtPointer) view_struct, NULL);

      /* clear previous domains */
      view_struct_clear_prev_obj( view_struct );

    }
    else {
      HGU_XmUserError(globals.topl,
		      "Read Section Parameters:\n"
		      "    Couldn't open the file for\n"
		      "    reading, please check\n"
		      "    permissions.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }
    AlcFree(fileStr);
  }

  return;
}

void PostIt(
  Widget	pb,
  XtPointer	client_data,
  XEvent	*event,
  Boolean	*continue_to_dispatch)
{
  Widget	popup = (Widget) client_data;
  XButtonPressedEvent	*bevent=(XButtonPressedEvent *) event;

  if( bevent->button != 3 ){
    return;
  }

  /* check sensitivity */
  if( XtIsSensitive( pb ) ){
    XmMenuPosition(popup, bevent);
    XtManageChild( popup );
  }
  else {
    *continue_to_dispatch = False;
  }

  return;
}

void popupDrawnButtonCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmDrawnButtonCallbackStruct
    *cbs=(XmDrawnButtonCallbackStruct *) call_data;
  Dimension	ht, st, width, height;
  Display	*dpy = XtDisplay(w);
  Window	win = XtWindow(w);
  Pixel		bottomShadowColor, topShadowColor;
  GC		gc;
  Position	x, y;
  XPoint	points[8];

  /* check if realised */
  if( XtIsRealized(w) == False ){
    return;
  }

  /* get widget parameters */
  XtVaGetValues(w,
		XmNwidth,	&width,
		XmNheight,	&height,
		XmNhighlightThickness,	&ht,
		XmNshadowThickness,	&st,
		XmNbottomShadowColor,	&bottomShadowColor,
		XmNtopShadowColor,	&topShadowColor,
		NULL);

  /* set the x, y position and the clip rectangle */
  x = width - ht - st - 15;
  y = height/2 - 5;

  gc = globals.gc_set;
  XSetLineAttributes(dpy, gc, st, LineSolid, CapButt, JoinMiter);

  /* draw the wee down arrow */
  points[0].x = x + 10;
  points[0].y = y;
  points[1].x = x;
  points[1].y = y;
  points[2].x = x + 5;
  points[2].y = y + 9;
  points[3].x = x + 10;
  points[3].y = y;
  XSetForeground(dpy, gc, topShadowColor);
  XDrawLines(dpy, win, gc, points, 4, CoordModeOrigin);
  points[0].x = x + 10;
  points[0].y = y;
  points[1].x = x + 5;
  points[1].y = y + 9;
  XSetForeground(dpy, gc, bottomShadowColor);
  XDrawLines(dpy, win, gc, points, 2, CoordModeOrigin);

  return;
}

void setupFixed_1_Menu(
  Widget		widget,
  ThreeDViewStruct	*view_struct)
{
  Widget	popup;

  /* set the push button enabled resource to give 3d shadow */
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL);

  /* add the callbacks */
/*  XtAddCallback(widget, XmNactivateCallback, fixed_1_cb, view_struct);*/
  XtAddCallback(widget, XmNexposeCallback, popupDrawnButtonCb, NULL);
  XtAddCallback(widget, XmNresizeCallback, popupDrawnButtonCb, NULL);

  /* set the menu item callbacks */
  fixed_1_menu_itemsP[0].callback = fixed_1_cb;
  fixed_1_menu_itemsP[0].callback_data = (XtPointer) view_struct;
  fixed_1_menu_itemsP[1].callback = fixed_1_text_in_cb;
  fixed_1_menu_itemsP[1].callback_data = (XtPointer) view_struct;
  fixed_1_menu_itemsP[3].callback = fixed_1_reset_cb;
  fixed_1_menu_itemsP[3].callback_data = (XtPointer) view_struct;
  fixed_1_menu_itemsP[4].callback = fixed_1_display_cb;
  fixed_1_menu_itemsP[4].callback_data = (XtPointer) view_struct;

/*  HGU_XmAttachTimeDelayPulldown(widget, XmTEAR_OFF_DISABLED,
				fixed_1_menu_itemsP);*/
  popup = HGU_XmBuildMenu(widget, XmMENU_POPUP, XtName(widget), '\0',
			  XmTEAR_OFF_ENABLED, fixed_1_menu_itemsP);
  XtAddEventHandler(widget, ButtonPressMask, True, PostIt, popup);

  return;
}

void setupFixed_2_Menu(
  Widget		widget,
  ThreeDViewStruct	*view_struct)
{
  Widget	popup;

  /* set the push button enabled resource to give 3d shadow */
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL);

  /* add the callbacks */
/*  XtAddCallback(widget, XmNactivateCallback, fixed_2_cb, view_struct);*/
  XtAddCallback(widget, XmNexposeCallback, popupDrawnButtonCb, NULL);
  XtAddCallback(widget, XmNresizeCallback, popupDrawnButtonCb, NULL);

  /* set the menu item callbacks */
  fixed_2_menu_itemsP[0].callback = fixed_2_cb;
  fixed_2_menu_itemsP[0].callback_data = (XtPointer) view_struct;
  fixed_2_menu_itemsP[3].callback = fixed_2_cancel_cb;
  fixed_2_menu_itemsP[3].callback_data = (XtPointer) view_struct;
  fixed_2_menu_itemsP[4].callback = fixed_2_display_cb;
  fixed_2_menu_itemsP[4].callback_data = (XtPointer) view_struct;

/*  HGU_XmAttachTimeDelayPulldown(widget, XmTEAR_OFF_DISABLED,
				fixed_2_menu_itemsP);*/
  popup = HGU_XmBuildMenu(widget, XmMENU_POPUP, XtName(widget), '\0',
			 XmTEAR_OFF_ENABLED, fixed_2_menu_itemsP);
  XtAddEventHandler(widget, ButtonPressMask, False, PostIt, popup);

  return;
}

void setupUpVectorMenu(
  Widget		widget,
  ThreeDViewStruct	*view_struct)
{
  Widget	popup;

  /* set the push button enabled resource to give 3d shadow */
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL);

  /* add the callbacks */
/*  XtAddCallback(widget, XmNactivateCallback, up_vector_cb, view_struct);*/
  XtAddCallback(widget, XmNexposeCallback, popupDrawnButtonCb, NULL);
  XtAddCallback(widget, XmNresizeCallback, popupDrawnButtonCb, NULL);

  /* set the menu item callbacks */
  up_vect_menu_itemsP[0].callback = up_vector_cb;
  up_vect_menu_itemsP[0].callback_data = (XtPointer) view_struct;
  up_vect_menu_itemsP[3].callback = up_vector_reset_cb;
  up_vect_menu_itemsP[3].callback_data = (XtPointer) view_struct;
  up_vect_menu_itemsP[4].callback = up_vector_display_cb;
  up_vect_menu_itemsP[4].callback_data = (XtPointer) view_struct;

/*  HGU_XmAttachTimeDelayPulldown(widget, XmTEAR_OFF_DISABLED,
				up_vect_menu_itemsP);*/
  popup = HGU_XmBuildMenu(widget, XmMENU_POPUP, XtName(widget), '\0',
			 XmTEAR_OFF_ENABLED, up_vect_menu_itemsP);
  XtAddEventHandler(widget, ButtonPressMask, False, PostIt, popup);

  return;
}

void setupUpIOMenu(
  Widget		widget,
  ThreeDViewStruct	*view_struct)
{
  Widget	popup;

  /* set the push button enabled resource to give 3d shadow */
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL);

  /* add the callbacks */
/*  XtAddCallback(widget, XmNactivateCallback,
    controls_io_write_cb, view_struct);*/
  XtAddCallback(widget, XmNexposeCallback, popupDrawnButtonCb, NULL);
  XtAddCallback(widget, XmNresizeCallback, popupDrawnButtonCb, NULL);

  /* set the menu item callbacks */
  io_menu_itemsP[0].callback = controls_io_write_cb;
  io_menu_itemsP[0].callback_data = (XtPointer) view_struct;
  io_menu_itemsP[1].callback = controls_io_read_cb;
  io_menu_itemsP[1].callback_data = (XtPointer) view_struct;
  io_menu_itemsP[2].callback = special_coords_write_cb;
  io_menu_itemsP[2].callback_data = (XtPointer) view_struct;
  io_menu_itemsP[3].callback = special_conf_display_cb;
  io_menu_itemsP[3].callback_data = (XtPointer) view_struct;
  
/*  HGU_XmAttachTimeDelayPulldown(widget, XmTEAR_OFF_DISABLED,
				io_menu_itemsP);*/
  popup = HGU_XmBuildMenu(widget, XmMENU_POPUP, XtName(widget), '\0',
			 XmTEAR_OFF_ENABLED, io_menu_itemsP);
  XtAddEventHandler(widget, ButtonPressMask, False, PostIt, popup);

  return;
}

