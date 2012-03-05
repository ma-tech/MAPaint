#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAWarpSignalPostProcPage_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAWarpSignalPostProcPage.c
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

#include <MAPaint.h>
#include <MAWarp.h>

#include <Xm/XmP.h>

typedef enum _MAPaintOperatorType{
  MAPAINT_OP_ERODE,
  MAPAINT_OP_DILATE,
  MAPAINT_OP_OPEN,
  MAPAINT_OP_CLOSE,
  MAPAINT_OP_DUMMY
}MAPaintOperatorType;

static AlcDLPList *sgnlPostProcUndoList=NULL;
static AlcDLPList *sgnlPostProcRedoList=NULL;

void postProcMajorPageSelectCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  /* we have shifted to post-processing,
     clear undo/redo lists */
  if( sgnlPostProcUndoList ){
    /* free list and all entries */
    AlcDLPListFree(sgnlPostProcUndoList);
  }
  /* create the list */
  sgnlPostProcUndoList = AlcDLPListNew(NULL);
  
  if( sgnlPostProcRedoList ){
    /* free list and all entries */
    AlcDLPListFree(sgnlPostProcRedoList);
  }
  /* create the list */
  sgnlPostProcRedoList = AlcDLPListNew(NULL);
  
  return;
}

void WlzObjListItemFree(
  void *entry)
{
  if( entry ){
    WlzFreeObj((WlzObject *) entry);
  }
  return;
}

void WlzObjListPush(
  AlcDLPList	*list,
  WlzObject	*obj)
{
  /* check list */
  if( list ){
    /* check object */
    if( obj ){
      AlcDLPItem *item;
      if( WlzAssignObject(obj, NULL) ){
	/* add to head of list */
	item = AlcDLPItemNew((void *) obj, WlzObjListItemFree, NULL);
	AlcDLPItemInsert(list, NULL, item);
      }
    }
  }

  return;
}

void WlzObjListPop(
  AlcDLPList	*list)
{
  /* check list */
  if( list ){
    if( list->head ){
      AlcDLPItemUnlink(list, list->head, 1, NULL);
    }
  }

  return;
}

WlzObject *WlzObjListObj(
  AlcDLPList	*list)
{
  if( list ){
    if( list->head ){
      return (WlzObject *) list->head->entry;
    }
  }
  return NULL;
}

void WlzObjListClear(
  AlcDLPList	*list)
{
  if( list ){
    while( list->head ){
      AlcDLPItemUnlink(list, list->head, 1, NULL);
    }
  }

  return;
}

static Widget sizeSelectOptionMenu;

void sizeSelectTypeCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	option_menu = sizeSelectOptionMenu;
  /* for testing option menu */
  int		numChildren;
  Widget	button, *children;
  int		xpos, ypos;
  WlzUInt	height, width, borderWidth, depth;
  Dimension	gHeight, thickness;
  Pixmap	pixmap;
  Window	root;
  Status	status;
  XmFontList	fl;
  XFontStruct	*font;

  /* some code here to pick apart the option menu */
  if((button = XtNameToWidget(option_menu, "*xmCascadeButtonGadget"))){
    fprintf(stderr, "CascadeButtonGadget found\n");
  }
  if((button = XtNameToWidget(option_menu, "*xmCascadeButton"))){
    fprintf(stderr, "CascadeButton found\n");
  }
  XtVaGetValues(option_menu,
		XmNnumChildren, &numChildren,
		XmNchildren, &children,
		XmNmarginHeight, &gHeight,
		NULL);
  fprintf(stderr, "option_menu: %d children\n", numChildren);
  fprintf(stderr, "option_menu: marginHeight: %d\n", gHeight);
  fprintf(stderr, "child[0]: %s\n", XtName(children[0]));
  fprintf(stderr, "child[1]: %s\n", XtName(children[1]));
  fprintf(stderr, "OptionButton is %s\n", XtIsWidget(children[1])?
	  "Widget":"Gadget");
  if( XtClass(children[1]) == xmCascadeButtonGadgetClass ){
    fprintf(stderr, "CascadeButtonGadget found\n");
  }
  XtVaGetValues(children[1],
		XmNheight, &gHeight,
		XmNlabelPixmap, &pixmap,
		XmNfontList,	&fl,
		XmNshadowThickness, &thickness,
		NULL);
  fprintf(stderr, "OptionButton: height: %d\n", gHeight);
  fprintf(stderr, "OptionButton: cascadePixmap: Ox%x\n", (unsigned int) pixmap);
  if( fl ){
    XmeRenderTableGetDefaultFont(fl, &font);
/*    _XmFontListGetDefaultFont(fl, &font);*/
    fprintf(stderr, "font hieghts: %d, %d\n", font->ascent, font->descent);
  }
  fprintf(stderr, "OptionButton: shadowThickness: %d\n", thickness);

  switch(status = XGetGeometry(globals.dpy, pixmap, &root,
			       &xpos, &ypos, &width, &height,
			       &borderWidth, &depth)){
  case BadDrawable:
    fprintf(stderr, "returns BadDrawable\n");
    break;
  case BadWindow:
    fprintf(stderr, "returns BadWindow\n");
    break;
  default:
    fprintf(stderr, "returns %d\n", status);
    break;
  }
  fprintf(stderr, "OptionButton pixmap: %d, %d, %d, %d, %d, %d\n",
	  xpos, ypos, width, height, borderWidth, depth);

  XtVaSetValues(children[0], XmNheight, 20, NULL);
  XtVaSetValues(children[1], XmNheight, 20, NULL);
  XtVaSetValues(option_menu, XmNheight, 26, NULL);

  return;
}

void sizeSelectAreaCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}

void sizeSelectApplyCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}

void smoothSelectTypeCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}

void smoothFilterWidthCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}

void smoothApplyCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}

  
void postProcUndoCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzObject	*tmpObj;
  /* free existing domain and replace with top
     of undo stack, do not delete if it is the last domain
  */
  if((tmpObj = WlzObjListObj(sgnlPostProcUndoList))){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
    if( warpGlobals.sgnlObj ){
      WlzObjListPush(sgnlPostProcRedoList, warpGlobals.sgnlObj);
      WlzFreeObj(warpGlobals.sgnlObj);
    }
    warpGlobals.sgnlObj  = WlzAssignObject(tmpObj, NULL);
    WlzObjListPop(sgnlPostProcUndoList);
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
    XFlush(XtDisplayOfObject(w));
  }
  
  return;
}

void postProcRedoCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzObject	*tmpObj;
  /* push current to undo stack and pop redo object */

  if((tmpObj = WlzObjListObj(sgnlPostProcRedoList))){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
    if( warpGlobals.sgnlObj ){
      WlzObjListPush(sgnlPostProcUndoList, warpGlobals.sgnlObj);
      WlzFreeObj(warpGlobals.sgnlObj);
    }
    warpGlobals.sgnlObj  = WlzAssignObject(tmpObj, NULL);
    WlzObjListPop(sgnlPostProcRedoList);
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
    XFlush(XtDisplayOfObject(w));
  }
  
  return;
}

void postProcMorphCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintOperatorType	opType=(MAPaintOperatorType) client_data;
  WlzObject		*seObj, *newDomain=NULL, *tmpObj;
  WlzConnectType	conn;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check signal object */
  if( warpGlobals.sgnlObj == NULL ){
    return;
  }

  /* build the structuring element */
  seObj = NULL;
  conn = WLZ_4_CONNECTED;
  switch( warpGlobals.seType ){
  case MAPAINT_8_CONN_SE:
    conn = WLZ_8_CONNECTED;
    break;

  case MAPAINT_4_CONN_SE:
    conn = WLZ_4_CONNECTED;
    break;

  case MAPAINT_CIRCLE_SE:
    if( warpGlobals.seElemRadius != 1 ){
      seObj = WlzMakeCircleObject((double) warpGlobals.seElemRadius,
				  0.0, 0.0, &errNum);
    }
    break;

  case MAPAINT_SQUARE_SE:
    if( warpGlobals.seElemRadius != 1 ){
      seObj = WlzMakeRectangleObject((double) warpGlobals.seElemRadius,
				     (double) warpGlobals.seElemRadius,
				     0.0, 0.0, &errNum);
    }
    break;

  case MAPAINT_SPHERE_SE:
  case MAPAINT_CUBE_SE:
  default:
    return;
  }

  /* apply the required operation */
  switch( opType ){
  case MAPAINT_OP_ERODE:
    if( seObj ){
      newDomain = WlzStructErosion(warpGlobals.sgnlObj, seObj, &errNum);
    }
    else {
      newDomain = WlzErosion(warpGlobals.sgnlObj, conn, &errNum);
    }      
    break;

  case MAPAINT_OP_DILATE:
    if( seObj ){
      newDomain = WlzStructDilation(warpGlobals.sgnlObj, seObj, &errNum);
    }
    else {
      newDomain = WlzDilation(warpGlobals.sgnlObj, conn, &errNum);
    }      
    break;

  case MAPAINT_OP_OPEN:
    if( seObj ){
      if((tmpObj = WlzStructErosion(warpGlobals.sgnlObj, seObj, &errNum))){
	newDomain = WlzStructDilation(tmpObj, seObj, &errNum);
      }
      else {
	newDomain = NULL;
      }
    }
    else {
      if((tmpObj = WlzErosion(warpGlobals.sgnlObj, conn, &errNum))){
	newDomain = WlzDilation(tmpObj, conn, &errNum);
      }
      else {
	newDomain = NULL;
      }
    }
    if( tmpObj ){
      WlzFreeObj(tmpObj);
    }
    break;

  case MAPAINT_OP_CLOSE:
    if( seObj ){
      if((tmpObj = WlzStructDilation(warpGlobals.sgnlObj, seObj, &errNum))){
	newDomain = WlzStructErosion(tmpObj, seObj, &errNum);
      }
      else {
	newDomain = NULL;
      }
    }
    else {
      if((tmpObj = WlzDilation(warpGlobals.sgnlObj, conn, &errNum))){
	newDomain = WlzErosion(tmpObj, conn, &errNum);
      }
      else {
	newDomain = NULL;
      }
    }      
    if( tmpObj ){
      WlzFreeObj(tmpObj);
    }
    break;

  default:
    break;
  }

  /* push existing onto undo stack
     note the domain is not incremented */
  /* should have an independent undo and redo stack here */
  if( newDomain ){
    WlzObjListPush(sgnlPostProcUndoList, warpGlobals.sgnlObj);
    WlzObjListClear(sgnlPostProcRedoList);
    WlzFreeObj(warpGlobals.sgnlObj);
    warpGlobals.sgnlObj = WlzAssignObject(newDomain, &errNum);
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
    XFlush(XtDisplayOfObject(w));
  }

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
  {NULL},
};

static MenuItem size_select_itemsP[] = {   /* size select menu items */
  {"Large", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   sizeSelectTypeCb, (XtPointer) 0,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Small", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   sizeSelectTypeCb, (XtPointer) 1,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {NULL},
};

static MenuItem smooth_filter_itemsP[] = {   /* smooth filter menu items */
  {"Simple 1-2-1", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   smoothSelectTypeCb, (XtPointer) 0,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Spline", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   smoothSelectTypeCb, (XtPointer) 1,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {NULL},
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
  XtAddCallback(button, XmNactivateCallback, postProcMajorPageSelectCb,
		(XtPointer) NULL);
  button = XtVaCreateManagedWidget("post_proc_morph_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);

  /* option menu and slider for the structuring element */
  option_menu = HGU_XmBuildMenu(page, XmMENU_OPTION, "struct_elem", 0,
				XmTEAR_OFF_DISABLED, struct_elem_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
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
  widget = slider;

  /* buttons for dilate, erode, open close,
     include undo since the canvas no longer has input */
  rc =  XtVaCreateManagedWidget("morph_post_proc_rc1",
				xmRowColumnWidgetClass, 	page,
				XmNpacking,	XmPACK_COLUMN,
				XmNorientation,	XmHORIZONTAL,
				XmNentryAlignment, XmALIGNMENT_CENTER,
				XmNnumColumns,	1,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, widget,
				XmNleftAttachment, XmATTACH_FORM,
				NULL);
  button = XtVaCreateManagedWidget("Dilate", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcMorphCb,
		(XtPointer) MAPAINT_OP_DILATE);
  button = XtVaCreateManagedWidget("Erode", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcMorphCb,
		(XtPointer) MAPAINT_OP_ERODE);
  button = XtVaCreateManagedWidget("Open", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcMorphCb,
		(XtPointer) MAPAINT_OP_OPEN);
  button = XtVaCreateManagedWidget("Close", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcMorphCb,
		(XtPointer) MAPAINT_OP_CLOSE);

  rc =  XtVaCreateManagedWidget("morph_post_proc_rc2",
				xmRowColumnWidgetClass, 	page,
				XmNpacking,	XmPACK_COLUMN,
				XmNorientation,	XmHORIZONTAL,
				XmNentryAlignment, XmALIGNMENT_CENTER,
				XmNnumColumns,	1,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, widget,
				XmNrightAttachment, XmATTACH_FORM,
				NULL);
  button = XtVaCreateManagedWidget("Undo", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcUndoCb,
		NULL);
  button = XtVaCreateManagedWidget("Redo", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcRedoCb,
		NULL);

  /* minor page for smoothing, cleaning, review */
  page = XtVaCreateManagedWidget("post_process_smooth_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("post_proc_smooth_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);

  /* controls for size-select */
  option_menu = HGU_XmBuildMenu(page, XmMENU_OPTION, "size_select_type", 0,
				XmTEAR_OFF_DISABLED, size_select_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);
  widget = option_menu;
  sizeSelectOptionMenu = option_menu;

  fval = 1.0;
  slider = HGU_XmCreateHorizontalSlider("size_select_area", page,
					fval, 1.0, 20.0, 0,
					sizeSelectAreaCb, NULL);
  XtVaSetValues(slider,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		widget,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  option_menu = HGU_XmBuildMenu(page, XmMENU_OPTION, "smooth_filter_type", 0,
				XmTEAR_OFF_DISABLED, smooth_filter_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);

  fval = 1.0;
  slider = HGU_XmCreateHorizontalSlider("smooth_filter_width", page,
					fval, 1.0, 20.0, 0,
					smoothFilterWidthCb, NULL);
  XtVaSetValues(slider,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		option_menu,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = option_menu;

  rc =  XtVaCreateManagedWidget("smooth_post_proc_rc1",
				xmRowColumnWidgetClass, 	page,
				XmNpacking,	XmPACK_COLUMN,
				XmNorientation,	XmHORIZONTAL,
				XmNentryAlignment, XmALIGNMENT_CENTER,
				XmNnumColumns,	1,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, widget,
				XmNleftAttachment, XmATTACH_FORM,
				NULL);

  button = XtVaCreateManagedWidget("size_select", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, sizeSelectApplyCb, NULL);

  button = XtVaCreateManagedWidget("smooth", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, smoothApplyCb, NULL);

  rc =  XtVaCreateManagedWidget("morph_post_proc_rc2",
				xmRowColumnWidgetClass, 	page,
				XmNpacking,	XmPACK_COLUMN,
				XmNorientation,	XmHORIZONTAL,
				XmNentryAlignment, XmALIGNMENT_CENTER,
				XmNnumColumns,	1,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, widget,
				XmNrightAttachment, XmATTACH_FORM,
				NULL);
  button = XtVaCreateManagedWidget("undo", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcUndoCb, NULL);
  button = XtVaCreateManagedWidget("Redo", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, postProcRedoCb,
		NULL);

  /* controls for domain smooth */

  /* minor page for density filtering */
  page = XtVaCreateManagedWidget("post_process_density_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("post_proc_density_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMINOR_TAB,
				   NULL);

  /* controls for density filtering and thresholding */

  return page;
}
