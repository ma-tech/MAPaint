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
* File        :    tools_morphological.c				     *
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Fri May 12 10:45:18 1995				     *
* Synopsis    :    Morphological tools for manipulating the paint domain     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

static Widget		morph_controls=NULL;
static WlzDistanceType	struct_elem_type = WLZ_8_DISTANCE;
static int		struct_elem_size = 1;

static void struct_elem_cb(Widget	w,
			   XtPointer	client_data,
			   XtPointer	call_data);

static MenuItem struct_elem_items[] = {	/* structuring element items */
{"8_neighbour", &xmPushButtonGadgetClass, 0, NULL, NULL, True,
     struct_elem_cb, (XtPointer) WLZ_8_DISTANCE,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"4_neighbour", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     struct_elem_cb, (XtPointer) WLZ_4_DISTANCE,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"octagonal", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     struct_elem_cb, (XtPointer) WLZ_OCTAGONAL_DISTANCE,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"euclidean", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     struct_elem_cb, (XtPointer) WLZ_EUCLIDEAN_DISTANCE,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
NULL,
};

void struct_elem_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    XmPushButtonCallbackStruct *cbs =
	(XmPushButtonCallbackStruct *) call_data;

    struct_elem_type = (WlzDistanceType) client_data;
    return;
}

void changeSESize(int downFlag)
{
  float		currVal, minVal, maxVal;
  Widget	slider;

  /* get the slider */
  if( (slider =
       XtNameToWidget(globals.topl,
		      "*tool_control_dialog*morph_controls_form*size"))
     == NULL ){
    return;
  }

  /* get current val and val range */
  currVal = HGU_XmGetSliderValue(slider);
  HGU_XmGetSliderRange(slider, &minVal, &maxVal);

  /* calculate next value */
  if( downFlag ){
    currVal = (currVal <= minVal) ? minVal : currVal - 1.0;
  }
  else {
    currVal = (currVal >= maxVal) ? maxVal : currVal + 1.0;
  }
  HGU_XmSetSliderValue(slider, currVal);
    
  /* call the callbacks - none */
  
  return;
}

void MAPaintMorphological2DInit(
  ThreeDViewStruct	*view_struct)
{
  return;
}

void MAPaintMorphological2DQuit(
  ThreeDViewStruct	*view_struct)
{
  return;
}

void MAPaintMorphological2DCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;
  DomainSelection	sel_domain;
  int			delFlag;
  WlzObject		*obj, *obj1, *obj2, *structElem;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

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
      delFlag = ((cbs->event->xbutton.state & Mod1Mask) ||
		 (cbs->event->xbutton.button == Button2));
      x = cbs->event->xbutton.x;
      y = cbs->event->xbutton.y;

      /* get the selected object */
      if( (sel_domain = getSelectedDomainType(x, y, view_struct)) < 1 ){
	break;
      }
      obj = getSelectedRegion(x, y, view_struct);

      /* if erode then get eroded object and find difference and delete,
	 if dilate then get dilated object and add */
      structElem = WlzMakeStdStructElement(WLZ_2D_DOMAINOBJ,
					   struct_elem_type,
					   struct_elem_size, &errNum);
      if( errNum == WLZ_ERR_NONE ){
	if( delFlag ){
	  if((obj1 = WlzStructErosion(obj, structElem, &errNum)) &&
	     (obj2 = WlzDiffDomain(obj, obj1, &errNum))){
	    WlzFreeObj(obj);
	    WlzFreeObj(obj1);
	    obj = WlzAssignObject(obj2, NULL);
	  }
	}
	else {
	  if( obj1 = WlzStructDilation(obj, structElem, &errNum) ){
	    WlzFreeObj(obj);
	    obj = WlzAssignObject(obj1, NULL);
	  }
	}
      }

      /* reset the painted object and redisplay */
      if( obj ){
	setDomainIncrement(obj, view_struct, sel_domain, delFlag);
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
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_KP_Right:
    case XK_f:
      /* if <alt> or <alt gr> pressed then change tool option */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	changeSESize(0);
      }
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_KP_Left:
    case XK_b:
      /* if <alt> or <alt gr> pressed then change tool option */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	changeSESize(1);
      }
      break;

    case XK_Down:
    case XK_n:
      break;
    }
    break;

  default:
    break;
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "MAPaintMorphological2DCb", errNum);
  }
  return;
}

Widget	CreateMorphologicalControls(
  Widget	parent)
{
  Widget	form, form1, label, slider, frame, widget, option_menu;

  /* create a parent form to hold all the tracking controls */
  form = XtVaCreateWidget("morph_controls_form", xmFormWidgetClass,
			  parent,
			  XmNtopAttachment,     XmATTACH_FORM,
			  XmNbottomAttachment,	XmATTACH_FORM,
			  XmNleftAttachment,    XmATTACH_FORM,
			  XmNrightAttachment,  	XmATTACH_FORM,
			  NULL);

  /* create frame, form and label for the morphological control parameters */
  frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass,
				  form,
				  XmNtopAttachment,     XmATTACH_FORM,
				  XmNleftAttachment,    XmATTACH_FORM,
				  XmNrightAttachment,  	XmATTACH_FORM,
				  NULL);

  form1 = XtVaCreateWidget("form1", xmFormWidgetClass,
			  frame,
			  XmNtopAttachment,     XmATTACH_FORM,
			  XmNbottomAttachment,	XmATTACH_FORM,
			  XmNleftAttachment,    XmATTACH_FORM,
			  XmNrightAttachment,  	XmATTACH_FORM,
			  NULL);

  label = XtVaCreateManagedWidget("morph_parameters", xmLabelWidgetClass,
				  frame,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  XmNborderWidth,	0,
				  NULL);

  /* create an option menu for the structuring element type */
  option_menu = HGU_XmBuildMenu( form1, XmMENU_OPTION, "struct_elem", 0,
				  XmTEAR_OFF_DISABLED, struct_elem_items);

  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  widget = option_menu;
  XtManageChild( option_menu );

  /* create slider for the structuring element radius */
  slider = HGU_XmCreateHorizontalSlider("size", form1,
					1.0, 1.0, 10.0, 0,
					NULL, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  XtManageChild( form1 );

  morph_controls = form;

  return( form );
}
