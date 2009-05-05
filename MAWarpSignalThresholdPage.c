#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MAWarpSignalThresholdPage_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MAWarpSignalThresholdPage.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:32:57 2009
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par Copyright:
* Copyright (C) 2005 Medical research Council, UK.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be
* useful but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301, USA.
* \ingroup      MAPaint
* \brief        
*               
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>
#include <MAWarp.h>

extern void sgnlInteractGetHighLowControls(
  WlzPixelV	*pix1,
  WlzPixelV	*pix2);

void warpSwitchIncrementDomain(
  int	incrFlg)
{
  Widget	toggle;

  if((toggle = XtNameToWidget(warpGlobals.sgnlControls,
			      "*incremental_thresh"))){
    XtVaSetValues(toggle,
		  XmNset, (incrFlg?True:False),
		  NULL);
    if( !incrFlg ){
      if((toggle = XtNameToWidget(warpGlobals.sgnlControls,
				  "*global_thresh"))){
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
  default:
    return;

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
  default:
    return;

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
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*thresh_range_low"))){

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
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*thresh_range_high"))){

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
    if((notebook = XtNameToWidget(warpGlobals.sgnlControls,
				  "*warp_sgnl_notebook"))){
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
  if((rc = XtNameToWidget(warpGlobals.sgnlControls,
			  "*threshold_interact_rc"))){
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
 
void warpThreshRadiusSet(
  WlzDVertex2	dist)
{
  Widget	slider;
  float		radius;

  if((slider = XtNameToWidget(warpGlobals.sgnlControls,
			      "*.thresh_radius"))){
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
  {NULL},
};

Widget createSignalThresholdPage(
  Widget	notebook)
{
  Widget	page, button, radio_box, toggle, widget;
  Widget	option_menu, slider, scale, label, rc, text;
  int		i;
  float		fval;

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
  XtVaSetValues(radio_box, XmNmenuHistory, toggle, NULL);
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

  return page;
}
