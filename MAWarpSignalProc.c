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
*   $Revision$								*
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

extern Widget createWarpDisplayFrame(
  Widget	parent,
  String	name,
  Visual	*visual,
  int		depth);

extern WlzObject *mapaintWarpObj(
  WlzObject	*obj,
  WlzInterpolationType	interpType);

extern void view_canvas_highlight(
  ThreeDViewStruct	*view_struct,
  Boolean		highlight);

extern MAPaintWarp2DStruct warpGlobals;
extern Widget warp_read_src_dialog;
extern Widget warp_read_sgnl_dialog;

extern void warpSetSignalDomain(void);

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

static void sgnlResetSgnlDomain(
  Widget	w)
{

  /* set the signal domain */
  if( warpGlobals.sgnlObj ){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
  }
  warpSetSignalDomain();
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }
  XFlush(XtDisplayOfObject(w));

  return;
}

static void sgnlInteractSetHighLowControls(
  WlzPixelV	pix1,
  WlzPixelV	pix2)
{
  float		val1, val2;
  UINT		r, g, b;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  Widget	slider, text;
  char		buf[64];

  /* switch on threshold type - determines the page */
  switch( warpGlobals.thresholdType ){

  case WLZ_RGBA_THRESH_SINGLE:
    val1 = WlzRGBAPixelValue(pix1, warpGlobals.threshColorChannel, &errNum);
    val2 = WlzRGBAPixelValue(pix2, warpGlobals.threshColorChannel, &errNum);
    warpGlobals.threshRangeLow = WLZ_MIN(val1, val2);
    warpGlobals.threshRangeHigh = WLZ_MAX(val1, val2);

    /* set the sliders */
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_low") ){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeLow);
    }
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_high") ){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeHigh);
    } 
    break;

  case WLZ_RGBA_THRESH_MULTI:
    /* use grey or rgb value to set max and min sliders */
    /* set red */
    val1 = WlzRGBAPixelValue(pix1, WLZ_RGBA_CHANNEL_RED, &errNum);
    val2 = WlzRGBAPixelValue(pix2, WLZ_RGBA_CHANNEL_RED, &errNum);
    warpGlobals.threshRangeRGBLow[0] = WLZ_MIN(val1, val2);
    warpGlobals.threshRangeRGBHigh[0] = WLZ_MAX(val1, val2);

    /* set the sliders */
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_red_low") ){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBLow[0]);
    }
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_red_high") ){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBHigh[0]);
    } 

    /* set green */
    val1 = WlzRGBAPixelValue(pix1, WLZ_RGBA_CHANNEL_GREEN, &errNum);
    val2 = WlzRGBAPixelValue(pix2, WLZ_RGBA_CHANNEL_GREEN, &errNum);
    warpGlobals.threshRangeRGBLow[1] = WLZ_MIN(val1, val2);
    warpGlobals.threshRangeRGBHigh[1] = WLZ_MAX(val1, val2);

    /* set the sliders */
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_green_low") ){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBLow[1]);
    } 
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_green_high") ){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBHigh[1]);
    } 

    /* set blue */
    val1 = WlzRGBAPixelValue(pix1, WLZ_RGBA_CHANNEL_BLUE, &errNum);
    val2 = WlzRGBAPixelValue(pix2, WLZ_RGBA_CHANNEL_BLUE, &errNum);
    warpGlobals.threshRangeRGBLow[2] = WLZ_MIN(val1, val2);
    warpGlobals.threshRangeRGBHigh[2] = WLZ_MAX(val1, val2);

    /* set the sliders */
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_blue_low") ){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBLow[2]);
    } 
    if( slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_blue_high") ){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBHigh[2]);
    } 
    break;

  case WLZ_RGBA_THRESH_SLICE:
  case WLZ_RGBA_THRESH_BOX:
  case WLZ_RGBA_THRESH_SPHERE:
    /* use grey or rgb value to set threshold end-points */
    r = WlzRGBAPixelValue(pix1, WLZ_RGBA_CHANNEL_RED, &errNum);
    g = WlzRGBAPixelValue(pix1, WLZ_RGBA_CHANNEL_GREEN, &errNum);
    b = WlzRGBAPixelValue(pix1, WLZ_RGBA_CHANNEL_BLUE, &errNum);
    WLZ_RGBA_RGBA_SET(warpGlobals.lowRGBPoint.v.rgbv, r, g, b, 255);
    if( text = XtNameToWidget(warpGlobals.sgnlControls,
			      "*textRGBStart") ){
      sprintf(buf, "%d,%d,%d", r, g, b);
      XtVaSetValues(text, XmNvalue, buf, NULL);
    }
    r = WlzRGBAPixelValue(pix2, WLZ_RGBA_CHANNEL_RED, &errNum);
    g = WlzRGBAPixelValue(pix2, WLZ_RGBA_CHANNEL_GREEN, &errNum);
    b = WlzRGBAPixelValue(pix2, WLZ_RGBA_CHANNEL_BLUE, &errNum);
    WLZ_RGBA_RGBA_SET(warpGlobals.highRGBPoint.v.rgbv, r, g, b, 255);
    if( text = XtNameToWidget(warpGlobals.sgnlControls,
			      "*textRGBFinish") ){
      sprintf(buf, "%d,%d,%d", r, g, b);
      XtVaSetValues(text, XmNvalue, buf, NULL);
    }
    break;
  }

  return;
}

static void warpSetSignalProcObj(void)
{
  WlzObject	*obj1, *obj2;
  Widget	toggle, slider;
  Boolean	setFlg;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if( warpGlobals.sgnlProcObj ){
    WlzFreeObj(warpGlobals.sgnlProcObj);
    warpGlobals.sgnlProcObj = NULL;
  }

  if( warpGlobals.sgnl.obj ){
    if( obj1 = WlzCopyObject(warpGlobals.sgnl.obj, &errNum) ){
      obj1 = WlzAssignObject(obj1, NULL);
    }
  }
  else {
    return;
  }

  /* normalise the data */
  if( errNum == WLZ_ERR_NONE ){
    if( toggle = XtNameToWidget(globals.topl,
				"*warp_sgnl_controls_form*normalise") ){
      XtVaGetValues(toggle, XmNset, &setFlg, NULL);
      if( setFlg ){
	errNum = WlzGreyNormalise(obj1);
      }
    }
  }

  /* Histo equalise */
  if( errNum == WLZ_ERR_NONE ){
    if( toggle = XtNameToWidget(globals.topl,
				"*warp_sgnl_controls_form*histo_equalise") ){
      XtVaGetValues(toggle, XmNset, &setFlg, NULL);
      if( setFlg ){
	errNum = WlzHistogramEqualiseObj(obj1, 1, 1);
      }
    }
  }

  /* Shade correction */
  if( errNum == WLZ_ERR_NONE ){
    if( toggle = XtNameToWidget(globals.topl,
				"*warp_sgnl_controls_form*shade_correction") ){
      XtVaGetValues(toggle, XmNset, &setFlg, NULL);
      if( setFlg ){
	errNum = WlzHistogramEqualiseObj(obj1, 1, 1);
      }
    }
  }

  /* Gauss smoothing */
  if( errNum == WLZ_ERR_NONE ){
    if( toggle = XtNameToWidget(globals.topl,
				"*warp_sgnl_controls_form*gauss_smooth") ){
      XtVaGetValues(toggle, XmNset, &setFlg, NULL);
      if( setFlg ){
	double	width;
	if( slider = XtNameToWidget(globals.topl,
				    "*warp_sgnl_controls_form*gauss_width") ){
	  width = HGU_XmGetSliderValue(slider);
	}
	else {
	  width = 3;
	}
	if( obj2 = WlzGauss2(obj1, width, width, 0, 0, &errNum) ){
	  WlzFreeObj(obj1);
	  obj1 = WlzAssignObject(obj2, NULL);
	}
      }
    }
  }

  /* if anything left set the processed object */
  if( errNum == WLZ_ERR_NONE ){
    if( obj1 ){
      warpGlobals.sgnlProcObj = obj1;
    }
  }
  else {
    if( obj1 ){
      WlzFreeObj(obj1);
    }
    MAPaintReportWlzError(globals.topl, "warpSetSignalProcObj", errNum);
  }

  return;
}

static void warpSetSignalThreshObj(void)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*obj, *obj1;
  WlzCompoundArray	*cobj;

  /* image processing sequence */
  if( warpGlobals.sgnlProcObj == NULL ){
    warpSetSignalProcObj();
  }
  if( warpGlobals.sgnlThreshObj ){
    WlzFreeObj(warpGlobals.sgnlThreshObj);
    warpGlobals.sgnlThreshObj = NULL;
  }

  /* extract object for thresholding */
  if( warpGlobals.sgnlProcObj ){
    /* detect if RGBA or multi-modal */
    switch( warpGlobals.sgnlProcObj->type ){
    case WLZ_COMPOUND_ARR_1:
      /* something, maybe do nothing? */
      break;

    case WLZ_2D_DOMAINOBJ:
    case WLZ_3D_DOMAINOBJ:
      /* switch on grey type - grey do nothing */
      if(WlzGreyTypeFromObj(warpGlobals.sgnlProcObj, &errNum) ==
	 WLZ_GREY_RGBA){
	switch( warpGlobals.thresholdType ){
	case WLZ_RGBA_THRESH_SINGLE:
	  switch( warpGlobals.threshColorChannel ){
	  case WLZ_RGBA_CHANNEL_GREY:
	    obj = WlzRGBAToModulus(warpGlobals.sgnlProcObj, &errNum);
	    warpGlobals.sgnlThreshObj = WlzAssignObject(obj, &errNum);
	    break;

	  case WLZ_RGBA_CHANNEL_RED:
	    if( cobj = WlzRGBAToCompound(warpGlobals.sgnlProcObj, &errNum) ){
	      warpGlobals.sgnlThreshObj = WlzAssignObject(cobj->o[0], &errNum);
	      WlzFreeObj((WlzObject *) cobj);
	    }
	    break;
	  case WLZ_RGBA_CHANNEL_GREEN:
	    if( cobj = WlzRGBAToCompound(warpGlobals.sgnlProcObj, &errNum) ){
	      warpGlobals.sgnlThreshObj = WlzAssignObject(cobj->o[1], &errNum);
	      WlzFreeObj((WlzObject *) cobj);
	    }
	    break;
	  case WLZ_RGBA_CHANNEL_BLUE:
	    if( cobj = WlzRGBAToCompound(warpGlobals.sgnlProcObj, &errNum) ){
	      warpGlobals.sgnlThreshObj = WlzAssignObject(cobj->o[2], &errNum);
	      WlzFreeObj((WlzObject *) cobj);
	    }
	    break;
	  case WLZ_RGBA_CHANNEL_HUE:
	    break;
	  case WLZ_RGBA_CHANNEL_SATURATION:
	    break;
	  case WLZ_RGBA_CHANNEL_BRIGHTNESS:
	    break;
	  }
	  break;

	case WLZ_RGBA_THRESH_MULTI:
	  if( cobj = WlzRGBAToCompound(warpGlobals.sgnlProcObj, &errNum) ){
	    warpGlobals.sgnlThreshObj =
	      WlzAssignObject((WlzObject *) cobj, &errNum);
	  }
	  break;

	case WLZ_RGBA_THRESH_SLICE:
	case WLZ_RGBA_THRESH_BOX:
	case WLZ_RGBA_THRESH_SPHERE:
	  warpGlobals.sgnlThreshObj = 
	    WlzAssignObject(warpGlobals.sgnlProcObj, &errNum);
	  break;

	default:
	  break;
	}
      }
      else {
	warpGlobals.sgnlThreshObj = 
	  WlzAssignObject(warpGlobals.sgnlProcObj, &errNum);
      }
      break;

    case WLZ_TRANS_OBJ: /* to be done */
      break;

    case WLZ_EMPTY_OBJ:
      warpGlobals.sgnlThreshObj = 
	WlzAssignObject(warpGlobals.sgnlProcObj, &errNum);
      break;

    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
    }
  }
  return;
}

void warpSetSignalDomain(void)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzPixelV	threshV, threshV1;
  WlzObject	*obj, *obj1;
  WlzCompoundArray	*cobj;
  UINT		combineMode;

  /* image processing sequence */
  if( warpGlobals.sgnlThreshObj == NULL ){
    warpSetSignalThreshObj();
  }
  if( warpGlobals.sgnlThreshObj ){
    obj1 = WlzAssignObject(warpGlobals.sgnlThreshObj, &errNum);
  }
  else {
    return;
  }

  /* threshold the resultant image */
  if( errNum == WLZ_ERR_NONE ){
    switch( warpGlobals.thresholdType ){
    case WLZ_RGBA_THRESH_SINGLE:
      threshV.type = WLZ_GREY_INT;
      threshV.v.inv = warpGlobals.threshRangeLow;
      if( obj1 ){
	/* clear signal object */
	if( warpGlobals.sgnlObj ){
	  WlzFreeObj(warpGlobals.sgnlObj);
	}
	  
	if( obj = WlzThreshold(obj1, threshV, WLZ_THRESH_HIGH, &errNum) ){
	  WlzFreeObj(obj1);
	  obj = WlzAssignObject(obj, &errNum);
	  threshV.v.inv = warpGlobals.threshRangeHigh + 1;
	  if( obj1 = WlzThreshold(obj, threshV, WLZ_THRESH_LOW, &errNum) ){
	    warpGlobals.sgnlObj = WlzAssignObject(obj1, &errNum);
	  }
	  else {
	    warpGlobals.sgnlObj = NULL;
	  }
	  WlzFreeObj(obj);
	}
	else {
	  WlzFreeObj(obj1);
	  warpGlobals.sgnlObj = NULL;
	}
      }
      break;

    case WLZ_RGBA_THRESH_MULTI:
      /* clear signal object */
      if( warpGlobals.sgnlObj ){
	WlzFreeObj(warpGlobals.sgnlObj);
      }

      /* set the thresholds and combine mode */
      threshV.type = WLZ_GREY_RGBA;
      WLZ_RGBA_RGBA_SET(threshV.v.rgbv,
			warpGlobals.threshRangeRGBLow[0],
			warpGlobals.threshRangeRGBLow[1],
			warpGlobals.threshRangeRGBLow[2],
			255);
      threshV1.type = WLZ_GREY_RGBA;
      WLZ_RGBA_RGBA_SET(threshV1.v.rgbv,
			warpGlobals.threshRangeRGBHigh[0],
			warpGlobals.threshRangeRGBHigh[1],
			warpGlobals.threshRangeRGBHigh[2],
			255);
      WLZ_RGBA_RGBA_SET(combineMode,
			WLZ_BO_AND, WLZ_BO_AND, WLZ_BO_AND, 255);

      /* use multi-threshold */
      if( obj = WlzRGBAMultiThreshold(obj1, threshV, threshV1,
				      combineMode, &errNum) ){
	warpGlobals.sgnlObj = WlzAssignObject(obj, &errNum);
      }
      else {
	warpGlobals.sgnlObj = NULL;
      }
      WlzFreeObj(obj1);
      break;

    case WLZ_RGBA_THRESH_BOX:
      /* clear signal object */
      if( warpGlobals.sgnlObj ){
	WlzFreeObj(warpGlobals.sgnlObj);
      }

      /* use box-threshold */
      if( obj = WlzRGBABoxThreshold(obj1,
				    warpGlobals.lowRGBPoint,
				    warpGlobals.highRGBPoint,
				    &errNum) ){
	warpGlobals.sgnlObj = WlzAssignObject(obj, &errNum);
      }
      else {
	warpGlobals.sgnlObj = NULL;
      }
      WlzFreeObj(obj1);	  
      break;

    case WLZ_RGBA_THRESH_SLICE:
      /* clear signal object */
      if( warpGlobals.sgnlObj ){
	WlzFreeObj(warpGlobals.sgnlObj);
      }

      /* use slice-threshold */
      if( obj = WlzRGBASliceThreshold(obj1,
				      warpGlobals.lowRGBPoint,
				      warpGlobals.highRGBPoint,
				      &errNum) ){
	warpGlobals.sgnlObj = WlzAssignObject(obj, &errNum);
      }
      else {
	warpGlobals.sgnlObj = NULL;
      }
      WlzFreeObj(obj1);	  
      break;

    case WLZ_RGBA_THRESH_SPHERE:
      /* clear signal object */
      if( warpGlobals.sgnlObj ){
	WlzFreeObj(warpGlobals.sgnlObj);
      }

      /* use Ellipsoid-threshold */
      if( obj = WlzRGBAEllipsoidThreshold(obj1,
					  warpGlobals.lowRGBPoint,
					  warpGlobals.highRGBPoint,
					  warpGlobals.colorEllipseEcc,
					  &errNum) ){
	warpGlobals.sgnlObj = WlzAssignObject(obj, &errNum);
      }
      else {
	warpGlobals.sgnlObj = NULL;
      }
      WlzFreeObj(obj1);	  
      break;

    default:
      errNum = WLZ_ERR_PARAM_DATA;
      if( obj1 ){
	WlzFreeObj(obj1);
      }
      if( warpGlobals.sgnlObj ){
	WlzFreeObj(warpGlobals.sgnlObj);
	warpGlobals.sgnlObj = NULL;
      }
      break;
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "warpSetSignalDomain", errNum);
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
  sgnlResetSgnlDomain(w);

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
  sgnlResetSgnlDomain(w);

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
    if( warpGlobals.threshRangeRGBHigh[0] < (int) val ){
      val = warpGlobals.threshRangeRGBHigh[0];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBLow[0] = val;
    break;

  case WLZ_RGBA_CHANNEL_GREEN:
    if( warpGlobals.threshRangeRGBHigh[1] < (int) val ){
      val = warpGlobals.threshRangeRGBHigh[1];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBLow[1] = val;
    break;

  case WLZ_RGBA_CHANNEL_BLUE:
    if( warpGlobals.threshRangeRGBHigh[2] < (int) val ){
      val = warpGlobals.threshRangeRGBHigh[2];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBLow[2] = val;
    break;

  case WLZ_RGBA_CHANNEL_HUE:
  case WLZ_RGBA_CHANNEL_SATURATION:
  case WLZ_RGBA_CHANNEL_BRIGHTNESS:
    return;
  }

  /* set the signal domain */
  sgnlResetSgnlDomain(w);

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
    if( warpGlobals.threshRangeRGBLow[0] > (int) val ){
      val = warpGlobals.threshRangeRGBLow[0];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBHigh[0] = val;
    break;

  case WLZ_RGBA_CHANNEL_GREEN:
    if( warpGlobals.threshRangeRGBLow[1] > (int) val ){
      val = warpGlobals.threshRangeRGBLow[1];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBHigh[1] = val;
    break;

  case WLZ_RGBA_CHANNEL_BLUE:
    if( warpGlobals.threshRangeRGBLow[2] > (int) val ){
      val = warpGlobals.threshRangeRGBLow[2];
      HGU_XmSetSliderValue(slider, val);
    }
    warpGlobals.threshRangeRGBHigh[2] = val;
    break;

  case WLZ_RGBA_CHANNEL_HUE:
  case WLZ_RGBA_CHANNEL_SATURATION:
  case WLZ_RGBA_CHANNEL_BRIGHTNESS:
    return;
  }

  /* set the signal domain */
  sgnlResetSgnlDomain(w);

  return;
}

void warpSetThreshColorTypeSensitive(
  Boolean	sensitive)
{
  Widget	toggle;

  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_notebook*threshold_single_page*red") ){
    XtSetSensitive(toggle, sensitive);
  }
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_notebook*threshold_single_page*green") ){
    XtSetSensitive(toggle, sensitive);
  }
  if( toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_notebook*threshold_single_page*blue") ){
    XtSetSensitive(toggle, sensitive);
  }

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
	warpSetSignalDomain();
	if( warpGlobals.sgnlObj ){
	  warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
	}

	/* set threshold button sensitivities */
	if( WlzGreyTypeFromObj(obj, NULL) == WLZ_GREY_RGBA ){
	  warpSetThreshColorTypeSensitive( True );
	}
	else {
	  warpSetThreshColorTypeSensitive( False );
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
  warpSetSignalDomain();
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  return;
}

static int sgnlTrigger=0;
static WlzPixelV sgnlStart, sgnlFinish;
static WlzGreyValueWSpace	*sgnl_gVWSp = NULL;

void sgnlCanvasInputCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmDrawingAreaCallbackStruct	*cbs=(XmDrawingAreaCallbackStruct *) call_data;
  int		x, y;
  double	line, kol;
  unsigned int	modMask=(ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|
			 Mod4Mask|Mod5Mask);
  WlzErrorNum	errNum;

  /* check for signal processed object - else do nothing */
  if( !warpGlobals.sgnlProcObj ){
    return;
  }

  /* switch on event type */
  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
      /* set initial point and initiate drag response */
      /* only if no modifiers pressed */
      if( !(cbs->event->xbutton.state&modMask) ){
	if( sgnl_gVWSp = WlzGreyValueMakeWSp(warpGlobals.sgnlProcObj,
					     &errNum) ){
	  sgnlTrigger = 1;
	  line = cbs->event->xbutton.y / warpGlobals.sgnl.mag;
	  kol = cbs->event->xbutton.x / warpGlobals.sgnl.mag;

	  /* get the value */
	  WlzGreyValueGet(sgnl_gVWSp, 0.0, line, kol);
	  sgnlStart.type = sgnl_gVWSp->gType;
	  sgnlFinish.type = sgnl_gVWSp->gType;
	  sgnlStart.v = sgnl_gVWSp->gVal[0];
	  sgnlFinish.v = sgnl_gVWSp->gVal[0];

	  /* set sliders or text areas */
	  sgnlInteractSetHighLowControls(sgnlStart, sgnlFinish);

	  /* reset the threshold domain */
	  sgnlResetSgnlDomain(w);
	}
	else {
	  /* report an error */
	  MAPaintReportWlzError(globals.topl,
			  "sgnlCanvasInputCb", errNum);
	}
      }
      break;

    case Button2:		/* domain and coordinate feedback */
      break;

    case Button3:		/* unused */
    default:
      break;

    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){

    case Button1:
      /* set endpoint - note could be max or min */
      if( sgnlTrigger ){
	sgnlTrigger = 0;
	line = cbs->event->xbutton.y / warpGlobals.sgnl.mag;
	kol = cbs->event->xbutton.x / warpGlobals.sgnl.mag;

	/* get the value */
	WlzGreyValueGet(sgnl_gVWSp, 0.0, line, kol);
	sgnlFinish.v = sgnl_gVWSp->gVal[0];
	
	/* set sliders or text areas */
	sgnlInteractSetHighLowControls(sgnlStart, sgnlFinish);
	
	/* reset the threshold domain */
	sgnlResetSgnlDomain(w);

	/* clean up */
	WlzGreyValueFreeWSp(sgnl_gVWSp);
	sgnl_gVWSp = NULL;
      }
      break;

    case Button2:		/* domain and coordinate feedback */
      break;

    default:
      break;

    }
    break;

  case MotionNotify:
    /* if drag then reset threshold levels and threshold domain */
      if( sgnlTrigger ){
	line = cbs->event->xmotion.y / warpGlobals.sgnl.mag;
	kol = cbs->event->xmotion.x / warpGlobals.sgnl.mag;

	/* get the value */
	WlzGreyValueGet(sgnl_gVWSp, 0.0, line, kol);
	sgnlFinish.v = sgnl_gVWSp->gVal[0];
	
	/* set sliders or text areas */
	sgnlInteractSetHighLowControls(sgnlStart, sgnlFinish);
	
	/* reset the threshold domain */
	sgnlResetSgnlDomain(w);
      }
      break;

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

    case XK_w:
      break;

    }
    break;

  default:
    break;
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
  warpSetSignalDomain();
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
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
  warpSetSignalDomain();
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  return;
}

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
  if( widget = XtNameToWidget(notebook, "PageScroller") ){
    XtUnmanageChild(widget);
  }
  XtAddCallback(notebook, XmNpageChangedCallback,
		sgnlNotebookPageChangedCb, NULL);

  /* page 1 - pre-processing */
  page = XtVaCreateManagedWidget("pre_process_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("pre_process_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMajorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_NONE);

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
				   XmNset, True,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, thresholdMinorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_BOX);
  toggle = XtVaCreateManagedWidget("slice",
				   xmToggleButtonGadgetClass, radio_box,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, thresholdMinorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_SLICE);
  toggle = XtVaCreateManagedWidget("sphere",
				   xmToggleButtonGadgetClass, radio_box,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback,thresholdMinorPageSelectCb ,
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
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, warpThreshEccentricityCb, NULL);

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
 
  /*button = XtVaCreateManagedWidget("install_data",
				   xmPushButtonGadgetClass, form,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	notebook,
				   XmNleftAttachment,	XmATTACH_POSITION,
				   XmNleftPosition,	69,
				   XmNrightAttachment,	XmATTACH_POSITION,
				   XmNrightPosition,	97,
				   XmNsensitive,	False,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
  installMappedWarpDataCb, (XtPointer) view_struct);*/

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
  XtAddEventHandler(warpGlobals.sgnl.canvas,
		    ButtonMotionMask|EnterWindowMask|
		    LeaveWindowMask|ButtonPressMask,
		    False, sgnlCanvasEventHandler, NULL);

  /* button callbacks */
  button = XtNameToWidget(widget, "*b_1");
  XtAddCallback(button, XmNactivateCallback, warpReadSignalPopupCb,
		view_struct);
  button = XtNameToWidget(widget, "*b_2");
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagPlusCb,
		(XtPointer) &(warpGlobals.sgnl));
  button = XtNameToWidget(widget, "*b_2a");
  XtManageChild(button);
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagMinusCb,
		(XtPointer) &(warpGlobals.sgnl));
  button = XtNameToWidget(widget, "*b_3");
  XtAddCallback(button, XmNactivateCallback, warpInvertGreyCb,
		(XtPointer) &(warpGlobals.sgnl));
  button = XtNameToWidget(widget, "*b_4");
  XtUnmanageChild(button);
  button = XtNameToWidget(widget, "*b_5");
  XtUnmanageChild(button);
  button = XtNameToWidget(widget, "*b_6");
  XtAddCallback(button, XmNactivateCallback, warpIncrGammaCb,
		(XtPointer) &(warpGlobals.sgnl));
  button = XtNameToWidget(widget, "*b_7");
  XtAddCallback(button, XmNactivateCallback, warpDecrGammaCb,
		(XtPointer) &(warpGlobals.sgnl));


  return( widget );
}
