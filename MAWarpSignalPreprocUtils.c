#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MAWarpSignalPrepr_cUtils_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MAWarpSignalPreprocUtils.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:42:59 2009
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

void warpSetSignalProcObj(void)
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
    if((obj1 = WlzCopyObject(warpGlobals.sgnl.obj, &errNum))){
      obj1 = WlzAssignObject(obj1, NULL);
    }
  }
  else {
    return;
  }

  /* normalise the data */
  if( errNum == WLZ_ERR_NONE ){
    if((toggle = XtNameToWidget(globals.topl,
				"*warp_sgnl_controls_form*normalise"))){
      XtVaGetValues(toggle, XmNset, &setFlg, NULL);
      if( setFlg ){
	errNum = WlzGreyNormalise(obj1, 0);
      }
    }
  }

  /* Histo equalise */
  if( errNum == WLZ_ERR_NONE ){
    if((toggle = XtNameToWidget(globals.topl,
				"*warp_sgnl_controls_form*histo_equalise"))){
      XtVaGetValues(toggle, XmNset, &setFlg, NULL);
      if( setFlg ){
	errNum = WlzHistogramEqualiseObj(obj1, 1, 1);
      }
    }
  }

  /* Shade correction */
  if( errNum == WLZ_ERR_NONE ){
    if((toggle = XtNameToWidget(globals.topl,
				"*warp_sgnl_controls_form*shade_correction"))){
      XtVaGetValues(toggle, XmNset, &setFlg, NULL);
      if( setFlg ){
	errNum = WlzHistogramEqualiseObj(obj1, 1, 1);
      }
    }
  }

  /* Gauss smoothing */
  if( errNum == WLZ_ERR_NONE ){
    if((toggle = XtNameToWidget(globals.topl,
				"*warp_sgnl_controls_form*gauss_smooth"))){
      XtVaGetValues(toggle, XmNset, &setFlg, NULL);
      if( setFlg ){
	double	width;
	if((slider = XtNameToWidget(globals.topl,
				    "*warp_sgnl_controls_form*gauss_width"))){
	  width = HGU_XmGetSliderValue(slider);
	}
	else {
	  width = 3;
	}
	if((obj2 = WlzGauss2(obj1, width, width, 0, 0, &errNum))){
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

void warpSetSignalThreshObj(void)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*obj;
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
	  if( warpGlobals.threshColorChannel == WLZ_RGBA_CHANNEL_GREY ){
	    obj = WlzRGBAToModulus(warpGlobals.sgnlProcObj, &errNum);
	    warpGlobals.sgnlThreshObj = WlzAssignObject(obj, &errNum);
	  }
	  else {
	    if((cobj = WlzRGBAToCompound(warpGlobals.sgnlProcObj,
					 warpGlobals.threshRGBSpace,
					 &errNum))){
	      switch( warpGlobals.threshColorChannel ){
	      default:
	      case WLZ_RGBA_CHANNEL_RED:
	      case WLZ_RGBA_CHANNEL_HUE:
	      case WLZ_RGBA_CHANNEL_CYAN:
		warpGlobals.sgnlThreshObj = WlzAssignObject(cobj->o[0], &errNum);
		break;
	      case WLZ_RGBA_CHANNEL_GREEN:
	      case WLZ_RGBA_CHANNEL_SATURATION:
	      case WLZ_RGBA_CHANNEL_MAGENTA:
		warpGlobals.sgnlThreshObj = WlzAssignObject(cobj->o[1], &errNum);
		break;
	      case WLZ_RGBA_CHANNEL_BLUE:
	      case WLZ_RGBA_CHANNEL_BRIGHTNESS:
	      case WLZ_RGBA_CHANNEL_YELLOW:
		warpGlobals.sgnlThreshObj = WlzAssignObject(cobj->o[2], &errNum);
		break;
	      }
	      WlzFreeObj((WlzObject *) cobj);
	    }
	  }
	  break;

	case WLZ_RGBA_THRESH_MULTI:
	  if((cobj = WlzRGBAToCompound(warpGlobals.sgnlProcObj,
				       WLZ_RGBA_SPACE_RGB, &errNum))){
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

	case WLZ_RGBA_THRESH_NONE:
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
