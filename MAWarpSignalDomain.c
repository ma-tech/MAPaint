#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Mouse Atlas MAPaint					*
*   File       :   MAWarpSignalDomain.c					*
*************************************************************************
*   Author Name :  richard						*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Tue Nov  4 17:29:28 2003				*
*   $Revision$							*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>
#include <MAWarp.h>

extern void sgnlIncrClear(void);
extern void sgnlIncrPush(
  WlzObject	*obj);
extern WlzObject *sgnlIncrPop(void);
extern WlzObject *sgnlIncrObj(void);


void warpSetSignalDomain(
  WlzIVertex2	*selVtx)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzPixelV	threshV, threshV1;
  WlzObject	*obj, *obj1;
  WlzCompoundArray	*cobj;
  WlzUInt	combineMode;

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
    case WLZ_RGBA_THRESH_NONE:
      break;

    case WLZ_RGBA_THRESH_SINGLE:
      threshV.type = WLZ_GREY_INT;
      threshV.v.inv = warpGlobals.threshRangeLow;
      if( obj1 ){
	/* clear signal object */
	if( warpGlobals.sgnlObj ){
	  WlzFreeObj(warpGlobals.sgnlObj);
	}
	  
	if( obj = WlzThreshold(obj1, threshV, WLZ_THRESH_HIGH, &errNum) ){
	  obj = WlzAssignObject(obj, &errNum);
	  WlzFreeObj(obj1);
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

  /* check for local mode */
  if( warpGlobals.sgnlObj && !warpGlobals.globalThreshFlg ){
    if( selVtx != NULL ){
      warpGlobals.globalThreshVtx = *selVtx;
    }
    /* extract a local domain if the vertex is sensible */
    if( warpGlobals.globalThreshVtx.vtX != -10000 ){
      WlzObject	**objs=NULL;
      int	i, numObjs;
      double	x, y;
      obj1 = NULL;
      x = warpGlobals.globalThreshVtx.vtX;
      y = warpGlobals.globalThreshVtx.vtY;
      errNum = WlzLabel(warpGlobals.sgnlObj, &numObjs, &objs, 8192, 0,
			WLZ_4_CONNECTED);
      if( (errNum == WLZ_ERR_INT_DATA) && (numObjs == 8192) ){
	WlzObject	*tmpObj1, *tmpObj2;
	WlzDomain	domain;
	WlzValues	values;

	/* try again, smaller domain */
	for(i=0; i < numObjs; i++){
	  WlzFreeObj( objs[i] );
	}
	AlcFree((void *) objs);
	objs = NULL;
	numObjs = 0;
	domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_RECT,
					 y - 80, y + 80,
					 x - 80, x + 80, &errNum);
	values.core = NULL;
	if( tmpObj1 = WlzMakeMain(warpGlobals.sgnlObj->type, domain, values,
				  NULL, NULL, &errNum) ){
	  if( tmpObj2 = WlzIntersect2(warpGlobals.sgnlObj, tmpObj1, &errNum) ){
	    tmpObj2->values = WlzAssignValues(warpGlobals.sgnlObj->values, NULL);
	    errNum = WlzLabel(warpGlobals.sgnlObj, &numObjs, &objs, 8192, 0,
			      WLZ_4_CONNECTED);
	    WlzFreeObj(tmpObj2);
	    if((errNum == WLZ_ERR_INT_DATA) && (numObjs == 8192) ){
	      errNum = WLZ_ERR_NONE;
	    }
	  }
	  WlzFreeObj(tmpObj1);
	}
	
      }
      if( errNum == WLZ_ERR_NONE ){

	for(i=0; i < numObjs; i++){
	  if( WlzInsideDomain( objs[i], 0.0, y, x, NULL ) ){
	    obj1 = WlzMakeMain(objs[i]->type,
			       objs[i]->domain,
			       objs[i]->values,
			       NULL, NULL, NULL);
	    obj1 = WlzAssignObject(obj1, NULL);
	  }
	  WlzFreeObj( objs[i] );
	}
	AlcFree((void *) objs);
      }
      if( obj1 ){
	WlzFreeObj(warpGlobals.sgnlObj);
	warpGlobals.sgnlObj = obj1;
      }
    }
    else {
      WlzFreeObj(warpGlobals.sgnlObj);
      warpGlobals.sgnlObj = NULL;
    }
  }

  /* check for increment mode */
  if( warpGlobals.incrThreshFlg && sgnlIncrObj() ){
    if( warpGlobals.sgnlObj ){
      if( obj1 = WlzUnion2(warpGlobals.sgnlObj, sgnlIncrObj(),
			   &errNum) ){
	WlzFreeObj(warpGlobals.sgnlObj);
	warpGlobals.sgnlObj = WlzAssignObject(obj1, &errNum);
      }
    }
    else {
      warpGlobals.sgnlObj = WlzAssignObject(sgnlIncrObj(),
					    &errNum);
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "warpSetSignalDomain", errNum);
  }
  return;
}

void sgnlResetSgnlDomain(
  Widget	w,
  WlzIVertex2	*selVtx)
{

  /* set the signal domain */
  if( warpGlobals.sgnlObj ){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
  }
  warpSetSignalDomain(selVtx);
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }
  XFlush(XtDisplayOfObject(w));

  return;
}

