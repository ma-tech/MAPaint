#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAWarpSignalInteract_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAWarpSignalInteract.c
* \author	Richard Baldock
* \date		April 2009
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
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
*/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>
#include <MAWarp.h>

extern void warpThreshRadiusSet(
  WlzDVertex2	dist);

extern WlzErrorNum MAPaintEventRemap(
  MAPaintContext	context,
  MAPaintContextMode	mode,
  XEvent		*event);

/* include a static undo list here.
   For simplicity no redo - no doubt the punters will want it */
static AlcDLPList *sgnlIncrList=NULL;

void sgnlListItemFree(
  void *entry)
{
  if( entry ){
    WlzFreeObj((WlzObject *) entry);
  }
  return;
}

void sgnlIncrClear(void)
{
  /* clear the list and reset to NULL */
  if( sgnlIncrList ){
    AlcDLPListFree(sgnlIncrList);
    sgnlIncrList = NULL;
  }
  return;
}

/* should make this a list so can have undo - done but no redo */
/* push takes the given object and copies 
   it onto the stack */
void sgnlIncrPush(
  WlzObject	*obj)
{
  if( obj ){
    if( sgnlIncrList == NULL ){
      sgnlIncrList = AlcDLPListNew(NULL);
    }
    if( sgnlIncrList ){
      AlcDLPItem *item;
      obj = WlzAssignObject(obj, NULL);
      item = AlcDLPItemNew((void *) obj, sgnlListItemFree, NULL);
      AlcDLPItemInsert(sgnlIncrList, NULL, item);
    }
  }

  return;
}

/* pop removes the head of the stack and returns the object
   from the new head  - just for convenience,
   could use sgnlIncrObj() */
WlzObject *sgnlIncrPop(void)
{
  WlzObject *rtnObj=NULL;

  if( sgnlIncrList ){
    if( sgnlIncrList->head ){
      AlcDLPItemUnlink(sgnlIncrList, sgnlIncrList->head, 1, NULL);
    }
    /* check if any left */
    if( sgnlIncrList->head ){
      rtnObj = (WlzObject *) sgnlIncrList->head->entry;
    }
  }

  return rtnObj;
}

WlzObject *sgnlIncrObj(void)
{
  if( sgnlIncrList ){
    if( sgnlIncrList->head ){
      return (WlzObject *) sgnlIncrList->head->entry;
    }
  }
  return NULL;
}

WlzPixelV warpThreshCentre(
  WlzPixelV	pix1,
  WlzPixelV	pix2)
{
  WlzPixelV	rtnPix;
  int		r, g, b;

  /* keep it simple, must be same type else return -1, int */
  if( pix1.type == pix2.type ){
    rtnPix = pix1;
    switch( pix1.type ){
    default:
      rtnPix.type = WLZ_GREY_INT;
      rtnPix.v.inv = -1;
      break;

    case WLZ_GREY_INT:
      rtnPix.v.inv = (pix1.v.inv + pix2.v.inv) / 2;
      break;

    case WLZ_GREY_SHORT:
      rtnPix.v.shv = (pix1.v.shv + pix2.v.shv) / 2;
      break;

    case WLZ_GREY_UBYTE:
      r = ((int) pix1.v.ubv + (int) pix2.v.ubv) / 2;
      rtnPix.v.ubv = r;
      break;

    case WLZ_GREY_FLOAT:
      rtnPix.v.flv = (pix1.v.flv + pix2.v.flv) / 2;
      break;

    case WLZ_GREY_DOUBLE:
      rtnPix.v.dbv = (pix1.v.dbv + pix2.v.dbv) / 2;
      break;

    case WLZ_GREY_RGBA:
      r = (WLZ_RGBA_RED_GET(pix1.v.rgbv) + WLZ_RGBA_RED_GET(pix2.v.rgbv)) / 2;
      g = (WLZ_RGBA_GREEN_GET(pix1.v.rgbv) + WLZ_RGBA_GREEN_GET(pix2.v.rgbv)) / 2;
      b = (WLZ_RGBA_BLUE_GET(pix1.v.rgbv) + WLZ_RGBA_BLUE_GET(pix2.v.rgbv)) / 2;
      WLZ_RGBA_RGBA_SET(rtnPix.v.rgbv, r, g, b, 255);
      break;
    }
  }
  else {
    rtnPix.type = WLZ_GREY_INT;
    rtnPix.v.inv = -1;
  }

  return rtnPix;
}

void sgnlInteractGetHighLowControls(
  WlzPixelV	*pix1,
  WlzPixelV	*pix2)
{

  /* switch on threshold type - determines the page */
  switch( warpGlobals.thresholdType ){

  default:
  case WLZ_RGBA_THRESH_NONE: /* set default values */
    if( pix1 ){
      pix1->type = WLZ_GREY_INT;
      pix1->v.inv = 255;
    }
    if( pix2 ){
      pix2->type = WLZ_GREY_INT;
      pix2->v.inv = 255;
    }
    break;

  case WLZ_RGBA_THRESH_SINGLE: /* single high low values */
    if( pix1 ){
      pix1->type = WLZ_GREY_INT;
      pix1->v.inv = warpGlobals.threshRangeLow;
    }
    if( pix2 ){
      pix2->type = WLZ_GREY_INT;
      pix2->v.inv = warpGlobals.threshRangeHigh;
    }
    break;

  case WLZ_RGBA_THRESH_MULTI: /* set rgb value - may imply clamp */
    if( pix1 ){
      pix1->type = WLZ_GREY_RGBA;
      WLZ_RGBA_RGBA_SET(pix1->v.rgbv,
			warpGlobals.threshRangeRGBLow[0],
			warpGlobals.threshRangeRGBLow[1],
			warpGlobals.threshRangeRGBLow[2],
			255);
    }
    if( pix2 ){
      pix2->type = WLZ_GREY_RGBA;
      WLZ_RGBA_RGBA_SET(pix2->v.rgbv,
			warpGlobals.threshRangeRGBHigh[0],
			warpGlobals.threshRangeRGBHigh[1],
			warpGlobals.threshRangeRGBHigh[2],
			255);
    }
    break;

  case WLZ_RGBA_THRESH_SLICE:
  case WLZ_RGBA_THRESH_BOX:
  case WLZ_RGBA_THRESH_SPHERE:
    /* use grey or rgb value to set threshold end-points */
    if( pix1 ){
      *pix1 = warpGlobals.lowRGBPoint;
    }
    if( pix2 ){
      *pix2 = warpGlobals.highRGBPoint;
    }
    break;
  }

  return;
}

void sgnlInteractSetHighLowControls(
  WlzPixelV	*pix1,
  WlzPixelV	*pix2)
{
  float		val1, val2;
  WlzUInt	r, g, b;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  Widget	slider, text;
  char		buf[64];

  /* switch on threshold type - determines the page */
  switch( warpGlobals.thresholdType ){

  case WLZ_RGBA_THRESH_NONE:
    break;

  case WLZ_RGBA_THRESH_SINGLE:
    if( pix1 ){
      val1 = WlzRGBAPixelValue(*pix1, warpGlobals.threshColorChannel, &errNum);
    }
    else {
      val1 = warpGlobals.threshRangeLow;
    }
    if( pix2 ){
      val2 = WlzRGBAPixelValue(*pix2, warpGlobals.threshColorChannel, &errNum);
    }
    else {
      val2 = warpGlobals.threshRangeHigh;
    }
    warpGlobals.threshRangeLow = WLZ_MIN(val1, val2);
    warpGlobals.threshRangeHigh = WLZ_MAX(val1, val2);

    /* set the sliders */
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_low"))){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeLow);
    }
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_high"))){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeHigh);
    } 
    break;

  case WLZ_RGBA_THRESH_MULTI:
    /* use grey or rgb value to set max and min sliders */
    /* set red */
    if( pix1 ){
      val1 = WlzRGBAPixelValue(*pix1, WLZ_RGBA_CHANNEL_RED, &errNum);
    }
    else {
      val1 = warpGlobals.threshRangeRGBLow[0];
    }
    if( pix2 ){
      val2 = WlzRGBAPixelValue(*pix2, WLZ_RGBA_CHANNEL_RED, &errNum);
    }
    else {
      val2 = warpGlobals.threshRangeRGBHigh[0];
    }
    warpGlobals.threshRangeRGBLow[0] = WLZ_MIN(val1, val2);
    warpGlobals.threshRangeRGBHigh[0] = WLZ_MAX(val1, val2);

    /* set the sliders */
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_red_low"))){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBLow[0]);
    }
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_red_high"))){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBHigh[0]);
    } 

    /* set green */
    if( pix1 ){
      val1 = WlzRGBAPixelValue(*pix1, WLZ_RGBA_CHANNEL_GREEN, &errNum);
    }
    else {
      val1 = warpGlobals.threshRangeRGBLow[1];
    }
    if( pix2 ){
      val2 = WlzRGBAPixelValue(*pix2, WLZ_RGBA_CHANNEL_GREEN, &errNum);
    }
    else {
      val2 = warpGlobals.threshRangeRGBHigh[1];
    }
    warpGlobals.threshRangeRGBLow[1] = WLZ_MIN(val1, val2);
    warpGlobals.threshRangeRGBHigh[1] = WLZ_MAX(val1, val2);

    /* set the sliders */
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_green_low"))){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBLow[1]);
    } 
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_green_high"))){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBHigh[1]);
    } 

    /* set blue */
    if( pix1 ){
      val1 = WlzRGBAPixelValue(*pix1, WLZ_RGBA_CHANNEL_BLUE, &errNum);
    }
    else {
      val1 = warpGlobals.threshRangeRGBLow[2];
    }
    if( pix2 ){
      val2 = WlzRGBAPixelValue(*pix2, WLZ_RGBA_CHANNEL_BLUE, &errNum);
    }
    else {
      val2 = warpGlobals.threshRangeRGBHigh[2];
    }
    warpGlobals.threshRangeRGBLow[2] = WLZ_MIN(val1, val2);
    warpGlobals.threshRangeRGBHigh[2] = WLZ_MAX(val1, val2);

    /* set the sliders */
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_blue_low"))){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBLow[2]);
    } 
    if((slider = XtNameToWidget(warpGlobals.sgnlControls,
				"*.thresh_range_blue_high"))){
      HGU_XmSetSliderValue(slider, warpGlobals.threshRangeRGBHigh[2]);
    } 
    break;

  case WLZ_RGBA_THRESH_SLICE:
  case WLZ_RGBA_THRESH_BOX:
  case WLZ_RGBA_THRESH_SPHERE:
    /* use grey or rgb value to set threshold end-points */
    if( pix1 ){
      r = WlzRGBAPixelValue(*pix1, WLZ_RGBA_CHANNEL_RED, &errNum);
      g = WlzRGBAPixelValue(*pix1, WLZ_RGBA_CHANNEL_GREEN, &errNum);
      b = WlzRGBAPixelValue(*pix1, WLZ_RGBA_CHANNEL_BLUE, &errNum);
      WLZ_RGBA_RGBA_SET(warpGlobals.lowRGBPoint.v.rgbv, r, g, b, 255);
      if((text = XtNameToWidget(warpGlobals.sgnlControls,
				"*textRGBStart"))){
	sprintf(buf, "%d,%d,%d", r, g, b);
	XtVaSetValues(text, XmNvalue, buf, NULL);
      }
    }
    if( pix2 ){
      r = WlzRGBAPixelValue(*pix2, WLZ_RGBA_CHANNEL_RED, &errNum);
      g = WlzRGBAPixelValue(*pix2, WLZ_RGBA_CHANNEL_GREEN, &errNum);
      b = WlzRGBAPixelValue(*pix2, WLZ_RGBA_CHANNEL_BLUE, &errNum);
      WLZ_RGBA_RGBA_SET(warpGlobals.highRGBPoint.v.rgbv, r, g, b, 255);
      if((text = XtNameToWidget(warpGlobals.sgnlControls,
				"*textRGBFinish"))){
	sprintf(buf, "%d,%d,%d", r, g, b);
	XtVaSetValues(text, XmNvalue, buf, NULL);
      }
    }
    break;

  default:
    break;
  }

  return;
}

static WlzDVertex2 warpThreshDist(
  int	x0,
  int	y0,
  int	x1,
  int	y1)
{
  WlzDVertex2	rtnVtx;

  /* just use real distance for now which should allow saturation
     threshold in most cases - maybe too fast for low values */
  rtnVtx.vtX = x1 - x0;
  rtnVtx.vtY = y1 - y0;

  return rtnVtx;
}

void warpThreshSetLimitsFromDist(
  WlzPixelV	centreCol,	
  WlzDVertex2	dist,
  WlzPixelV	*sgnlStart,
  WlzPixelV	*sgnlFinish)
{
  int	radius;
  int	red, green, blue;

  /* use absolute value of x-distance for now */
  radius = WLZ_NINT(fabs(dist.vtX));
  switch( centreCol.type ){
  case WLZ_GREY_INT:
    sgnlStart->v.inv = centreCol.v.inv - radius;
    sgnlFinish->v.inv = centreCol.v.inv + radius;
    break;

  case WLZ_GREY_SHORT:
    sgnlStart->v.shv = centreCol.v.shv - radius;
    sgnlFinish->v.shv = centreCol.v.shv + radius;
    break;

  case WLZ_GREY_UBYTE:
    sgnlStart->v.ubv = WLZ_CLAMP(centreCol.v.ubv - radius, 0, 255);
    sgnlFinish->v.ubv = WLZ_CLAMP(centreCol.v.ubv + radius, 0, 255);
    break;

  case WLZ_GREY_FLOAT:
    sgnlStart->v.flv = centreCol.v.flv - radius;
    sgnlFinish->v.flv = centreCol.v.flv + radius;
    break;

  case WLZ_GREY_DOUBLE:
    sgnlStart->v.dbv = centreCol.v.dbv - radius;
    sgnlFinish->v.dbv = centreCol.v.dbv + radius;
    break;

  case WLZ_GREY_RGBA:
    red = WLZ_RGBA_RED_GET(centreCol.v.rgbv) - radius;
    red = WLZ_CLAMP(red, 0, 255);
    green = WLZ_RGBA_GREEN_GET(centreCol.v.rgbv) - radius;
    green = WLZ_CLAMP(green, 0, 255);
    blue = WLZ_RGBA_BLUE_GET(centreCol.v.rgbv) - radius;
    blue = WLZ_CLAMP(blue, 0, 255);
    WLZ_RGBA_RGBA_SET(sgnlStart->v.rgbv, red, green, blue, 255);
    red = WLZ_RGBA_RED_GET(centreCol.v.rgbv) + radius;
    red = WLZ_CLAMP(red, 0, 255);
    green = WLZ_RGBA_GREEN_GET(centreCol.v.rgbv) + radius;
    green = WLZ_CLAMP(green, 0, 255);
    blue = WLZ_RGBA_BLUE_GET(centreCol.v.rgbv) + radius;
    blue = WLZ_CLAMP(blue, 0, 255);
    WLZ_RGBA_RGBA_SET(sgnlFinish->v.rgbv, red, green, blue, 255);
    break;

  default:
    /* improbable error, do nothing */
    break;
  }

  return;
}

static int 			sgnlTrigger=0, liftTrigger=0;
static int			sgnlFinishFlg=0;
static WlzPixelV 		sgnlStart, sgnlFinish;
static WlzGreyValueWSpace	*sgnl_gVWSp = NULL;
static int 			startX, startY;
static WlzPixelV		centreCol;

void sgnlCanvasInputCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmDrawingAreaCallbackStruct	*cbs=(XmDrawingAreaCallbackStruct *) call_data;
  int		domainResetFlg, domainIncrFlg;
  double	line, kol;
  unsigned int	modMask=(ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|
			 Mod4Mask|Mod5Mask);
  WlzErrorNum	errNum;
  WlzIVertex2	selVtx, *rtnVtx=NULL;

  /* check for signal processed object - else do nothing */
  if( warpGlobals.sgnlProcObj ){
    if( sgnl_gVWSp ){
      if((sgnl_gVWSp->domain.core != warpGlobals.sgnlProcObj->domain.core) ||
	 (sgnl_gVWSp->values.core != warpGlobals.sgnlProcObj->values.core)){
	WlzGreyValueFreeWSp(sgnl_gVWSp);
	sgnl_gVWSp = WlzGreyValueMakeWSp(warpGlobals.sgnlProcObj, &errNum);
      }
    }
    else {
      sgnl_gVWSp = WlzGreyValueMakeWSp(warpGlobals.sgnlProcObj, &errNum);
    }
  }
  else {
    return;
  }

  /* switch on event type */
  domainResetFlg = 0;
  domainIncrFlg = 0;
  switch( cbs->event->type ){

  case ButtonPress:
    /* remap the event */
    if( MAPaintEventRemap(MAPAINT_WARP_SGNL_CONTEXT,
			  warpGlobals.pickThreshFlg?
			  MAPAINT_PICK_MODE:MAPAINT_THRESHOLD_MODE,
			  cbs->event) != WLZ_ERR_NONE ){
      break;
    }

    if( sgnl_gVWSp ){
      line = cbs->event->xbutton.y / warpGlobals.sgnl.mag;
      kol = cbs->event->xbutton.x / warpGlobals.sgnl.mag;
      line += warpGlobals.sgnlProcObj->domain.i->line1;
      kol += warpGlobals.sgnlProcObj->domain.i->kol1;
      selVtx.vtX = kol;
      selVtx.vtY = line;
      rtnVtx = &selVtx;

      /* get the value */
      WlzGreyValueGet(sgnl_gVWSp, 0.0, line, kol);
      sgnlStart.type = sgnl_gVWSp->gType;
      sgnlFinish.type = sgnl_gVWSp->gType;
      sgnlStart.v = sgnl_gVWSp->gVal[0];
      sgnlFinish.v = sgnl_gVWSp->gVal[0];
    }
    else {
      /* report an error */
      MAPaintReportWlzError(globals.topl,
			    "sgnlCanvasInputCb", errNum);
      break;
    }
    switch( cbs->event->xbutton.button ){

    case Button1:
      /* set initial point and initiate drag response */
      /* only if no modifiers pressed */
      if( warpGlobals.pickThreshFlg ){
	
	/* pick mode, set start only */
	sgnlTrigger = 1;
	sgnlFinishFlg = 0;
	sgnlInteractSetHighLowControls(&sgnlStart, NULL);
	domainResetFlg = 1;
      }
      else if( !(cbs->event->xbutton.state&modMask) ){
	/* set the start vertex to calculate distance */
	startX = cbs->event->xbutton.x;
	startY = cbs->event->xbutton.y;
	centreCol = sgnlStart;

	/* normal and distance mode */
	sgnlTrigger = 1;
	sgnlInteractSetHighLowControls(&sgnlStart, &sgnlFinish);
	domainResetFlg = 1;
      }
      else if( cbs->event->xbutton.state&ControlMask ){
	liftTrigger = 1;
	warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
      }
      break;

    case Button2:
      /* only pick mode and only if no modifiers pressed */
      if( warpGlobals.pickThreshFlg && !(cbs->event->xbutton.state&modMask) ){
	sgnlTrigger = 1;
	sgnlFinishFlg = 1;
	sgnlInteractSetHighLowControls(NULL, &sgnlFinish);
	domainResetFlg = 1;
      }
      break;

    case Button3:		/* unused */
    default:
      break;

    }
    break;

  case ButtonRelease:
    if( sgnl_gVWSp ){
      line = cbs->event->xbutton.y / warpGlobals.sgnl.mag;
      kol = cbs->event->xbutton.x / warpGlobals.sgnl.mag;
      line += warpGlobals.sgnlProcObj->domain.i->line1;
      kol += warpGlobals.sgnlProcObj->domain.i->kol1;

      /* get the value */
      WlzGreyValueGet(sgnl_gVWSp, 0.0, line, kol);
      sgnlFinish.v = sgnl_gVWSp->gVal[0];
    }
    else {
      /* report an error */
      MAPaintReportWlzError(globals.topl,
			    "sgnlCanvasInputCb", errNum);
      break;
    }
    switch( cbs->event->xbutton.button ){

    case Button1:
      /* set endpoint */
      if( sgnlTrigger ){
	sgnlTrigger = 0;
	if( warpGlobals.pickThreshFlg ){
	  sgnlInteractSetHighLowControls(&sgnlFinish, NULL);
	  domainResetFlg = 1;
	}
	else {
	  /* normal or distance mode */
	  if( warpGlobals.distanceThreshFlg ){
	    WlzDVertex2	dist;
	    dist = warpThreshDist(startX, startY,
				  cbs->event->xbutton.x,
				  cbs->event->xbutton.y);
	    warpThreshSetLimitsFromDist(centreCol, dist,
					&sgnlStart, &sgnlFinish);
	    warpThreshRadiusSet(dist);
	  }
	  sgnlInteractSetHighLowControls(&sgnlStart, &sgnlFinish);
	  domainResetFlg = 1;
	}

	/* if increment then push onto stack but only in interactive mode
	   for other modes then the segmentation is confirmed by <return>
	   in the canvas window - why? */
	/* Remove this for now see if anything terrible happens */
	/*if(domainResetFlg && warpGlobals.incrThreshFlg &&
	   (warpGlobals.thresholdType != WLZ_RGBA_THRESH_SINGLE) &&
	   (warpGlobals.thresholdType != WLZ_RGBA_THRESH_MULTI)){
	  domainIncrFlg = 1;
	  }*/
	domainIncrFlg = 1;
      }

      if( liftTrigger ){
	if( warpGlobals.sgnlObj ){
	  warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
	}
	XFlush(XtDisplayOfObject(w));
	liftTrigger = 0;
      }
      break;

    case Button2:
      /* only pick mode and only if no modifiers pressed */
      if( warpGlobals.pickThreshFlg && !(cbs->event->xbutton.state&modMask) ){
	sgnlTrigger = 0;

	/* set the value */
	sgnlInteractSetHighLowControls(NULL, &sgnlFinish);
	domainResetFlg = 1;

	/* if increment then push onto stack but only in interactive mode
	   for other modes then the segmentation is confirmed by <return>
	   in the canvas window */
	/*if(warpGlobals.incrThreshFlg &&
	   (warpGlobals.thresholdType != WLZ_RGBA_THRESH_SINGLE) &&
	   (warpGlobals.thresholdType != WLZ_RGBA_THRESH_MULTI)){
	  domainIncrFlg = 1;
	  }*/
	domainIncrFlg = 1;
      }
      break;

    case Button3:
      if(warpGlobals.incrThreshFlg){
	sgnlIncrPush(warpGlobals.sgnlObj);
      }
      break;

    default:
      break;

    }
    break;

  case MotionNotify:
    /* in all cases report the grey-level value */
    line = cbs->event->xmotion.y / warpGlobals.sgnl.mag;
    kol = cbs->event->xmotion.x / warpGlobals.sgnl.mag;
    line += warpGlobals.sgnlProcObj->domain.i->line1;
    kol += warpGlobals.sgnlProcObj->domain.i->kol1;

    /* get the value */
    WlzGreyValueGet(sgnl_gVWSp, 0.0, line, kol);
    sgnlFinish.v = sgnl_gVWSp->gVal[0];

    /* if drag then reset threshold levels and threshold domain */
    if( cbs->event->xmotion.state & (Button1Mask|Button2Mask|Button3Mask) ){
      if( sgnlTrigger ){
	if( warpGlobals.pickThreshFlg ){
	  if( sgnlFinishFlg ){
	    sgnlInteractSetHighLowControls(NULL, &sgnlFinish);
	    domainResetFlg = 1;
	  }
	  else {
	    sgnlInteractSetHighLowControls(&sgnlFinish, NULL);
	    domainResetFlg = 1;
	  }
	}
	else {
	  if( warpGlobals.distanceThreshFlg ){
	    WlzDVertex2	dist;
	    dist = warpThreshDist(startX, startY,
				  cbs->event->xmotion.x,
				  cbs->event->xmotion.y);
	    warpThreshSetLimitsFromDist(centreCol, dist,
					&sgnlStart, &sgnlFinish);
	    warpThreshRadiusSet(dist);
	  }
	  sgnlInteractSetHighLowControls(&sgnlStart, &sgnlFinish);
	  domainResetFlg = 1;
	}
      }
    }
    break;

  case KeyPress:
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){

    case XK_Return:/* push current domain onto stack in increment mode */
      if(warpGlobals.incrThreshFlg){
	domainIncrFlg = 1;
      }
      break;

    case XK_z:
      /* check modifiers */
      if( !(cbs->event->xkey.state&ControlMask) ){
	break;
      }
    case XK_Undo:
      /* clear the display */
      warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);

      /* pop one and get the object at the head of the list */
      (void) sgnlIncrPop();
      if( warpGlobals.sgnlObj ){
	WlzFreeObj(warpGlobals.sgnlObj);
	warpGlobals.sgnlObj = NULL;
      }
      warpGlobals.sgnlObj  = WlzAssignObject(sgnlIncrObj(), NULL);
      if( warpGlobals.sgnlObj ){
	warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
      }
      XFlush(XtDisplayOfObject(w));
      break;
	
    case XK_Right:
    case XK_KP_Right:
    case XK_f:
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_KP_Left:
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

  /* reset domain */
  if( domainResetFlg ){
    sgnlResetSgnlDomain(w, rtnVtx);
  }

  /* set increment */
  if( domainIncrFlg ){
    sgnlIncrPush(warpGlobals.sgnlObj);
  }

  return;
}
 
void sgnlCanvasMotionInputCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmDrawingAreaCallbackStruct	*cbs=(XmDrawingAreaCallbackStruct *) call_data;
  double	line, kol;
  WlzErrorNum	errNum;

  /* check for signal processed object - else do nothing */
  if( warpGlobals.sgnlProcObj ){
    if( sgnl_gVWSp ){
      if((sgnl_gVWSp->domain.core != warpGlobals.sgnlProcObj->domain.core) ||
	 (sgnl_gVWSp->values.core != warpGlobals.sgnlProcObj->values.core)){
	WlzGreyValueFreeWSp(sgnl_gVWSp);
	sgnl_gVWSp = WlzGreyValueMakeWSp(warpGlobals.sgnlProcObj, &errNum);
      }
    }
    else {
      sgnl_gVWSp = WlzGreyValueMakeWSp(warpGlobals.sgnlProcObj, &errNum);
    }
  }
  else {
    return;
  }

  /* switch on event type */
  switch( cbs->event->type ){

  case MotionNotify:
    /* in all cases report the grey-level value */
    line = cbs->event->xmotion.y / warpGlobals.sgnl.mag;
    line += warpGlobals.sgnlProcObj->domain.i->line1;
    kol = cbs->event->xmotion.x / warpGlobals.sgnl.mag;
    kol += warpGlobals.sgnlProcObj->domain.i->kol1;

    /* get the value */
    WlzGreyValueGet(sgnl_gVWSp, 0.0, line, kol);

    /* get the text widget and reset value */
    if( warpGlobals.sgnl.text ){
      char	feedbackBuf[16];
      WlzPixelV	pix;

      pix.type = sgnl_gVWSp->gType;
      pix.v = sgnl_gVWSp->gVal[0];
      switch( pix.type ){
      case WLZ_GREY_INT:
	sprintf(feedbackBuf, "%d", pix.v.inv);
	break;

      case WLZ_GREY_SHORT:
	sprintf(feedbackBuf, "%d", pix.v.shv);
	break;

      case WLZ_GREY_UBYTE:
	sprintf(feedbackBuf, "%d", (int) pix.v.ubv);
	break;

      case WLZ_GREY_FLOAT:
	sprintf(feedbackBuf, "%.2f", pix.v.flv);
	break;

      case WLZ_GREY_DOUBLE:
	sprintf(feedbackBuf, "%.2f", pix.v.dbv);
	break;

      case WLZ_GREY_RGBA:
	sprintf(feedbackBuf, "%d,%d,%d",
		WLZ_RGBA_RED_GET(pix.v.rgbv),
		WLZ_RGBA_GREEN_GET(pix.v.rgbv),
		WLZ_RGBA_BLUE_GET(pix.v.rgbv));
	break;
      default:
	sprintf(feedbackBuf, "Bad grey type");
      }
      XtVaSetValues(warpGlobals.sgnl.text,
		    XmNvalue, feedbackBuf,
		    NULL);
      XFlush(XtDisplayOfObject(w));
    }
    break;
  }

  return;
}

