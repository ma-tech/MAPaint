#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Mouse Atlas Project					*
*   File       :   domain_utils.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Feb 20 07:36:41 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

WlzObject *get_domain_from_object(
  WlzObject		*obj,
  DomainSelection	domain)
{
  WlzObject	*obj1, *obj2;
  WlzPixelV	low_thresh, high_thresh;

  /* check the object */
  if((obj == NULL) ||
     (obj->type != WLZ_2D_DOMAINOBJ && obj->type != WLZ_3D_DOMAINOBJ) ||
     (obj->values.core == NULL) ){
    return( NULL );
  }

  /* check the domain */
  switch( domain ){
  case GREYS_DOMAIN:
  case DOMAIN_1:
  case DOMAIN_2:
  case DOMAIN_3:
  case DOMAIN_4:
  case DOMAIN_5:
  case DOMAIN_6:
  case DOMAIN_7:
  case DOMAIN_8:
  case DOMAIN_9:
  case DOMAIN_10:
  case DOMAIN_11:
  case DOMAIN_12:
  case DOMAIN_13:
  case DOMAIN_14:
  case DOMAIN_15:
  case DOMAIN_16:
  case DOMAIN_17:
  case DOMAIN_18:
  case DOMAIN_19:
  case DOMAIN_20:
  case DOMAIN_21:
  case DOMAIN_22:
  case DOMAIN_23:
  case DOMAIN_24:
  case DOMAIN_25:
  case DOMAIN_26:
  case DOMAIN_27:
  case DOMAIN_28:
  case DOMAIN_29:
  case DOMAIN_30:
  case DOMAIN_31:
  case DOMAIN_32:
    break;
  default:
    return( NULL );
  }

  /* set up the threshold values */
  low_thresh.type = WLZ_GREY_INT;
  low_thresh.v.inv = globals.cmapstruct->ovly_cols[domain];
  high_thresh.type = WLZ_GREY_INT;
  high_thresh.v.inv = low_thresh.v.inv + globals.cmapstruct->gmax -
    globals.cmapstruct->gmin + 1;
  if( domain > globals.cmapstruct->num_overlays )
  {
    high_thresh.v.inv = low_thresh.v.inv + 1;
  }

  /* threshold below */
  if( (obj1 = WlzThreshold( obj, low_thresh, WLZ_THRESH_HIGH, NULL )) == NULL )
    return( NULL );

  /* threshold above */
  obj1 = WlzAssignObject(obj1, NULL);
  obj2 = WlzThreshold( obj1, high_thresh, WLZ_THRESH_LOW, NULL );
  WlzFreeObj( obj1 );
  return( obj2 );
}

int imageValueToDomain( unsigned int val )
{
  DomainSelection	sel_domain;
  unsigned int		maxval;
  int			i;

  /* check if part of the displayed image */
  if( val < globals.cmapstruct->gmin )
  {
    return( -1 );
  }

  /* check if wash overlay or greys */
  maxval = globals.cmapstruct->gmax +
    globals.cmapstruct->ovly_incr[globals.cmapstruct->num_overlays];
  if( val > maxval )
  {
    sel_domain = val - maxval + globals.cmapstruct->num_overlays;
  }
  else
  {
    val &= globals.cmapstruct->ovly_planes;
    for(i=0; i <= globals.cmapstruct->num_overlays; i++)
    {
      if( val == globals.cmapstruct->ovly_cols[i] )
      {
	break;
      }
    }
    sel_domain = i;
  }

  if( sel_domain > (globals.cmapstruct->num_overlays +
		    globals.cmapstruct->num_solid_overlays) )
  {
    return( -1 );
  }

  return( sel_domain );
}

void setGreyValuesFromObject(
  WlzObject	*destObj,
  WlzObject	*srcObj)
{
  WlzObject		*obj1, *obj2;
  WlzIntervalWSpace	iwsp1, iwsp2;
  WlzGreyWSpace		gwsp1, gwsp2;
  int			x, y, z, i, l, k, p;
  WlzPixelV		min, max, Min, Max;

  /* check the objects */
  if( destObj == NULL || srcObj == NULL ){
    return;
  }
  if( destObj->type != srcObj->type ){
    return;
  }
  switch( destObj->type ){
  case WLZ_2D_DOMAINOBJ:
    if( destObj->values.core == NULL || srcObj->values.core == NULL ){
      return;
    }
    break;

  default:
    return;
  }

  /* set the required grey ranges */
  min.type = WLZ_GREY_UBYTE;
  max.type = WLZ_GREY_UBYTE;
  Min.type = WLZ_GREY_UBYTE;
  Max.type = WLZ_GREY_UBYTE;
  min.v.ubv = 0;
  max.v.ubv = 255;
  Min.v.ubv = globals.cmapstruct->gmin;
  Max.v.ubv = globals.cmapstruct->gmax;

  /* scan through the intersection */
  obj1 = WlzIntersect2(destObj, srcObj, NULL);
  if( obj1 ){
    obj1 = WlzAssignObject(obj1, NULL);
    obj1->values = WlzAssignValues(destObj->values, NULL);
    obj2 = WlzMakeMain(obj1->type, obj1->domain, srcObj->values,
		       NULL, NULL, NULL);
  }
  else {
    return;
  }
  WlzInitGreyScan( obj1, &iwsp1, &gwsp1 );
  WlzInitGreyScan( obj2, &iwsp2, &gwsp2 );
  while( WlzNextGreyInterval( &iwsp1 ) == WLZ_ERR_NONE &&
	WlzNextGreyInterval( &iwsp2 ) == WLZ_ERR_NONE )
  {
    switch( gwsp2.pixeltype ){
    case WLZ_GREY_LONG:
      for(i=0; i < iwsp1.colrmn; i++){
	gwsp1.u_grintptr.ubp[i] = (UBYTE) (gwsp2.u_grintptr.lnp[i]);
      }
      break;

    case WLZ_GREY_INT:
      for(i=0; i < iwsp1.colrmn; i++){
	gwsp1.u_grintptr.ubp[i] = (UBYTE) (gwsp2.u_grintptr.inp[i]);
      }
      break;

    case WLZ_GREY_SHORT:
      for(i=0; i < iwsp1.colrmn; i++){
	gwsp1.u_grintptr.ubp[i] = (UBYTE) (gwsp2.u_grintptr.shp[i]);
      }
      break;

    case WLZ_GREY_UBYTE:
      memcpy((void *) gwsp1.u_grintptr.ubp,
	     (const void *) gwsp2.u_grintptr.ubp,
	     (size_t) (iwsp1.rgtpos - iwsp1.lftpos + 1));
      break;

    case WLZ_GREY_FLOAT:
      for(i=0; i < iwsp1.colrmn; i++){
	gwsp1.u_grintptr.ubp[i] = (UBYTE) (gwsp2.u_grintptr.flp[i]);
      }
      break;

    case WLZ_GREY_DOUBLE:
      for(i=0; i < iwsp1.colrmn; i++){
	gwsp1.u_grintptr.ubp[i] = (UBYTE) (gwsp2.u_grintptr.dbp[i]);
      }
      break;
    }
  }
  WlzGreySetRange(obj1, min, max, Min, Max);
  WlzFreeObj( obj1 );
  WlzFreeObj( obj2 );

  return;
}

void setGreysIncrement(
  WlzObject		*obj,
  ThreeDViewStruct	*view_struct)
{
  WlzObject	*obj1;
  int		i, numOverlays;

  /* check the object */
  if( obj == NULL || WlzArea(obj, NULL) <= 0 )
    return;

  /* check the view object */
  if( view_struct->view_object == NULL ){
    view_struct->view_object = WlzGetSectionFromObject(globals.orig_obj,
						       view_struct->wlzViewStr,
						       NULL);
  }
    
  /* delete from the painted object */
  obj1 = WlzMakeMain(obj->type, obj->domain, view_struct->view_object->values,
		     NULL, NULL, NULL);
  setGreyValuesFromObject(view_struct->painted_object, obj1);
  WlzFreeObj(obj1);
  redisplay_view_cb(view_struct->canvas, (XtPointer) view_struct,
		    NULL);

  /* subtract it from existing domains */
  numOverlays = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;
  for(i=1; i < numOverlays; i++){
    obj1 = WlzDiffDomain(view_struct->curr_domain[i], obj, NULL);
    WlzFreeObj(view_struct->curr_domain[i]);
    if( obj1 ){
      view_struct->curr_domain[i] = WlzAssignObject(obj1, NULL);
    }
    else {
      view_struct->curr_domain[i] = NULL;
    }
  }

  return;
}

void setDomainIncrement(
  WlzObject		*obj,
  ThreeDViewStruct	*view_struct,
  DomainSelection	domain,
  int			delFlag)
{
  WlzObject	*obj1, *obj2;
  int		i, j, numOverlays;
  int		dominanceFlag;
  WlzErrorNum	wlzErr;

  /* check the object */
  if( obj == NULL || WlzArea(obj, NULL) <= 0 )
    return;

  /* check the view object */
  if( view_struct->view_object == NULL ){
    view_struct->view_object =
      WlzGetSectionFromObject(globals.orig_obj,
			      view_struct->wlzViewStr,
			      NULL);
  }
    
  /* if delete then remove intersection with the existing region */
  if( delFlag ){
    obj1 = WlzIntersect2(obj, view_struct->curr_domain[domain], NULL);
    if( obj1 ){
      obj1->values = WlzAssignValues(view_struct->view_object->values,
				     &wlzErr);
      setGreyValuesFromObject(view_struct->painted_object, obj1);
      WlzFreeObj(obj1);
      redisplay_view_cb(view_struct->canvas, (XtPointer) view_struct,
			NULL);

      /* subtract it from the existing domain */
      obj1 = WlzDiffDomain(view_struct->curr_domain[domain], obj, NULL);
      WlzFreeObj(view_struct->curr_domain[domain]);
      if( obj1 ){
	view_struct->curr_domain[domain] = WlzAssignObject(obj1, NULL);
      }
      else {
	view_struct->curr_domain[domain] = NULL;
      }

      /* mark it as changed */
      globals.domain_changed_since_saved[domain] = 1;
    }

    return;
  }

  /* if increment then check dominance */
  numOverlays = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;
  obj1 = WlzMakeMain(obj->type, obj->domain, obj->values, NULL, NULL, NULL);
  obj1 = WlzAssignObject(obj1, NULL);
  dominanceFlag = 1;
  for(j=1; j <= numOverlays; j++){
    i = globals.priority_to_domain_lut[j];

    /* if current domain */
    if( i == (int) domain ){
      /* clear to greys here to get rid of solid colours */
      obj1->values =
	WlzAssignValues(view_struct->view_object->values, &wlzErr);
      setGreyValuesFromObject(view_struct->painted_object, obj1);

      if( view_struct->curr_domain[i] ){
	obj2 = WlzAssignObject(WlzUnion2(view_struct->curr_domain[i],
					 obj1, NULL), NULL);
	WlzFreeObj(view_struct->curr_domain[i]);
	view_struct->curr_domain[i] = obj2;
      }
      else {
	view_struct->curr_domain[i] = WlzAssignObject(obj1, NULL);
      }
      dominanceFlag = 0;
      globals.domain_changed_since_saved[i] = 1;
      continue;
    }

    /* if dominant to current domain */
    if( view_struct->curr_domain[i] && dominanceFlag ){
      obj2 = WlzDiffDomain(obj1, view_struct->curr_domain[i], NULL);
      WlzFreeObj(obj1);
      if( obj2 && (WlzArea(obj2, NULL) > 0) ){
	obj1 = WlzAssignObject(obj2, NULL);
      }
      else {
	return;
      }
    }

    /* if not dominant to current domain */
    if( view_struct->curr_domain[i] && !dominanceFlag ){
      int orig_area, new_area;
      obj2 = WlzDiffDomain(view_struct->curr_domain[i], obj1, NULL);
      orig_area = WlzArea(view_struct->curr_domain[i], NULL);
      WlzFreeObj(view_struct->curr_domain[i]);
      new_area = WlzArea(obj2, NULL);
      if( new_area < orig_area ){
	globals.domain_changed_since_saved[i] = 1;
      }
      if( obj2 && (new_area > 0) ){
	view_struct->curr_domain[i] = WlzAssignObject(obj2, NULL);
      }
      else {
	view_struct->curr_domain[i] = NULL;
      }
    }
  }

  /* check remaining domains */
  if( obj1 ){
    WlzFreeObj(obj1);
  }

  /* set the new domain and display */
  i = (int) domain;
  if( i > globals.cmapstruct->num_overlays )
  {
    set_grey_values_from_domain(view_struct->curr_domain[i],
				view_struct->painted_object,
				globals.cmapstruct->ovly_cols[i],
				255);
  }
  else
  {
    set_grey_values_from_domain(view_struct->curr_domain[i],
				view_struct->painted_object,
				globals.cmapstruct->ovly_cols[i],
				globals.cmapstruct->ovly_planes);
  }
  
  /* redisplay this view */
  redisplay_view_cb(view_struct->canvas, (XtPointer) view_struct,
		    NULL);

  return;
}
