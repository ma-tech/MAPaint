#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Woolz Library					*
*   File       :   MAWarpInputXUtils.c					*
*************************************************************************
* This module has been copied from the original woolz library and       *
* modified for the public domain distribution. The original authors of  *
* the code and the original file headers and comments are in the        *
* HISTORY file.                                                         *
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Nov 29 14:18:34 1999				*
*   $Revision$							*
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

#include <MAPaint.h>
#include <MAWarp.h>

void warpSetXOvlyImageDstCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;

  if( winStruct ){
    warpSetOvlyXImages(winStruct, 0);
    warpSetOvlyXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);
  return;
}

void warpSetXOvlyImageSrcCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;

  if( winStruct ){
    warpSetOvlyXImages(winStruct, 1);
    warpSetOvlyXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);
  return;
}

void warpSetXImageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;

  if( winStruct ){
    warpSetXImage(winStruct);
  }
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);
  return;
}

WlzObject *warpTransformObj(
  WlzObject	*obj,
  MAPaintImageWinStruct *winStruct,
  int		ovlyIndex)
{
  WlzObject	*rtnObj=NULL, *tmpObj;
  Widget	toggle=NULL;
  Boolean	toggleFlg;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check the object */
  if( !obj || !(obj->type == WLZ_2D_DOMAINOBJ) ){
    return rtnObj;
  }
  rtnObj = WlzCopyObject(obj, &errNum);

  /* get transformation options */
  /* take gradient */
  if( errNum == WLZ_ERR_NONE ){
    if(winStruct->popup){
      switch(ovlyIndex){
      default:
      case -1:
	toggle = XtNameToWidget(winStruct->popup, "*.gradient");
	break;
      case 0:
	toggle = XtNameToWidget(winStruct->popup, "*.gradient dest-image");
	break;
      case 1:
	toggle = XtNameToWidget(winStruct->popup, "*.gradient src-image");
	break;
      }
      if( toggle ){
	toggleFlg = XmToggleButtonGetState(toggle);
      }
      else {
	toggleFlg = False;
      }
    }
    else {
      toggleFlg = False;
    }
    if( toggleFlg ){
      rtnObj = WlzAssignObject(rtnObj, NULL);
      if( tmpObj = WlzGreyModGradient(rtnObj, 3.0, &errNum) ){
	WlzFreeObj(rtnObj);
	rtnObj = tmpObj;
	errNum = WlzGreyNormalise(rtnObj);
      }
    }
  }

  /* inversion */
  if( errNum == WLZ_ERR_NONE ){
    if(winStruct->popup){
      switch(ovlyIndex){
      default:
      case -1:
	toggle = XtNameToWidget(winStruct->popup, "*.invert");
	break;
      case 0:
	toggle = XtNameToWidget(winStruct->popup, "*.invert dest-image");
	break;
      case 1:
	toggle = XtNameToWidget(winStruct->popup, "*.invert src-image");
	break;
      }
      if( toggle ){
	toggleFlg = XmToggleButtonGetState(toggle);
      }
      else {
	toggleFlg = False;
      }
    }
    else {
      toggleFlg = winStruct->grey_invert;
    }
    if( toggleFlg ){
      WlzPixelV	min, max;

      WlzGreyRange(rtnObj, &min, &max);
      WlzValueConvertPixel(&min, min, WLZ_GREY_INT);
      WlzValueConvertPixel(&max, max, WLZ_GREY_INT);
      if( (min.v.inv >= 0) && (max.v.inv <= 255) ){
	min.v.inv = 0;
	max.v.inv = 255;
      }
      errNum = WlzGreyInvertMinMax(rtnObj, min, max);
    }
  }

  /* normalise  - note gradient operation normalises anyway */
  if( errNum == WLZ_ERR_NONE ){
    if(winStruct->popup){
      switch(ovlyIndex){
      default:
      case -1:
	toggle = XtNameToWidget(winStruct->popup, "*.normalise");
	break;
      case 0:
	toggle = XtNameToWidget(winStruct->popup, "*.normalise dest-image");
	break;
      case 1:
	toggle = XtNameToWidget(winStruct->popup, "*.normalise src-image");
	break;
      }
      if( toggle ){
	toggleFlg = XmToggleButtonGetState(toggle);
      }
      else {
	toggleFlg = False;
      }
    }
    else {
      toggleFlg = False;
    }
    if( toggleFlg ){
      errNum = WlzGreyNormalise(rtnObj);
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "warpTransformObj", errNum);
    if( rtnObj ){
      WlzFreeObj(rtnObj);
      rtnObj = NULL;
    }
  }
  return rtnObj;
}

int HGU_XGetColorIndexFromMask24(
  unsigned long mask,
  int		order)
{
  int index;

  switch( mask ){
  case 0xff:
    index = 0;
    break;
  case 0xff00:
    index = 1;
    break;
  case 0xff0000:
    index = 2;
    break;
  case 0xff000000:
    index = 3;
    break;
  default:
    index = 0;
    break;
  }
  if( order == MSBFirst ){
    index = 3 - index;
  }

  return index;
}

XImage	*warpCreateXImage(
  WlzObject	*obj,
  WlzObject	*refObj,
  MAPaintImageWinStruct *winStruct)
{
  XImage		*rtnImage=NULL;
  XWindowAttributes	win_att;
  Dimension		src_width, src_height, width, height;
  int			kOffset, lOffset;
  WlzUByte			*data, *dst_data, gammaLUT[256];
  int			i, j;
  WlzGreyValueWSpace	*gVWSp = NULL;
  int			cmpndFlg=0;
  WlzUInt		r, g, b, a;
  int			rIndx, gIndx, bIndx, aIndx;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* create the ximage using current mag and rotate */
  if( XGetWindowAttributes(XtDisplay(winStruct->canvas),
			   XtWindow(winStruct->canvas), &win_att) == 0 ){
    return rtnImage;
  }
  if( refObj ){
    src_width = refObj->domain.i->lastkl - refObj->domain.i->kol1 + 1; 
    src_height = refObj->domain.i->lastln - refObj->domain.i->line1 + 1;
    kOffset = refObj->domain.i->kol1;
    lOffset = refObj->domain.i->line1;
  }
  else {
    src_width = obj->domain.i->lastkl - obj->domain.i->kol1 + 1; 
    src_height = obj->domain.i->lastln - obj->domain.i->line1 + 1;
    kOffset = obj->domain.i->kol1;
    lOffset = obj->domain.i->line1;
  }
  if( (winStruct->rot)%2 ){
    width = src_height*winStruct->mag;
    height = src_width*winStruct->mag;
  }
  else {
    width = src_width*winStruct->mag;
    height = src_height*winStruct->mag;
  }

  /*  XtResizeWidget(winStruct->canvas, width, height, 0);*/
  XtVaSetValues(winStruct->canvas,
		XmNwidth, width,
		XmNheight, height,
		NULL);

  if( gVWSp = WlzGreyValueMakeWSp(obj, &errNum) ){
    if( dst_data = (WlzUByte *) AlcMalloc(((win_att.depth == 8)?1:4)
				       *width*height*sizeof(char)) ){
      data = dst_data;
      rtnImage = XCreateImage(XtDisplay(winStruct->canvas),
			      win_att.visual, win_att.depth,
			      ZPixmap, 0, (char *) dst_data,
			      width, height, 8, 0);
      
      /* establish rgb index values if 24 bit */
      if( win_att.depth == 24 ){
	rIndx = HGU_XGetColorIndexFromMask24(win_att.visual->red_mask,
					     rtnImage->byte_order);
	gIndx = HGU_XGetColorIndexFromMask24(win_att.visual->green_mask,
					     rtnImage->byte_order);
	bIndx = HGU_XGetColorIndexFromMask24(win_att.visual->blue_mask,
					     rtnImage->byte_order);
	aIndx = HGU_XGetColorIndexFromMask24(~(win_att.visual->red_mask|
					       win_att.visual->green_mask|
					       win_att.visual->blue_mask),
					     rtnImage->byte_order);
      }

      /* set up the gamma LUT */
      gammaLUT[0] = 0;
      for(i=1; i < 256; i++){
	double	val;
	val = i;
	gammaLUT[i] = (int) (pow(val/255, winStruct->gamma) * 255);
      }

      /* fill in the values */
      for(j=0; j < height; j++){
	for(i=0; i < width; i++, data++){
	  WlzDVertex2 vtx1, vtx2;
	  vtx1.vtX = i;
	  vtx1.vtY = j;
	  vtx2 = warpDisplayTransBack(vtx1, winStruct);
	  WlzGreyValueGet(gVWSp, 0,
			  vtx2.vtY + lOffset,
			  vtx2.vtX + kOffset);
	  switch( win_att.depth ){
	  case 8:
	    switch( gVWSp->gType ){
	    default:
	    case WLZ_GREY_INT:
	      *data = WLZ_CLAMP(*(gVWSp->gPtr[0].inp), 0, 255);
	      break;
	    case WLZ_GREY_SHORT:
	      *data = WLZ_CLAMP(*(gVWSp->gPtr[0].shp), 0, 255);
	      break;
	    case WLZ_GREY_UBYTE:
	      *data = *(gVWSp->gPtr[0].ubp);
	      break;
	    case WLZ_GREY_FLOAT:
	      *data = WLZ_CLAMP(*(gVWSp->gPtr[0].flp), 0, 255);
	      break;
	    case WLZ_GREY_DOUBLE:
	      *data = WLZ_CLAMP(*(gVWSp->gPtr[0].dbp), 0, 255);
	      break;
	    case WLZ_GREY_RGBA:
	      *data = WLZ_RGBA_MODULUS(*(gVWSp->gPtr[0].rgbp));
	      break;
	    }
	    *data = gammaLUT[data[0]];
	    break;

	  case 24:
	    switch( gVWSp->gType ){
	    default:
	    case WLZ_GREY_INT:
	      r = WLZ_CLAMP(*(gVWSp->gPtr[0].inp), 0, 255);
	      r = gammaLUT[r];
	      g = b = r;
	      a = 0xff;
	      break;
	    case WLZ_GREY_SHORT:
	      r = WLZ_CLAMP(*(gVWSp->gPtr[0].shp), 0, 255);
	      r = gammaLUT[r];
	      g = b = r;
	      a = 0xff;
	      break;
	    case WLZ_GREY_UBYTE:
	      r = *(gVWSp->gPtr[0].ubp);
	      r = gammaLUT[r];
	      g = b = r;
	      a = 0xff;
	      break;
	    case WLZ_GREY_FLOAT:
	      r = WLZ_CLAMP(*(gVWSp->gPtr[0].flp), 0, 255);
	      r = gammaLUT[r];
	      g = b = r;
	      a = 0xff;
	      break;
	    case WLZ_GREY_DOUBLE:
	      r = WLZ_CLAMP(*(gVWSp->gPtr[0].dbp), 0, 255);
	      r = gammaLUT[r];
	      g = b = r;
	      a = 0xff;
	      break;
	    case WLZ_GREY_RGBA:
	      r = WLZ_RGBA_RED_GET(*(gVWSp->gPtr[0].rgbp));
	      g = WLZ_RGBA_GREEN_GET(*(gVWSp->gPtr[0].rgbp));
	      b = WLZ_RGBA_BLUE_GET(*(gVWSp->gPtr[0].rgbp));
	      a = WLZ_RGBA_ALPHA_GET(*(gVWSp->gPtr[0].rgbp));
	      break;
	    }
	    data[rIndx] = r;
	    data[gIndx] = g;
	    data[bIndx] = b;
	    data[aIndx] = a;
	    data += 3;
	    break;

	  default:
	    break;
	  }
	}
      }
    }
    else {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    WlzGreyValueFreeWSp(gVWSp);
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "warpCreateXImage", errNum);
  }
  return rtnImage;
}

void warpSetCmpdXImage(
  MAPaintImageWinStruct *winStruct)
{
  return;
}

void warpSetXImage(
  MAPaintImageWinStruct *winStruct)
{
  int			i;
  WlzObject		*obj = NULL;
  WlzCompoundArray	*cobj, *cobj1 = NULL;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* clear the old ximage */
  if( winStruct->ximage ){
    AlcFree(winStruct->ximage->data);
    winStruct->ximage->data = NULL;
    XDestroyImage(winStruct->ximage);
    winStruct->ximage = NULL;
  }

  /* get the object */
  if( !winStruct->obj ){
    return;
  }
  else {
    switch( winStruct->obj->type ){
    case WLZ_2D_DOMAINOBJ:
      obj = WlzAssignObject(warpTransformObj(winStruct->obj, winStruct, -1),
			    &errNum);
      break;

    case WLZ_COMPOUND_ARR_1:
    case WLZ_COMPOUND_ARR_2:
      cobj = (WlzCompoundArray *) winStruct->obj;
      if( cobj->n > 0 ){
	if(cobj1 = WlzMakeCompoundArray(cobj->type, 1, cobj->n,
					NULL, cobj->otype, &errNum) ){
	  for(i=0; i < cobj->n; i++){
	    cobj1->o[i] = WlzAssignObject(warpTransformObj((cobj->o)[i],
							   winStruct, -1),
					  NULL);
	  }
	  obj = WlzAssignObject(cobj1->o[0], NULL);
	}
      }
      break;

    default:
      return;
    }
  }

  if( obj ){

    if( cobj1 ){
      winStruct->ximage = warpCreateXImage(obj, obj, winStruct);
    }
    else {
      winStruct->ximage = warpCreateXImage(obj, NULL, winStruct);
    }

    WlzFreeObj(obj);
    if( cobj1 ){
      WlzFreeObj((WlzObject *) cobj1);
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "warpSetXImage", errNum);
  }
  return;
}

void warpSetOvlyXImages(
  MAPaintImageWinStruct *winStruct,
  int			imageIndex)
{
  XWindowAttributes	win_att;
  int			i;
  WlzObject		*obj0=NULL, *obj1=NULL;
  WlzCompoundArray	*cobj;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check the index */
  if( (imageIndex != 0) && (imageIndex != 1) ){
    return;
  }

  /* clear the old ximage */
  if( winStruct->ovlyImages[imageIndex] ){
    AlcFree(winStruct->ovlyImages[imageIndex]->data);
    winStruct->ovlyImages[imageIndex]->data = NULL;
    XDestroyImage(winStruct->ovlyImages[imageIndex]);
    winStruct->ovlyImages[imageIndex] = NULL;
  }

  /* get the object */
  if( !winStruct->obj ){
    return;
  }
  else {
    switch( winStruct->obj->type ){
    case WLZ_2D_DOMAINOBJ:
      /* something wrong here it should be a compound object */
      return;

    case WLZ_COMPOUND_ARR_1:
    case WLZ_COMPOUND_ARR_2:
      cobj = (WlzCompoundArray *) winStruct->obj;
      if( cobj->n <= imageIndex ){
	/* not enough objects */
	return;
      }
      switch( imageIndex ){
      case 0:
	obj0 = WlzAssignObject(warpTransformObj((cobj->o)[0],
						winStruct, 0), NULL);
	winStruct->ovlyImages[imageIndex] =
	  warpCreateXImage(obj0, NULL, winStruct);
	break;
	
      case 1:
	obj0 = WlzAssignObject((cobj->o)[0], NULL);
	obj1 = WlzAssignObject(warpTransformObj((cobj->o)[1],
						winStruct, 1), NULL);
	winStruct->ovlyImages[imageIndex] =
	  warpCreateXImage(obj1, obj0, winStruct);
	break;
      }
      break;

    default:
      return;
    }
  }

  if( obj1 ){
    WlzFreeObj(obj1);
  }
  if( obj0 ){
    WlzFreeObj(obj0);
  }

  return;
}

void warpSetOvlyXImage(
  MAPaintImageWinStruct *winStruct)
{
  XWindowAttributes	win_att;
  Dimension		width, height;
  WlzUByte			*data0, *data1, *dst_data, *data;
  int			i, j, acc;
  double		a, b, c;
  int			rIndx, gIndx, bIndx, aIndx;

  /* clear the old ximage */
  if( winStruct->ximage ){
    AlcFree(winStruct->ximage->data);
    winStruct->ximage->data = NULL;
    XDestroyImage(winStruct->ximage);
    winStruct->ximage = NULL;
  }

  /* overlay ximages must exist */
  if((winStruct->ovlyImages[0] == NULL) ||
     (winStruct->ovlyImages[1] == NULL)){
    return;
  }

  /* create the ximage using current mag and rotate */
  if( XGetWindowAttributes(XtDisplay(winStruct->canvas),
			   XtWindow(winStruct->canvas), &win_att) == 0 ){
    return;
  }
  if( win_att.depth != 24 ){
    return;
  }
  width = winStruct->ovlyImages[0]->width;
  height = winStruct->ovlyImages[0]->height;

  dst_data = (WlzUByte *) AlcCalloc(4*width*height, sizeof(char));
  data = dst_data;
  data0 = (WlzUByte *) winStruct->ovlyImages[0]->data;
  data1 = (WlzUByte *) winStruct->ovlyImages[1]->data;

  winStruct->ximage = XCreateImage(XtDisplay(winStruct->canvas),
				   win_att.visual, win_att.depth,
				   ZPixmap, 0, (char *) dst_data,
				   width, height, 8, 0);

  /* set up mixing factors */
  if( winStruct->mixRatio > 50.0){
    a = 1.0;
    b = (100.0 - winStruct->mixRatio) / 50.0;
  }
  else {
    a = winStruct->mixRatio / 50.0;
    b = 1.0;
  }
  c = winStruct->mixRatio / 100.0;

  /* merge the images */
  rIndx = HGU_XGetColorIndexFromMask24(win_att.visual->red_mask,
				       winStruct->ximage->byte_order);
  gIndx = HGU_XGetColorIndexFromMask24(win_att.visual->green_mask,
				       winStruct->ximage->byte_order);
  bIndx = HGU_XGetColorIndexFromMask24(win_att.visual->blue_mask,
				       winStruct->ximage->byte_order);
  aIndx = HGU_XGetColorIndexFromMask24(~(win_att.visual->red_mask|
					 win_att.visual->green_mask|
					 win_att.visual->blue_mask),
				       winStruct->ximage->byte_order);
 
  switch( winStruct->mixType ){
  case MA_OVERLAY_MIXTYPE_RG1:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[rIndx] = c * 255 + (1 - c) * data1[gIndx];
      data[gIndx] = c * data0[gIndx] + (1 - c) * 255;
      data[bIndx] = c * data0[gIndx] + (1 - c) * data1[gIndx];
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_RG2:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[rIndx] = a * (255 - data0[gIndx]);
      data[gIndx] = b * (255 - data1[gIndx]);
      data[bIndx] = 0;
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_RB1:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[rIndx] = c * 255 + (1 - c) * data1[gIndx];
      data[bIndx] = c * data0[gIndx] + (1-c) * 255;
      data[gIndx] = c * data0[gIndx] + (1-c) * data1[gIndx];
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_RB2:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[rIndx] = a * (255 - data0[gIndx]);
      data[bIndx] = b * (255 - data1[gIndx]);
      data[gIndx] = 0;
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_GB1:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[gIndx] = c * 255 + (1 - c) * data1[gIndx];
      data[bIndx] = c * data0[gIndx] + (1-c) * 255;
      data[rIndx] = c * data0[gIndx] + (1-c) * data1[gIndx];
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_GB2:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[gIndx] = a * (255 - data0[gIndx]);
      data[bIndx] = b * (255 - data1[gIndx]);
      data[rIndx] = 0;
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_RNR:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[rIndx] = a * (255 - data0[gIndx]);
      data[gIndx] = b * (255 - data1[gIndx]);
      data[bIndx] = b * (255 - data1[gIndx]);
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_GNG:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[gIndx] = a * (255 - data0[gIndx]);
      data[rIndx] = b * (255 - data1[gIndx]);
      data[bIndx] = b * (255 - data1[gIndx]);
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_BNB:
    for(i=0; i < width*height; i++, data += 4, data0 += 4, data1 += 4){
      data[bIndx] = a * (255 - data0[gIndx]);
      data[gIndx] = b * (255 - data1[gIndx]);
      data[rIndx] = b * (255 - data1[gIndx]);
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_DITHER1:
    for(i=0, acc=0; i < width*height;
	i++, data += 4, data0 += 4, data1 += 4){
      acc += 255 - data0[gIndx];
      if( acc > 127 ){
	data[rIndx] = a * 255 + (1-a) * data1[rIndx];
	data[gIndx] = (1-a) * data1[gIndx];
	data[bIndx] = (1-a) * data1[bIndx];
	acc -= 256;
      }
      else {
	data[rIndx] = b * data1[rIndx];
	data[gIndx] = b * data1[gIndx];
	data[bIndx] = b * data1[bIndx];
      }
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_DITHER2:
    for(i=0, acc=0; i < width*height;
	i++, data += 4, data0 += 4, data1 += 4){
      acc += 255 - data1[gIndx];
      if( acc > 127 ){
	data[rIndx] = b * 255 + (1-b) * data0[rIndx];
	data[gIndx] = (1-b) * data0[gIndx];
	data[bIndx] = (1-b) * data0[bIndx];
	acc -= 256;
      }
      else {
	data[rIndx] = a * data0[rIndx];
	data[gIndx] = a * data0[gIndx];
	data[bIndx] = a * data0[bIndx];
      }
      data[aIndx] = 0;
    }
    break;

  case MA_OVERLAY_MIXTYPE_BLINK:
    break;
  }

  return;
}

void warpUndisplayVtx(
  MAPaintImageWinStruct	*winStruct,
  WlzDVertex2	vtx)
{
  int	x, y, width, height;
  WlzDVertex2	vtx1;

  if( winStruct->ximage ){

    width = winStruct->ximage->width;
    height = winStruct->ximage->height;
    vtx1 = warpDisplayTransFore(vtx, winStruct);
    
    x = vtx1.vtX - 3;
    y = vtx1.vtY - 3;

    if( (x < -4) || (x >= width) || (y < -4) || (y >= height) ){
      return;
    }

    x = (x < 0) ? 0 : x;
    y = (y < 0) ? 0 : y;

    width = WLZ_MIN(width - x, 8);
    height = WLZ_MIN(height - y, 8);

    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage, x, y, x, y,
	      width, height);
    XFlush(XtDisplay(winStruct->canvas));
  }

  return;
}

void warpDisplayTiePoints(void)
{
  Display	*dpy=XtDisplay(warpGlobals.dst.canvas);
  Window	dst_win = XtWindow(warpGlobals.dst.canvas);
  Window	src_win = XtWindow(warpGlobals.src.canvas);
  GC		gc;
  int		i, x, y;
  WlzDVertex2	vtx1, vtx2;
  XWindowAttributes	win_att;

  /* check gc's */
  if( XGetWindowAttributes(dpy, dst_win, &win_att) == 0 ){
    return;
  }

  if( warpGlobals.red_gc == (GC) -1 ){
    XGCValues	values;

    values.foreground = win_att.visual->red_mask;
    values.background = 0;
    warpGlobals.red_gc = XCreateGC(dpy, dst_win,
				   GCForeground|GCBackground,
				   &values);
    values.foreground = win_att.visual->green_mask;
    warpGlobals.green_gc = XCreateGC(dpy, dst_win,
				   GCForeground|GCBackground,
				   &values);
  }

  for(i=0; i < warpGlobals.num_vtxs; i++){
    if(((warpGlobals.tp_state == TP_SELECTED) && (i == warpGlobals.sel_vtx)) ||
       (warpGlobals.sel_vtxs[i])){
      gc = warpGlobals.red_gc;
    }
    else {
      gc = warpGlobals.green_gc;
    }
    vtx2 = warpDisplayTransFore(warpGlobals.dst_vtxs[i],
				&(warpGlobals.dst));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, dst_win, gc, x, y, 4, 4);

    vtx2 = warpDisplayTransFore(warpGlobals.src_vtxs[i],
				&(warpGlobals.src));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, src_win, gc, x, y, 4, 4);
  }

  if( warpGlobals.tp_state == TP_DST_DEFINED ){
    gc = warpGlobals.red_gc;

    vtx2 = warpDisplayTransFore(warpGlobals.dst_vtxs[warpGlobals.num_vtxs],
				&(warpGlobals.dst));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, dst_win, gc, x, y, 4, 4);
  }

  if( warpGlobals.tp_state == TP_SRC_DEFINED ){
    gc = warpGlobals.red_gc;
    vtx2 = warpDisplayTransFore(warpGlobals.src_vtxs[warpGlobals.num_vtxs],
				&(warpGlobals.src));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, src_win, gc, x, y, 4, 4);
  }

  XFlush(dpy);
  return;
}

void warpDisplayDstMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  WlzMeshTransform	*meshTr = warpGlobals.meshTr;
  int			elemIdx;
  WlzMeshElem		*elem;
  int			xOff, yOff;
  Display	*dpy=XtDisplay(warpGlobals.src.canvas);
  Window	dst_win = XtWindow(warpGlobals.dst.canvas);
  GC		gc;
  XWindowAttributes	win_att;

  /* check there is a source image */
  if( !warpGlobals.src.obj || !warpGlobals.meshTr ){
    return;
  }
  xOff = warpGlobals.dst.obj->domain.i->kol1;
  yOff = warpGlobals.dst.obj->domain.i->line1;

  /* check the ximage */
  if( !winStruct->ximage ){
    warpSetXImage(winStruct);
    warpSetOvlyXImage(winStruct);
    if( !winStruct->ximage ){
      return;
    }
  }

  if( XGetWindowAttributes(dpy, dst_win, &win_att) == 0 ){
    return;
  }
  /* check the graphics context */
  if( warpGlobals.yellow_gc == (GC) -1 ){
    XGCValues	values;

    values.foreground = win_att.visual->red_mask | win_att.visual->green_mask;
    values.background = 0;
    warpGlobals.yellow_gc = XCreateGC(dpy, dst_win,
				      GCForeground|GCBackground,
				      &values);
  }
  gc = warpGlobals.yellow_gc;

  /* loop through the mesh displaying edges
     - note this covers each edge twice */
  elem = meshTr->elements;
  for(elemIdx=0; elemIdx < meshTr->nElem; elemIdx++, elem++){
    XPoint	points[4];
    WlzDVertex2	vtx;
    int		k;

    for(k=0; k < 3; k++){
      vtx = meshTr->nodes[elem->nodes[k]].position;
      vtx.vtX += meshTr->nodes[elem->nodes[k]].displacement.vtX;
      vtx.vtY += meshTr->nodes[elem->nodes[k]].displacement.vtY;
      vtx.vtX -= xOff;
      vtx.vtY -= yOff;
      vtx = warpDisplayTransFore(vtx, winStruct);
      points[k].x = vtx.vtX;
      points[k].y = vtx.vtY;
    }
    points[3] = points[0];
    
    /* check for duff elements */
    if( warpGlobals.meshErrFlg ){
      if( WlzMeshElemVerify(meshTr, 1, elem, NULL) != WLZ_ERR_NONE ){
	gc = warpGlobals.red_gc;
      }
      else {
	gc = warpGlobals.yellow_gc;
      }
    }
    XDrawLines(dpy, dst_win, gc, points, 4, CoordModeOrigin);
  }
  XFlush(dpy);

  return;
}

void warpDisplaySrcMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  WlzMeshTransform	*meshTr = warpGlobals.meshTr;
  int			elemIdx;
  WlzMeshElem		*elem;
  int			xOff, yOff;
  Display	*dpy=XtDisplay(warpGlobals.src.canvas);
  Window	src_win = XtWindow(warpGlobals.src.canvas);
  GC		gc;

  /* check there is a source image */
  if( !warpGlobals.src.obj || !warpGlobals.meshTr ){
    return;
  }
  xOff = warpGlobals.src.obj->domain.i->kol1;
  yOff = warpGlobals.src.obj->domain.i->line1;

  /* check the ximage */
  if( !winStruct->ximage ){
    warpSetXImage(winStruct);
    warpSetOvlyXImage(winStruct);
    if( !winStruct->ximage ){
      return;
    }
  }

  /* check the graphics context */
  if( warpGlobals.blue_gc == (GC) -1 ){
    XGCValues	values;

    values.foreground = 0xff0000;
    values.background = 0;
    warpGlobals.blue_gc = XCreateGC(dpy, src_win,
				   GCForeground|GCBackground,
				   &values);
  }
  gc = warpGlobals.blue_gc;

  /* loop through the mesh displaying edges
     - note this covers each edge twice */
  elem = meshTr->elements;
  for(elemIdx=0; elemIdx < meshTr->nElem; elemIdx++, elem++){
    XPoint	points[4];
    WlzDVertex2	vtx;
    int		k;

    for(k=0; k < 3; k++){
      vtx = meshTr->nodes[elem->nodes[k]].position;
      vtx.vtX -= xOff;
      vtx.vtY -= yOff;
      vtx = warpDisplayTransFore(vtx, winStruct);
      points[k].x = vtx.vtX;
      points[k].y = vtx.vtY;
    }
    points[3] = points[0];
    XDrawLines(dpy, src_win, gc, points, 4, CoordModeOrigin);
  }
  XFlush(dpy);

  return;
}

void warpCanvasExposeCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;

  if( !winStruct->ximage ){
    if( winStruct == &(warpGlobals.ovly) ){
      if( !winStruct->ovlyImages[0] ){
	warpSetOvlyXImages(winStruct, 0);
      }
      if( !winStruct->ovlyImages[1] ){
	warpSetOvlyXImages(winStruct, 1);
      }
      warpSetOvlyXImage(winStruct);
    }
    else {
      warpSetXImage(winStruct);
    }
    if( !winStruct->ximage ){
      return;
    }
  }

  /* check the graphics context */
  if( winStruct->gc == (GC) -1 ){
    XGCValues  gcvalues;
    winStruct->gc = XCreateGC(XtDisplay(winStruct->canvas),
			      XtWindow(winStruct->canvas),
			      0, &gcvalues);
  }

  /* display the image - if it is a real expose event just
     put the exposed rectangle */
  if( cbs && cbs->event && (cbs->event->type == Expose) ){
    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage,
	      cbs->event->xexpose.x, cbs->event->xexpose.y,
	      cbs->event->xexpose.x, cbs->event->xexpose.y,
	      cbs->event->xexpose.width, cbs->event->xexpose.height);
  }
  else {
    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage, 0, 0, 0, 0,
	      winStruct->ximage->width, winStruct->ximage->height);
  }
  XFlush(XtDisplay(winStruct->canvas));

  warpDisplayTiePoints();
  return;
}

void warpDisplayDomain(
  MAPaintImageWinStruct	*winStruct,
  WlzObject	*obj,
  int		displayFlg)
{
  Display	*dpy=XtDisplay(winStruct->canvas);
  Window	win = XtWindow(winStruct->canvas);
  GC		gc;
  int		x, y;
  Dimension	width, height;
  WlzDVertex2	vtx, vtx1, vtx2;
  WlzIntervalWSpace	iwsp;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  XWindowAttributes	win_att;

  /* check there is a displayed image */
  if(!winStruct->obj || !winStruct->ximage || !obj){
    return;
  }
  if( XGetWindowAttributes(dpy, win, &win_att) == 0 ){
    return;
  }

  /* check the graphics context */
  if( warpGlobals.yellow_gc == (GC) -1 ){
    XGCValues	values;

    values.foreground = win_att.visual->red_mask | win_att.visual->green_mask;
    values.background = 0;
    warpGlobals.yellow_gc = XCreateGC(dpy, win,
				      GCForeground|GCBackground,
				      &values);
  }
  gc = warpGlobals.yellow_gc;

  /* get the opposite vertices of the bounding box of each interval */
  errNum = WlzInitRasterScan(obj, &iwsp, WLZ_RASTERDIR_ILIC);
  while((errNum == WLZ_ERR_NONE) &&
	((errNum = WlzNextInterval(&iwsp)) == WLZ_ERR_NONE) ){
    vtx.vtX = iwsp.lftpos;
    vtx.vtY = iwsp.linpos;
    vtx.vtX -= winStruct->obj->domain.i->kol1;
    vtx.vtY -= winStruct->obj->domain.i->line1;
    vtx1 = warpDisplayTransFore(vtx, winStruct);

    vtx.vtX = iwsp.rgtpos+1;
    vtx.vtY = iwsp.linpos+1;
    vtx.vtX -= winStruct->obj->domain.i->kol1;
    vtx.vtY -= winStruct->obj->domain.i->line1;
    vtx2 = warpDisplayTransFore(vtx, winStruct);

    x = (int) (WLZ_MIN(vtx1.vtX, vtx2.vtX));
    y = (int) (WLZ_MIN(vtx1.vtY, vtx2.vtY));
    width = (int) ((vtx1.vtX < vtx2.vtX) ?
		   (vtx2.vtX - vtx1.vtX):(vtx1.vtX - vtx2.vtX));
    height = (int) ((vtx1.vtY < vtx2.vtY) ?
		    (vtx2.vtY - vtx1.vtY):(vtx1.vtY - vtx2.vtY));
    width = WLZ_MAX(width-1, 1);
    height = WLZ_MAX(height-1, 1);

    if( displayFlg ){
      XFillRectangle(dpy, win, gc, x, y, width, height);
    }
    else {
      XPutImage(dpy, win, winStruct->gc, winStruct->ximage, x, y, x, y,
		width, height);
    }
  }
  if( errNum == WLZ_ERR_EOO ){
    errNum = WLZ_ERR_NONE;
  }

  XFlush(dpy);
  
  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "warpDisplayDomain", errNum);
  }
  return;
}
