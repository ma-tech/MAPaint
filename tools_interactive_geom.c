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
*   File       :   tools_interactive_geom.c				*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Sat Nov  2 16:48:12 1996				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>


static PaintGeometryObjectType geomObjectType=MAPAINT_GEOMETRY_OBJ_FCIRCLE;
static WlzObject *geomObj=NULL;
static Cursor geomCursor;

void geomObjectCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	slider;
  int		radius;
  WlzDomain	geomDom;
  WlzValues	geomVals;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  geomObjectType = (PaintGeometryObjectType) client_data;

  /* clear old object and cursor */
  if( geomObj ){
    WlzFreeObj(geomObj);
    XFreeCursor(XtDisplayOfObject(w), geomCursor);
  }

  /* get the object size slider and get the size */
  slider = XtNameToWidget(globals.topl,
			  "*geometry_controls_form*object_size");
  radius = (int) HGU_XmGetSliderValue( slider ) / 2;

  /* create the geometry object */
  switch( geomObjectType ){
  case MAPAINT_GEOMETRY_OBJ_FCIRCLE:
  case MAPAINT_GEOMETRY_OBJ_VCIRCLE:
    geomObj = WlzMakeCircleObject((double) radius + 0.3, 0.0, 0.0, &errNum);
    break;

  case MAPAINT_GEOMETRY_OBJ_FSQUARE:
  case MAPAINT_GEOMETRY_OBJ_VSQUARE:
    geomDom.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_RECT,
				      -radius, radius, -radius, radius,
				      &errNum);
    geomVals.core = NULL;
    geomObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, geomDom, geomVals,
			    NULL, NULL, NULL);
    break;

  default:
    geomObj = NULL;
    return;
  }

  /* create the cursor */
  if( errNum == WLZ_ERR_NONE ){
    geomCursor = HGU_XCreateObjectCursor(XtDisplayOfObject(w),
					 XtWindowOfObject(w), geomObj, 0);

    /* if geometry painting then define it for the window */
    if((paint_key) &&
       (globals.currentPaintAction == MAPAINT_GEOMETRY_OBJ_2D)){
      XUndefineCursor(XtDisplay(paint_key->canvas),
		      XtWindow(paint_key->canvas));
      XDefineCursor(XtDisplay(paint_key->canvas),
		    XtWindow(paint_key->canvas), geomCursor);
    }
  }
  else {
    MAPaintReportWlzError(globals.topl, "geomObjectCb", errNum);
  }

  return;
}

void geomSliderCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  geomObjectCb(w, (XtPointer) geomObjectType, NULL);
  return;
}

void changeObjectSize(int downFlag)
{
  float		currVal, minVal, maxVal;
  Widget	slider;

  /* get the slider */
  if( (slider =
       XtNameToWidget(globals.topl,
		      "*tool_control_dialog*geometry_form1*object_size"))
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
    
  /* call the callbacks */
  geomSliderCb(slider, NULL, NULL);
  
  return;
}

void MAPaintGeom2DInit(
  ThreeDViewStruct	*view_struct)
{
  /* set the geometry object and cursor */
  if( geomObj == NULL ){
    geomObjectCb(view_struct->canvas, (XtPointer) geomObjectType, NULL);
  }

  XUndefineCursor(XtDisplay(view_struct->canvas),
		  XtWindow(view_struct->canvas));
  
  XDefineCursor(XtDisplay(view_struct->canvas),
		XtWindow(view_struct->canvas), geomCursor);

  return;
}

void MAPaintGeom2DQuit(
  ThreeDViewStruct	*view_struct)
{
  /* unset the cursor */
  XUndefineCursor(XtDisplay(view_struct->canvas),
		  XtWindow(view_struct->canvas));

  return;
}

void MAPaintGeom2DCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y, radius, delX, delY;
  DomainSelection	currDomain;
  int			delFlag;
  WlzObject		*obj, *obj1;
  Widget		slider;
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
      currDomain = globals.current_domain;
      delFlag = ((cbs->event->xbutton.state & Mod1Mask) ||
		 (cbs->event->xbutton.button == Button2));

      /* scale and shift the geomObj */
      /* use the cursor object to increment the domain */
      delX = (int) cbs->event->xbutton.x - 1;
      delY = (int) cbs->event->xbutton.y - 1;
      if( obj = WlzAssignObject(WlzShiftObject(geomObj, delX, delY,
					       0, &errNum), NULL) ){
					   
	if( wlzViewStr->scale > 0.95 ){
	  obj1 = WlzIntRescaleObj(obj, WLZ_NINT(wlzViewStr->scale), 0, &errNum);
	}
	else {
	  float invScale = 1.0 / wlzViewStr->scale;
	  obj1 = WlzIntRescaleObj(obj, WLZ_NINT(invScale), 1, &errNum);
	}
	obj1 = WlzAssignObject(obj1, NULL);
	WlzFreeObj(obj);

	if( obj1 ){
	  delX =  wlzViewStr->minvals.vtX;
	  delY =  wlzViewStr->minvals.vtY;
	  obj = WlzAssignObject(WlzShiftObject(obj1, delX, delY,
					       0, &errNum), NULL);
	  WlzFreeObj(obj1);
	}
	else {
	  obj = NULL;
	}
      }

      /* reset the painted object and redisplay */
      if( obj ){
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
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      /* if <alt> or <alt gr> pressed then change tool option */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	changeObjectSize(0);
      }
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_b:
      /* if <alt> or <alt gr> pressed then change tool option */
      if( cbs->event->xkey.state&(Mod1Mask|Mod2Mask) ){
	changeObjectSize(1);
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
    MAPaintReportWlzError(globals.topl, "MAPaintGeom2DCb", errNum);
  }
  return;
}


static MenuItem geom_object_items[] = {	/* geometry object items */
{"fixed_circle", &xmPushButtonGadgetClass, 0, NULL, NULL, True,
     geomObjectCb, (XtPointer) MAPAINT_GEOMETRY_OBJ_FCIRCLE,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"fixed_square", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     geomObjectCb, (XtPointer) MAPAINT_GEOMETRY_OBJ_FSQUARE,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"variable_circle", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     geomObjectCb, (XtPointer) MAPAINT_GEOMETRY_OBJ_VCIRCLE,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"variable_square", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     geomObjectCb, (XtPointer) MAPAINT_GEOMETRY_OBJ_VSQUARE,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
NULL,
};

Widget	CreateGeometryObjectControls(
  Widget	parent)
{
  Widget	form, form1, frame, label, widget;
  Widget	option_menu, slider;

  /* create a parent form to hold all the tracking controls */
  form = XtVaCreateWidget("geometry_controls_form", xmFormWidgetClass,
			  parent,
			  XmNtopAttachment,     XmATTACH_FORM,
			  XmNbottomAttachment,	XmATTACH_FORM,
			  XmNleftAttachment,    XmATTACH_FORM,
			  XmNrightAttachment,  	XmATTACH_FORM,
			  NULL);

  /* create frame, form and label for the tracking parameters */
  frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass,
				  form,
				  XmNtopAttachment,     XmATTACH_FORM,
				  XmNleftAttachment,    XmATTACH_FORM,
				  XmNrightAttachment,  	XmATTACH_FORM,
				  NULL);

  form1 = XtVaCreateWidget("geometry_form1", xmFormWidgetClass,
			  frame,
			  XmNtopAttachment,     XmATTACH_FORM,
			  XmNbottomAttachment,	XmATTACH_FORM,
			  XmNleftAttachment,    XmATTACH_FORM,
			  XmNrightAttachment,  	XmATTACH_FORM,
			  NULL);

  label = XtVaCreateManagedWidget("geometry_params", xmLabelWidgetClass,
				  frame,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  XmNborderWidth,	0,
				  NULL);

  /* create an option menu for the threshold connectivity type */
  option_menu = HGU_XmBuildMenu( form1, XmMENU_OPTION, "geom_type", 0,
				  XmTEAR_OFF_DISABLED, geom_object_items);

  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  widget = option_menu;
  XtManageChild( option_menu );

  /* create slider for the initial threshold range */
  slider = HGU_XmCreateHorizontalSlider("object_size", form1,
					5.0, 1.0, 20.0, 0,
					geomSliderCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  XtManageChild( form1 );

  return( form );
}
