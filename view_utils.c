#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	view_utils.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>

#include <MAPaint.h>


int init_view_struct(
  ThreeDViewStruct	*view_struct)
{
  XWindowAttributes	win_att;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win = XtWindow(view_struct->canvas);
  unsigned int		widthp, heightp;
  Dimension		win_width, win_height;
  char			*data;
  WlzThreeDViewStruct	*wlzViewStr = view_struct->wlzViewStr;
  float			minz, maxz, z;
  Widget		widget;

  if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
    return( 1 );

  /* initialise the 3D view structure */
  WlzInit3DViewStruct(wlzViewStr, globals.obj);

  /* set the limits set the slider values here */
  minz = wlzViewStr->minvals.vtZ;
  maxz = wlzViewStr->maxvals.vtZ;
  if( wlzViewStr->dist < wlzViewStr->minvals.vtZ )
    wlzViewStr->dist = wlzViewStr->minvals.vtZ;
  if( wlzViewStr->dist > wlzViewStr->maxvals.vtZ )
    wlzViewStr->dist = wlzViewStr->maxvals.vtZ;
  z = wlzViewStr->dist;

  HGU_XmSetSliderRange( view_struct->slider, minz, maxz );
  HGU_XmSetSliderValue( view_struct->slider, z );

  /* set the zeta slider */
  if( widget = XtNameToWidget(view_struct->dialog, "*.zeta_slider") ){
    HGU_XmSetSliderValue(widget,
			 view_struct->wlzViewStr->zeta * 180.0 / WLZ_M_PI);
  }

  /* set up the ximage */
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
  view_struct->ximage = XCreateImage(dpy, win_att.visual, win_att.depth,
				     ZPixmap, 0, NULL, widthp,
				     heightp, 8, widthp);

  /* resize the canvas */
  win_width = widthp * wlzViewStr->scale;
  win_height = heightp * wlzViewStr->scale;
  XtVaSetValues(view_struct->canvas,
		XmNwidth, win_width,
		XmNheight, win_height,
		NULL);

  /* set backing store */
/*  set_backingX( dpy, win );*/

  view_struct->display_list = 0;
  return( 0 );
}

int reset_view_struct(
  ThreeDViewStruct	*view_struct)
{
  unsigned int	widthp, heightp;
  Dimension		win_width, win_height;
  WlzThreeDViewStruct	*wlzViewStr = view_struct->wlzViewStr;
  float		minz, maxz, z;
  Widget	widget;

  /* initialise the 3D view structure */
  WlzInit3DViewStruct(wlzViewStr, globals.obj);

  /* set the limits set the slider values here */
  minz = wlzViewStr->minvals.vtZ;
  maxz = wlzViewStr->maxvals.vtZ;
  if( wlzViewStr->dist < wlzViewStr->minvals.vtZ )
    wlzViewStr->dist = wlzViewStr->minvals.vtZ;
  if( wlzViewStr->dist > wlzViewStr->maxvals.vtZ )
    wlzViewStr->dist = wlzViewStr->maxvals.vtZ;
  z = wlzViewStr->dist;

  HGU_XmSetSliderRange( view_struct->slider, minz, maxz );
  HGU_XmSetSliderValue( view_struct->slider, z );

  /* resize the canvas */
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
  win_width = widthp * wlzViewStr->scale;
  win_height = heightp * wlzViewStr->scale;
  XtVaSetValues(view_struct->canvas,
		XmNwidth, win_width,
		XmNheight, win_height,
		NULL);

  /* reset the dialog title */
  set_view_dialog_title(view_struct->dialog, wlzViewStr->theta,
			wlzViewStr->phi);

  /* reset the ximage */
  view_struct->ximage->width = widthp;
  view_struct->ximage->height = heightp;
  view_struct->ximage->bytes_per_line = widthp;

  /* tablet needs re-initialising if in use */
  view_struct->tablet_initialised = 0;

  /* reset the view object */
  if( view_struct->view_object != NULL )
  {
    WlzFreeObj( view_struct->view_object );
    view_struct->view_object = NULL;
  }

  /* free the painted object */
  if( view_struct->painted_object != NULL )
  {
    WlzFreeObj( view_struct->painted_object );
    view_struct->painted_object = NULL;
  }  

  return( 0 );
}

int free_view_struct(
  ThreeDViewStruct	*view_struct)
{
  Display	*dpy = XtDisplay(view_struct->canvas);

  WlzFree3DViewStruct(view_struct->wlzViewStr);

  view_struct->ximage->data = NULL;
  XDestroyImage(view_struct->ximage);

  glDeleteLists(view_struct->display_list, 1);
  MAOpenGLDrawScene( globals.canvas );

  /* check the view and painted objects */
  if( view_struct->view_object != NULL )
  {
    WlzFreeObj( view_struct->view_object );
    view_struct->view_object = NULL;
  }

  if( view_struct->painted_object != NULL )
  {
    WlzFreeObj( view_struct->painted_object );
    view_struct->painted_object = NULL;
  }  

  if( view_struct->prev_view_obj != NULL )
  {
    WlzFreeObj( view_struct->prev_view_obj );
    view_struct->prev_view_obj = NULL;
  }  

  return( 0 );
}

void getViewDomains(
  ThreeDViewStruct	*view_struct)
{
  int		i, j, numOverlays;
  WlzObject	*allDomainsObj=NULL, *tmpObj;
  WlzPixelV	thresh;

  /* clear any current domains */
  numOverlays = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;
  for(i=0; i < numOverlays; i++){
    if( view_struct->curr_domain[i] ){
      WlzFreeObj(view_struct->curr_domain[i]);
      view_struct->curr_domain[i] = NULL;
    }
  }

  /* get the union of all domains */
  thresh.type = WLZ_GREY_INT;
  thresh.v.inv = globals.cmapstruct->ovly_cols[DOMAIN_1];
  tmpObj = WlzThreshold(view_struct->painted_object,
			thresh, WLZ_THRESH_HIGH, NULL);
  if( tmpObj ){
    if( WlzArea(tmpObj, NULL) > 0 ){
      allDomainsObj = WlzAssignObject(tmpObj, NULL);
    }
    else {
      WlzFreeObj(tmpObj);
    }
  }

  /* segment the painted section image to establish each domain object */
  if( allDomainsObj ){
    WlzPixelV	min, max;
    int		max_i=numOverlays;

    /* get maximum number of domains */
    WlzGreyRange(allDomainsObj, &min, &max);
    WlzValueConvertPixel(&max, max, WLZ_GREY_INT);

    while( (max_i > 0) && (globals.cmapstruct->ovly_cols[max_i] > max.v.inv) ){
      max_i--;
    }

    for(i=1; i <= max_i; i++){
      tmpObj = get_domain_from_object(view_struct->painted_object, i);
      if( tmpObj ){
	view_struct->curr_domain[i] = WlzAssignObject(tmpObj, NULL);
      }
    }
  }

  /* propogate from previous section 
     asking for confirmation if a domain will be changed by
     propogation.
     This will either be if the current domain exists or if the 
     propogating domain overlaps the union of all existing domains */
  if( globals.propogate ){
    for(i=1; i <= numOverlays; i++){
      if((view_struct->prev_domain[i]) &&
	 (view_struct->prev_domain[i]->type != WLZ_EMPTY_OBJ)){
	if( allDomainsObj ){
	  /* first check if there is a curr_domain */
	  int needConfirm = 0;
	  if((view_struct->curr_domain[i]) && 
	     (view_struct->curr_domain[i]->type != WLZ_EMPTY_OBJ)){
	    needConfirm = 1;
	  }
	  else {
	    tmpObj = WlzIntersect2(view_struct->prev_domain[i],
				   allDomainsObj, NULL);
	    if( tmpObj && (WlzArea(tmpObj, NULL) > 0) ){
	      needConfirm = 1;
	      WlzFreeObj(tmpObj);
	    }
	  }
	  if( needConfirm ){
	    if( HGU_XmUserConfirm(view_struct->dialog,
				  "Warning: propogation may modify\n"
				  "    one or more existing domains.\n"
				  "    Propogate anyway?",
				  "Yes", "No", 0) ){
	      if( view_struct->curr_domain[i] ){
		tmpObj = WlzUnion2(view_struct->curr_domain[i],
				   view_struct->prev_domain[i], NULL);
		WlzFreeObj(view_struct->curr_domain[i]);
		view_struct->curr_domain[i] = 
		  WlzAssignObject(tmpObj, NULL);
	      }
	      else {
		view_struct->curr_domain[i] =
		  WlzAssignObject(view_struct->prev_domain[i], NULL);
	      }
	      globals.domain_changed_since_saved[i] = 1;
	    }
	  }
	  else {
	    view_struct->curr_domain[i] =
	      WlzAssignObject(view_struct->prev_domain[i], NULL);
	    globals.domain_changed_since_saved[i] = 1;
	  }
	}
	else {
	  view_struct->curr_domain[i] =
	    WlzAssignObject(view_struct->prev_domain[i], NULL);
	  globals.domain_changed_since_saved[i] = 1;
	}
      }
    }
  }

  /* put all objects onto painted image in reverse dominance order */
  for(j=numOverlays; j > 0; j--){
    i = globals.priority_to_domain_lut[j];
    if( view_struct->curr_domain[i] ){
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
    }
  }      

  /* redisplay the view */
  redisplay_view_cb(view_struct->canvas, view_struct, NULL);

  return;
}

void installViewDomains(
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  WlzIntervalWSpace	iwsp;
  WlzGreyWSpace		gwsp;
  WlzGreyValueWSpace	*gVWSp = NULL;
  double		x, y, z;
  int			xp, yp;
  int			i;
  ViewListEntry		*vl = global_view_list;

  /* all domains on this plane will already have been
     checked for consistency, dominance etc. therefore
     simply set the corresponding domains in the painted
     object */
  /* this is the set version of get section and should be in the
     woolz library. This is probably faster because many assumptions
     are made, mainly the grey-type */
  gVWSp = WlzGreyValueMakeWSp(globals.obj, NULL);
  WlzInitGreyScan( view_struct->painted_object, &iwsp, &gwsp );
  while( WlzNextGreyInterval( &iwsp ) == WLZ_ERR_NONE )
  {
    yp = iwsp.linpos - iwsp.linbot;
    for(i=0; i <iwsp.colrmn; i++){
      xp = iwsp.lftpos - iwsp.intdmn->kol1 + i;
      x = wlzViewStr->xp_to_x[xp] + wlzViewStr->yp_to_x[yp];
      y = wlzViewStr->xp_to_y[xp] + wlzViewStr->yp_to_y[yp];
      z = wlzViewStr->xp_to_z[xp] + wlzViewStr->yp_to_z[yp];
      x = WLZ_NINT(x);
      y = WLZ_NINT(y);
      z = WLZ_NINT(z);
      WlzGreyValueGet(gVWSp, z, y, x);
      *(gVWSp->gPtr[0].ubp) = *(gwsp.u_grintptr.ubp+i);
    }
  }
  WlzGreyValueFreeWSp(gVWSp);

  /* copy the new domains to the previous domains */
  for(i=0; i < 33; i++){
    if( view_struct->prev_domain[i] ){
      WlzFreeObj(view_struct->prev_domain[i]);
      view_struct->prev_domain[i] = NULL;
    }
    if( view_struct->curr_domain[i] ){
      view_struct->prev_domain[i] =
	WlzAssignObject(view_struct->curr_domain[i], NULL);
    }
  }

  /* set the previous distance and view object  - calculate when needed */
  view_struct->prev_dist = wlzViewStr->dist;
  if( view_struct->prev_view_obj ){
    WlzFreeObj(view_struct->prev_view_obj);
  }
  if( view_struct->view_object ){
    view_struct->prev_view_obj = WlzAssignObject(view_struct->view_object,
						 NULL);
  }
  else {
    view_struct->prev_view_obj = NULL;
  }
  
  /* display all views to update the new domains */
  while( vl != NULL ){
    if( vl->view_struct != paint_key ){
      if( vl->view_struct == view_struct ){
	redisplay_view_cb(view_struct->canvas,
			  (XtPointer) vl->view_struct, NULL);
      }
      else {
	display_view_cb(view_struct->canvas,
			(XtPointer) vl->view_struct, NULL);
      }
    }
    vl = vl->next;
  }

  return;
}
