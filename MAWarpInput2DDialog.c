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
*   File       :   MAWarpInput2DDialog.c				*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Jun 11 14:13:51 1999				*
*   $Revision$							*
*   $Name$			*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <MAPaint.h>
#include <MAWarp.h>

extern void warpSetSignalDomain(void);

extern void warpReadSignalPopupCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void view_canvas_highlight(
  ThreeDViewStruct	*view_struct,
  Boolean		highlight);

static Widget create2DWarpDialog(Widget parent,
				 ThreeDViewStruct *view_struct);

MAPaintWarp2DStruct warpGlobals;
Widget warp_read_src_dialog=NULL;
Widget warp_read_sgnl_dialog=NULL;
const char *warpErrStr;
char warpErrBuf[256];

void warpMeshMethodCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  WlzMeshGenMethod	meshMthd=(WlzMeshGenMethod) client_data;
  WlzErrorNum		errNum;

  warpGlobals.meshMthd = meshMthd;

  /* reset the source mesh */
  if( warpGlobals.meshTr ){
    WlzMeshFreeTransform(warpGlobals.meshTr);
  }
  warpGlobals.meshTr = WlzMeshFromObj(warpGlobals.src.obj,
				      warpGlobals.meshMthd,
				      warpGlobals.meshMinDst,
				      warpGlobals.meshMaxDst,
				      &errNum);

  /* call all callbacks for now */
  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback, call_data);
  XtCallCallbacks(warpGlobals.src.canvas, XmNexposeCallback, call_data);
  XtCallCallbacks(warpGlobals.ovly.canvas, XmNexposeCallback, call_data);

  return;
}

void warpMeshFunctionCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  WlzFnType		wlzFnType=(WlzFnType) client_data;
  WlzErrorNum		errNum;

  warpGlobals.wlzFnType = wlzFnType;

  return;
}

void warpAffineTypeCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  WlzTransformType	type=(WlzTransformType) client_data;
  WlzErrorNum		errNum;

  warpGlobals.affineType = type;

  return;
}

void warpMeshMinDistCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	slider = w;
  WlzErrorNum	errNum;
  
  /* get the parent slider */
  while( strcmp(XtName(slider), "mesh_min_dist") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }
  warpGlobals.meshMinDst = (int) HGU_XmGetSliderValue( slider );

  /* reset the source mesh */
  if( warpGlobals.meshTr ){
    WlzMeshFreeTransform(warpGlobals.meshTr);
  }
  warpGlobals.meshTr = WlzMeshFromObj(warpGlobals.src.obj,
				      warpGlobals.meshMthd,
				      warpGlobals.meshMinDst,
				      warpGlobals.meshMaxDst,
				      &errNum);

  /* call all callbacks for now */
  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback, call_data);
  XtCallCallbacks(warpGlobals.src.canvas, XmNexposeCallback, call_data);
  XtCallCallbacks(warpGlobals.ovly.canvas, XmNexposeCallback, call_data);

  return;
}

void warpRemeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  /* only if there is a source object */
  if( warpGlobals.src.obj == NULL ){
    return;
  }

  /* reset the source mesh */
  if( warpGlobals.meshTr ){
    WlzMeshFreeTransform(warpGlobals.meshTr);
  }
  warpGlobals.meshTr = WlzMeshFromObj(warpGlobals.src.obj,
				      warpGlobals.meshMthd,
				      warpGlobals.meshMinDst,
				      warpGlobals.meshMaxDst,
				      NULL);
  warpSetOvlyObject();
  warpSetOvlyXImages(&(warpGlobals.ovly), 1);
  warpSetOvlyXImage(&(warpGlobals.ovly));
  warpCanvasExposeCb(warpGlobals.ovly.canvas,
		     (XtPointer) &(warpGlobals.ovly),
		     call_data);
  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		  call_data);
  XtCallCallbacks(warpGlobals.src.canvas, XmNexposeCallback,
		  call_data);

  return;
}

void warpMeshMaxDistCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	slider = w;
  WlzErrorNum	errNum;
  
  /* get the parent slider */
  while( strcmp(XtName(slider), "mesh_max_dist") ){
    if( (slider = XtParent(slider)) == NULL )
      return;
  }
  warpGlobals.meshMaxDst = (int) HGU_XmGetSliderValue( slider );

  /* reset the source mesh */
  if( warpGlobals.meshTr ){
    WlzMeshFreeTransform(warpGlobals.meshTr);
  }
  warpGlobals.meshTr = WlzMeshFromObj(warpGlobals.src.obj,
				      warpGlobals.meshMthd,
				      warpGlobals.meshMinDst,
				      warpGlobals.meshMaxDst,
				      &errNum);

  /* call all callbacks for now */
  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback, call_data);
  XtCallCallbacks(warpGlobals.src.canvas, XmNexposeCallback, call_data);
  XtCallCallbacks(warpGlobals.ovly.canvas, XmNexposeCallback, call_data);

  return;
}


void recalcWarpProcObjCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  if( warpGlobals.sgnlProcObj ){
    WlzFreeObj(warpGlobals.sgnlProcObj);
    warpGlobals.sgnlProcObj = NULL;
  }
  if( warpGlobals.sgnlThreshObj ){
    WlzFreeObj(warpGlobals.sgnlThreshObj);
    warpGlobals.sgnlThreshObj = NULL;
  }

  /* set the signal domain */
  if( warpGlobals.sgnlObj ){
    warpCanvasExposeCb(w, (XtPointer) &(warpGlobals.sgnl), NULL);
  }
  warpSetSignalDomain();
  if( warpGlobals.sgnlObj ){
    warpDisplayDomain(&(warpGlobals.sgnl), warpGlobals.sgnlObj, 1);
  }

  return;
}


void warpReadSourceCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzObject		*obj, *obj1;
  WlzIBox2		cutBox;
  WlzGreyType		greyType;
  FILE			*fp;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  Widget		scrw;

  /* check we can get the object */
  if( (obj = WlzXmReadExtFFObject(warp_read_src_dialog,
				  cbs,
				  &(warpGlobals.srcFileType),
				  &errNum)) == NULL ){
    return;
  }
  if( warpGlobals.srcFile ){
    AlcFree(warpGlobals.srcFile);
  }
  warpGlobals.srcFile = HGU_XmGetFileStr(view_struct->dialog, cbs->value,
				 cbs->dir);

  /* check the object */
  if( errNum == WLZ_ERR_NONE ){
    if( obj ){
      switch( obj->type ){
      case WLZ_2D_DOMAINOBJ:
	/* set the source object */
	if( warpGlobals.src.obj ){
	  WlzFreeObj(warpGlobals.src.obj);
	}
	obj = WlzAssignObject(obj, &errNum);
	cutBox.xMin = obj->domain.i->kol1;
	cutBox.xMax = obj->domain.i->lastkl;
	cutBox.yMin = obj->domain.i->line1;
	cutBox.yMax = obj->domain.i->lastln;
	greyType = WlzGreyTableTypeToGreyType(obj->values.core->type,
					      &errNum);
	obj1 = WlzCutObjToBox2D(obj, cutBox, greyType,
				   0, 0.0, 0.0, &errNum);
	WlzFreeObj(obj);
	
	warpGlobals.srcXOffset = -obj1->domain.i->kol1;
	warpGlobals.srcYOffset = -obj1->domain.i->line1;
	obj = WlzAssignObject(obj1, &errNum);
	warpGlobals.src.obj = 
	  WlzAssignObject(WlzShiftObject(obj,
					 warpGlobals.srcXOffset,
					 warpGlobals.srcYOffset,
					 0, &errNum), NULL);
	WlzFreeObj(obj);
	obj = warpGlobals.src.obj;

	/* set the display magnification and display get frame
	   size & image size */
	if( scrw = XtParent(warpGlobals.src.canvas) ){
	  Dimension	scrwHeight, scrwWidth;
	  int		width, height;
	  double	scale;
	  XtVaGetValues(scrw, XmNheight, &scrwHeight,
			XmNwidth, &scrwWidth, NULL);
	  
	}
	
	warpSetXImage(&(warpGlobals.src));
	warpCanvasExposeCb(warpGlobals.src.canvas,
			   (XtPointer) &(warpGlobals.src),
			   call_data);

	/* reset the source mesh */
	if( warpGlobals.meshTr ){
	  WlzMeshFreeTransform(warpGlobals.meshTr);
	}
	warpGlobals.meshTr = WlzMeshFromObj(warpGlobals.src.obj,
					    warpGlobals.meshMthd,
					    warpGlobals.meshMinDst,
					    warpGlobals.meshMaxDst,
					    &errNum);

	/* set the overlay object */
	((WlzCompoundArray *) (warpGlobals.ovly.obj))->n = 2;
	((WlzCompoundArray *) (warpGlobals.ovly.obj))->o[1] =
	  WlzAssignObject(obj, &errNum);
	warpSetOvlyXImages(&(warpGlobals.ovly), 1);
	warpSetOvlyXImage(&(warpGlobals.ovly));
	warpCanvasExposeCb(warpGlobals.ovly.canvas,
			   (XtPointer) &(warpGlobals.ovly),
			   call_data);

	break;

      default:
	WlzFreeObj(obj);
	HGU_XmUserError(globals.topl,
			"Read Source Object:\n"
			"    Wrong object type, must be 2D,\n"
			"    please select another image file.",
			XmDIALOG_FULL_APPLICATION_MODAL);
	break;
      }
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "warpReadSourceCb", errNum);
  }
  return;
}

void warpReadSourcePopupCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  /* now use generic Ext format dialog */
  if( warp_read_src_dialog == NULL ){
    warp_read_src_dialog = WlzXmCreateExtFFObjectFSB(view_struct->dialog,
						     "warp_read_src_dialog",
						     warpReadSourceCb,
						     client_data);
  }

  XtManageChild(warp_read_src_dialog);
  PopupCallback(w, (XtPointer) XtParent(warp_read_src_dialog), NULL);
  XtCallCallbacks(warp_read_src_dialog, XmNmapCallback, call_data);

  return;
}

void warpInput2DCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	dialog, widget;

  if( dialog = createWarpInput2DDialog(globals.topl) ){
    if( widget = XtNameToWidget(globals.topl,
				"*options_menu*warp_input_2d") ){
      XtSetSensitive(widget, False);
    }
    XtManageChild(dialog);
  }

  return;
}

static void warpSetupCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmToggleButtonCallbackStruct
    *cbs = (XmToggleButtonCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Widget		dialog, button;

  dialog = view_struct->dialog;

  if( cbs->set == True ){
    if( paint_key == view_struct ){
      if( !warpGlobals.warp2DInteractDialog ){
	warpGlobals.warp2DInteractDialog =
/* 	  create2DWarpDialog(dialog, view_struct);*/
	  create2DWarpDialog(globals.topl, view_struct);

	/* add a new dismiss callback to destroy the warp dialog */
	if( button = XtNameToWidget(dialog, "*.dismiss") ){
	  XtAddCallback(button, XmNactivateCallback,
			destroy_cb, XtParent(warpGlobals.warp2DInteractDialog));
	}

      }
      XtManageChild(warpGlobals.warp2DInteractDialog);
    }
    else {
      XtVaSetValues(widget, XmNset, False, NULL);
    }
  }
  else {
    if( warpGlobals.warp2DInteractDialog ){
      XtUnmanageChild( warpGlobals.warp2DInteractDialog );
    }
  }

  return;
}

static void warpImportSignalCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Widget	shell, dialog, control, section_frame, controls_frame;
  Widget	warp_input_2d_frame, warp_sgnl_frame, warp_sgnl_controls_frame;
  Dimension	shellWidth, sectionWidth, signalWidth;

  /* get the frames */
  dialog = view_struct->dialog;
  shell = XtParent(dialog);
  control = XtNameToWidget(dialog, "*.control");
  controls_frame = XtNameToWidget( control, "*.controls_frame");
  section_frame = XtNameToWidget( control, "*.section_frame");
  warp_input_2d_frame = XtNameToWidget( control, "*.warp_input_2d_frame");
  warp_sgnl_frame = XtNameToWidget( control, "*.warp_sgnl_frame");
  warp_sgnl_controls_frame = XtNameToWidget( control,
					    "*.warp_sgnl_controls_frame");
  XtVaGetValues(shell, XmNwidth, &shellWidth, NULL);
  XtVaGetValues(section_frame, XmNwidth, &sectionWidth, NULL);

  /* if already managed collapse */
  if( XtIsManaged(warp_sgnl_frame) ){
    XtVaGetValues(warp_sgnl_frame, XmNwidth, &signalWidth, NULL);
    XtUnmanageChild(warp_sgnl_frame);
    XtUnmanageChild(warp_sgnl_controls_frame);

    /* reset attachments */
    XtVaSetValues(section_frame, XmNrightAttachment, XmATTACH_FORM, NULL);
    XtVaSetValues(controls_frame, XmNrightAttachment, XmATTACH_FORM, NULL);
    XtVaSetValues(warp_input_2d_frame,
		  XmNrightAttachment, XmATTACH_FORM, NULL);

    /* adjust width and manage the signal frame */
    XtVaSetValues(shell, XmNwidth, shellWidth - signalWidth - 5, NULL);
  }
  else {
    /* reset attachments */
    XtVaSetValues(section_frame, XmNrightAttachment, XmATTACH_NONE, NULL);
    XtVaSetValues(controls_frame, XmNrightAttachment, XmATTACH_NONE, NULL);
    XtVaSetValues(warp_input_2d_frame,
		  XmNrightAttachment, XmATTACH_NONE, NULL);

    /* reset widths */
    XtVaSetValues(section_frame, XmNwidth, sectionWidth, NULL);
    XtVaSetValues(controls_frame, XmNwidth, sectionWidth, NULL);
    XtVaSetValues(warp_input_2d_frame, XmNwidth, sectionWidth, NULL);

    /* adjust width and manage the signal frame */
    XtVaSetValues(shell, XmNwidth, shellWidth+500, NULL);
    XtManageChild(warp_sgnl_controls_frame);
    XtManageChild(warp_sgnl_frame);

  }

  return;
}

static void warpControlsCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Widget		shell, dialog, cntrlFrame, cntrlForm, magFncForm;
  Widget		warp_sgnl_frame;
  int			wasManaged;
  Dimension		shellHeight, shellWidth, viewFrameWidth;
  Dimension		cntrlFormHeight, cntrlFormWidth;
  WlzCompoundArray	*cobj;

  /* get the section viewer frames and unmanage everything */
  dialog = view_struct->dialog;
  shell = XtParent(dialog);
  cntrlFrame = XtNameToWidget(dialog, "*.warp_input_2d_frame");
  cntrlForm = XtNameToWidget(dialog, "*.warp_input_2d_form");
  warp_sgnl_frame = XtNameToWidget( dialog, "*.warp_sgnl_frame");
  XtVaGetValues(shell, XmNheight, &shellHeight, NULL);
  XtVaGetValues(cntrlForm, XmNheight, &cntrlFormHeight, NULL);
  XtVaSetValues(dialog, XmNdefaultPosition, False, NULL);
  if( XtIsManaged(cntrlForm) ){
    if( XtIsManaged(warp_sgnl_frame) ){
      warpImportSignalCb(widget, client_data, call_data);
    }
    wasManaged = True;
    XtUnmanageChild(cntrlForm);
    XtUnmanageChild(cntrlFrame);
    shellHeight -= cntrlFormHeight;
  }
  else {
    /* check if painting */
    if( paint_key ){
      HGU_XmUserError(view_struct->dialog,
		      "One of the view windows has been\n"
		      "selected for painting. Please quit\n"
		      "painting and try again.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      XtVaSetValues(widget, XmNset, False, NULL);
      return;
    }
    wasManaged = False;
    shellHeight += cntrlFormHeight;
  }

  /* re-manage everything and re-map the widget */
  if( wasManaged == False ){
    XtManageChild(cntrlForm);
  }
  XtManageChild(cntrlFrame);
  XtVaSetValues(shell, XmNheight, shellHeight, NULL);

  /* reset input callback and orientation controls sensitivity */
  XtSetSensitive(view_struct->controls, wasManaged);
  XtSetSensitive(view_struct->slider, wasManaged );
  setControlButtonsSensitive(view_struct, wasManaged);
  /*if( magFncForm = XtNameToWidget(dialog, "*.paint_function_row_col") ){
    XtSetSensitive(magFncForm, wasManaged);
    }*/

  if( wasManaged == False ){
    /* get the paint key */
    paint_key = view_struct;

    /* pause the 3D feedback display */
    HGUglwCanvasTbPause( globals.canvas );
    XtSetSensitive( globals.canvas, False );

    /* remove the normal callbacks - why not swap to painting? */
    XtRemoveCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		     client_data);
    XtAddCallback(view_struct->canvas, XmNinputCallback, canvas_warp_painting_cb,
		  client_data);
    installCurrentPaintTool(view_struct->canvas, view_struct);
    
    /* highlight the canvas and get domains */
    view_canvas_highlight( view_struct, True );
    getViewDomains(view_struct);

    /* set the destination object and clear the ximage */
    if( warpGlobals.dst.obj ){
      WlzFreeObj(warpGlobals.dst.obj);
    }
    warpGlobals.dst.obj =
      WlzAssignObject(WlzGetSectionFromObject(globals.orig_obj,
					      view_struct->wlzViewStr,
					      WLZ_INTERPOLATION_NEAREST,
					      NULL),
		      NULL);

    if( warpGlobals.dst.ximage ){
      AlcFree(warpGlobals.dst.ximage->data);
      warpGlobals.dst.ximage->data = NULL;
      XDestroyImage(warpGlobals.dst.ximage);
      warpGlobals.dst.ximage = NULL;
    }

    /* set the overlay compound object */
    if( warpGlobals.ovly.obj ){
      WlzFreeObj( warpGlobals.ovly.obj );
    }
    cobj = WlzMakeCompoundArray(WLZ_COMPOUND_ARR_2, 1, 2, NULL,
				WLZ_2D_DOMAINOBJ, NULL);
    cobj->n = 1;
    cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);
    warpGlobals.ovly.obj = (WlzObject *) cobj;

    if( warpGlobals.ovly.ximage ){
      AlcFree(warpGlobals.ovly.ximage->data);
      warpGlobals.ovly.ximage->data = NULL;
      XDestroyImage(warpGlobals.ovly.ximage);
      warpGlobals.ovly.ximage = NULL;
    }
    if( warpGlobals.ovly.ovlyImages[0] ){
      AlcFree(warpGlobals.ovly.ovlyImages[0]->data);
      warpGlobals.ovly.ovlyImages[0]->data = NULL;
      XDestroyImage(warpGlobals.ovly.ovlyImages[0]);
      warpGlobals.ovly.ovlyImages[0] = NULL;
    }
    if( warpGlobals.ovly.ovlyImages[1] ){
      AlcFree(warpGlobals.ovly.ovlyImages[1]->data);
      warpGlobals.ovly.ovlyImages[1]->data = NULL;
      XDestroyImage(warpGlobals.ovly.ovlyImages[1]);
      warpGlobals.ovly.ovlyImages[1] = NULL;
    }
	
  }
  else {
    /* release the paint key */
    removeCurrentPaintTool(view_struct->canvas, view_struct);
    paint_key = NULL;

    /* clear the undo domains */
    clearUndoDomains();

    /* clear the section view image */
    if( view_struct->view_object ){
      WlzFreeObj(view_struct->view_object);
      view_struct->view_object = NULL;
    }

    /* restart the 3D feedback display */
    HGUglwCanvasTbAnimate( globals.canvas );
    XtSetSensitive( globals.canvas, True );

    /* install new domains, update all views */
    installViewDomains(view_struct);
    
    /* unhighlight this canvas */
    view_canvas_highlight( view_struct, False );

    /* swap the callbacks to normal input mode */
    XtRemoveCallback(view_struct->canvas, XmNinputCallback, canvas_warp_painting_cb,
		     client_data);
    XtAddCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		  client_data);

  }

  return;
}

WlzObject *mapaintWarpObj(
  WlzObject	*obj,
  WlzInterpolationType	interpType)
{
  WlzObject		*obj1=NULL;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  if( obj == NULL ){
    return obj1;
  }

  if( warpGlobals.num_vtxs < 4 ){
    if( warpGlobals.affine ){
      obj1 = WlzAffineTransformObj(obj, warpGlobals.affine,
				   interpType, &errNum);
    }
  }
  else {
    if( warpGlobals.meshTr ){
      /* int	i, numObjs;
	 WlzObject	**objsArray;*/

      /* one object at a time to avoid BoundToObj bug */
      /* errNum = WlzLabel(obj, &numObjs, &objsArray, 2048, 0, WLZ_8_CONNECTED);
      if( errNum == WLZ_ERR_NONE ){
	for(i=0; i < numObjs; i++){
	  if( obj1 = WlzMeshTransformObj(objsArray[i], warpGlobals.meshTr,
					 interpType, &errNum) ){
	    WlzFreeObj(objsArray[i]);
	    objsArray[i] = WlzAssignObject(obj1, NULL);
	  }
	  else {
	    break;
	  }
	}
	if( errNum == WLZ_ERR_NONE ){
	  obj1 = WlzUnionN(numObjs, objsArray, 0, &errNum);
	}
      }
      for(i=0; i < numObjs; i++){
	WlzFreeObj(objsArray[i]);
	}*/
      obj1 = WlzMeshTransformObj(obj, warpGlobals.meshTr,
				 interpType, &errNum);
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "mapaintWarpObj", errNum);
  }
  return obj1;
}

static  void warp2DInteractDismissCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	toggle;
  XmToggleButtonCallbackStruct	cbs;

  /* get the warp controls toggle */
  if( toggle = XtNameToWidget(globals.topl, "*warp_input_2d_frame_title") ){
    XtVaSetValues(toggle, XmNset, False, NULL);
    cbs.set = False;
    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
  }

  return;
}

void warpCanvasMotionEventHandler(
  Widget        w,
  XtPointer     client_data,
  XEvent        *event,
  Boolean       *continue_to_dispatch)
{
  XmDrawingAreaCallbackStruct cbs;

  /* call the canvas input callbacks */
  cbs.reason = XmCR_INPUT;
  cbs.event = event;
  cbs.window = XtWindow(w);
  XtCallCallbacks(w, XmNinputCallback, (XtPointer) &cbs);

  return;
}

Widget createWarpDisplayFrame(
  Widget	parent,
  String	name,
  Visual	*visual,
  int		depth)
{
  Widget	frame, title, scrolled_window, canvas;
  Widget	button;

  /* create the frame and frame children */
  frame = XtVaCreateWidget(name, xmFrameWidgetClass,
				  parent, NULL);
  title = XtVaCreateManagedWidget("title", xmRowColumnWidgetClass, frame,
				  XmNchildType, 	XmFRAME_TITLE_CHILD,
				  XmNorientation,	XmHORIZONTAL,
				  NULL);
  scrolled_window =
    XtVaCreateWidget("warp_disp_scrolled_window",
			    xmScrolledWindowWidgetClass, frame,
			    XmNchildType, XmFRAME_WORKAREA_CHILD,
			    XmNscrollingPolicy, 	XmAUTOMATIC,
			    XmNborderWidth, 0,
			    NULL);

  /* create the buttons */
  button = XtVaCreateManagedWidget("b_1", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateWidget("b_1a", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateManagedWidget("b_2", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateWidget("b_2a", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateManagedWidget("b_3", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateManagedWidget("b_4", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateManagedWidget("b_5", xmToggleButtonWidgetClass,
				   title,
				   XmNindicatorOn,	True,
				   XmNfillOnSelect,	True,
				   XmNshadowThickness,	1,
				   NULL);

  /* add the gamma controls */
  button = XtVaCreateManagedWidget("b_6", xmArrowButtonWidgetClass,
				   title,
				   XmNarrowDirection,	XmARROW_UP,
				   XmNshadowThickness,	0,
				   NULL);

  button = XtVaCreateManagedWidget("b_7", xmArrowButtonWidgetClass,
				   title,
				   XmNarrowDirection,	XmARROW_DOWN,
				   XmNshadowThickness,	0,
				   NULL);

  /* now a mixture slider */
  button = XtVaCreateWidget("b_8", xmScaleWidgetClass, title,
			    XmNorientation,	XmHORIZONTAL,
			    XmNshowValue,	False,
			    XmNmaximum,		100,
			    XmNminimum,		0,
			    XmNvalue,		50,
			    NULL);


  /* create the canvas */
  canvas = XtVaCreateManagedWidget("canvas", hgu_DrawingAreaWidgetClass,
				   scrolled_window,
				   XmNwidth, 512,
				   XmNheight, 512,
				   XmNshadowThickness, 0,
				   XtNdepth,	depth,
				   XtNvisual,	visual,
				   NULL);
  XtManageChild(scrolled_window);
  XtManageChild(frame);
  return frame;
}

void warp2DInteractDeleteSelectedCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  int		resetOvlyFlg=0;
  int		i, j;

  /* delete any partially selected tie-point pair
     since these will appear "selected" on the display */
  switch( warpGlobals.tp_state ){
  case TP_INACTIVE:
    break;

  case TP_DST_DEFINED:
    /* delete selected destination vertex */
    warpUndisplayVtx(&(warpGlobals.dst),
		     warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
    warpGlobals.tp_state = TP_INACTIVE;
    break;

  case TP_SRC_DEFINED:
    /* delete selected source vertex */
    warpUndisplayVtx(&(warpGlobals.src),
		     warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
    warpGlobals.tp_state = TP_INACTIVE;
    break;

  case TP_SELECTED:
    break;
  }

  /* now delete the multi-selected tie-points */
  for(i = warpGlobals.num_vtxs-1; i >= 0; i--){
    if( warpGlobals.sel_vtxs[i] ){
      warpGlobals.sel_vtxs[i] = 0;
      warpUndisplayVtx(&(warpGlobals.dst),
		       warpGlobals.dst_vtxs[i]);
      warpUndisplayVtx(&(warpGlobals.src),
		       warpGlobals.src_vtxs[i]);
      warpGlobals.num_vtxs--;
      j = i;	
      while( j < warpGlobals.num_vtxs ){
	warpGlobals.dst_vtxs[j] = warpGlobals.dst_vtxs[j+1];
	warpGlobals.src_vtxs[j] = warpGlobals.src_vtxs[j+1];
	j++;
      }
      resetOvlyFlg = 1;
    }
  }
      
  warpDisplayTiePoints();


  if( resetOvlyFlg ){
    /* update the overlay image and display */
    warpSetOvlyObject();
    warpSetOvlyXImages(&(warpGlobals.ovly), 1);
    warpSetOvlyXImage(&(warpGlobals.ovly));
    warpCanvasExposeCb(warpGlobals.ovly.canvas,
		       (XtPointer) &(warpGlobals.ovly),
		       call_data);
    XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		    call_data);
  }

  return;
}

void warp2DInteractDeleteLastCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  int		resetOvlyFlg=0;

  switch( warpGlobals.tp_state ){
  case TP_INACTIVE:
    /* delete last vertex */
    if( warpGlobals.num_vtxs ){
      warpUndisplayVtx(&(warpGlobals.dst),
		       warpGlobals.dst_vtxs[warpGlobals.num_vtxs-1]);
      warpUndisplayVtx(&(warpGlobals.src),
		       warpGlobals.src_vtxs[warpGlobals.num_vtxs-1]);
      warpGlobals.num_vtxs--;
      resetOvlyFlg = 1;
    }
    break;

  case TP_DST_DEFINED:
    /* delete selected destination vertex */
    warpUndisplayVtx(&(warpGlobals.dst),
		     warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
    warpGlobals.tp_state = TP_INACTIVE;
    break;

  case TP_SRC_DEFINED:
    /* delete selected source vertex */
    warpUndisplayVtx(&(warpGlobals.src),
		     warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
    warpGlobals.tp_state = TP_INACTIVE;
    break;

  case TP_SELECTED:
    /* delete seleted vertex */
    warpUndisplayVtx(&(warpGlobals.dst),
		     warpGlobals.dst_vtxs[warpGlobals.sel_vtx]);
    warpUndisplayVtx(&(warpGlobals.src),
		     warpGlobals.src_vtxs[warpGlobals.sel_vtx]);
    warpGlobals.num_vtxs--;
    while( warpGlobals.sel_vtx < warpGlobals.num_vtxs ){
      warpGlobals.dst_vtxs[warpGlobals.sel_vtx] = 
	warpGlobals.dst_vtxs[warpGlobals.sel_vtx + 1];
      warpGlobals.src_vtxs[warpGlobals.sel_vtx] = 
	warpGlobals.src_vtxs[warpGlobals.sel_vtx + 1];
      warpGlobals.sel_vtx++;
    }
    warpGlobals.tp_state = TP_INACTIVE;
    resetOvlyFlg = 1;
    break;
  }
  warpDisplayTiePoints();


  if( resetOvlyFlg ){
    /* update the overlay image and display */
    warpSetOvlyObject();
    warpSetOvlyXImages(&(warpGlobals.ovly), 1);
    warpSetOvlyXImage(&(warpGlobals.ovly));
    warpCanvasExposeCb(warpGlobals.ovly.canvas,
		       (XtPointer) &(warpGlobals.ovly),
		       call_data);
    XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		    call_data);
  }

  return;
}

void warp2DInteractDeleteAllCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  int		resetOvlyFlg=0;

  switch( warpGlobals.tp_state ){
  case TP_INACTIVE:
    break;

  case TP_DST_DEFINED:
    /* delete selected destination vertex */
    warpUndisplayVtx(&(warpGlobals.dst),
		     warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
    warpGlobals.tp_state = TP_INACTIVE;
    break;

  case TP_SRC_DEFINED:
    /* delete selected source vertex */
    warpUndisplayVtx(&(warpGlobals.src),
		     warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
    warpGlobals.tp_state = TP_INACTIVE;
    break;

  case TP_SELECTED:
    break;
  }
  while( warpGlobals.num_vtxs ){
    warpGlobals.num_vtxs--;
    warpUndisplayVtx(&(warpGlobals.dst),
		     warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
    warpUndisplayVtx(&(warpGlobals.src),
		     warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
    resetOvlyFlg = 1;
  }
  warpDisplayTiePoints();


  if( resetOvlyFlg ){
    /* update the overlay image and display */
    warpSetOvlyObject();
    warpSetOvlyXImages(&(warpGlobals.ovly), 1);
    warpSetOvlyXImage(&(warpGlobals.ovly));
    warpCanvasExposeCb(warpGlobals.ovly.canvas,
		       (XtPointer) &(warpGlobals.ovly),
		       call_data);
    XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		    call_data);
  }

  return;
}

void warpUpdateCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{

  /* display the tie-points */
  warpDisplayTiePoints();

  /* update the overlay image and display */
  warpSetOvlyObject();
  warpSetOvlyXImages(&(warpGlobals.ovly), 1);
  warpSetOvlyXImage(&(warpGlobals.ovly));
  warpCanvasExposeCb(warpGlobals.ovly.canvas,
		     (XtPointer) &(warpGlobals.ovly),
		     call_data);
  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		  call_data);

  return;
}

static ActionAreaItem   warp_interact_actions[] = {
{"delete_last",	warp2DInteractDeleteLastCb,		NULL},
{"delete_selected",	warp2DInteractDeleteSelectedCb,		NULL},
{"delete_all",	warp2DInteractDeleteAllCb,		NULL},
{"dismiss",	warp2DInteractDismissCb,           NULL},
{"help",	NULL,           NULL},
};

static MenuItem warpDisplayFramePopupItemsP[] = { 
  {"gradient", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXImageCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"invert", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXImageCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"normalise", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXImageCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem overlayMethodsItemsP[] = {
  {"red-green 1", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_RG1,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"red-green 2", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_RG2,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"red-blue 1", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_RB1,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"red-blue 2", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_RB2,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"green-blue 1", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_GB1,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"green-blue 2", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_GB2,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"red-cyan", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_RNR,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"green-magenta", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_GNG,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"blue-yellow", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_BNB,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"dither image 1", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_DITHER1,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"dither image 2", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetOvlyMethodCb, (XtPointer) MA_OVERLAY_MIXTYPE_DITHER2,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem warpOvlyDisplayFramePopupItemsP[] = { 
  {"gradient dest-image", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXOvlyImageDstCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"gradient src-image", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXOvlyImageSrcCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"invert dest-image", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXOvlyImageDstCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"invert src-image", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXOvlyImageSrcCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"normalise dest-image", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXOvlyImageDstCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"normalise src-image", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   warpSetXOvlyImageSrcCb, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"overlay method", &xmCascadeButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_ENABLED, True, True, &(overlayMethodsItemsP[0])},
  NULL,
};

static Widget create2DWarpDialog(
  Widget parent,
  ThreeDViewStruct *view_struct)
{
  Widget	dialog, control, child, button, toggle;
  Visual	*visual;

  /* create a dialog widget and get control form and 24-bit visual */
  dialog = HGU_XmCreateStdDialog(parent, "warp2DInteractDialog",
				 xmFormWidgetClass,
				 warp_interact_actions, 5);
  control = XtNameToWidget( dialog, "*.control" );
  if( !(visual = HGU_XGetVisual(globals.dpy, DefaultScreen(globals.dpy),
			      TrueColor, 24)) ){
    visual = HGU_XmWidgetToVisual(globals.topl);
  }

  /* add some control buttons */
  toggle = XtVaCreateManagedWidget("autoUpdate",
				   xmToggleButtonGadgetClass, control,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_FORM,
				   XmNset,	True,
				   NULL);

  button = XtVaCreateManagedWidget("update", xmPushButtonGadgetClass,
				   control,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	toggle,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, warpUpdateCb, NULL);

  button = XtVaCreateManagedWidget("remesh", xmPushButtonGadgetClass,
				   control,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	button,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, warpRemeshCb, NULL);

  button = XtVaCreateManagedWidget("blink_comp", xmToggleButtonGadgetClass,
				   control,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	button,
				   XmNset,	False,
				   NULL);

  /* add children */
  child = createWarpDisplayFrame(control, "warp_dst_frame", visual, 24);
  XtVaSetValues(child,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		toggle,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	0,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	33,
		NULL);
  warpGlobals.dst.canvas = XtNameToWidget(child, "*canvas");
  warpDisplayFramePopupItemsP[0].callback_data = &warpGlobals.dst;
  warpDisplayFramePopupItemsP[1].callback_data = &warpGlobals.dst;
  warpDisplayFramePopupItemsP[2].callback_data = &warpGlobals.dst;
  warpGlobals.dst.popup = HGU_XmBuildMenu(warpGlobals.dst.canvas,
					  XmMENU_POPUP, "warp_dst_popup",
					  '\0', XmTEAR_OFF_DISABLED,
					  warpDisplayFramePopupItemsP);

  /*warpGlobals.dst.popup = NULL;*/
  warpGlobals.dst.ximage = NULL;
  warpGlobals.dst.mag = 1.0;
  warpGlobals.dst.rot = 0;
  warpGlobals.dst.transpose = 0;

  /* canvas callbacks and event handler */
  XtAddCallback(warpGlobals.dst.canvas, XmNexposeCallback,
		warpCanvasExposeCb, (XtPointer) &(warpGlobals.dst));
  XtAddCallback(warpGlobals.dst.canvas, XmNinputCallback,
		warpDstCanvasInputCb, (XtPointer) &(warpGlobals.dst));
  XtAddEventHandler(warpGlobals.dst.canvas, 
		    PointerMotionMask|EnterWindowMask|LeaveWindowMask,
		    False, warpCanvasMotionEventHandler, NULL);

  /* button callbacks */
  button = XtNameToWidget(child, "*b_1");
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagPlusCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_1a");
  XtManageChild(button);
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagMinusCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_2");
  XtAddCallback(button, XmNactivateCallback, warpCanvasRotCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_3");
  XtAddCallback(button, XmNactivateCallback, warpCanvasFlipCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_4");
  XtAddCallback(button, XmNactivateCallback, warpCanvasResetCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_5");
  XtAddCallback(button, XmNvalueChangedCallback, warpCanvasMeshCb,
		(XtPointer) 0);
  button = XtNameToWidget(child, "*b_6");
  XtAddCallback(button, XmNactivateCallback, warpIncrGammaCb,
		(XtPointer) &(warpGlobals.dst));
  button = XtNameToWidget(child, "*b_7");
  XtAddCallback(button, XmNactivateCallback, warpDecrGammaCb,
		(XtPointer) &(warpGlobals.dst));

  child = createWarpDisplayFrame(control, "warp_src_frame", visual, 24);
  XtVaSetValues(child,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		toggle,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	33,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	67,
		NULL);
  warpGlobals.src.canvas = XtNameToWidget(child, "*canvas");
  warpDisplayFramePopupItemsP[0].callback_data = &warpGlobals.src;
  warpDisplayFramePopupItemsP[1].callback_data = &warpGlobals.src;
  warpDisplayFramePopupItemsP[2].callback_data = &warpGlobals.src;
  warpGlobals.src.popup = HGU_XmBuildMenu(warpGlobals.src.canvas,
					  XmMENU_POPUP, "warp_src_popup",
					  '\0', XmTEAR_OFF_DISABLED,
					  warpDisplayFramePopupItemsP);
  warpGlobals.src.ximage = NULL;
  warpGlobals.src.mag = 1.0;
  warpGlobals.src.rot = 0;
  warpGlobals.src.transpose = 0;

  /* canvas callbacks and event handler */
  XtAddCallback(warpGlobals.src.canvas, XmNexposeCallback,
		warpCanvasExposeCb, (XtPointer) &(warpGlobals.src));
  XtAddCallback(warpGlobals.src.canvas, XmNinputCallback,
		warpSrcCanvasInputCb, (XtPointer) &(warpGlobals.src));
  XtAddEventHandler(warpGlobals.src.canvas, 
		    PointerMotionMask|EnterWindowMask|LeaveWindowMask,
		    False, warpCanvasMotionEventHandler, NULL);

  /* button callbacks */
  button = XtNameToWidget(child, "*b_1");
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagPlusCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_1a");
  XtManageChild(button);
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagMinusCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_2");
  XtAddCallback(button, XmNactivateCallback, warpCanvasRotCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_3");
  XtAddCallback(button, XmNactivateCallback, warpCanvasFlipCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_4");
  XtAddCallback(button, XmNactivateCallback, warpCanvasResetCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_5");
  XtAddCallback(button, XmNvalueChangedCallback, warpCanvasMeshCb,
		(XtPointer) 1);
  button = XtNameToWidget(child, "*b_6");
  XtAddCallback(button, XmNactivateCallback, warpIncrGammaCb,
		(XtPointer) &(warpGlobals.src));
  button = XtNameToWidget(child, "*b_7");
  XtAddCallback(button, XmNactivateCallback, warpDecrGammaCb,
		(XtPointer) &(warpGlobals.src));

  child = createWarpDisplayFrame(control, "warp_ovly_frame", visual, 24);
  XtVaSetValues(child,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		toggle,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	67,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	100,
		NULL);
  warpGlobals.ovly.canvas = XtNameToWidget(child, "*canvas");
  warpOvlyDisplayFramePopupItemsP[0].callback_data = &warpGlobals.ovly;
  warpOvlyDisplayFramePopupItemsP[1].callback_data = &warpGlobals.ovly;
  warpOvlyDisplayFramePopupItemsP[2].callback_data = &warpGlobals.ovly;
  warpOvlyDisplayFramePopupItemsP[3].callback_data = &warpGlobals.ovly;
  warpOvlyDisplayFramePopupItemsP[4].callback_data = &warpGlobals.ovly;
  warpOvlyDisplayFramePopupItemsP[5].callback_data = &warpGlobals.ovly;
  warpGlobals.ovly.popup = 
    HGU_XmBuildMenu(warpGlobals.ovly.canvas,
		    XmMENU_POPUP,
		    "warp_ovly_popup",
		    '\0', XmTEAR_OFF_DISABLED,
		    warpOvlyDisplayFramePopupItemsP);
  warpGlobals.ovly.ximage = NULL;
  warpGlobals.ovly.mag = 1.0;
  warpGlobals.ovly.rot = 0;
  warpGlobals.ovly.transpose = 0;

  /* canvas callbacks and event handler */
  XtAddCallback(warpGlobals.ovly.canvas, XmNexposeCallback,
		warpCanvasExposeCb, (XtPointer) &(warpGlobals.ovly));
  XtAddCallback(warpGlobals.ovly.canvas, XmNinputCallback,
		warpOvlyCanvasInputCb, (XtPointer) &(warpGlobals.ovly));

  /* button callbacks */
  button = XtNameToWidget(child, "*b_1");
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagPlusCb,
		(XtPointer) &(warpGlobals.ovly));
  button = XtNameToWidget(child, "*b_1a");
  XtManageChild(button);
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagMinusCb,
		(XtPointer) &(warpGlobals.ovly));
  button = XtNameToWidget(child, "*b_2");
  XtAddCallback(button, XmNactivateCallback, warpCanvasRotCb,
		(XtPointer) &(warpGlobals.ovly));
  button = XtNameToWidget(child, "*b_3");
  XtAddCallback(button, XmNactivateCallback, warpCanvasFlipCb,
		(XtPointer) &(warpGlobals.ovly));
  button = XtNameToWidget(child, "*b_4");
  XtAddCallback(button, XmNactivateCallback, warpCanvasResetCb,
		(XtPointer) &(warpGlobals.ovly));
  button = XtNameToWidget(child, "*b_5");
  XtAddCallback(button, XmNvalueChangedCallback, warpCanvasMeshCb,
		(XtPointer) 2);
  button = XtNameToWidget(child, "*b_6");
  XtAddCallback(button, XmNactivateCallback, warpIncrGammaCb,
		(XtPointer) &(warpGlobals.ovly));
  button = XtNameToWidget(child, "*b_7");
  XtAddCallback(button, XmNactivateCallback, warpDecrGammaCb,
		(XtPointer) &(warpGlobals.ovly));
  button = XtNameToWidget(child, "*b_8");
  XtManageChild(button);
  XtAddCallback(button, XmNdragCallback, warpMixRatioCb,
		(XtPointer) &(warpGlobals.ovly));
  XtAddCallback(button, XmNvalueChangedCallback, warpMixRatioCb,
		(XtPointer) &(warpGlobals.ovly));

  return dialog;
}

static ActionAreaItem   warp_controls_actions[] = {
{"src_read",	NULL,		NULL},
{"sgnl_read",	NULL,		NULL},
{"re_mesh",	NULL,		NULL},
{"i_o",		NULL,           NULL},
{"import",	NULL,           NULL},
};

static MenuItem mesh_menu_itemsP[] = {		/* mesh_menu items */
  {"gradient", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshMethodCb, (XtPointer) WLZ_MESH_GENMETHOD_GRADIENT,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"block", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshMethodCb, (XtPointer) WLZ_MESH_GENMETHOD_BLOCK,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem interpFunctionItemsP[] = { /* interp_function_menu items */
  {"multiquadric", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshFunctionCb, (XtPointer) WLZ_FN_BASIS_2DMQ,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"thin-plate spline", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshFunctionCb, (XtPointer) WLZ_FN_BASIS_2DTPS,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"polynomial", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshFunctionCb, (XtPointer) WLZ_FN_BASIS_2DPOLY,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
/*{"gaussian", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshFunctionCb, (XtPointer) WLZ_FN_BASIS_2DGAUSS,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},*/
  NULL,
};

static MenuItem affineTypeItemsP[] = { /* interp_function_menu items */
  {"noshear", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpAffineTypeCb, (XtPointer) WLZ_TRANSFORM_2D_NOSHEAR,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"rigid", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpAffineTypeCb, (XtPointer) WLZ_TRANSFORM_2D_REG,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"affine", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpAffineTypeCb, (XtPointer) WLZ_TRANSFORM_2D_AFFINE,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};


Widget createWarpInput2DDialog(
  Widget	topl)
{
  Widget	dialog=NULL;
  WlzDVertex3		fixed;
  ThreeDViewStruct	*view_struct;
  Widget	control, frame, form, title, controls_frame, section_frame;
  Widget	option_menu, button, buttons, radio_box, label, widget;
  Widget	toggle, slider, scale, sgnl_controls;
  Widget	notebook, page;
  float		fval, fmin, fmax;
  Visual	*visual;
  int		i;

  /* check for reference object */
  if( !globals.obj ){
    return NULL;
  }

  /* should call reset fixed point for this */
  fixed.vtX = (globals.obj->domain.p->kol1 + globals.obj->domain.p->lastkl)/2;
  fixed.vtY = (globals.obj->domain.p->line1 + globals.obj->domain.p->lastln)/2;
  fixed.vtZ = (globals.obj->domain.p->plane1 + globals.obj->domain.p->lastpl)/2;
  dialog = create_view_window_dialog(topl, 0.0, 0.0, &fixed);

  /* can use for painting  - but switch off when warping */
  XtVaGetValues(dialog, XmNuserData, &view_struct, NULL);
  view_struct->titleStr = "2D warp input dialog";

  /* reset the dialog title */
  set_view_dialog_title(dialog, 0.0, 0.0);

  /* get controls form and controls frame to add warping controls */
  control = XtNameToWidget( dialog, "*.control" );
  controls_frame = XtNameToWidget( control, "*.controls_frame");
  section_frame = XtNameToWidget( control, "*.section_frame");
  if( !(visual = HGU_XGetVisual(globals.dpy, DefaultScreen(globals.dpy),
			      TrueColor, 24)) ){
    visual = HGU_XmWidgetToVisual(globals.topl);
  }

  /* add a new frame */
  frame = XtVaCreateManagedWidget("warp_input_2d_frame",
				  xmFrameWidgetClass, control,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  XmNbottomAttachment,	XmATTACH_FORM,
				  NULL);
  form = XtVaCreateWidget("warp_input_2d_form",
			  xmFormWidgetClass, 	frame,
			  XmNleftAttachment,	XmATTACH_FORM,
			  XmNrightAttachment,	XmATTACH_FORM,
			  XmNtopAttachment,	XmATTACH_FORM,
			  XmNchildType, XmFRAME_WORKAREA_CHILD,
			  NULL);  
  title = XtVaCreateManagedWidget("warp_input_2d_frame_title",
				  xmToggleButtonGadgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  XtAddCallback(title, XmNvalueChangedCallback, warpControlsCb,
		view_struct);
  XtAddCallback(title, XmNvalueChangedCallback, warpSetupCb,
		view_struct);

  /* now the controls */
  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "mesh_method", 0,
				XmTEAR_OFF_DISABLED, mesh_menu_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);
  widget = option_menu;

  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "mesh_function", 0,
				XmTEAR_OFF_DISABLED, interpFunctionItemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		widget,
		NULL);
  XtManageChild(option_menu);
  widget = option_menu;

  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "affine_type", 0,
				XmTEAR_OFF_DISABLED, affineTypeItemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		widget,
		NULL);
  XtManageChild(option_menu);

  /* defaults for the mesh distances */
  warpGlobals.meshMinDst = 20.0;
  warpGlobals.meshMaxDst = 40.0;

  fval = warpGlobals.meshMinDst;
  slider = HGU_XmCreateHorizontalSlider("mesh_min_dist", form,
					fval, 5.0, 100.0, 0,
					warpMeshMinDistCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  fval = warpGlobals.meshMaxDst;
  slider = HGU_XmCreateHorizontalSlider("mesh_max_dist", form,
					fval, 20.0, 500.0, 0,
					warpMeshMaxDistCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  /* now some buttons */
  buttons = HGU_XmCreateWActionArea(form,
				    warp_controls_actions,
				    XtNumber(warp_controls_actions),
				    xmDrawnButtonWidgetClass);

  /* set the buttons attachments */
  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  /* now the callbacks */
  widget = XtNameToWidget(buttons, "*.src_read");
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_CENTER,
		NULL); 
  XtAddCallback(widget, XmNactivateCallback, warpReadSourcePopupCb,
		view_struct);

  widget = XtNameToWidget(buttons, "*.sgnl_read");
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_CENTER,
		NULL); 
  XtAddCallback(widget, XmNactivateCallback, warpReadSignalPopupCb,
		view_struct);

  widget = XtNameToWidget(buttons, "*.re_mesh");
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_CENTER,
		NULL); 
  XtAddCallback(widget, XmNactivateCallback, warpRemeshCb,
		view_struct);

  widget = XtNameToWidget(buttons, "*.i_o");
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL); 
  XtAddCallback(widget, XmNexposeCallback, popupDrawnButtonCb, NULL);
  XtAddCallback(widget, XmNresizeCallback, popupDrawnButtonCb, NULL);
  setupWarpIOMenu(widget, view_struct);

  widget = XtNameToWidget(buttons, "*.import");
  XtVaSetValues(widget,
		XmNpushButtonEnabled, True,
		XmNalignment, XmALIGNMENT_CENTER,
		NULL);
  XtAddCallback(widget, XmNactivateCallback, warpImportSignalCb,
		view_struct);

  /* create the signal controls frame */
  sgnl_controls = createWarpSgnlControlsFrame(control, "warp_sgnl_controls_frame",
					      view_struct);
  XtVaSetValues(sgnl_controls,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		section_frame,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		NULL);

  /* create the signal image frame and process controls -
     unmanaged for now */
  widget = createWarpSgnlDisplayFrame(control, "warp_sgnl_frame", view_struct);
  XtVaSetValues(widget,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,	sgnl_controls,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		section_frame,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  /* modify attachments for the section and orientation controls frame */
  XtVaSetValues(controls_frame,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,	frame,
		NULL);

  /* add a callback to the destroy callback list */
  XtAddCallback(dialog, XmNdestroyCallback, warpInput2DDestroyCb,
		view_struct);

  /* set the interact dialog to NULL and set up structure */
  warpGlobals.view_struct = view_struct;
  warpGlobals.warp2DInteractDialog = NULL;
  warpGlobals.srcFile = NULL;
  warpGlobals.srcFileType = WLZEFF_FORMAT_NONE;
  warpGlobals.sgnlFile = NULL;
  warpGlobals.sgnlObj = NULL;
  warpGlobals.sgnlFileType = WLZEFF_FORMAT_NONE;
  warpGlobals.sgnlProcObj = NULL;
  warpGlobals.sgnlThreshObj = NULL;
  warpGlobals.warpBibFile = NULL;
  warpGlobals.dst.obj = NULL;
  warpGlobals.src.obj = NULL;
  warpGlobals.ovly.obj = NULL;
  warpGlobals.sgnl.obj = NULL;
  warpGlobals.dst.ximage = NULL;
  warpGlobals.src.ximage = NULL;
  warpGlobals.ovly.ximage = NULL;
  warpGlobals.ovly.ovlyImages[0] = NULL;
  warpGlobals.ovly.ovlyImages[1] = NULL;
  warpGlobals.sgnl.ximage = NULL;
  warpGlobals.dst.gc = (GC) -1;
  warpGlobals.src.gc = (GC) -1;
  warpGlobals.ovly.gc = (GC) -1;
  warpGlobals.sgnl.gc = (GC) -1;
  warpGlobals.dst.gamma = 1.0;
  warpGlobals.src.gamma = 1.0;
  warpGlobals.ovly.gamma = 1.0;
  warpGlobals.ovly.mixRatio = 50.0;
  warpGlobals.ovly.mixType = MA_OVERLAY_MIXTYPE_RG1;
  warpGlobals.sgnl.gamma = 1.0;
  warpGlobals.dst.grey_invert = 0;
  warpGlobals.src.grey_invert = 0;
  warpGlobals.ovly.grey_invert = 0;
  warpGlobals.sgnl.grey_invert = 0;

  warpGlobals.num_vtxs = 0;
  warpGlobals.sel_vtx = 0;
  for(i=0; i < WARP_MAX_NUM_VTXS; i++){
    warpGlobals.sel_vtxs[i] = 0;
  }
  warpGlobals.tp_state = TP_INACTIVE;
  warpGlobals.green_gc = (GC) -1;
  warpGlobals.red_gc = (GC) -1;
  warpGlobals.blue_gc = (GC) -1;
  warpGlobals.yellow_gc = (GC) -1;

  warpGlobals.affine = NULL;
  warpGlobals.affineType = WLZ_TRANSFORM_2D_NOSHEAR;
  warpGlobals.basisTr = NULL;
  warpGlobals.wlzFnType = WLZ_FN_BASIS_2DMQ;
  warpGlobals.meshTr = NULL;
  warpGlobals.meshErrFlg = 0;
  warpGlobals.meshMthd = WLZ_MESH_GENMETHOD_GRADIENT;

  warpGlobals.thresholdType = WLZ_RGBA_THRESH_NONE;
  warpGlobals.threshColorChannel = WLZ_RGBA_CHANNEL_GREY;
  warpGlobals.threshRangeLow = 256;
  warpGlobals.threshRangeHigh = 256;
  for(i=0; i < 3; i++){
    warpGlobals.threshRangeRGBLow[i] = 255;
    warpGlobals.threshRangeRGBHigh[i] = 255;
  }
  warpGlobals.threshRGBSpace = WLZ_RGBA_SPACE_GREY;
  warpGlobals.threshRGBCombination = 0;
  warpGlobals.lowRGBPoint.type = WLZ_GREY_RGBA;
  WLZ_RGBA_RGBA_SET(warpGlobals.lowRGBPoint.v.rgbv,255,255,255,255);
  warpGlobals.highRGBPoint.type = WLZ_GREY_RGBA;
  WLZ_RGBA_RGBA_SET(warpGlobals.highRGBPoint.v.rgbv,255,255,255,255);
/*  warpGlobals.colorEllipseEcc = 1.0;*/
  warpGlobals.lastThresholdPageNum = -1;

  return( dialog );
}
