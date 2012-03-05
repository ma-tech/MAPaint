#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _tools_edge_tracking_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         tools_edge_tracking.c
* \author	Richard Baldock
* \date		April 2009
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <MAPaint.h>

static Widget	spacing_slider, range_slider, size_slider;
static Widget	direction_slider, gradient_slider;
static Widget	dist_slider, alpha_slider, kappa_slider;

void imageEdgeTrackDomain(
  ThreeDViewStruct	*view_struct,
  DomainSelection	selDomain,
  WlzObject		*domain)
{
  WlzObject		*new_domain, *ref_domain;
  WlzObject		*obj;
  MATrackDomainSearchParams	searchParams;
  EdgeCostParams		costParams, *costParamsP;
  PMSnakeLCParams		LCParams;
  PMSnakeNLCParams		snakeParams;
  PMEdgeSnakeNLCParams		NLCParams;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* get the reference image and domain */
  ref_domain = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain->domain,
			   domain->values, NULL, NULL, &errNum);
    
  if( errNum == WLZ_ERR_NONE ){
    /* set up the search control parameters */
    searchParams.spacing = HGU_XmGetSliderValue( spacing_slider );
    searchParams.range = HGU_XmGetSliderValue( range_slider );

    /* set up the image cost parameters */
    costParams.width = HGU_XmGetSliderValue( size_slider );
    costParams.gradient = HGU_XmGetSliderValue( gradient_slider );
    costParams.LCParams = &LCParams;
    costParams.NLCParams = &NLCParams;

  /* now the local and non-local snake costs */
    LCParams.nu_dist = HGU_XmGetSliderValue(dist_slider);
    NLCParams.snakeParams = &snakeParams;
    NLCParams.snakeParams->nu_alpha = HGU_XmGetSliderValue(alpha_slider);
    NLCParams.snakeParams->nu_kappa = HGU_XmGetSliderValue(kappa_slider);
    NLCParams.nu_direction = HGU_XmGetSliderValue(direction_slider);

    /* get the current image and new domain by tracking */
    if( view_struct->view_object == NULL ){
      if((obj = WlzGetSectionFromObject(globals.orig_obj,
					view_struct->wlzViewStr,
					WLZ_INTERPOLATION_NEAREST, &errNum))){
	view_struct->view_object = WlzAssignObject(obj, NULL);
      }
      else {
	MAPaintReportWlzError(globals.topl, "imageEdgeTrackDomain", errNum);
	return;
      }
    }
  }

  if( errNum == WLZ_ERR_NONE ){
    costParamsP = &costParams;
    new_domain = HGU_TrackDomain(view_struct->view_object,
				 view_struct->view_object, ref_domain,
				 MATRACK_PMSNAKE_SEARCH_METHOD,
				 &searchParams,
				 MATRACK_EDGE_COST_TYPE,
				 (MATrackDomainCostParams *) costParamsP,
				 &errNum);
    WlzFreeObj( ref_domain );

    /* increment the current domain */
    if( new_domain ){
      setDomainIncrement(new_domain, view_struct, selDomain, 0);
      WlzFreeObj( new_domain );
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "imageEdgeTrackDomain", errNum);
  }
  return;
}

void   imageEdgeTrackCurrentDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzObject		*domain;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* get the current domain */
  if(view_struct->curr_domain[globals.current_domain] &&
     (WlzArea(view_struct->curr_domain[globals.current_domain], &errNum) > 0)){
    domain = WlzAssignObject(view_struct->curr_domain[globals.current_domain],
			     NULL);
    imageEdgeTrackDomain(view_struct, globals.current_domain, domain);
    WlzFreeObj(domain);
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "imageEdgeTrackCurrentDomainCb", errNum);
  }
  return;
}

void MAPaintEdgeTracking2DInit(
  ThreeDViewStruct	*view_struct)
{
  return;
}

void MAPaintEdgeTracking2DQuit(
  ThreeDViewStruct	*view_struct)
{
  return;
}

void MAPaintEdgeTracking2DCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;
  DomainSelection	selDomain;
  WlzObject		*obj;

  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
    case Button2:
      /* do nothing if shift pressed - magnify option */
      if( cbs->event->xbutton.state & ButtonStateIgnoreMask ){
	break;
      }

      /* get the selected domain and object */
      x = cbs->event->xbutton.x;
      y = cbs->event->xbutton.y;
      if( (selDomain = getSelectedDomainType(x, y, view_struct)) < 1 ){
	break;
      }
      if( !(obj = getSelectedRegion(x, y, view_struct)) ){
	break;
      }
      obj = WlzAssignObject(obj, NULL);
      setDomainIncrement(obj, view_struct, selDomain, 1);

      /* edge track the selected object */
      imageEdgeTrackDomain(view_struct, selDomain, obj);

      WlzFreeObj( obj );
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


Widget	CreateEdgeTracking2DControls(
  Widget	parent)
{
  Widget	form, label, form1, slider, frame, widget;

  /* create a parent form to hold all the tracking controls */
  form = XtVaCreateWidget("edge_tracking_controls_form", xmFormWidgetClass,
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
					4.0, 0.0, 10.0, 1,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  size_slider = slider;

  slider = HGU_XmCreateHorizontalSlider("nu_direction", form1,
					1.0, 0.0, 10.0, 2,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  direction_slider = slider;

  slider = HGU_XmCreateHorizontalSlider("gradient", form1,
					0.1, 0.0, 1.0, 3,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;
  gradient_slider = slider;

  XtManageChild( form1 );

  return( form );
}
