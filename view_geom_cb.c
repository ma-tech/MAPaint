#pragma ident "MRC HGU $Id$"
/************************************************************
* Copyright   :    Medical Research Council, UK. 
*                  Human Genetics Unit, 
*                  Western General Hospital. 
*                  Edinburgh. 
*************************************************************
* Project     :    Mouse Atlas Project
* File        :    view_geom_cb.c
*************************************************************
* Author      :    Richard Baldock  (richard@hgu.mrc.ac.uk)
* Date        :    Tue Jun 21 09:47:15 1994
* Synopsis    : 
*************************************************************/
#include <stdio.h>
#include <math.h>
#include <values.h>

#include <MAPaint.h>

void view_struct_clear_prev_obj(
  ThreeDViewStruct	*view_struct)
{
  int	i;

  for(i=0; i < 33; i++)
  {
    if( view_struct->prev_domain[i] != NULL )
    {
      WlzFreeObj( view_struct->prev_domain[i] );
      view_struct->prev_domain[i] = NULL;
    }
  }

  return;
}

void distance_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Widget	slider = w;
  float	new_dist;

  if( !wlzViewStr->initialised )
    if( init_view_struct( view_struct ) )
      return;

  while( strcmp(XtName(slider), "distance_slider") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }

  /* get the new distance and reset the LUT's */
  new_dist = HGU_XmGetSliderValue( slider );
  if( wlzViewStr->dist == new_dist )
    return;
  Wlz3DSectionIncrementDistance(wlzViewStr,
				(new_dist - wlzViewStr->dist));
  wlzViewStr->dist = new_dist;

    /* this is the one callback that does not reset the view therefore
       clear the view_object if defined */
  if( view_struct->view_object != NULL )
  {
    WlzFreeObj( view_struct->view_object );
    view_struct->view_object = NULL;
  }

  /* cancel fixed line mode if set */
  setViewSliderSensitivities(view_struct, True);
  view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

    /* tablet needs re-initialising if in use */
  view_struct->tablet_initialised = 0;

  /* redisplay the section */
  display_view_cb(w, (XtPointer) view_struct, call_data);
  return;
}

void scale_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  double		new_scale;

  if( !view_struct->wlzViewStr->initialised )
    if( init_view_struct( view_struct ) )
      return;

  /* get the new scale */
  if( strncmp(XtName(w), "0_25", 4) == 0 ){
    new_scale = 0.25;
  }
  else if( strncmp(XtName(w), "0_5", 3) == 0 ){
    new_scale = 0.5;
  }
  else if(sscanf(XtName(w), "%lf", &new_scale) < 1){
    return;
  }
  setViewScale(view_struct, new_scale, -1, -1);

  return;
}

void view_mode_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Widget		widget;

  if( !wlzViewStr->initialised )
    if( init_view_struct( view_struct ) )
      return;

  /* get the new view mode */
  if( strcmp(XtName(w), "up-is-up") == 0 ){
    wlzViewStr->view_mode = WLZ_UP_IS_UP_MODE;
  }
  else if( strcmp(XtName(w), "statue") == 0 ){
    wlzViewStr->view_mode = WLZ_STATUE_MODE;
  }
  else {
    wlzViewStr->view_mode = WLZ_ZETA_MODE;
  }
  reset_view_struct( view_struct );

  /* set the zeta slider */
  if( widget = XtNameToWidget(view_struct->dialog, "*.zeta_slider") ){
    if( wlzViewStr->view_mode == WLZ_ZETA_MODE ){
      XtSetSensitive(widget, True);
    }
    else {
      XtSetSensitive(widget, False);
      HGU_XmSetSliderValue(widget,
			   wlzViewStr->zeta * 180.0 / WLZ_M_PI);
    }
  }

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void theta_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Widget	slider = w;

  if( !wlzViewStr->initialised )
    if( init_view_struct( view_struct ) )
      return;

  while( strcmp(XtName(slider), "theta_slider") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }

  /* get the new theta and reset the LUT's */
  wlzViewStr->theta = HGU_XmGetSliderValue( slider );
  wlzViewStr->theta *= WLZ_M_PI / 180.0;
  reset_view_struct( view_struct );

  /* set the zeta slider */
  if( slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider") ){
    HGU_XmSetSliderValue(slider,
			 view_struct->wlzViewStr->zeta * 180.0 / WLZ_M_PI);
  }

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void phi_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Widget	slider = w;

  if( !wlzViewStr->initialised )
    if( init_view_struct( view_struct ) )
      return;

  while( strcmp(XtName(slider), "phi_slider") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }

  /* get the new phi and reset the LUT's */
  wlzViewStr->phi = HGU_XmGetSliderValue( slider );
  wlzViewStr->phi *= WLZ_M_PI / 180.0;
  reset_view_struct( view_struct );

  /* set the zeta slider */
  if( slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider") ){
    HGU_XmSetSliderValue(slider,
			 view_struct->wlzViewStr->zeta * 180.0 / WLZ_M_PI);
  }

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void zeta_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Widget	slider = w;

  if( !wlzViewStr->initialised )
    if( init_view_struct( view_struct ) )
      return;

  while( strcmp(XtName(slider), "zeta_slider") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }

  /* get the new zeta and reset the LUT's */
  wlzViewStr->zeta = HGU_XmGetSliderValue( slider );
  wlzViewStr->zeta *= WLZ_M_PI / 180.0;
  reset_view_struct( view_struct );

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

void psi_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Widget	slider = w;
  float	theta, phi, psi;

  if( !wlzViewStr->initialised )
    if( init_view_struct( view_struct ) )
      return;

  while( strcmp(XtName(slider), "psi_slider") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }

  /* get the new psi and reset the LUT's */
  psi = HGU_XmGetSliderValue( slider );
  psi *= WLZ_M_PI / 180.0;

  phi = acos(cos(psi)*view_struct->norm2.vtZ +
	     sin(psi)*view_struct->norm3.vtZ);
  if( fabs(sin(phi)) < .01 ){
    theta = 0.0;
  }
  else {
    theta = atan2(cos(psi)*view_struct->norm2.vtY +
		  sin(psi)*view_struct->norm3.vtY,
		  cos(psi)*view_struct->norm2.vtX +
		  sin(psi)*view_struct->norm3.vtX);
    if( theta < 0.0 ){
      theta += 2 * WLZ_M_PI;
    }
  }

  wlzViewStr->theta = theta;
  wlzViewStr->phi = phi;
  theta *= 180 / M_PI;
  phi   *= 180 / M_PI;

  slider = XtNameToWidget(view_struct->dialog, "*.theta_slider");
  HGU_XmSetSliderValue(slider, theta);
  slider = XtNameToWidget(view_struct->dialog, "*.phi_slider");
  HGU_XmSetSliderValue(slider, phi);

  reset_view_struct( view_struct );

  /* set the zeta slider */
  if( slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider") ){
    HGU_XmSetSliderValue(slider,
			 view_struct->wlzViewStr->zeta * 180.0 / WLZ_M_PI);
  }

  /* redisplay the section */
  XClearWindow(XtDisplay(view_struct->canvas),
	       XtWindow(view_struct->canvas));
  display_view_cb(w, (XtPointer) view_struct, call_data);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  return;
}

