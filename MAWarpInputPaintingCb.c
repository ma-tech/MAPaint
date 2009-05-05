#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MAWarpInputPaintin_cb_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MAWarpInputPaintingCb.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:46:08 2009
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

extern void view_canvas_highlight(ThreeDViewStruct	*view_struct,
				  Boolean		highlight);

extern void display_pointer_feedback_information(
  ThreeDViewStruct	*view_struct,
  int			x, 
  int			y);

static int		paintLiftFlag=0;
static DomainSelection	paintLiftDomain;
static WlzObject	*paintLiftObj=NULL;

void canvas_warp_painting_cb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;
  WlzCompoundArray	*cobj;

  /* check for lock */
  if( view_struct->viewLockedFlag ){
    return;
  }

  /* switch on event type */
  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
      /* if shift is pressed then increase the magnification
	 the scrolled window must be set to keep the pointer
	 position fixed */
      switch(cbs->event->xbutton.state & (ShiftMask|ControlMask|Mod1Mask)){

      case ShiftMask: /* magnify */
	setViewScale(view_struct, wlzViewStr->scale * 2.0,
		     cbs->event->xbutton.x, cbs->event->xbutton.y);
	break;

      case ShiftMask|Mod1Mask: /* reduce */
	setViewScale(view_struct, wlzViewStr->scale / 2.0,
		     cbs->event->xbutton.x, cbs->event->xbutton.y);
	break;

      case ControlMask: /* lift paint */
	if( paintLiftFlag ){
	  break;
	}
	paintLiftFlag = 1;
	if( (paintLiftDomain =
	     getSelectedDomainType(cbs->event->xbutton.x,
				   cbs->event->xbutton.y,
				   view_struct)) < 1 ){
	  break;
	}
	paintLiftObj = getSelectedRegion(cbs->event->xbutton.x,
					 cbs->event->xbutton.y,
					 view_struct);
	paintLiftObj = WlzAssignObject(paintLiftObj, NULL);

	/* clear the selected paint object object and redisplay */
	if( paintLiftObj ){
	  setDomainIncrement(paintLiftObj, view_struct, paintLiftDomain, 1);
	}
	break;

      default:
	break;
      }
	
      break;

    case Button2:		/* domain and coordinate feedback */
      /* if shift is pressed then decrease the magnification
	 the scrolled window must be set to keep the pointer
	 position fixed */
      if( cbs->event->xbutton.state & ShiftMask ){
	setViewScale(view_struct, wlzViewStr->scale / 2.0,
		     cbs->event->xbutton.x, cbs->event->xbutton.y);
      }
      else {
	x = cbs->event->xbutton.x / wlzViewStr->scale;
	y = cbs->event->xbutton.y / wlzViewStr->scale;
	display_pointer_feedback_information(view_struct, x, y);
      }
      break;

    case Button3:
      break;

    default:
      break;

    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){

    case Button1:
      if( paintLiftFlag ){
	paintLiftFlag = 0;
	if( paintLiftObj ){
	  setDomainIncrement(paintLiftObj, view_struct,
			     paintLiftDomain, 0);
	  WlzFreeObj(paintLiftObj);
	  paintLiftObj = NULL;
	}
      }

    case Button2:		/* domain and coordinate feedback */
      XtVaSetValues(view_struct->text, XmNvalue, "", NULL);
      break;

    case Button3:
      break;

    default:
      break;

    }
    break;

  case MotionNotify:

    if( cbs->event->xmotion.state & Button2Mask )
    {
      x = cbs->event->xmotion.x / wlzViewStr->scale;
      y = cbs->event->xmotion.y / wlzViewStr->scale;
      display_pointer_feedback_information(view_struct, x, y);
    }
    break;

  case KeyPress:
    if( !cbs->event->xkey.state&(Button1Mask|Button2Mask|Button3Mask) ){
      fprintf(stderr, "\007 Too fast with the fingers %s\n", getenv("USER"));
      break;
    }
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_KP_Right:
    case XK_f:
      /* in this mode - next section installing and resetting domains
	 if <alt> or <alt gr> pressed then change tool option
	 (tool dependent) */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	break;
      }
      else {
	if( wlzViewStr->dist <= (wlzViewStr->maxvals.vtZ - 1.0) ){
	  installViewDomains(view_struct);
	  clearUndoDomains();
	  wlzViewStr->dist += 1.0;
	  reset_view_struct( view_struct );
	  display_view_cb(w, (XtPointer) view_struct, call_data);
	  view_feedback_cb(w, (XtPointer) view_struct, NULL);
	  getViewDomains(view_struct);

	/* need to reset warp destination image and overlay */
	  if( warpGlobals.dst.obj ){
	    WlzFreeObj(warpGlobals.dst.obj);
	  }
	  warpGlobals.dst.obj =
	    WlzAssignObject(WlzGetSectionFromObject(globals.orig_obj,
						    view_struct->wlzViewStr,
						    WLZ_INTERPOLATION_NEAREST,
						    NULL),
			    NULL);
	  warpSetXImage(&(warpGlobals.dst));
	  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
			  call_data);

	  /* reset the overlay object */
	  cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
	  if( cobj->o[0] ){
	    WlzFreeObj(cobj->o[0]);
	  }
	  cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);
	  warpSetXImage(&(warpGlobals.ovly));
	  XtCallCallbacks(warpGlobals.ovly.canvas, XmNexposeCallback,
			  call_data);

	/* check for warp transform */
	  if( warpGlobals.num_vtxs ){
	    warpDisplayTiePoints();
	    warpSetOvlyObject();
	    warpSetOvlyXImage(&(warpGlobals.ovly));
	    warpCanvasExposeCb(warpGlobals.ovly.canvas,
			       (XtPointer) &(warpGlobals.ovly),
			       call_data);
	    XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
			    call_data);
	    XtCallCallbacks(warpGlobals.src.canvas, XmNexposeCallback,
			    call_data);
	  }
	}
      }
      break;

    case XK_Up:
    case XK_p:
      /* next domain or next paint tool if <alt> (which seems
	 to be Mod1Mask) pressed */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	selectNext2DTool(0);
      }
      else {
	selectNextDomain(0);
      }
      break;

    case XK_Left:
    case XK_KP_Left:
    case XK_b:
      /* in this mode - next section installing and resetting domains 
	 if <alt> pressed then change tool option (tool dependent) */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	break;
      }
      else {
	if( wlzViewStr->dist >= (wlzViewStr->minvals.vtZ + 1.0) ){
	  installViewDomains(view_struct);
	  clearUndoDomains();
	  wlzViewStr->dist -= 1.0;
	  reset_view_struct( view_struct );
	  display_view_cb(w, (XtPointer) view_struct, call_data);
	  view_feedback_cb(w, (XtPointer) view_struct, NULL);
	  getViewDomains(view_struct);

	  /* need to reset warp destination image and overlay */
	  if( warpGlobals.dst.obj ){
	    WlzFreeObj(warpGlobals.dst.obj);
	  }
	  warpGlobals.dst.obj =
	    WlzAssignObject(WlzGetSectionFromObject(globals.orig_obj,
						    view_struct->wlzViewStr,
						    WLZ_INTERPOLATION_NEAREST,
						    NULL),
			    NULL);
	  warpSetXImage(&(warpGlobals.dst));
	  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
			  call_data);

	  /* reset the overlay object */
	  cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
	  if( cobj->o[0] ){
	    WlzFreeObj(cobj->o[0]);
	  }
	  cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);
	  warpSetXImage(&(warpGlobals.ovly));
	  XtCallCallbacks(warpGlobals.ovly.canvas, XmNexposeCallback,
			  call_data);

	  /* check for warp transform */
	  if( warpGlobals.num_vtxs ){
	    warpDisplayTiePoints();
	    warpSetOvlyObject();
	    warpSetOvlyXImage(&(warpGlobals.ovly));
	    warpCanvasExposeCb(warpGlobals.ovly.canvas,
			       (XtPointer) &(warpGlobals.ovly),
			       call_data);
	    XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
			    call_data);
	    XtCallCallbacks(warpGlobals.src.canvas, XmNexposeCallback,
			    call_data);
	  }
	}
      }
      break;

    case XK_Down:
    case XK_n:
      /* previous paint tool or previous domain if <alt> pressed */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	selectNext2DTool(1);
      }
      else {
	selectNextDomain(1);
      }
      break;

    case XK_w:
      /* get a filename for the view image */
      break;

    case XK_u:	/* undo */
    case XK_U:
      if( !(cbs->event->xkey.state & ControlMask) ){
	break;
      }
      UndoDomains(view_struct);
      break;

    case XK_r:	/* redo */
    case XK_R:
      if( !(cbs->event->xkey.state & ControlMask) ){
	break;
      }
      RedoDomains(view_struct);
      break;

    case XK_Undo:
    case XK_F14:
      UndoDomains(view_struct);
      break;

    case XK_Redo:
    case XK_F12:
      RedoDomains(view_struct);
      break;

    }
    break;

  case EnterNotify: /* need to get keyboard focus */
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
    break;

  case LeaveNotify: /* get rid of it to avoid accidents */
    XmProcessTraversal(w, XmTRAVERSE_PREV_TAB_GROUP);
    break;

  default:
    break;
  }

  return;
}
 
