#pragma ident "MRC HGU $Id$"
/*****************************************************************************
* Copyright   :    1994 Medical Research Council, UK.                        *
*                  All rights reserved.                                      *
******************************************************************************
* Address     :    MRC Human Genetics Unit,                                  *
*                  Western General Hospital,                                 *
*                  Edinburgh, EH4 2XU, UK.                                   *
******************************************************************************
* Project     :    Mouse Atlas Project					     *
* File        :    tools_fill_domain.c					     *
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Fri May 12 19:20:36 1995				     *
* Synopsis    :    Interactive tool to fill the connected pixels in a given  *
*		colour up to the edge of the selected colour.		     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

DomainSelection getSelectedDomainType(
  int			x, 
  int			y, 
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  DomainSelection	sel_domain;

  /* check scaling */
  x /= wlzViewStr->scale;
  y /= wlzViewStr->scale;

  /* determine selected colour and get domain should get this in Wlz not
     rely on the ximage */
  if(x < 0 || x >= view_struct->ximage->width ||
     y < 0 || y >= view_struct->ximage->height ){
    return( NULL );
  }

  sel_domain = imageValueToDomain
    ((unsigned int ) *(((UBYTE *)view_struct->ximage->data) +
		       x + y * view_struct->ximage->bytes_per_line));

  return sel_domain;
}

WlzObject *getSelectedRegion(
  int			x, 
  int			y, 
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  WlzObject		*obj, *obj1, **objs;
  unsigned int		sel_col;
  DomainSelection	sel_domain;
  int			i, nobjs;

  /* get the selected domain */
  if( (sel_domain = getSelectedDomainType(x, y, view_struct)) < 0 ){
    return NULL;
  }

  /* check scaling and offsets */
  x /= wlzViewStr->scale;
  y /= wlzViewStr->scale;
  x += view_struct->painted_object->domain.i->kol1;
  y += view_struct->painted_object->domain.i->line1;

  if( (obj = get_domain_from_object(view_struct->painted_object,
				    sel_domain)) == NULL )
  {
    return( NULL );
  }
  obj = WlzAssignObject(obj, NULL);

  /* segment domain and find connected region */
  if( WlzLabel(obj, &nobjs, &objs, 256, 1, WLZ_4_CONNECTED) == WLZ_ERR_NONE ){
    for(i=0, obj1=NULL; i < nobjs; i++){
      if( WlzInsideDomain(objs[i], 0, y, x, NULL) ){
	obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			   objs[i]->domain, objs[i]->values,
			   NULL, NULL, NULL);
      }
      WlzFreeObj( objs[i] );
    }
    AlcFree((void *) objs);
  }
  else {
    obj1 = NULL;
  }
  WlzFreeObj( obj );

  /* trim off the values */
  if( obj1 && obj1->values.core ){
    WlzFreeValues(obj1->values);
    obj1->values.core = NULL;
  }

  return obj1;
}

void MAPaintFill2DInit(
  ThreeDViewStruct	*view_struct)
{
  return;
}

void MAPaintFill2DQuit(
  ThreeDViewStruct	*view_struct)
{
  return;
}

void MAPaintFill2DCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;
  DomainSelection	currDomain;
  int			delFlag;
  WlzObject		*obj, *obj1;

  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
    case Button2:
      /* do nothing if shift pressed - magnify option */
      if( cbs->event->xbutton.state & ButtonStateIgnoreMask ){
	break;
      }

      /* save the current domain selection and meta button state */
      currDomain = globals.current_domain;
      delFlag = ((cbs->event->xbutton.state & Mod1Mask) ||
		 (cbs->event->xbutton.button == Button2));

      /* get the fill object */
      obj = getSelectedRegion(cbs->event->xbutton.x, cbs->event->xbutton.y,
			      view_struct);
      obj = WlzAssignObject(obj, NULL);

      /* push domains for undo reset the painted object and redisplay */
      if( obj ){
	pushUndoDomains(view_struct);
	setDomainIncrement(obj, view_struct, currDomain, delFlag);
	WlzFreeObj( obj );
      }
      break;

    default:
      break;

    }
    break;

  case ButtonRelease:
    break;

  case MotionNotify:
    break;

  case KeyPress:
    break;

  default:
    break;
  }

  return;
}
