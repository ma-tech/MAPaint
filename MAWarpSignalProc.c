#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   MAPaint						*
*   File       :   MAWarpSignalProc.c					*
*************************************************************************
*   Author Name :  richard						*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri May  2 17:55:41 2003				*
*   $Revision$					       		*
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
#include <unistd.h>

#include <MAPaint.h>
#include <MAWarp.h>

void warpSgnlDomainCanvasExposeCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  return;
}

WlzErrorNum warpResetSignalObj(
  WlzObject		*obj)
{
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* set the source object */
  if( warpGlobals.sgnl.obj ){
    WlzFreeObj(warpGlobals.sgnl.obj);
  }
  warpGlobals.sgnl.obj = WlzAssignObject(obj, &errNum);
  warpSetXImage(&(warpGlobals.sgnl));
  warpCanvasExposeCb(warpGlobals.sgnl.canvas,
		     (XtPointer) &(warpGlobals.sgnl),
		     NULL);

  /* reset the processed object and signal domain */
  if( warpGlobals.sgnlProcObj ){
    WlzFreeObj( warpGlobals.sgnlProcObj );
    warpGlobals.sgnlProcObj = NULL;
  }
  if( warpGlobals.sgnlThreshObj ){
    WlzFreeObj(warpGlobals.sgnlThreshObj);
    warpGlobals.sgnlThreshObj = NULL;
  }
  if( warpGlobals.sgnlObj ){
    warpCanvasExposeCb(globals.topl, (XtPointer) &(warpGlobals.sgnl), NULL);
  }

  /* note need to cancel any local thresholding */
  warpGlobals.globalThreshVtx.vtX = -10000;
  warpGlobals.globalThreshVtx.vtY = -10000;

  /* increment if requested? - new object switch off and clear */
  warpSwitchIncrementDomain(0);
  sgnlIncrClear();
  warpSetSignalDomain(NULL);
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  /* set threshold button sensitivities */
  if( WlzGreyTypeFromObj(obj, NULL) == WLZ_GREY_RGBA ){
    Widget option_menu, option;
    warpSetThreshColorTypeSensitive( True );
    if( option_menu = 
       XtNameToWidget(globals.topl,
		      "*warp_sgnl_controls_form*color_space") ){
      XtVaGetValues(option_menu, XmNmenuHistory, &option, NULL);
      XtCallCallbacks(option, XmNactivateCallback, NULL);
    }
  }
  else {
    warpSetThreshColorTypeSensitive( False );
    warpGlobals.threshRGBSpace = WLZ_RGBA_SPACE_GREY;
  }

  /* if colour image and grey channel reset sliders */
  warpResetThresholdSliderRange();

  return errNum;
}

void warpReadSignalCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzObject		*obj;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check we can get the object */
  if( (obj = HGU_XmReadExtFFObject(warp_read_sgnl_dialog,
				  cbs,
				  &(warpGlobals.sgnlFileType),
				  &errNum)) == NULL ){
    return;
  }
  if( warpGlobals.sgnlFile ){
    AlcFree(warpGlobals.sgnlFile);
  }
  warpGlobals.sgnlFile = HGU_XmGetFileStr(view_struct->dialog, cbs->value,
				 cbs->dir);

  /* check the object */
  /* if read incomplete or 3D may wish to try anyway */
  if( errNum == WLZ_ERR_READ_INCOMPLETE ){
    errNum = WLZ_ERR_NONE;
  }
  if( errNum == WLZ_ERR_NONE ){
    if( obj ){
      switch( obj->type ){
      case WLZ_2D_DOMAINOBJ:
	/* set the source object */
	errNum = warpResetSignalObj(obj);
	break;

      default:
	WlzFreeObj(obj);
	HGU_XmUserError(globals.topl,
			"Read Signal Object:\n"
			"    Wrong object type, must be 2D,\n"
			"    please select another image file.",
			XmDIALOG_FULL_APPLICATION_MODAL);
	break;
      }
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "warpReadSignalCb", errNum);
  }
  return;
}

void warpResetCWDCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Widget	toggle;
  String	dirStr;
  Boolean	setFlg;

  if( toggle = XtNameToWidget(w, "*.reset_cwd") ){
    XtVaGetValues(toggle, XmNset, &setFlg, NULL);
    if( setFlg == True ){
      if( XmStringGetLtoR(cbs->dir, XmSTRING_DEFAULT_CHARSET, &dirStr) ){
	if( chdir((const char *) dirStr) ){
	  /* report an error */
	  HGU_XmUserError(view_struct->dialog,
			  "Read signal image callback:\n"
			  "  Failed to change current working\n"
			  "  directory (folder). For the next\n"
			  "  save please check the domain file\n"
			  "  names.",
			  XmDIALOG_FULL_APPLICATION_MODAL);
	}
	XtFree(dirStr);
      }
    }
  }

  return;
}

void warpReadSignalPopupCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Widget	toggle, form;

  /* now use generic Ext format dialog */
  if( warp_read_sgnl_dialog == NULL ){
    warp_read_sgnl_dialog = HGU_XmCreateExtFFObjectFSB(view_struct->dialog,
						     "warp_read_sgnl_dialog",
						     warpReadSignalCb,
						     client_data);

    /* add a check box to reset the current working directory */
    if( form = XtNameToWidget(warp_read_sgnl_dialog, "*.formatFormRC") ){
      toggle = XtVaCreateManagedWidget("reset_cwd", xmToggleButtonGadgetClass,
				       form,
				       XmNset,	True,
				       NULL);
    }

    XtAddCallback(warp_read_sgnl_dialog, XmNokCallback,
		  warpResetCWDCb, client_data);

    /* set jpeg default */
    HGU_XmExtFFObjectFSBSetType(warp_read_sgnl_dialog, WLZEFF_FORMAT_JPEG);
  }

  XtManageChild(warp_read_sgnl_dialog);
  PopupCallback(w, (XtPointer) XtParent(warp_read_sgnl_dialog), NULL);
  XtCallCallbacks(warp_read_sgnl_dialog, XmNmapCallback, call_data);

  return;
}

void mapWarpDataCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzObject		*obj, *tmpObj;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  FILE			*fp;

  /* check for signal domain and warp transform */
  if((warpGlobals.sgnlObj == NULL)){
    return;
  }

  /* shift the signal object for the -ve bounds-obj mesh bug */
  if( tmpObj = WlzAssignObject(
    WlzShiftObject(warpGlobals.sgnlObj, warpGlobals.srcXOffset,
		   warpGlobals.srcYOffset, 0, &errNum), NULL) ){
    WlzObject	*obj1, *obj2;
    WlzPixelV	bckgrnd;
    int		*greyVals, size;
		       
    /* transform the domain and add to current domain */
    /* transform as a grey-value image and threshold
       - avoid boundlist <-> domain bug */
    bckgrnd.type = WLZ_GREY_UBYTE;
    bckgrnd.v.ubv = 0;
    size = ((tmpObj->domain.i->lastln - tmpObj->domain.i->line1 + 1) *
	    (tmpObj->domain.i->lastkl - tmpObj->domain.i->kol1 + 1));
    greyVals = (int *) AlcCalloc(size, sizeof(WlzUByte));
    obj1 = WlzMakeRect(tmpObj->domain.i->line1, tmpObj->domain.i->lastln,
		       tmpObj->domain.i->kol1, tmpObj->domain.i->lastkl,
		       WLZ_GREY_UBYTE, greyVals, bckgrnd,
		       NULL, NULL, &errNum);
    obj1 = WlzAssignObject(obj1, NULL);
    obj1->values.r->freeptr = AlcFreeStackPush(obj1->values.r->freeptr,
					       (void *) greyVals, NULL);
    bckgrnd.v.ubv = 100;
    obj2 = WlzAssignObject(WlzGreyMask(obj1, tmpObj, bckgrnd, &errNum), NULL);
    WlzFreeObj(obj1);
    if( obj = mapaintWarpObj(obj2,
			     WLZ_INTERPOLATION_NEAREST) ){
      obj = WlzAssignObject(obj, NULL);
      obj1 = WlzThreshold(obj, bckgrnd, WLZ_THRESH_HIGH, &errNum);
      pushUndoDomains(view_struct);
      setDomainIncrement(obj1, view_struct, globals.current_domain, 0);
      WlzFreeObj(obj1);
      WlzFreeObj(obj);
    }
    WlzFreeObj(obj2);
    WlzFreeObj(tmpObj);
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "mapWarpDataCb", errNum);
  }
  return;
}

void undoMappedWarpDataCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  UndoDomains(view_struct);
  return;
}

void installMappedWarpDataCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  installViewDomains(view_struct);
  return;
}

void resetSignalDefaultsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Widget		toggle, slider, radio_box;
  int			i;
  XmToggleButtonCallbackStruct cbs;

  /* reset the signal default values */
  /* pre-processing toggles and controls
     not the paint mode selection since this is determined by
     the selected page */
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*normalise") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
  }
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*histo_equalise") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
  }
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*shade_correction") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
  }
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*gauss_smooth") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
  }
  if( slider = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*gauss_width") ){
    HGU_XmSetSliderValue(slider, (float) 5.0);
  }

  /* do modes before threshold controls */
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*global_thresh") ){
    XtVaSetValues(toggle, XmNset, True, NULL);
    cbs.set = True;
    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
  }
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*incremental_thresh") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
    cbs.set = False;
    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
  }
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*pick_mode_thresh") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
    cbs.set = False;
    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
  }
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*distance_mode_thresh") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
    cbs.set = False;
    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
  }
  
  /* reset the threshold and interact controls */
  /* single colour mode */
  if( slider = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*thresh_range_low") ){
    HGU_XmSetSliderValue(slider, (float) 256.0);
    warpGlobals.threshRangeLow = 256;
  }
  if( slider = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*thresh_range_high") ){
    HGU_XmSetSliderValue(slider, (float) 256.0);
    warpGlobals.threshRangeHigh = 256;
  }
  if( radio_box = 
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*color_space") ){
    if( toggle = XtNameToWidget(radio_box, "*RGB") ){
      XtVaSetValues(radio_box, XmNmenuHistory, toggle, NULL);
      XtCallCallbacks(toggle, XmNactivateCallback, NULL);
    }
  }
  if( radio_box = 
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*threshold_channel_rc") ){
    if( toggle = XtNameToWidget(radio_box, "*grey") ){
      XtVaSetValues(radio_box, XmNmenuHistory, toggle, NULL);
      XtCallCallbacks(toggle, XmNvalueChangedCallback, NULL);
    }
  }

  /* multi-colour mode */
  if( slider =
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*thresh_range_red_low") ){
    HGU_XmSetSliderValue(slider, (float) 255.0);
  }
  if( slider =
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*thresh_range_red_high") ){
    HGU_XmSetSliderValue(slider, (float) 255.0);
  }
  if( slider =
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*thresh_range_green_low") ){
    HGU_XmSetSliderValue(slider, (float) 255.0);
  }
  if( slider =
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*thresh_range_green_high") ){
    HGU_XmSetSliderValue(slider, (float) 255.0);
  }
  if( slider =
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*thresh_range_blue_low") ){
    HGU_XmSetSliderValue(slider, (float) 255.0);
  }
  if( slider =
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*thresh_range_blue_high") ){
    HGU_XmSetSliderValue(slider, (float) 255.0);
  }
  for(i=0; i < 3; i++){
    warpGlobals.threshRangeRGBLow[i] = 255;
    warpGlobals.threshRangeRGBHigh[i] = 255;
  }

  /* interactive mode */
  if( slider = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*thresh_eccentricity") ){
    HGU_XmSetSliderValue(slider, (float) 1.0);
    warpGlobals.colorEllipseEcc = 1.0;
  }
  if( slider = XtNameToWidget(globals.topl,
			      "*warp_sgnl_controls_form*thresh_radius") ){
    HGU_XmSetSliderValue(slider, (float) 10.0);
  }
  if( radio_box = 
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_controls_form*threshold_interact_rc") ){
    if( toggle = XtNameToWidget(radio_box, "*box") ){
      XtVaSetValues(radio_box, XmNmenuHistory, toggle, NULL);
      XtCallCallbacks(toggle, XmNvalueChangedCallback, NULL);
    }
  }

  return;
}

void sgnlNotebookPageChangedCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmNotebookCallbackStruct *cbs=(XmNotebookCallbackStruct *) call_data;

  /* test for a non-threshold page preceded by a threshold page */
  if(cbs->page_widget && strncmp(XtName(cbs->page_widget), "threshold_", 10) ){
    if(cbs->prev_page_widget &&
       (strncmp(XtName(cbs->prev_page_widget), "threshold_", 10) == 0) ){
      warpGlobals.lastThresholdPageNum = cbs->prev_page_number;
    }
  }

  return;
}

Widget createWarpSgnlControlsFrame(
  Widget	parent,
  String	name,
  ThreeDViewStruct	*view_struct)
{
  Widget	form, title;
  Widget	button, widget;
  Widget	sgnl_controls;
  Widget	notebook, page;

 /* create the signal controls frame */
  sgnl_controls = XtVaCreateWidget("warp_sgnl_controls_frame",
				   xmFrameWidgetClass, parent,
				   NULL);
  title = XtVaCreateManagedWidget("warp_sgnl_frame_title",
				  xmLabelGadgetClass, sgnl_controls,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  form = XtVaCreateManagedWidget("warp_sgnl_controls_form",
				 xmFormWidgetClass, sgnl_controls,
				 XmNchildType, XmFRAME_WORKAREA_CHILD,
				 NULL);
  notebook = XtVaCreateManagedWidget("warp_sgnl_notebook",
				     xmNotebookWidgetClass, form,
				     XmNbindingType,	XmNONE,
				     XmNbackPagePlacement, XmTOP_RIGHT,
				     XmNbackPageSize, 3,
				     XmNorientation, XmVERTICAL,
				     XmNmajorTabSpacing, 0,
				     XmNminorTabSpacing, 0,
				     XmNtopAttachment, XmATTACH_FORM,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNrightAttachment, XmATTACH_FORM,
				     NULL);
  if( widget = XtNameToWidget(notebook, "*PageScroller") ){
    XtUnmanageChild(widget);
  }
  XtAddCallback(notebook, XmNpageChangedCallback,
		sgnlNotebookPageChangedCb, NULL);

  /* major page 1 - pre-processing */
  page = createSignalPreProcPage(notebook);

  /* major page 2 - filtering */
  page = createSignalFilterPage(notebook);

  /* major page 3 - thresholding */
  page = createSignalThresholdPage(notebook);

  /* major page for automatic segmentation, apply fixed algorithms,
     classifier training, parameter settings, macros */
  page = createSignalAutoPage(notebook);

  /* major page for domain post-procesing - morphological, smoothing
     tracking, cleaning, review */
  /* first the morphological page */
  page = createSignalPostProcPage(notebook);

  /* End of notebook pages */
  /* common buttons to map the data */
  button = XtVaCreateManagedWidget("map_it",
				   xmPushButtonGadgetClass, form,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	notebook,
				   XmNleftAttachment,	XmATTACH_POSITION,
				   XmNleftPosition,	3,
				   XmNrightAttachment,	XmATTACH_POSITION,
				   XmNrightPosition,	30,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		mapWarpDataCb, (XtPointer) view_struct);

  button = XtVaCreateManagedWidget("undo_map",
				   xmPushButtonGadgetClass, form,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	notebook,
				   XmNleftAttachment,	XmATTACH_POSITION,
				   XmNleftPosition,	36,
				   XmNrightAttachment,	XmATTACH_POSITION,
				   XmNrightPosition,	63,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		undoMappedWarpDataCb, (XtPointer) view_struct);
 
  /* reset IP controls defaults button */
  button = XtVaCreateManagedWidget("reset_defaults",
				   xmPushButtonGadgetClass, form,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	notebook,
				   XmNleftAttachment,	XmATTACH_POSITION,
				   XmNleftPosition,	69,
				   XmNrightAttachment,	XmATTACH_POSITION,
				   XmNrightPosition,	97,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		resetSignalDefaultsCb, (XtPointer) view_struct);

  warpGlobals.sgnlControls = sgnl_controls;

  return sgnl_controls;
}
 
void sgnlCanvasEventHandler(
  Widget        w,
  XtPointer     client_data,
  XEvent        *event,
  Boolean       *continue_to_dispatch)
{
  XmDrawingAreaCallbackStruct cbs;

  /* for reasons unknown need to pass on
     <Control>Button1 ignore all other
     ButtonEvents */
  if((event->type == ButtonPress) &&
     (event->xbutton.button == Button1)){
    if( !(event->xbutton.state&ControlMask) ){
      return;
    }
  }

  /* call the canvas input callbacks */
  cbs.reason = XmCR_INPUT;
  cbs.event = event;
  cbs.window = XtWindow(w);
  XtCallCallbacks(w, XmNinputCallback, (XtPointer) &cbs);

  return;
}

Widget createWarpSgnlDisplayFrame(
  Widget	parent,
  String	name,
  ThreeDViewStruct	*view_struct)
{
  Widget	widget, button;
  Visual	*visual;
  
  /* get the visual */
  if( !(visual = HGU_XGetVisual(globals.dpy, DefaultScreen(globals.dpy),
			      TrueColor, 24)) ){
    visual = HGU_XmWidgetToVisual(globals.topl);
  }

  /* create the signal image frame and process controls -
     unmanaged for now */
  widget = createWarpDisplayFrame(parent, name, visual, 24);
  warpGlobals.sgnl.canvas = XtNameToWidget(widget, "*canvas");
  warpGlobals.sgnl.ximage = NULL;
  warpGlobals.sgnl.mag = 1.0;
  warpGlobals.sgnl.rot = 0;
  warpGlobals.sgnl.transpose = 0;
  XtUnmanageChild(widget);

  /* canvas callbacks and event handler */
  XtAddCallback(warpGlobals.sgnl.canvas, XmNexposeCallback,
		warpCanvasExposeCb, (XtPointer) &(warpGlobals.sgnl));
  XtAddCallback(warpGlobals.sgnl.canvas, XmNexposeCallback,
		warpSgnlDomainCanvasExposeCb, (XtPointer) &(warpGlobals.sgnl));
  XtAddCallback(warpGlobals.sgnl.canvas, XmNinputCallback,
		sgnlCanvasMotionInputCb, NULL);
  XtAddEventHandler(warpGlobals.sgnl.canvas,
		    PointerMotionMask|
		    EnterWindowMask|LeaveWindowMask|ButtonPressMask,
		    False, sgnlCanvasEventHandler, NULL);

  /* button callbacks - read */
  button = XtNameToWidget(widget, "*b_1");
  XtAddCallback(button, XmNactivateCallback, warpReadSignalPopupCb,
		view_struct);

  /* now mag plus and mag minus */
  button = XtNameToWidget(widget, "*b_2");
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagPlusCb,
		(XtPointer) &(warpGlobals.sgnl));
  button = XtNameToWidget(widget, "*b_2a");
  XtManageChild(button);
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagMinusCb,
		(XtPointer) &(warpGlobals.sgnl));

  /* Grey invert */
  button = XtNameToWidget(widget, "*b_3");
  XtAddCallback(button, XmNactivateCallback, warpInvertGreyCb,
		(XtPointer) &(warpGlobals.sgnl));
  button = XtNameToWidget(widget, "*b_4");
  XtUnmanageChild(button);
  /*button = XtNameToWidget(widget, "*b_5"); - now removed
    XtUnmanageChild(button);*/

  /* the gamma arrows */
  button = XtNameToWidget(widget, "*b_6");
  XtAddCallback(button, XmNactivateCallback, warpIncrGammaCb,
		(XtPointer) &(warpGlobals.sgnl));
  button = XtNameToWidget(widget, "*b_7");
  XtAddCallback(button, XmNactivateCallback, warpDecrGammaCb,
		(XtPointer) &(warpGlobals.sgnl));

  /* the grey/color text feedback widget */
  button = XtNameToWidget(widget, "*b_9");
  XtManageChild(button);
  warpGlobals.sgnl.text = button;

  return( widget );
}
