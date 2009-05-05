#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _tools_tr_cking_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         tools_tracking.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:30:19 2009
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
#include <math.h>

#include <MAPaint.h>

static Widget	spacing_slider, range_slider, size_slider;
static Widget	dist_slider, alpha_slider, kappa_slider;

void imageTrackCurrentDomain(
  ThreeDViewStruct	*view_struct,
  DomainSelection	domain)
{
  WlzObject		*ref_obj=NULL, *new_domain, *ref_domain;
  WlzObject		*obj;
  MATrackDomainSearchParams	searchParams;
  MATrackDomainCostParams	costParams;
  PMSnakeLCParams		LCParams;
  PMSnakeNLCParams		NLCParams;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check there is a previous object for this domain */
  if((view_struct->prev_domain[domain] == NULL) ||
     (WlzArea(view_struct->prev_domain[domain], NULL) <= 0))
  {
    return;
  }

  /* if there is an existing object then ask for confirmation before
     continuing */
  if(view_struct->curr_domain[domain] &&
     (WlzArea(view_struct->curr_domain[domain], &errNum) > 0)){
    /* get confirmation */
    if( !HGU_XmUserConfirm(globals.topl,
			   "Image Track:\n"
			   "    Tracking may change the existing\n"
			   "    domain on this section.\n"
			   "Press quit to abandon tracking",
			   "continue", "quit", 0) )
    {
      return;
    }
  }

  /* get the reference image and domain */
  if( errNum == WLZ_ERR_NONE ){
    if( view_struct->prev_view_obj ){
      ref_obj = WlzAssignObject(view_struct->prev_view_obj, NULL);
    }
    else {
      float	tmpDist, curr_dist;

      curr_dist = view_struct->wlzViewStr->dist;
      tmpDist = view_struct->prev_dist;
      Wlz3DSectionIncrementDistance(view_struct->wlzViewStr,
				    (tmpDist - curr_dist));
      obj = WlzGetSectionFromObject(globals.orig_obj, view_struct->wlzViewStr,
				    WLZ_INTERPOLATION_NEAREST, &errNum);
      view_struct->prev_view_obj = WlzAssignObject(obj, NULL);
      Wlz3DSectionIncrementDistance(view_struct->wlzViewStr,
				    -(tmpDist - curr_dist));
    }
  }
    
  if( errNum == WLZ_ERR_NONE ){
    ref_domain = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			     view_struct->prev_domain[domain]->domain,
			     view_struct->prev_domain[domain]->values,
			     NULL, NULL, &errNum);
    
    /* set up the search control parameters */
    searchParams.spacing = HGU_XmGetSliderValue( spacing_slider );
    searchParams.range = HGU_XmGetSliderValue( range_slider );

    /* set up the image cost parameters */
    costParams.size = HGU_XmGetSliderValue( size_slider );
    costParams.LCParams = &LCParams;
    costParams.NLCParams = &NLCParams;

    /* now the local and non-local snake costs */
    LCParams.nu_dist = HGU_XmGetSliderValue(dist_slider);
    NLCParams.nu_alpha = HGU_XmGetSliderValue(alpha_slider);
    NLCParams.nu_kappa = HGU_XmGetSliderValue(kappa_slider);
  }

  /* get the current image and new domain by tracking */
  if( (errNum == WLZ_ERR_NONE) && (view_struct->view_object == NULL) ){
    if((obj = WlzGetSectionFromObject(globals.orig_obj,
				      view_struct->wlzViewStr,
				      WLZ_INTERPOLATION_NEAREST, &errNum))){
      view_struct->view_object = WlzAssignObject(obj, NULL);
    }
    else {
      WlzFreeObj( ref_obj );
      WlzFreeObj( ref_domain );
      return;
    }
  }

  if( errNum == WLZ_ERR_NONE ){
    new_domain = HGU_TrackDomain(ref_obj, view_struct->view_object, ref_domain,
				 MATRACK_PMSNAKE_SEARCH_METHOD,
				 &searchParams,
				 MATRACK_CORRELATION_COST_TYPE,
				 &costParams,
				 NULL);
    WlzFreeObj( ref_obj );
    WlzFreeObj( ref_domain );

    /* increment the current domain */
    if( new_domain ){
      setDomainIncrement(new_domain, view_struct, domain, 0);
      WlzFreeObj( new_domain );
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "imageTrackCurrentDomain", errNum);
  }
  return;
}

void   imageTrackCurrentDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  imageTrackCurrentDomain((ThreeDViewStruct *) client_data,
			  globals.current_domain);

  return;
}

void MAPaintTracking2DInit(
  ThreeDViewStruct	*view_struct)
{
  /* add a callback to each domain select toggle button */
  addSelectDomainCallback(imageTrackCurrentDomainCb,
			  (XtPointer) view_struct);

  /* track the current domain */
  imageTrackCurrentDomain(view_struct, globals.current_domain);

  /* initialise the paint ball callback */
  MAPaintPaintBall2DInit(view_struct);
  
  return;
}

void MAPaintTracking2DQuit(
  ThreeDViewStruct	*view_struct)
{
  /* quit the paint ball callback */
  MAPaintPaintBall2DQuit(view_struct);

  /* remove the callback from each domain select button */
  removeSelectDomainCallback(imageTrackCurrentDomainCb,
			      (XtPointer) view_struct);

  return;
}

void MAPaintTracking2DCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int		sectFlg=0;

  /* process events */
  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
    case Button2:
      /* do nothing if shift pressed - magnify option */
      if( cbs->event->xbutton.state & ButtonStateIgnoreMask ){
	break;
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
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
    case XK_Right:
    case XK_KP_Right:
    case XK_f:
    case XK_Left:
    case XK_KP_Left:
    case XK_b:
      /* section changed */
      sectFlg = 1;
      break;

    default:
      break;
    }
    break;

  default:
    break;
  }

  /* pass events to the the draw callback */
  MAPaintPaintBall2DCb(w, client_data, call_data);

  /* check for section increment */
  if( sectFlg ){
    imageTrackCurrentDomain(view_struct, globals.current_domain);
  }
  
  return;
}


Widget	CreateTracking2DControls(
  Widget	parent)
{
  Widget	form, label, form1, slider, frame, widget;

  /* create a parent form to hold all the tracking controls */
  form = XtVaCreateWidget("tracking_controls_form", xmFormWidgetClass,
			  parent,
			  XmNtopAttachment,     XmATTACH_FORM,
			  XmNbottomAttachment,	XmATTACH_FORM,
			  XmNleftAttachment,    XmATTACH_FORM,
			  XmNrightAttachment,  	XmATTACH_FORM,
			  NULL);

  /* create frame, form and label for the tracking parameters */
  frame = XtVaCreateManagedWidget("tracking_frame1", xmFrameWidgetClass,
				  form,
				  XmNtopAttachment,     XmATTACH_FORM,
				  XmNleftAttachment,    XmATTACH_FORM,
				  XmNrightAttachment,  	XmATTACH_FORM,
				  NULL);

  form1 = XtVaCreateWidget("tracking_form1", xmFormWidgetClass,
			   frame,
			   XmNtopAttachment,	XmATTACH_FORM,
			   XmNbottomAttachment,	XmATTACH_FORM,
			   XmNleftAttachment,	XmATTACH_FORM,
			   XmNrightAttachment,	XmATTACH_FORM,
			   NULL);

  label = XtVaCreateManagedWidget("tracking_parameters", xmLabelWidgetClass,
				  frame,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  XmNborderWidth,	0,
				  NULL);

  /* create sliders for the parameter values */
  slider = HGU_XmCreateHorizontalSlider("spacing", form1,
					5.0, 3.0, 20.0, 0,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  spacing_slider = slider;

  slider = HGU_XmCreateHorizontalSlider("range", form1,
					5.0, 3.0, 20.0, 0,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  range_slider = slider;

  XtManageChild( form1 );

  /* create frame, form and label for the snake cost parameters */
  frame = XtVaCreateManagedWidget("tracking_frame2", xmFrameWidgetClass,
				  form,
				  XmNtopAttachment,     XmATTACH_WIDGET,
				  XmNtopWidget,		frame,
				  XmNleftAttachment,    XmATTACH_FORM,
				  XmNrightAttachment,  	XmATTACH_FORM,
				  NULL);

  form1 = XtVaCreateWidget("tracking_form2", xmFormWidgetClass,
			   frame,
			   XmNtopAttachment,	XmATTACH_FORM,
			   XmNbottomAttachment,	XmATTACH_FORM,
			   XmNleftAttachment,	XmATTACH_FORM,
			   XmNrightAttachment,	XmATTACH_FORM,
			   NULL);

  label = XtVaCreateManagedWidget("snake_cost_parameters", xmLabelWidgetClass,
				  frame,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  XmNborderWidth,	0,
				  NULL);

  /* create sliders for the parameter values */
  slider = HGU_XmCreateHorizontalSlider("nu_dist", form1,
					0.5, 0.0, 5.0, 2,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  dist_slider = slider;

  slider = HGU_XmCreateHorizontalSlider("nu_alpha", form1,
					0.0, 0.0, 5.0, 2,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  alpha_slider = slider;

  slider = HGU_XmCreateHorizontalSlider("nu_kappa", form1,
					0.1, 0.0, 10.0, 2,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  kappa_slider = slider;

  XtManageChild( form1 );

  /* create frame, form and label for the image cost parameters */
  frame = XtVaCreateManagedWidget("tracking_frame3", xmFrameWidgetClass,
				  form,
				  XmNtopAttachment,     XmATTACH_WIDGET,
				  XmNtopWidget,		frame,
				  XmNleftAttachment,    XmATTACH_FORM,
				  XmNrightAttachment,  	XmATTACH_FORM,
				  NULL);

  form1 = XtVaCreateWidget("tracking_form3", xmFormWidgetClass,
			   frame,
			   XmNtopAttachment,	XmATTACH_FORM,
			   XmNbottomAttachment,	XmATTACH_FORM,
			   XmNleftAttachment,	XmATTACH_FORM,
			   XmNrightAttachment,	XmATTACH_FORM,
			   NULL);

  label = XtVaCreateManagedWidget("image_cost_parameters", xmLabelWidgetClass,
				  frame,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  XmNborderWidth,	0,
				  NULL);

  /* create sliders for the parameter values */
  slider = HGU_XmCreateHorizontalSlider("size", form1,
					4.0, 0.0, 10.0, 0,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  size_slider = slider;

  XtManageChild( form1 );

  return( form );
}
