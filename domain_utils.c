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
  WlzObject	*obj1, *obj2=NULL;
  WlzPixelV	low_thresh, high_thresh;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

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
  if( obj1 = WlzThreshold(obj, low_thresh, WLZ_THRESH_HIGH, &errNum) ){

    /* threshold above */
    obj1 = WlzAssignObject(obj1, NULL);
    obj2 = WlzThreshold( obj1, high_thresh, WLZ_THRESH_LOW, &errNum );
    WlzFreeObj( obj1 );
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "get_domain_from_object", errNum);
  }
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
  WlzErrorNum		errNum=WLZ_ERR_NONE;

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
  obj1 = WlzIntersect2(destObj, srcObj, &errNum);
  if( obj1 ){
    obj1 = WlzAssignObject(obj1, NULL);
    obj1->values = WlzAssignValues(destObj->values, NULL);
    if( obj2 = WlzMakeMain(obj1->type, obj1->domain, srcObj->values,
			   NULL, NULL, &errNum) ){

      errNum = WlzInitGreyScan( obj1, &iwsp1, &gwsp1 );
      WlzInitGreyScan( obj2, &iwsp2, &gwsp2 );
      while((errNum == WLZ_ERR_NONE) &&
	    ((errNum = WlzNextGreyInterval(&iwsp1)) == WLZ_ERR_NONE) &&
	    (WlzNextGreyInterval(&iwsp2) == WLZ_ERR_NONE) )
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
      if( errNum == WLZ_ERR_EOO ){
	errNum = WLZ_ERR_NONE;
      }
      WlzFreeObj( obj2 );
    }
    WlzGreySetRange(obj1, min, max, Min, Max);
    WlzFreeObj( obj1 );
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "setGreyValuesFromObject", errNum);
  }
  return;
}

void setGreysIncrement(
  WlzObject		*obj,
  ThreeDViewStruct	*view_struct)
{
  WlzObject	*obj1;
  int		i, numOverlays;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check the object */
  if( obj == NULL ){
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if( WlzArea(obj, &errNum) > 0 ){

    /* check the view object */
    if( view_struct->view_object == NULL ){
      if( obj1 = WlzGetSectionFromObject(globals.orig_obj,
					 view_struct->wlzViewStr,
					 WLZ_INTERPOLATION_NEAREST, &errNum) ){
	view_struct->view_object = WlzAssignObject(obj1, NULL);
      }
    }
    
    /* delete from the painted object */
    if( errNum == WLZ_ERR_NONE ){
      if( obj1 = WlzMakeMain(obj->type, obj->domain, view_struct->view_object->values,
			     NULL, NULL, &errNum) ){
	setGreyValuesFromObject(view_struct->painted_object, obj1);
	WlzFreeObj(obj1);
	redisplay_view_cb(view_struct->canvas, (XtPointer) view_struct,
			  NULL);

	/* subtract it from existing domains */
	numOverlays = globals.cmapstruct->num_overlays +
	  globals.cmapstruct->num_solid_overlays;
	for(i=1; (errNum == WLZ_ERR_NONE) && (i < numOverlays); i++){
	  obj1 = WlzDiffDomain(view_struct->curr_domain[i], obj, &errNum);
	  WlzFreeObj(view_struct->curr_domain[i]);
	  if( obj1 ){
	    view_struct->curr_domain[i] = WlzAssignObject(obj1, NULL);
	  }
	  else {
	    view_struct->curr_domain[i] = NULL;
	  }
	}
      }
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "setGreysIncrement", errNum);
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
  WlzValues	values;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  FILE		*fp;

  /* check the object */
  if( obj == NULL || WlzIsEmpty(obj, NULL) )
    return;

  /* check the view object */
  if( view_struct->view_object == NULL ){
    if( obj1 = WlzGetSectionFromObject(globals.orig_obj,
				       view_struct->wlzViewStr,
				       WLZ_INTERPOLATION_NEAREST, &errNum) ){
      view_struct->view_object = WlzAssignObject(obj1, NULL);
    }
  }
    
  /* if delete then remove intersection with the existing region */
  if( (errNum == WLZ_ERR_NONE) && delFlag ){
    obj1 = WlzIntersect2(obj, view_struct->curr_domain[domain], &errNum);
    if( obj1 ){
      obj1->values = WlzAssignValues(view_struct->view_object->values,
				     &errNum);
      setGreyValuesFromObject(view_struct->painted_object, obj1);
      WlzFreeObj(obj1);
      redisplay_view_cb(view_struct->canvas, (XtPointer) view_struct,
			NULL);

      /* subtract it from the existing domain */
      obj1 = WlzDiffDomain(view_struct->curr_domain[domain], obj, &errNum);
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

    if( errNum != WLZ_ERR_NONE ){
      MAPaintReportWlzError(globals.topl, "setDomainIncrement", errNum);
    }
    return;
  }

  /* if increment then check dominance */
  if( errNum == WLZ_ERR_NONE ){
    numOverlays = globals.cmapstruct->num_overlays +
      globals.cmapstruct->num_solid_overlays;
    values.core = NULL;
    obj1 = WlzMakeMain(obj->type, obj->domain, values, NULL, NULL, NULL);
    obj1 = WlzAssignObject(obj1, NULL);
    dominanceFlag = 1;
  }

  for(j=1;(errNum == WLZ_ERR_NONE) && (j <= numOverlays); j++){
    i = globals.priority_to_domain_lut[j];

    /* if current domain */
    if( i == (int) domain ){
      /* clear to greys here to get rid of solid colours */
      obj1->values =
	WlzAssignValues(view_struct->view_object->values, &errNum);
      setGreyValuesFromObject(view_struct->painted_object, obj1);

      if( view_struct->curr_domain[i] ){
	obj2 = WlzAssignObject(WlzUnion2(view_struct->curr_domain[i],
					 obj1, &errNum), NULL);
	WlzFreeObj(view_struct->curr_domain[i]);
	view_struct->curr_domain[i] = obj2;
      }
      else {
	view_struct->curr_domain[i] = WlzAssignObject(obj1, &errNum);
      }
      dominanceFlag = 0;
      globals.domain_changed_since_saved[i] = 1;
      continue;
    }

    /* if dominant to current domain */
    if( view_struct->curr_domain[i] && dominanceFlag ){
      obj2 = WlzDiffDomain(obj1, view_struct->curr_domain[i], &errNum);
      WlzFreeObj(obj1);
      obj1 = NULL;
      if( obj2 ){
	if( WlzArea(obj2, &errNum) > 0 ){
	  obj1 = WlzAssignObject(obj2, NULL);
	}
	else {
	  WlzFreeObj(obj2);
	  return;
	}
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

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "setDomainIncrement", errNum);
  }
  return;
}

int domainChanged(void)
{
  int	i;

  for(i=1; i < 33; i++){
    if( globals.domain_changed_since_saved[i] ){
      return 1;
    }
  }
  return 0;
}

