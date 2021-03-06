#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _objPropsDialog_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         objPropsDialog.c
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

static Widget obj_props_dialog;

void obj_props_cb(
Widget w, XtPointer client_data, XtPointer call_data)
{
  XtManageChild( obj_props_dialog );
  XtPopup( XtParent(obj_props_dialog), XtGrabNone );
  return;
}

void setup_obj_props_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzPixelV		bckgrnd, pVal;
  float			val;
  Widget		widget;
  char			str_buf[32];
  XmString		xmstr;
  WlzPlaneDomain	*planedom;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check object */
  if( globals.orig_obj == NULL ){
    return;
  }

  /* get the backgound value and set the display */
  if( (widget = XtNameToWidget(obj_props_dialog, "*background")) ){
    bckgrnd = WlzGetBackground(globals.orig_obj, &errNum);
    WlzValueConvertPixel(&pVal, bckgrnd, WLZ_GREY_FLOAT);
    val = pVal.v.flv;
    HGU_XmSetSliderValue(widget, val);
  }

  /* set the voxel sizes */
  planedom = globals.orig_obj->domain.p;
  if( (widget = XtNameToWidget(obj_props_dialog, "*x_size")) ){
    HGU_XmSetSliderValue( widget, planedom->voxel_size[0] );
  }
  if( (widget = XtNameToWidget(obj_props_dialog, "*y_size")) ){
    HGU_XmSetSliderValue( widget, planedom->voxel_size[1] );
  }
  if( (widget = XtNameToWidget(obj_props_dialog, "*z_size")) ){
    HGU_XmSetSliderValue( widget, planedom->voxel_size[2] );
  }

  /* get the object type */
  if( (widget = XtNameToWidget(obj_props_dialog, "*origObjType")) ){
    sprintf(str_buf, "%s", WlzStringFromObjTypeValue(globals.origObjType, NULL));
    xmstr = XmStringCreateSimple(str_buf);
    XtVaSetValues(widget, XmNlabelString, xmstr, NULL);
    XmStringFree(xmstr);
  }

  /* set the bounding box info */
  if( (widget = XtNameToWidget(obj_props_dialog, "*planes_vals")) ){
    sprintf(str_buf, "(%d, %d)", planedom->plane1, planedom->lastpl);
    xmstr = XmStringCreateSimple(str_buf);
    XtVaSetValues(widget, XmNlabelString, xmstr, NULL);
    XmStringFree(xmstr);
  }
  if( (widget = XtNameToWidget(obj_props_dialog, "*lines_vals")) ){
    sprintf(str_buf, "(%d, %d)", planedom->line1, planedom->lastln);
    xmstr = XmStringCreateSimple(str_buf);
    XtVaSetValues(widget, XmNlabelString, xmstr, NULL);
    XmStringFree(xmstr);
  }
  if( (widget = XtNameToWidget(obj_props_dialog, "*kols_vals")) ){
    sprintf(str_buf, "(%d, %d)", planedom->kol1, planedom->lastkl);
    xmstr = XmStringCreateSimple(str_buf);
    XtVaSetValues(widget, XmNlabelString, xmstr, NULL);
    XmStringFree(xmstr);
  }
  
  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "setup_obj_props_cb", errNum);
  }
  return;
}

static void obj_props_set_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzPixelV		new_background, old_background, pVal;
  Widget		widget;
  WlzPlaneDomain	*planedom1, *planedom2;
  WlzPixelV		min, max, Min, Max;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check object */
  if( (globals.orig_obj == NULL) || (globals.obj == NULL) ){
    return;
  }

  /* get new backgound value, if changed then reset the image values */
  if( !(widget = XtNameToWidget(obj_props_dialog, "*background")) ){
    return;
  }
  new_background.type = WLZ_GREY_INT;
  new_background.v.inv = (int) HGU_XmGetSliderValue( widget );

  old_background = WlzGetBackground(globals.orig_obj, &errNum);
  WlzValueConvertPixel(&pVal, old_background, WLZ_GREY_INT);
  if( pVal.v.inv != new_background.v.inv ){
    Widget	toggle;
    WlzSetBackground(globals.orig_obj, new_background);
    if( globals.obj != NULL ){
      WlzFreeObj( globals.obj );
    }
    if((globals.obj = WlzConvertPix( globals.orig_obj, WLZ_GREY_UBYTE,
				    &errNum ))){
      globals.obj = WlzAssignObject(globals.obj, NULL);

      min.type = WLZ_GREY_UBYTE;
      max.type = WLZ_GREY_UBYTE;
      Min.type = WLZ_GREY_UBYTE;
      Max.type = WLZ_GREY_UBYTE;
      min.v.ubv = 0;
      max.v.ubv = 255;
      Min.v.ubv = globals.cmapstruct->gmin;
      Max.v.ubv = globals.cmapstruct->gmax;
      WlzGreySetRange(globals.obj, min, max, Min, Max, 0);

      /* also convert the background values */
      min = WlzGetBackground(globals.orig_obj, NULL);
      WlzValueConvertPixel(&min, min, WLZ_GREY_INT);
      max.type = WLZ_GREY_INT;
      max.v.inv = ((min.v.inv * (Max.v.ubv - Min.v.ubv)) / 255) + Min.v.ubv;
      WlzSetBackground(globals.obj, max);

      /* fill blank planes here - should be a resource option */
      if( (toggle = XtNameToWidget(globals.topl,
				   "*read_obj_dialog*fill_blanks")) ){
	Boolean	fill_blanks, min_domain;
	XtVaGetValues(toggle, XmNset, &fill_blanks, NULL);
	if( fill_blanks ){
	  if( (toggle = XtNameToWidget(globals.topl,
				       "*read_obj_dialog*min_domain")) ){
	    XtVaGetValues(toggle, XmNset, &min_domain, NULL);
	    if( min_domain ){
	      WlzFillBlankPlanes(globals.obj, 1);
	    } else {
	      WlzFillBlankPlanes(globals.obj, 0);
	    }
	  }
	  else {
	    WlzFillBlankPlanes(globals.obj, 1);
	  }
	}
      }
    }
  }

  if( errNum == WLZ_ERR_NONE ){
    /* set the voxel sizes on both reference and painted objects */
    planedom1 = (WlzPlaneDomain *) globals.orig_obj->domain.p;
    planedom2 = (WlzPlaneDomain *) globals.obj->domain.p;
    if( (widget = XtNameToWidget(obj_props_dialog, "*x_size")) ){
      planedom1->voxel_size[0] = HGU_XmGetSliderValue( widget );
      planedom2->voxel_size[0] = planedom1->voxel_size[0];
    }
    if( (widget = XtNameToWidget(obj_props_dialog, "*y_size")) ){
      planedom1->voxel_size[1] = HGU_XmGetSliderValue( widget );
      planedom2->voxel_size[1] = planedom1->voxel_size[1];
    }
    if( (widget = XtNameToWidget(obj_props_dialog, "*z_size")) ){
      planedom1->voxel_size[2] = HGU_XmGetSliderValue( widget );
      planedom2->voxel_size[2] = planedom1->voxel_size[2];
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "obj_props_set_cb", errNum);
  }
  return;
}

void moreFactsCb(
  Widget w,
  XtPointer client_data,
  XtPointer call_data)
{
  char 	*factsStr;

  /* check for reference object */
  if( globals.orig_obj == NULL ){
    return;
  }

  /* create a temporary object for 2D case */
  if( globals.origObjType == WLZ_2D_DOMAINOBJ ){
    WlzObject *tmpObj;

    tmpObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, 
			 globals.orig_obj->domain.p->domains[0],
			 globals.orig_obj->values.vox->values[0],
			 globals.orig_obj->plist,
			 NULL, NULL);
     WlzObjectFacts(tmpObj, NULL, &factsStr, 0);
  }
  else {
    WlzObjectFacts(globals.orig_obj, NULL, &factsStr, 0);
  }
  HGU_XmUserMessage(w, factsStr, XmDIALOG_FULL_APPLICATION_MODAL);
  AlcFree(factsStr);

  return;
}


static ActionAreaItem   obj_props_dialog_actions[] = {
{"Set",		NULL,		NULL},
{"More",	moreFactsCb,		NULL},
{"Dismiss",     NULL,           NULL},
{"Help",        NULL,           NULL},
};

Widget create_obj_props_dialog(
  Widget parent)
{
  Widget	dialog, control, form;
  Widget	frame1, frame2, title;
  Widget	slider, widg1, widg2;
  float		val, minval, maxval;

  dialog = HGU_XmCreateStdDialog(parent, "object_props_dialog",
				 xmFormWidgetClass,
				 obj_props_dialog_actions, 4);

  if( (widg1 = XtNameToWidget(dialog, "*.Set")) != NULL ){
    XtAddCallback(widg1, XmNactivateCallback, obj_props_set_cb,
		  XtParent(dialog));
  }

  if( (widg1 = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widg1, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  control = XtNameToWidget( dialog, "*.control" );

  widg1 = XtVaCreateManagedWidget("origObjTypeLabel", xmLabelGadgetClass,
				  control,
				  XmNborderWidth,	0,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_FORM,
				  NULL);
  widg1 = XtVaCreateManagedWidget("origObjType", xmLabelGadgetClass, control,
				  XmNborderWidth,	0,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_WIDGET,
				  XmNleftWidget,	widg1,
				  XmNrightAttachment,	XmATTACH_FORM,
				  NULL);

  frame1 = XtVaCreateManagedWidget("frame1", xmFrameWidgetClass,
				   control,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	widg1,
				   XmNleftAttachment,	XmATTACH_FORM,
				   XmNrightAttachment,	XmATTACH_FORM,
				   NULL);
  title = XtVaCreateManagedWidget("title1", xmLabelGadgetClass,
				  frame1,
				  XmNborderWidth,	0,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  NULL);
  form = XtVaCreateManagedWidget("form1", xmFormWidgetClass, frame1,
				 XmNborderWidth,	0,
				 XmNchildType,		XmFRAME_WORKAREA_CHILD,
				 NULL);
				 
  widg1 = XtVaCreateManagedWidget("planes", xmLabelGadgetClass, form,
				  XmNborderWidth,	0,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_FORM,
				  NULL);
  widg2 = XtVaCreateManagedWidget("planes_vals", xmLabelGadgetClass, form,
				  XmNborderWidth,	0,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_WIDGET,
				  XmNleftWidget,	widg1,
				  XmNrightAttachment,	XmATTACH_FORM,
				  NULL);

  widg1 = XtVaCreateManagedWidget("lines", xmLabelGadgetClass, form,
				  XmNborderWidth,	0,
				  XmNtopAttachment,	XmATTACH_WIDGET,
				  XmNtopWidget,		widg1,
				  XmNleftAttachment,	XmATTACH_FORM,
				  NULL);
  widg2 = XtVaCreateManagedWidget("lines_vals", xmLabelGadgetClass, form,
				  XmNborderWidth,	0,
				  XmNtopAttachment,	XmATTACH_WIDGET,
				  XmNtopWidget,		widg2,
				  XmNleftAttachment,	XmATTACH_WIDGET,
				  XmNleftWidget,	widg1,
				  XmNrightAttachment,	XmATTACH_FORM,
				  NULL);

 widg1 = XtVaCreateManagedWidget("kols", xmLabelGadgetClass, form,
				  XmNborderWidth,	0,
				  XmNtopAttachment,	XmATTACH_WIDGET,
				  XmNtopWidget,		widg1,
				  XmNleftAttachment,	XmATTACH_FORM,
				  NULL);
  widg2 = XtVaCreateManagedWidget("kols_vals", xmLabelGadgetClass, form,
				  XmNborderWidth,	0,
				  XmNtopAttachment,	XmATTACH_WIDGET,
				  XmNtopWidget,		widg2,
				  XmNleftAttachment,	XmATTACH_WIDGET,
				  XmNleftWidget,	widg1,
				  XmNrightAttachment,	XmATTACH_FORM,
				  NULL);


  frame2 = XtVaCreateManagedWidget("frame2", xmFrameWidgetClass,
				   control,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	frame1,
				   XmNleftAttachment,	XmATTACH_FORM,
				   XmNrightAttachment,	XmATTACH_FORM,
				   NULL);
  title = XtVaCreateManagedWidget("title2", xmLabelGadgetClass,
				  frame2,
				  XmNborderWidth,	0,
				  XmNchildType,		XmFRAME_TITLE_CHILD,
				  NULL);
  form = XtVaCreateManagedWidget("form2", xmRowColumnWidgetClass, frame2,
				 XmNborderWidth,	0,
				 XmNchildType,		XmFRAME_WORKAREA_CHILD,
				 
				 NULL);
				 
  val = 0.0;
  minval = 0.0;
  maxval = 255.0;
  slider = HGU_XmCreateHorizontalSlider("background", form,
					val, minval, maxval, 0,
					NULL, NULL);

  val = 1.0;
  minval = 0.1;
  maxval = 100.0;
  slider = HGU_XmCreateHorizontalSlider("x_size", form,
					val, minval, maxval, 2,
					NULL, NULL);
  val = 1.0;
  minval = 0.1;
  maxval = 100.0;
  slider = HGU_XmCreateHorizontalSlider("y_size", form,
					val, minval, maxval, 2,
					NULL, NULL);
  val = 1.0;
  minval = 0.1;
  maxval = 100.0;
  slider = HGU_XmCreateHorizontalSlider("z_size", form,
					val, minval, maxval, 2,
					NULL, NULL);
  return( dialog );
}

void obj_props_dialog_init(
  Widget	topl)
{
  /* setup the dialog */
  obj_props_dialog = create_obj_props_dialog( topl );
}  
