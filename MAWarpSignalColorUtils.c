#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MAWarpSigna_colorUtils_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MAWarpSignalColorUtils.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:41:30 2009
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

extern void thresholdChannelSelectCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

extern void warpSwitchIncrementDomain(
  int	incrFlg);

void warpSetThreshColorTypeSensitive(
  Boolean	sensitive)
{
  Widget	toggle;

  if((toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_notebook*threshold_single_page*red"))){
    XtSetSensitive(toggle, sensitive);
  }
  if((toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_notebook*threshold_single_page*green"))){
    XtSetSensitive(toggle, sensitive);
  }
  if((toggle = XtNameToWidget(globals.topl,
			      "*warp_sgnl_notebook*threshold_single_page*blue"))){
    XtSetSensitive(toggle, sensitive);
  }

  return;
}

void warpSetThreshColorTypeToggles(
  WlzRGBAColorSpace	colSpc)
{
  Widget	toggle;
  XmString	xmstrR, xmstrG, xmstrB;
  WlzRGBAColorChannel	chanR, chanG, chanB;

  /* set up the strings */
  switch( colSpc ){
  default:
  case WLZ_RGBA_SPACE_RGB:
    xmstrR = XmStringCreateSimple("red");
    xmstrG = XmStringCreateSimple("green");
    xmstrB = XmStringCreateSimple("blue");
    chanR = WLZ_RGBA_CHANNEL_RED;
    chanG = WLZ_RGBA_CHANNEL_GREEN;
    chanB = WLZ_RGBA_CHANNEL_BLUE;
    break;

  case WLZ_RGBA_SPACE_HSB:
    xmstrR = XmStringCreateSimple("hue");
    xmstrG = XmStringCreateSimple("sat");
    xmstrB = XmStringCreateSimple("bright");
    chanR = WLZ_RGBA_CHANNEL_HUE;
    chanG = WLZ_RGBA_CHANNEL_SATURATION;
    chanB = WLZ_RGBA_CHANNEL_BRIGHTNESS;
    break;

  case WLZ_RGBA_SPACE_CMY:
    xmstrR = XmStringCreateSimple("cyan");
    xmstrG = XmStringCreateSimple("mag");
    xmstrB = XmStringCreateSimple("yell");
    chanR = WLZ_RGBA_CHANNEL_CYAN;
    chanG = WLZ_RGBA_CHANNEL_MAGENTA;
    chanB = WLZ_RGBA_CHANNEL_YELLOW;
    break;
  }

  if((toggle = 
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_notebook*threshold_single_page*red"))){
    XtVaSetValues(toggle, XmNlabelString, xmstrR, NULL);
    XtRemoveAllCallbacks(toggle, XmNvalueChangedCallback);
    XtAddCallback(toggle, XmNvalueChangedCallback,
		  thresholdChannelSelectCb, (XtPointer) chanR);
  }
  if((toggle = 
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_notebook*threshold_single_page*green"))){
    XtVaSetValues(toggle, XmNlabelString, xmstrG, NULL);
    XtRemoveAllCallbacks(toggle, XmNvalueChangedCallback);
    XtAddCallback(toggle, XmNvalueChangedCallback,
		  thresholdChannelSelectCb, (XtPointer) chanG);
  }
  if((toggle = 
     XtNameToWidget(globals.topl,
		    "*warp_sgnl_notebook*threshold_single_page*blue"))){
    XtVaSetValues(toggle, XmNlabelString, xmstrB, NULL);
    XtRemoveAllCallbacks(toggle, XmNvalueChangedCallback);
    XtAddCallback(toggle, XmNvalueChangedCallback,
		  thresholdChannelSelectCb, (XtPointer) chanB);
  }

  XmStringFree(xmstrR);
  XmStringFree(xmstrG);
  XmStringFree(xmstrB);

  return;
}

void warpColorSpaceCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzRGBAColorSpace	colSpc=(WlzRGBAColorSpace) client_data;

  /* set the space global and reset toggles and sliders */
  warpGlobals.threshRGBSpace = colSpc;
  warpSetThreshColorTypeToggles(colSpc);

  /* reset the selected channel */
  switch( colSpc ){
  default:
  case WLZ_RGBA_SPACE_RGB:
    switch( warpGlobals.threshColorChannel ){
    default:
    case WLZ_RGBA_CHANNEL_RED:
    case WLZ_RGBA_CHANNEL_HUE:
    case WLZ_RGBA_CHANNEL_CYAN:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_RED;
      break;
    case WLZ_RGBA_CHANNEL_GREEN:
    case WLZ_RGBA_CHANNEL_SATURATION:
    case WLZ_RGBA_CHANNEL_MAGENTA:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_GREEN;
      break;
    case WLZ_RGBA_CHANNEL_BLUE:
    case WLZ_RGBA_CHANNEL_BRIGHTNESS:
    case WLZ_RGBA_CHANNEL_YELLOW:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_BLUE;
      break;
    }
    break;
  case WLZ_RGBA_SPACE_HSB:
    switch( warpGlobals.threshColorChannel ){
    default:
    case WLZ_RGBA_CHANNEL_RED:
    case WLZ_RGBA_CHANNEL_HUE:
    case WLZ_RGBA_CHANNEL_CYAN:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_HUE;
      break;
    case WLZ_RGBA_CHANNEL_GREEN:
    case WLZ_RGBA_CHANNEL_SATURATION:
    case WLZ_RGBA_CHANNEL_MAGENTA:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_SATURATION;
      break;
    case WLZ_RGBA_CHANNEL_BLUE:
    case WLZ_RGBA_CHANNEL_BRIGHTNESS:
    case WLZ_RGBA_CHANNEL_YELLOW:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_BRIGHTNESS;
      break;
    }
    break;
  case WLZ_RGBA_SPACE_CMY:
    switch( warpGlobals.threshColorChannel ){
    default:
    case WLZ_RGBA_CHANNEL_RED:
    case WLZ_RGBA_CHANNEL_HUE:
    case WLZ_RGBA_CHANNEL_CYAN:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_CYAN;
      break;
    case WLZ_RGBA_CHANNEL_GREEN:
    case WLZ_RGBA_CHANNEL_SATURATION:
    case WLZ_RGBA_CHANNEL_MAGENTA:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_MAGENTA;
      break;
    case WLZ_RGBA_CHANNEL_BLUE:
    case WLZ_RGBA_CHANNEL_BRIGHTNESS:
    case WLZ_RGBA_CHANNEL_YELLOW:
      warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_YELLOW;
      break;
    }
    break;
  }

  /* reset the threshold object */
  if( warpGlobals.sgnlThreshObj ){
    WlzFreeObj(warpGlobals.sgnlThreshObj);
    warpGlobals.sgnlThreshObj = NULL;
  }
  if( warpGlobals.sgnlObj ){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
  }
  /*warpSwitchIncrementDomain(0);*/ /* why switch increment? */
  warpSetSignalDomain(NULL);
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  return;
}

