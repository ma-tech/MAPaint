#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Mouse Atlas MAPaint					*
*   File       :   MAWarpSignalPostProcPage.c				*
*************************************************************************
*   Author Name :  richard						*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Tue Dec 16 18:25:51 2003				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>
#include <MAWarp.h>

typedef enum _MAPaintOperatorType{
  MAPAINT_OP_ERODE,
  MAPAINT_OP_DILATE,
  MAPAINT_OP_OPEN,
  MAPAINT_OP_CLOSE,
  MAPAINT_OP_DUMMY
}MAPaintOperatorType;
  
void postProcMorphCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintOperatorType	opType=(MAPaintOperatorType) client_data;
  WlzObject		*seObj, *newDomain;

  /* build the structuring element */
  if( warpGlobals.seElemRadius != 1 ){
    switch( warpGlobals.seType ){
    case MAPAINT_8_CONN_SE:
      break;

    case MAPAINT_4_CONN_SE:
      break;

    case MAPAINT_CIRCLE_SE:
      break;

    case MAPAINT_SQUARE_SE:
      break;
    }
  }
  else {
    seObj = NULL;
  }

  /* apply the required operation */
  switch( opType ){
  case MAPAINT_OP_ERODE:

    break;

  case MAPAINT_OP_DILATE:
    break;

  case MAPAINT_OP_OPEN:
    break;

  case MAPAINT_OP_CLOSE:
    break;

  default:
    break;
  }

  /* push existing onto undo stack */

  return;
}

void warpPostProcSE_Cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  warpGlobals.seType = (MAPaintStructElemType) client_data;
  return;
}

void morphSE_RadiusCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	slider = w;
    
  /* get the parent slider */
  while( strcmp(XtName(slider), "struct_elem_radius") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }
  warpGlobals.seElemRadius = HGU_XmGetSliderValue(slider);

  return;
}

static MenuItem struct_elem_itemsP[] = {   /* structuring element menu items */
  {"8-connected", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpPostProcSE_Cb, (XtPointer) MAPAINT_8_CONN_SE,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"4-connected", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpPostProcSE_Cb, (XtPointer) MAPAINT_4_CONN_SE,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"circle", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpPostProcSE_Cb, (XtPointer) MAPAINT_CIRCLE_SE,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"square", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpPostProcSE_Cb, (XtPointer) MAPAINT_SQUARE_SE,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

Widget createSignalPostProcPage(
  Widget	notebook)
{
  Widget	page, button, rc, option_menu, widget, slider;
  float		fval;

  /* major page for domain post-procesing - morphological, smoothing
     tracking, cleaning, review */
  /* first the morphological page */
  page = XtVaCreateManagedWidget("post_process_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("post_process_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMajorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_NONE);
  button = XtVaCreateManagedWidget("post_proc_morph_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);

  /* buttons for dilate, erode, open close,
     include undo since the canvas no longer has input */
  rc =  XtVaCreateManagedWidget("morph_post_proc_rc",
				xmRowColumnWidgetClass, 	page,
				XmNpacking,	XmPACK_COLUMN,
				XmNorientation,	XmHORIZONTAL,
				XmNnumColumns,	1,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				NULL);
  button = XtVaCreateManagedWidget("Dilate", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcMorphCb,
		(XtPointer) MAPAINT_OP_ERODE);
  button = XtVaCreateManagedWidget("Erode", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcMorphCb,
		(XtPointer) MAPAINT_OP_DILATE);
  button = XtVaCreateManagedWidget("Open", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcMorphCb,
		(XtPointer) MAPAINT_OP_OPEN);
  button = XtVaCreateManagedWidget("Close", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcMorphCb,
		(XtPointer) MAPAINT_OP_CLOSE);
  button = XtVaCreateManagedWidget(" ", xmLabelWidgetClass,
				   rc, NULL);
  button = XtVaCreateManagedWidget(" ", xmLabelWidgetClass,
				   rc, NULL);
  button = XtVaCreateManagedWidget("Undo", xmPushButtonWidgetClass,
				   rc, NULL);

  /* option menu and slider for the structuring element */
  option_menu = HGU_XmBuildMenu(page, XmMENU_OPTION, "struct_elem", 0,
				XmTEAR_OFF_DISABLED, struct_elem_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		rc,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);
  widget = option_menu;

  fval = 1.0;
  slider = HGU_XmCreateHorizontalSlider("struct_elem_radius", page,
					fval, 1.0, 20.0, 0,
					morphSE_RadiusCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  return page;
}
