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

void warpSwitchIncrementDomain(
  int	incrFlg)
{
  Widget	toggle;

  if( toggle = XtNameToWidget(warpGlobals.sgnlControls,
			      "*incremental_thresh") ){
    XtVaSetValues(toggle,
		  XmNset, (incrFlg?True:False),
		  NULL);
    if( !incrFlg ){
      if( toggle = XtNameToWidget(warpGlobals.sgnlControls,
				  "*global_thresh") ){
	XtVaSetValues(toggle, XmNset, True, NULL);
      }
    }
  }
  return;
}

void warpThreshLowCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	slider = w;
  
  /* get the parent slider */
  while( strcmp(XtName(slider), "thresh_range_low") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }
  warpGlobals.threshRangeLow = (int) HGU_XmGetSliderValue( slider );
  if( warpGlobals.threshRangeLow > warpGlobals.threshRangeHigh ){
    warpGlobals.threshRangeLow = warpGlobals.threshRangeHigh;
    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeLow);
  }

  /* set the signal domain */
  sgnlResetSgnlDomain(w, NULL);

  return;
}

void warpThreshHighCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	slider = w;
    
  /* get the parent slider */
  while( strcmp(XtName(slider), "thresh_range_high") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }
  warpGlobals.threshRangeHigh = (int) HGU_XmGetSliderValue( slider );
  if( warpGlobals.threshRangeLow > warpGlobals.threshRangeHigh ){
    warpGlobals.threshRangeHigh = warpGlobals.threshRangeLow;
    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeHigh);
  }

  /* set the signal domain */
  sgnlResetSgnlDomain(w, NULL);

  return;
}

void warpThreshLowRGBCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	slider = w;
  WlzRGBAColorChannel	channel=(WlzRGBAColorChannel) client_data;
  float		val;
    
  /* get the parent slider  - use special knowledge of slider type
     should put this as a function in the library*/
  if( XtIsSubclass(slider, xmFormWidgetClass) != True ){
    if( XtIsSubclass(slider, xmScaleWidgetClass) == True ){
      slider = XtParent(slider);
    }
    else {
      return;
    }
  }
  if( XtNameToWidget(slider, "scale") == NULL ){
    return;
  }

  /* get value and switch on channel */
  val = HGU_XmGetSliderValue( slider );
  switch( channel ){
  case WLZ_RGBA_CHANNEL_GREY:
    if( warpGlobals.threshRangeHigh < (int) val ){
      val = warpGlobals.threshRangeHigh;
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeLow = val;
    break;

  case WLZ_RGBA_CHANNEL_RED:
  case WLZ_RGBA_CHANNEL_HUE:
  case WLZ_RGBA_CHANNEL_CYAN:
    if( warpGlobals.threshRangeRGBHigh[0] < (int) val ){
      val = warpGlobals.threshRangeRGBHigh[0];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBLow[0] = val;
    break;

  case WLZ_RGBA_CHANNEL_GREEN:
  case WLZ_RGBA_CHANNEL_SATURATION:
  case WLZ_RGBA_CHANNEL_MAGENTA:
    if( warpGlobals.threshRangeRGBHigh[1] < (int) val ){
      val = warpGlobals.threshRangeRGBHigh[1];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBLow[1] = val;
    break;

  case WLZ_RGBA_CHANNEL_BLUE:
  case WLZ_RGBA_CHANNEL_BRIGHTNESS:
  case WLZ_RGBA_CHANNEL_YELLOW:
    if( warpGlobals.threshRangeRGBHigh[2] < (int) val ){
      val = warpGlobals.threshRangeRGBHigh[2];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBLow[2] = val;
    break;
  }

  /* set the signal domain */
  sgnlResetSgnlDomain(w, NULL);

  return;
}

void warpThreshHighRGBCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	slider = w;
  WlzRGBAColorChannel	channel=(WlzRGBAColorChannel) client_data;
  float	      	val;
    
  /* get the parent slider  - use special knowledge of slider type
     should put this as a function in the library*/
  if( XtIsSubclass(slider, xmFormWidgetClass) != True ){
    if( XtIsSubclass(slider, xmScaleWidgetClass) == True ){
      slider = XtParent(slider);
    }
    else {
      return;
    }
  }
  if( XtNameToWidget(slider, "scale") == NULL ){
    return;
  }

  /* get value and switch on channel */
  val = HGU_XmGetSliderValue( slider );
  switch( channel ){
  case WLZ_RGBA_CHANNEL_GREY:
    if( warpGlobals.threshRangeLow > (int) val ){
      val = warpGlobals.threshRangeLow;
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeHigh = val;
    break;

  case WLZ_RGBA_CHANNEL_RED:
  case WLZ_RGBA_CHANNEL_HUE:
  case WLZ_RGBA_CHANNEL_CYAN:
    if( warpGlobals.threshRangeRGBLow[0] > (int) val ){
      val = warpGlobals.threshRangeRGBLow[0];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBHigh[0] = val;
    break;

  case WLZ_RGBA_CHANNEL_GREEN:
  case WLZ_RGBA_CHANNEL_SATURATION:
  case WLZ_RGBA_CHANNEL_MAGENTA:
    if( warpGlobals.threshRangeRGBLow[1] > (int) val ){
      val = warpGlobals.threshRangeRGBLow[1];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBHigh[1] = val;
    break;

  case WLZ_RGBA_CHANNEL_BLUE:
  case WLZ_RGBA_CHANNEL_BRIGHTNESS:
  case WLZ_RGBA_CHANNEL_YELLOW:
    if( warpGlobals.threshRangeRGBLow[2] > (int) val ){
      val = warpGlobals.threshRangeRGBLow[2];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBHigh[2] = val;
    break;
  }

  /* set the signal domain */
  sgnlResetSgnlDomain(w, NULL);

  return;
}

void warpResetThresholdSliderRange(void)
{
  Widget	slider;

  if( warpGlobals.sgnl.obj ){
    /* slider for low end of the range */
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*thresh_range_low") ){

      if((WlzGreyTypeFromObj(warpGlobals.sgnl.obj, NULL) == WLZ_GREY_RGBA) &&
	 (warpGlobals.threshColorChannel == WLZ_RGBA_CHANNEL_GREY)){
	HGU_XmSetSliderRange(slider, 0.0, 444.0);
      }
      else {
	if( warpGlobals.threshRangeLow > 256 ){
	  warpGlobals.threshRangeLow = 256;
	  HGU_XmSetSliderValue(slider, 256.0);
	}
	HGU_XmSetSliderRange(slider, 0.0, 256.0);
      }
    }

    /* slider for high end of range */
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*thresh_range_high") ){

      if((WlzGreyTypeFromObj(warpGlobals.sgnl.obj, NULL) == WLZ_GREY_RGBA) &&
	 (warpGlobals.threshColorChannel == WLZ_RGBA_CHANNEL_GREY)){
	HGU_XmSetSliderRange(slider, 0.0, 444.0);
      }
      else {
	if( warpGlobals.threshRangeHigh > 256 ){
	  warpGlobals.threshRangeHigh = 256;
	  HGU_XmSetSliderValue(slider, 256.0);
	}
	HGU_XmSetSliderRange(slider, 0.0, 256.0);
      }
    }
  }

  return;
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
  FILE			*fp;
  Widget		slider;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check we can get the object */
  if( (obj = WlzXmReadExtFFObject(warp_read_sgnl_dialog,
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
  if( errNum == WLZ_ERR_NONE ){
    if( obj ){
      switch( obj->type ){
      case WLZ_2D_DOMAINOBJ:
	/* set the source object */
	if( warpGlobals.sgnl.obj ){
	  WlzFreeObj(warpGlobals.sgnl.obj);
	}
	warpGlobals.sgnl.obj = WlzAssignObject(obj, &errNum);
	warpSetXImage(&(warpGlobals.sgnl));
	warpCanvasExposeCb(warpGlobals.sgnl.canvas,
			   (XtPointer) &(warpGlobals.sgnl),
			   call_data);

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
	  warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
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
    warp_read_sgnl_dialog = WlzXmCreateExtFFObjectFSB(view_struct->dialog,
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
    WlzXmExtFFObjectFSBSetType(warp_read_sgnl_dialog, WLZEFF_FORMAT_JPEG);
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
		       
    /* transform the domain and add to current domain */
    if( obj = mapaintWarpObj(tmpObj,
			     WLZ_INTERPOLATION_NEAREST) ){
      pushUndoDomains(view_struct);
      setDomainIncrement(obj, view_struct, globals.current_domain, 0);
      WlzFreeObj(obj);
    }
    WlzFreeObj(tmpObj);
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "mapWarpDataCb", errNum);
  }
  return;
}

void setThreshModesCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		modeFlg = (int) client_data;
  XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct *) call_data;

  switch( modeFlg ){
  case 0:
    warpGlobals.globalThreshFlg = cbs->set;
    if( cbs->set ){
      warpGlobals.globalThreshVtx.vtX = -10000;
      warpGlobals.globalThreshVtx.vtY = -10000;
    }
    break;

  case 1:
    warpGlobals.incrThreshFlg = cbs->set;
    if( cbs->set ){
      sgnlIncrPush(warpGlobals.sgnlObj);
    }
    else {
      sgnlIncrClear();
    }
    break;

  case 2:
    warpGlobals.pickThreshFlg = cbs->set;
    break;

  case 3:
    warpGlobals.distanceThreshFlg = cbs->set;
    break;
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

void thresholdMinorPageSelectCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  warpGlobals.thresholdType = (WlzRGBAThresholdType) client_data;

  /* reset the threshold object */
  if( warpGlobals.sgnlThreshObj ){
    WlzFreeObj(warpGlobals.sgnlThreshObj);
    warpGlobals.sgnlThreshObj = NULL;
  }
  if( warpGlobals.sgnlObj ){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
  }
  warpSetSignalDomain(NULL);
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  return;
}

void thresholdInteractToggleSelectCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  warpGlobals.thresholdType = (WlzRGBAThresholdType) client_data;

  /* reset the threshold object */
  if( warpGlobals.sgnlThreshObj ){
    WlzFreeObj(warpGlobals.sgnlThreshObj);
    warpGlobals.sgnlThreshObj = NULL;
  }
  if( warpGlobals.sgnlObj ){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
  }
  warpSetSignalDomain(NULL);
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  return;
}

void warpRGBTextValueCb(
  Widget	w,
  XtPointer 	client_data,
  XtPointer	call_data)
{
  int		startFinishFlg = (int) client_data;
  int		r, g, b;
  String	textVal;

  /* get the text value */
  XtVaGetValues(w, XmNvalue, &textVal, NULL);
  if( textVal ){
    if( sscanf(textVal, "%d,%d,%d", &r, &g, &b) >= 3 ){
      if( startFinishFlg == 0 ){
	/* set the start value */
	WLZ_RGBA_RGBA_SET(warpGlobals.lowRGBPoint.v.rgbv, r, g, b, 255);
      }
      else {
	/* set the finish value */
	WLZ_RGBA_RGBA_SET(warpGlobals.highRGBPoint.v.rgbv, r, g, b, 255);
      }

      /* reset the threshold domain */
      sgnlResetSgnlDomain(w, NULL);
    }
  }

  return;
}
 
void thresholdMajorPageSelectCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	notebook;
  XmNotebookPageInfo	pageInfo;

  /* find the last used page and call the minor tab callback */
  if( (WlzRGBAThresholdType) client_data != WLZ_RGBA_THRESH_NONE ){

    warpGlobals.thresholdType = (WlzRGBAThresholdType) client_data;

    /* reset appropriate minor page */
    if( notebook = XtNameToWidget(warpGlobals.sgnlControls,
				  "*warp_sgnl_notebook") ){
      if( warpGlobals.lastThresholdPageNum > 0 ){
	XtVaSetValues(notebook,
		      XmNcurrentPageNumber, warpGlobals.lastThresholdPageNum,
		      NULL);

	/* call the activate callback */
	if( XmNotebookGetPageInfo(notebook,
				  warpGlobals.lastThresholdPageNum,
				  &pageInfo) == XmPAGE_FOUND ){
	  XmPushButtonCallbackStruct cbs;

	  cbs.reason = XmCR_ACTIVATE;
	  cbs.event = NULL;
	  cbs.click_count = 1;
	  XtCallCallbacks(pageInfo.minor_tab_widget,
			  XmNactivateCallback, &cbs);
	}
      }
    }

    /* set interactive callback on the view canvas */
    XtAddCallback(warpGlobals.sgnl.canvas, XmNinputCallback,
		  sgnlCanvasInputCb, NULL);
  }
  else {
    /* remove the callback on the view canvas */
    XtRemoveCallback(warpGlobals.sgnl.canvas, XmNinputCallback,
		     sgnlCanvasInputCb, NULL);
  }

  return;
}

void thresholdChannelSelectCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmToggleButtonCallbackStruct
    *cbs=(XmToggleButtonCallbackStruct *) call_data;

  if( cbs->set ){
    fprintf(stderr, "Reseting Channel to %d\n", client_data);
    warpGlobals.threshColorChannel = (WlzRGBAColorChannel) client_data;
    warpResetThresholdSliderRange();

    /* reset the threshold object */
    if( warpGlobals.sgnlThreshObj ){
      WlzFreeObj(warpGlobals.sgnlThreshObj);
      warpGlobals.sgnlThreshObj = NULL;
    }
    if( warpGlobals.sgnlObj ){
      warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
    }
/*  warpSwitchIncrementDomain(0);*/ /* why switch increment ? */
    warpSetSignalDomain(NULL);
    if( warpGlobals.sgnlObj ){
      warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
    }
  }

  return;
}

void warpThreshInteractPageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	rc, toggle;
  XmToggleButtonCallbackStruct	cbs;

  /* get the threshold type row-column and the selected toggle */
  if( rc = XtNameToWidget(warpGlobals.sgnlControls,
			  "*threshold_interact_rc") ){
    XtVaGetValues(rc, XmNmenuHistory, &toggle, NULL);

    /* execute callbacks */
    cbs.reason = XmCR_VALUE_CHANGED;
    cbs.event = NULL;
    cbs.set = True;
    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
  }

  /* install callback on the display window */


  return;
}

void warpThreshEccentricityCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	slider = w;
  float		val;
    
  /* get the parent slider  - use special knowledge of slider type
     should put this as a function in the library*/
  if( XtIsSubclass(slider, xmFormWidgetClass) != True ){
    if( XtIsSubclass(slider, xmScaleWidgetClass) == True ){
      slider = XtParent(slider);
    }
    else {
      return;
    }
  }
  if( XtNameToWidget(slider, "scale") == NULL ){
    return;
  }

  /* get value and switch on channel */
  warpGlobals.colorEllipseEcc = HGU_XmGetSliderValue( slider );

  /* reset the threshold object */
  if( warpGlobals.sgnlThreshObj ){
    WlzFreeObj(warpGlobals.sgnlThreshObj);
    warpGlobals.sgnlThreshObj = NULL;
  }
  if( warpGlobals.sgnlObj ){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
  }
  warpSetSignalDomain(NULL);
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  return;
}

void warpThreshRadiusSet(
  WlzDVertex2	dist)
{
  Widget	slider;
  float		radius;

  if( slider = XtNameToWidget(warpGlobals.sgnlControls,
			      "*.thresh_radius") ){
    radius = WLZ_NINT(fabs(dist.vtX));
    HGU_XmSetSliderValue(slider, radius);
  }

  return;
}

void warpThreshRadiusCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	slider = w;
  float		val;
  WlzPixelV	centreCol, sgnlStart, sgnlFinish;
  WlzDVertex2	dist;
    
  /* get the parent slider  - use special knowledge of slider type
     should put this as a function in the library */
  if( XtIsSubclass(slider, xmFormWidgetClass) != True ){
    if( XtIsSubclass(slider, xmScaleWidgetClass) == True ){
      slider = XtParent(slider);
    }
    else {
      return;
    }
  }
  if( XtNameToWidget(slider, "scale") == NULL ){
    return;
  }

  /* get value and set distance */
  val = HGU_XmGetSliderValue( slider );
  dist.vtX = dist.vtY = val;

  /* get existing start and finish */
  sgnlInteractGetHighLowControls(&sgnlStart, &sgnlFinish);

  /* get centre value, set limits and reset signal domain */
  centreCol = warpThreshCentre(sgnlStart, sgnlFinish);
  warpThreshSetLimitsFromDist(centreCol, dist, &sgnlStart,
			      &sgnlFinish);
  sgnlInteractSetHighLowControls(&sgnlStart, &sgnlFinish);
  sgnlResetSgnlDomain(w, NULL);

  return;
}

void sgnlBackgroundRemoveCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzObject	*obj1, *obj2;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check for signal input object and signal domain */
  if( warpGlobals.sgnl.obj && warpGlobals.sgnlObj ){
    if( obj1 = WlzDiffDomain(warpGlobals.sgnl.obj,
			     warpGlobals.sgnlObj, &errNum) ){
      if( obj2 = WlzMakeMain(warpGlobals.sgnl.obj->type,
			     obj1->domain,
			     warpGlobals.sgnl.obj->values,
			     NULL, NULL, &errNum) ){
	(void) WlzFreeObj(warpGlobals.sgnl.obj);
	WlzStandardIntervalDomain(obj2->domain.i);
	warpGlobals.sgnl.obj = WlzAssignObject(obj2, &errNum);
	warpSetXImage(&(warpGlobals.sgnl));
	warpCanvasExposeCb(warpGlobals.sgnl.canvas,
			   (XtPointer) &(warpGlobals.sgnl),
			   call_data);

	/* note need to cancel any local thresholding */
	warpGlobals.globalThreshVtx.vtX = -10000;
	warpGlobals.globalThreshVtx.vtY = -10000;

	/* clear increment stack */
	sgnlIncrClear();

	/* reset all pre-processing and signal domain */
 	if( warpGlobals.sgnlProcObj ){
	  WlzFreeObj( warpGlobals.sgnlProcObj );
	  warpGlobals.sgnlProcObj = NULL;
	}
	if( warpGlobals.sgnlThreshObj ){
	  WlzFreeObj(warpGlobals.sgnlThreshObj);
	  warpGlobals.sgnlThreshObj = NULL;
	}
	warpSetSignalDomain(NULL);
	warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
     }
      errNum = WlzFreeObj(obj1);
    }
  }
  else {
    /* warn no objects found */
    HGU_XmUserMessage(globals.topl,
		      "Background removal requires an input\n"
		      "signal image and a defined domain.\n"
		      "Please use the thresholding controls\n"
		      "to define the background region first.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "sgnlBackgroundRemoveCb", errNum);
  }
  return;
}

void sgnlBackgroundSaveCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  String	fileStr;
  FILE		*fp;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check if there is a signal object */
  if( warpGlobals.sgnl.obj ){
    /* get a filename for the section object */
    if( fileStr = HGU_XmUserGetFilename(globals.topl,
					"Please type in a filename\n"
					"for the signal image which\n"
					"be saved as a woolz object",
					"OK", "cancel", "MAPaintSignalObj.wlz",
					NULL, "*.wlz") ){
      if( fp = fopen(fileStr, "wb") ){
	if( WlzWriteObj(fp, warpGlobals.sgnl.obj) != WLZ_ERR_NONE ){
	  HGU_XmUserError(globals.topl,
			  "Save Signal Image:\n"
			  "    Incomplete write, please\n"
			  "    check disk space or quotas.\n"
			  "    Signal image not saved",
			  XmDIALOG_FULL_APPLICATION_MODAL);
	}
	if( fclose(fp) == EOF ){
	  HGU_XmUserError(globals.topl,
			  "Save Signal Image:\n"
			  "    File close error, please\n"
			  "    check disk space or quotas.\n"
			  "    Signal image not saved",
			  XmDIALOG_FULL_APPLICATION_MODAL);
	}
      }
      else {
	HGU_XmUserError(globals.topl,
			"Save Signal Image:\n"
			"    Couldn't open the file for\n"
			"    writing, please check\n"
			"    permissions.\n"
			"    Signal image not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }
      AlcFree(fileStr);
    }
  }

  return;
}

static MenuItem color_space_itemsP[] = {   /* colour space menu items */
  {"RGB", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpColorSpaceCb, (XtPointer) WLZ_RGBA_SPACE_RGB,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"HSB", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpColorSpaceCb, (XtPointer) WLZ_RGBA_SPACE_HSB,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"CMY", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpColorSpaceCb, (XtPointer) WLZ_RGBA_SPACE_CMY,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

Widget createWarpSgnlControlsFrame(
  Widget	parent,
  String	name,
  ThreeDViewStruct	*view_struct)
{
  Widget	control, frame, form, title;
  Widget	option_menu, button, buttons, radio_box, label, widget;
  Widget	toggle, slider, scale, sgnl_controls;
  Widget	notebook, page, rc, text;
  float		fval, fmin, fmax;
  Visual	*visual;
  int		i;
  XmString	xmstr;

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

  /* page 1 - pre-processing */
  /* make a major page for background removal */
  page = XtVaCreateManagedWidget("background_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("pre_process_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMajorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_NONE);

  button = XtVaCreateManagedWidget("background_minor_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);

   /* label to provide instructions for background removal */
  label = XtVaCreateManagedWidget("background_label",
				  xmLabelWidgetClass, page,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNborderWidth,	1,
				  XmNshadowThickness,	2,
				  XmNalignment,		XmALIGNMENT_BEGINNING,
				  NULL);
  xmstr = XmStringCreateLtoR(
    "To remove the background, use the\nthreshold controls to define\n"
    "the background then press Remove.\nTo save the result press Save. ",
    XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(label, XmNlabelString, xmstr, NULL);
  XmStringFree(xmstr);

  /* now the buttons */
  button = XtVaCreateManagedWidget("background_remove",
				   xmPushButtonWidgetClass, page,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	label,
				   XmNleftAttachment,	XmATTACH_FORM,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, sgnlBackgroundRemoveCb, NULL);

  button = XtVaCreateManagedWidget("background_save",
				   xmPushButtonWidgetClass, page,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	label,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	button,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, sgnlBackgroundSaveCb, NULL);

  /* make a minor page for pre-processing sequence */
  page = XtVaCreateManagedWidget("pre_process_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);

  button = XtVaCreateManagedWidget("pre_process_minor_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);

  /* toggles for the image processing sequence */
  toggle = XtVaCreateManagedWidget("normalise",
				   xmToggleButtonGadgetClass, page,
				   XmNleftAttachment,	XmATTACH_FORM,
				   XmNtopAttachment,	XmATTACH_FORM,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, recalcWarpProcObjCb, NULL);
  widget = toggle;
  toggle = XtVaCreateManagedWidget("histo_equalise",
				   xmToggleButtonGadgetClass, page,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	toggle,
				   XmNleftAttachment,	XmATTACH_FORM,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, recalcWarpProcObjCb, NULL);
  toggle = XtVaCreateManagedWidget("shade_correction",
				   xmToggleButtonGadgetClass, page,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	toggle,
				   XmNleftAttachment,	XmATTACH_FORM,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, recalcWarpProcObjCb, NULL);

  /* page 2 - filtering */
  page = XtVaCreateManagedWidget("filter_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("filter_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMajorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_NONE);

  /* gauss smoothing toggle and width parameter */
  toggle = XtVaCreateManagedWidget("gauss_smooth",
				   xmToggleButtonGadgetClass, page,
				   XmNleftAttachment,	XmATTACH_FORM,
				   XmNtopAttachment,	XmATTACH_FORM,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, recalcWarpProcObjCb, NULL);
  widget = toggle;
  fval = 5.0;
  slider = HGU_XmCreateHorizontalSlider("gauss_width", page,
					fval, 0.5, 20.0, 1,
					recalcWarpProcObjCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  /* page 3 - thresholding - single channnel */
  page = XtVaCreateManagedWidget("threshold_single_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("threshold_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMajorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_SINGLE);
  button = XtVaCreateManagedWidget("threshold_single_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMinorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_SINGLE);

  /* toggles for threshold option */
  radio_box = XmCreateRadioBox(page, "threshold_channel_rb", NULL, 0);
  XtVaSetValues(radio_box,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNorientation,		XmHORIZONTAL,
		XmNresizeWidth,		True,
		NULL);
  widget = radio_box;
		
  toggle = XtVaCreateManagedWidget("grey",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNset, True,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, thresholdChannelSelectCb,
		(XtPointer) WLZ_RGBA_CHANNEL_GREY);
  toggle = XtVaCreateManagedWidget("red",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNsensitive, False,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, thresholdChannelSelectCb,
		(XtPointer) WLZ_RGBA_CHANNEL_RED);
  toggle = XtVaCreateManagedWidget("green",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNsensitive, False,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, thresholdChannelSelectCb,
		(XtPointer) WLZ_RGBA_CHANNEL_GREEN);
  toggle = XtVaCreateManagedWidget("blue",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNsensitive, False,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, thresholdChannelSelectCb,
		(XtPointer) WLZ_RGBA_CHANNEL_BLUE);
  XtManageChild(radio_box);

  /* option menu for the colour space */
  option_menu = HGU_XmBuildMenu(page, XmMENU_OPTION, "color_space", 0,
				XmTEAR_OFF_DISABLED, color_space_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);
  widget = option_menu;
  XtVaSetValues(radio_box,
		XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget,	option_menu,
		NULL);

  /* defaults for the threshold range */
  warpGlobals.threshRangeLow = 256;
  warpGlobals.threshRangeHigh = 256;

  fval = warpGlobals.threshRangeLow;
  slider = HGU_XmCreateHorizontalSlider("thresh_range_low", page,
					fval, 0, 256, 0,
					warpThreshLowCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshLowCb, NULL);

  fval = warpGlobals.threshRangeHigh;
  slider = HGU_XmCreateHorizontalSlider("thresh_range_high", page,
					fval, 0, 256, 0,
					warpThreshHighCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshHighCb, NULL);

  /* page 4 - thresholding - multi channnel */
  page = XtVaCreateManagedWidget("threshold_multi_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("threshold_multi_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMinorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_MULTI);

  /* defaults for the colour threshold range */
  for(i=0; i < 3; i++){
    warpGlobals.threshRangeRGBLow[i] = 255;
    warpGlobals.threshRangeRGBHigh[i] = 255;
  }

  /* red */
  fval = warpGlobals.threshRangeRGBLow[0];
  slider = HGU_XmCreateHorizontalSlider("thresh_range_red_low", page,
					fval, 0, 255, 0,
					warpThreshLowRGBCb,
					(XtPointer) WLZ_RGBA_CHANNEL_RED);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	50,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshLowRGBCb,
		(XtPointer) WLZ_RGBA_CHANNEL_RED);

  fval = warpGlobals.threshRangeRGBHigh[0];
  slider = HGU_XmCreateHorizontalSlider("thresh_range_red_high", page,
					fval, 0, 255, 0,
					warpThreshHighRGBCb,
					(XtPointer) WLZ_RGBA_CHANNEL_RED);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	50,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshHighRGBCb,
		(XtPointer) WLZ_RGBA_CHANNEL_RED);

  /* green */
  fval = warpGlobals.threshRangeRGBLow[1];
  slider = HGU_XmCreateHorizontalSlider("thresh_range_green_low", page,
					fval, 0, 255, 0,
					warpThreshLowRGBCb,
					(XtPointer) WLZ_RGBA_CHANNEL_GREEN);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	50,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshLowRGBCb,
		(XtPointer) WLZ_RGBA_CHANNEL_GREEN);

  fval = warpGlobals.threshRangeRGBHigh[1];
  slider = HGU_XmCreateHorizontalSlider("thresh_range_green_high", page,
					fval, 0, 255, 0,
					warpThreshHighRGBCb,
					(XtPointer) WLZ_RGBA_CHANNEL_GREEN);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	50,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshHighRGBCb,
		(XtPointer) WLZ_RGBA_CHANNEL_GREEN);

  /* blue */
  fval = warpGlobals.threshRangeRGBLow[2];
  slider = HGU_XmCreateHorizontalSlider("thresh_range_blue_low", page,
					fval, 0, 255, 0,
					warpThreshLowRGBCb,
					(XtPointer) WLZ_RGBA_CHANNEL_BLUE);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	50,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshLowRGBCb,
		(XtPointer) WLZ_RGBA_CHANNEL_BLUE);

  fval = warpGlobals.threshRangeRGBHigh[2];
  slider = HGU_XmCreateHorizontalSlider("thresh_range_blue_high", page,
					fval, 0, 255, 0,
					warpThreshHighRGBCb,
					(XtPointer) WLZ_RGBA_CHANNEL_BLUE);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	50,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshHighRGBCb,
		(XtPointer) WLZ_RGBA_CHANNEL_BLUE);


  /* page 5 - thresholding - multi channnel - interactive*/
  page = XtVaCreateManagedWidget("threshold_interactive_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("threshold_interactive_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, warpThreshInteractPageCb,
		NULL);

  /* buttons to initiate interaction */
  radio_box = XmCreateRadioBox(page, "threshold_interact_rc", NULL, 0);
  XtVaSetValues(radio_box,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNorientation,		XmHORIZONTAL,
		NULL);
  widget = radio_box;
		
  toggle = XtVaCreateManagedWidget("box",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNset, False,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback,
		thresholdInteractToggleSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_BOX);
  toggle = XtVaCreateManagedWidget("slice",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNset, False,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback,
		thresholdInteractToggleSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_SLICE);
  toggle = XtVaCreateManagedWidget("sphere",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNset, True,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback,
		thresholdInteractToggleSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_SPHERE);
  XtManageChild(radio_box);

  /* parameters for ellipsoid */
  warpGlobals.colorEllipseEcc = 1.0;
  fval = warpGlobals.colorEllipseEcc;
  slider = HGU_XmCreateHorizontalExpSlider("thresh_eccentricity", page,
					   fval, 0, 10, 2,
					   warpThreshEccentricityCb,
					   NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshEccentricityCb, NULL);

  /* Distance slider - use scale variable for convenience */
  fval = 10.0;
  scale = HGU_XmCreateHorizontalSlider("thresh_radius", page,
				       fval, 0, 256, 0,
				       warpThreshRadiusCb,
				       NULL);
  XtVaSetValues(scale,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		slider,
		NULL);
  scale = XtNameToWidget(scale, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshRadiusCb, NULL);
  widget = slider; /* top widget of next line */

  /* text display of high and low rgb values */
  rc = XtVaCreateManagedWidget("threshold_interact_rc2",
			     xmRowColumnWidgetClass, page,
			     XmNpacking,	XmPACK_TIGHT,
			     XmNorientation,	XmHORIZONTAL,
			     XmNtopAttachment,	XmATTACH_WIDGET,
			     XmNtopWidget,	widget,
			     XmNleftAttachment,	XmATTACH_FORM,
			     NULL);
  label = XtVaCreateManagedWidget("RGB low:",
				  xmLabelWidgetClass, rc,
				  NULL);
  text = XtVaCreateManagedWidget("textRGBStart", xmTextWidgetClass, rc,
				 XmNeditable,  	True,
				 XmNrows,	1,
				 XmNcolumns,	12,
				 XmNshadowThickness,	0,
				 XmNcursorPositionVisible,	True,
				 XmNautoShowCursorPosition,	True,
				 XmNvalue,	"-,-,-",
				 NULL);
  XtAddCallback(text, XmNactivateCallback, warpRGBTextValueCb,
		(XtPointer) 0);
  label = XtVaCreateManagedWidget("RGB high:",
				  xmLabelWidgetClass, rc,
				  NULL);
  text = XtVaCreateManagedWidget("textRGBFinish", xmTextWidgetClass, rc,
				 XmNeditable,  	True,
				 XmNrows,	1,
				 XmNcolumns,	12,
				 XmNshadowThickness,	0,
				 XmNcursorPositionVisible,	True,
				 XmNautoShowCursorPosition,	True,
				 XmNvalue,	"-,-,-",
				 NULL);
  XtAddCallback(text, XmNactivateCallback, warpRGBTextValueCb,
		(XtPointer) 1);

  /* page 6 - thresholding behaviour controls */
  page = XtVaCreateManagedWidget("threshold_controls_page",
				 xmRowColumnWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 XmNpacking,	XmPACK_COLUMN,
				 XmNorientation,	XmVERTICAL,
				 XmNnumColumns,	2,
				 NULL);
  button = XtVaCreateManagedWidget("threshold_controls_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, warpThreshInteractPageCb,
		NULL);

  /* global thresholding toggle - default True */
  toggle = XtVaCreateManagedWidget("global_thresh",
				   xmToggleButtonGadgetClass, page,
				   XmNset,		True,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, setThreshModesCb,
		(XtPointer) 0);
  warpGlobals.globalThreshFlg = 1;
  warpGlobals.globalThreshVtx.vtX = -10000;
  warpGlobals.globalThreshVtx.vtY = -10000;

  /* Incremental thresholding toggle - default False */
  toggle = XtVaCreateManagedWidget("incremental_thresh",
				   xmToggleButtonGadgetClass, page,
				   XmNset,		False,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, setThreshModesCb,
		(XtPointer) 1);
  warpGlobals.incrThreshFlg = 0;
  warpGlobals.incrThreshObj = NULL;

  /* threshold "pick-mode" toggle for defining threshold endpoints
     default False */
  toggle = XtVaCreateManagedWidget("pick_mode_thresh",
				   xmToggleButtonGadgetClass, page,
				   XmNset,		False,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, setThreshModesCb,
		(XtPointer) 2);
  warpGlobals.pickThreshFlg = 0;

  /* threshold "distance-mode" toggle for defining threshold endpoints
     default False */
  toggle = XtVaCreateManagedWidget("distance_mode_thresh",
				   xmToggleButtonGadgetClass, page,
				   XmNset,		False,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, setThreshModesCb,
		(XtPointer) 3);
  warpGlobals.distanceThreshFlg = 0;

  /* page for automatic segmentation, apply fixed algorithms,
     classifier training, parameter settings, macros */
  page = XtVaCreateManagedWidget("auto_process_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("auto_process_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);

  /* page for domain post-procesing - morphological, smoothing
     tracking, cleaning, review */
  page = XtVaCreateManagedWidget("post_process_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("post_process_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);

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
		    ButtonMotionMask|EnterWindowMask|
		    LeaveWindowMask|ButtonPressMask,
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
