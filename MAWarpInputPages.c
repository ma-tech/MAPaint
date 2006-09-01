#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   MAPaint						*
*   File       :   MAWarpInputPages.c					*
*************************************************************************
*   Author Name :  richard						*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Tue Nov 23 10:53:56 2004				*
*   $Revision$							*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>
#include <MAWarp.h>

extern void warpSetSignalDomain(
  WlzIVertex2	*selVtx);

extern void warpReadSignalPopupCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void view_canvas_highlight(
  ThreeDViewStruct	*view_struct,
  Boolean		highlight);

extern WlzErrorNum warpResetSignalObj(
  WlzObject		*obj);

extern void tpTrackSaveCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data);

void warpRedisplayOvly(void)
{
  if( warpGlobals.num_vtxs ){
    warpDisplayTiePoints();
    warpSetOvlyObject();
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
    warpSetOvlyXImage(&(warpGlobals.ovly));
    XtCallCallbacks(warpGlobals.ovly.canvas, XmNexposeCallback,
		    NULL);
    XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		    NULL);
    XtCallCallbacks(warpGlobals.src.canvas, XmNexposeCallback,
		    NULL);
  }

  return;
}

int warpGetBibfileTiePoints(
  char		*fileStr,
  WlzDVertex2	**dstVtxs,
  WlzDVertex2	**srcVtxs)
{
  FILE	*fp;
  int	numVtxs=0;

  /* open file */
  if( fp = fopen(fileStr, "r") ){
    BibFileRecord	*bibfileRecord;
    BibFileError	bibFileErr;
    char		*errMsg;

    /* read the bibfile */
    numVtxs = 0;
    bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
    while( bibFileErr == BIBFILE_ER_NONE ){

      /* read tie-points */
      if( !strncmp(bibfileRecord->name, "WlzTiePointVtxs", 15) ){
	int		index;
	WlzDVertex3	dstVtx;
	WlzDVertex3	srcVtx;
	int		relFlg;

	WlzEffBibParseTiePointVtxsRecord(bibfileRecord, &index,
					 &dstVtx, &srcVtx, &relFlg);
	if( relFlg ){
	  dstVtx.vtX += warpGlobals.dst.obj->domain.i->kol1;
	  dstVtx.vtY += warpGlobals.dst.obj->domain.i->line1;
	  srcVtx.vtX -= warpGlobals.srcXOffset;
	  srcVtx.vtY -= warpGlobals.srcYOffset;
	}

	/* add to vtx arrays */
	numVtxs++;
	if( *dstVtxs ){
	  *dstVtxs = (WlzDVertex2 *) AlcRealloc(*dstVtxs,
						sizeof(WlzDVertex2) * numVtxs);
	}
	else {
	  *dstVtxs = (WlzDVertex2 *) AlcMalloc(sizeof(WlzDVertex2) * numVtxs);
	}
	if( *srcVtxs ){
	  *srcVtxs = (WlzDVertex2 *) AlcRealloc(*srcVtxs,
						sizeof(WlzDVertex2) * numVtxs);
	}
	else {
	  *srcVtxs = (WlzDVertex2 *) AlcMalloc(sizeof(WlzDVertex2) * numVtxs);
	}

	(*dstVtxs)[numVtxs-1].vtX = dstVtx.vtX;
	(*dstVtxs)[numVtxs-1].vtY = dstVtx.vtY;
	(*srcVtxs)[numVtxs-1].vtX = srcVtx.vtX;
	(*srcVtxs)[numVtxs-1].vtY = srcVtx.vtY;
      }

      BibFileRecordFree(&bibfileRecord);
      bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
    }
    (void) fclose(fp);
  }

  return numVtxs;
}

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
  warpSetSignalDomain(NULL);
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
  if( (obj = HGU_XmReadExtFFObject(warp_read_src_dialog,
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
	if( WlzGreyTypeFromObj(warpGlobals.src.obj, NULL) == WLZ_GREY_RGBA ){
	  Widget	option_menu, widget, menuHistory;
	  /* switch mesh method to block */
	  if( option_menu = XtNameToWidget(view_struct->dialog,
					   "*.mesh_method") ){
	    if( widget = XtNameToWidget(option_menu, "*.block") ){
	      XtVaGetValues(option_menu, XmNmenuHistory, &menuHistory, NULL);
	      if( widget != menuHistory ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		warpGlobals.meshMthd = WLZ_MESH_GENMETHOD_BLOCK;
		HGU_XmUserMessage(globals.topl,
				  "The mesh generation method has been\n"
				  "switched to Block mode because the source\n"
				  "image just read is a colour image. The\n"
				  "gradient mesh method does not produce a\n"
				  "usable mesh in this version.\n\n"
				  "You may want to adjust the mesh minimum\n"
				  "distance parameter",
				  XmDIALOG_FULL_APPLICATION_MODAL);
	      }
	    }
	  }
	}
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
  Widget		toggle, form;

  /* now use generic Ext format dialog */
  if( warp_read_src_dialog == NULL ){
    warp_read_src_dialog = HGU_XmCreateExtFFObjectFSB(view_struct->dialog,
						     "warp_read_src_dialog",
						     warpReadSourceCb,
						     client_data);

    /* add a check box to reset the current working directory */
    if( form = XtNameToWidget(warp_read_src_dialog, "*.formatFormRC") ){
      toggle = XtVaCreateManagedWidget("reset_cwd", xmToggleButtonGadgetClass,
				       form,
				       XmNset,	True,
				       NULL);
    }

    XtAddCallback(warp_read_src_dialog, XmNokCallback,
		  warpResetCWDCb, client_data);

    /* set jpeg default */
    HGU_XmExtFFObjectFSBSetType(warp_read_src_dialog, WLZEFF_FORMAT_JPEG);
  }

  XtManageChild(warp_read_src_dialog);
  PopupCallback(w, (XtPointer) XtParent(warp_read_src_dialog), NULL);
  XtCallCallbacks(warp_read_src_dialog, XmNmapCallback, call_data);

  return;
}

void warpImportSignalCb(
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

void rapidMapPageSelectCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Widget	warp_sgnl_frame;

  /* check that the warp-import page is mapped else call the import callback */
  if( warp_sgnl_frame = XtNameToWidget(view_struct->dialog,
				       "*control*warp_sgnl_frame") ){
    if( !XtIsManaged(warp_sgnl_frame) ){
      warpImportSignalCb(widget, client_data, call_data);
    }
  }

  /* unset tie-point tracking flag */
  warpGlobals.tpTrackingFlg = 0;

  return;
}

void tpTrackPageSelectCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Widget		warp_sgnl_frame, toggle;

  /* check that the warp-import page is unmapped else call the
     import callback */
  if( warp_sgnl_frame = XtNameToWidget(view_struct->dialog,
				       "*control*warp_sgnl_frame") ){
    if( XtIsManaged(warp_sgnl_frame) ){
      warpImportSignalCb(widget, client_data, call_data);
    }
  }

  /* set auto-update to unselected */
    if( toggle = XtNameToWidget(warpGlobals.warp2DInteractDialog,
				"*.autoUpdate") ){
      XtVaSetValues(toggle, XmNset, False, NULL);
    }

  /* set tie-point tracking flag */
  warpGlobals.tpTrackingFlg = 1;
  warpGlobals.tpTrackingInit = 0;

  return;
}

void standardPageSelectCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  /* unset tie-point tracking flag */
  warpGlobals.tpTrackingFlg = 0;

  return;
}

char *HGU_XmPathToFile(
  char	*pathStr)
{
  char	*fileStr=NULL;

  if( pathStr ){
    fileStr = strrchr(pathStr, '/');
  }

  return fileStr?fileStr+1:pathStr;
}

char *HGU_XmPathToDirectory(
  char	*pathStr)
{
  char	*dirStr=NULL;

  if( pathStr ){
    if( strchr(pathStr, '/') ){
      dirStr = AlcStrDup(pathStr);
      strrchr(dirStr, '/')[0] = '\0';
    }
  }

  return dirStr;
}

void rapidMapSaveCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  FILE		*fp;
  XmString	xmstr;
  String	fileStr, dirStr;
  int		len, num_overlays;
  int		i, confirmFlg;

  /* check if save required */
  if( warpGlobals.bibfileSavedFlg ){
    return;
  }

  /* only continue if the bibfile is defined */
  if( !warpGlobals.warpBibFile ){
    /* should put up a warning here to save using i/o menu */
    return;
  }

  /* save current warp params bibfile - create backup file */
  if( xmstr = XmStringCreateSimple(warpGlobals.warpBibFile) ){
    confirmFlg = 0;
    if( fp = HGU_XmGetFilePointerBckCnfm(globals.topl, xmstr, NULL, "w",
					 ".bak", &confirmFlg) ){
      warpBibfileWrite(fp, view_struct);
      fclose(fp);
    }
    XmStringFree(xmstr);
  }

  /* get directory component of bibfile path, reset cwd */
  if( dirStr = HGU_XmPathToDirectory(warpGlobals.warpBibFile) ){
    if( chdir((const char *) dirStr) ){
      /* report an error */
      HGU_XmUserError(view_struct->dialog,
		      "Rapid-Map save callback:\n"
		      "  Failed to change current working\n"
		      "  directory (folder). For the next\n"
		      "  save please check the domain file\n"
		      "  names.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }
    XtFree(dirStr);
  }    

  /* remove directory component from each domain filename */
  num_overlays = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;
  for(i=1; i <= num_overlays; i++){
    if( globals.domain_filename[i] ){
      fileStr = HGU_XmPathToFile(globals.domain_filename[i]);
      /*AlcFree(globals.domain_filename[i]); not safe */
      globals.domain_filename[i] = fileStr;
    }
  }

  /* save current domains to same directory as the bibfile
     note need to come out of painting mode and update domains */
  /* so let us see if this works -drop paint-key, install, save,
     reclaim paint-key */
  if( paint_key == view_struct ){
    paint_key = NULL;
    installViewDomains(view_struct);
    save_all_domains_cb(globals.topl, client_data, call_data);
    paint_key = view_struct;
  }
  else {
    /* warn nothing saved - shouldn't get here */
  }

  /* set saved flags - done for domains now for the bibfile */
  warpGlobals.bibfileSavedFlg = 1;

  return;
}

void rapidInstallBibfile(
  String		bibfileStr,
  ThreeDViewStruct	*view_struct)
{
  String	dirStr;

  /* check file string */
  if( bibfileStr == NULL ){
    /* something wrong */
    return;
  }

  /*  drop paint key and clear current domains */
  if( paint_key == view_struct ){
    paint_key = NULL;
    clearUndoDomains();

    /* clear the section view image */
    if( view_struct->view_object ){
      WlzFreeObj(view_struct->view_object);
      view_struct->view_object = NULL;
    }
  }
  else {
    /* shouldn't get here */
    return;
  }

  /* clear tie-points */
  warp2DInteractDeleteAllCb(view_struct->dialog,
			    (XtPointer) view_struct, NULL);

  /* change current working directory */
  if( dirStr = HGU_XmPathToDirectory(bibfileStr) ){
    if( chdir((const char *) dirStr) ){
      /* report an error */
      HGU_XmUserError(view_struct->dialog,
		      "Rapid-Map install:\n"
		      "  Failed to change current working\n"
		      "  directory (folder). For the next\n"
		      "  save please check the domain file\n"
		      "  names.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }
    XtFree(dirStr);
  }

  /* rapid-read next bibfile */
  warpRapidIORead(bibfileStr, view_struct);

  /* set warp dialog to the top */
  paint_key = view_struct;

  return;
}

void rapidMapSaveAsCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  String		bibfileStr;

  /* get bibfile from file browser */
  if( bibfileStr = 
	HGU_XmUserGetFilenameT(globals.topl,
			      "Please select a new warp bibfile.\n",
			      "OK", "cancel", warpGlobals.warpBibFile,
			      NULL, "*.bib", NULL) ){
    if(warpGlobals.warpBibFile ){
      AlcFree( warpGlobals.warpBibFile );
    }
    warpGlobals.warpBibFile = bibfileStr;;
    warpGlobals.bibfileSavedFlg = 0;
  }
  else {
    /* maybe give a message here */
    return;
  }

  /* rapid save of existing */
  /* check if save required */
  if( !warpGlobals.bibfileSavedFlg ){
    if( HGU_XmUserConfirm(globals.topl,
			  "Current bibfile and domains not saved.\n"
			  "Do you want to save before reading the\n"
			  "next warp bibfile?\n",
			  "Yes", "No", 1) ){
      rapidMapSaveCb(widget, client_data, call_data);
    }
  }

  return;
}
void rapidMapSelectCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  String		bibfileStr;

  /* get next bibfile from file browser */
  if( !(bibfileStr = 
	HGU_XmUserGetFilenameT(globals.topl,
			      "Please select a new warp bibfile.\n",
			      "OK", "cancel", NULL,
			      globals.origDir, "*.bib", NULL)) ){
    return;
  }

  /* rapid save of existing */
  /* check if save required */
  if( !warpGlobals.bibfileSavedFlg ){
    if( HGU_XmUserConfirm(globals.topl,
			  "Current bibfile and domains not saved.\n"
			  "Do you want to save before reading the\n"
			  "next warp bibfile?\n",
			  "Yes", "No", 1) ){
      rapidMapSaveCb(widget, client_data, call_data);
    }
  }

  /* rapid install next bibfile */
  rapidInstallBibfile(bibfileStr, view_struct);

  return;
}

void rapidMapNextCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  String		bibfileStr;

  /* rapid save of existing */
  /* check if save required */
  if( !warpGlobals.bibfileSavedFlg ){
    switch( HGU_XmUserConfirm3(globals.topl,
			  "Current bibfile and domains not saved.\n"
			  "Do you want to save before reading the\n"
			  "next warp bibfile?\n",
			  "Yes", "No", "Cancel", 1) ){
    case 0:
      rapidMapSaveCb(widget, client_data, call_data);
      break;

    case 1:
      /* do nothing */
      break;

    case 2:
      /* cancel */
      return;
    }
  }

  /* get next bibfile from bibfile-list say list empty */
  warpGlobals.bibfileListIndex++;
  if( warpGlobals.bibfileListIndex < warpGlobals.bibfileListCount ){
    bibfileStr = warpGlobals.bibfileList[warpGlobals.bibfileListIndex];
  }
  else {
    /* end of list, call rapidMapSelect */
    rapidMapSelectCb(widget, client_data, call_data);
    return;
  }

  /* rapid install next bibfile */
  rapidInstallBibfile(bibfileStr, view_struct);

  return;
}

static char *tpTrackGetBibfileName(
  double	dist)
{
  char		*file;
  int		plane;

  if( !warpGlobals.tpTrackingInit ){
    return NULL;
  }

  /* get the current plane number and set filename */
  plane = dist;
  file = (char *) AlcMalloc(sizeof(char) * 32);
  sprintf(file, "tp_track_%04d_%04d.bib", plane, plane+1);
  
  return file;
}

static void tpTrackSetSrc(
  ThreeDViewStruct	*view_struct)
{
  WlzObject	*srcObj;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Display		*dpy=XtDisplay(view_struct->canvas);
  Window		win=XtWindow(view_struct->canvas);  
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  
  if( wlzViewStr->dist >= wlzViewStr->maxvals.vtZ ){
    /* end of object provide warning, clear tie-points & src image */
    warp2DInteractDeleteAllCb(view_struct->dialog,
			      (XtPointer) view_struct, NULL);
    if( warpGlobals.src.obj ){
      WlzFreeObj(warpGlobals.src.obj);
      warpGlobals.src.obj = NULL;
    }
    warpSetXImage(&(warpGlobals.src));
    warpCanvasExposeCb(warpGlobals.src.canvas,
		       (XtPointer) &(warpGlobals.src),
		       NULL);
  }
  else {
    Wlz3DSectionIncrementDistance(wlzViewStr, 1.0);
    if( srcObj = WlzGetSectionFromObject(globals.orig_obj, wlzViewStr,
					       WLZ_INTERPOLATION_NEAREST,
					       &errNum) ){
      srcObj = WlzAssignObject(srcObj, &errNum);
      warpGlobals.srcFile = "tpTracking";
      warpGlobals.srcFileType = WLZEFF_FORMAT_WLZ;
      warpGlobals.srcXOffset = -srcObj->domain.i->kol1;
      warpGlobals.srcYOffset = -srcObj->domain.i->line1;
      if( warpGlobals.src.obj ){
	WlzFreeObj(warpGlobals.src.obj);
	warpGlobals.src.obj = NULL;
      }
      warpGlobals.src.obj =
	WlzAssignObject(WlzShiftObject(srcObj,
				       warpGlobals.srcXOffset,
				       warpGlobals.srcYOffset,
				       0, &errNum), NULL);
      WlzFreeObj(srcObj);

      warpSetXImage(&(warpGlobals.src));
      warpCanvasExposeCb(warpGlobals.src.canvas,
			 (XtPointer) &(warpGlobals.src),
			 NULL);
      if( warpGlobals.meshTr ){
	WlzMeshFreeTransform(warpGlobals.meshTr);
      }
      warpGlobals.meshTr = WlzMeshFromObj(warpGlobals.src.obj,
					  warpGlobals.meshMthd,
					  warpGlobals.meshMinDst,
					  warpGlobals.meshMaxDst,
					  &errNum);
    }
    else {
      /* something wrong here */
      fprintf(stderr, "tpTrackStartCb(): something wrong, no source image\n");
      Wlz3DSectionIncrementDistance(wlzViewStr, -1.0);
      return;
    }
    Wlz3DSectionIncrementDistance(wlzViewStr, -1.0);
  }

  return;
}

void tpTrackStartCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Widget		slider;
  Display		*dpy=XtDisplay(view_struct->canvas);
  Window		win=XtWindow(view_struct->canvas);
  WlzObject		*srcObj;
  WlzCompoundArray	*cobj;
  char			planeBuf[16], *startStr;
  int			startPlane;
  int			i, nPoints;
  WlzDVertex2		*dstVtxs, *srcVtxs;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /*  drop paint key and clear current domains */
  if( paint_key == view_struct ){
    paint_key = NULL;
    clearUndoDomains();

    /* clear the section view image */
    if( view_struct->view_object ){
      WlzFreeObj(view_struct->view_object);
      view_struct->view_object = NULL;
    }
  }
  else {
    /* shouldn't get here */
    return;
  }

  /* clear tie-points */
  warp2DInteractDeleteAllCb(view_struct->dialog,
			    (XtPointer) view_struct, NULL);

  /* request start plane */
  startPlane = globals.obj->domain.p->plane1;
  sprintf(planeBuf, "%d", startPlane);
  if( startStr = HGU_XmUserGetstr(widget,
			      "Please type in the start plane:",
			      "Continue",
			      "Cancel",
			      planeBuf) ){
    if( sscanf(startStr, "%d", &startPlane) == 1 ){
      if((startPlane < globals.obj->domain.p->plane1) ||
	 (startPlane > globals.obj->domain.p->lastpl) ){
	HGU_XmUserMessage(widget,
		      "Invalid plane number, the plane value\n"
		      "must be within the minimum and maximum\n"
		      "plane values for the reference object\n\n"
		      "Select \"Start\" to try again.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
	return;
      }
    }
    else {
      HGU_XmUserMessage(widget,
		    "Couldn't read the input value.\n\n"
		    "Select \"Start\" to try again.",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    }
  }
  else {
    return;
  }

  /* tp tracking must be on x-y planes only therefore reset the
     viewing angles to zero, fixed point to the origin, absolute
     view mode. */
  wlzViewStr->initialised = 0;
  wlzViewStr->scale       = 1.0;
  wlzViewStr->theta       = 0.0;
  wlzViewStr->phi         = 0.0;
  wlzViewStr->zeta        = 0.0;
  wlzViewStr->fixed.vtX   = 0.0;
  wlzViewStr->fixed.vtY   = 0.0;
  wlzViewStr->fixed.vtZ   = 0.0;
  wlzViewStr->dist        = startPlane;
  wlzViewStr->up.vtX	  = 0.0;
  wlzViewStr->up.vtY	  = 0.0;
  wlzViewStr->up.vtZ	  = -1.0;
  wlzViewStr->view_mode   = WLZ_ZETA_MODE;
  setViewMode(view_struct, wlzViewStr->view_mode);
  slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
  HGU_XmSetSliderValue
    (slider, (float) (wlzViewStr->zeta * 180.0 / WLZ_M_PI));
  reset_view_struct( view_struct );

  /* redisplay */
  XClearWindow(dpy, win);
  display_view_cb(view_struct->dialog, (XtPointer) view_struct, NULL);
  view_feedback_cb(view_struct->dialog, (XtPointer) view_struct, NULL);

  /* re-acquire the view domains for painting */
  getViewDomains(view_struct);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  /* get the next section, i.e. distance+1 and set it as the source image */
  tpTrackSetSrc(view_struct);

  /* reset the destination image */
  /* set the destination object and reset the ximage */
  if( warpGlobals.dst.obj ){
    WlzFreeObj(warpGlobals.dst.obj);
  }
  warpGlobals.dst.obj =
    WlzAssignObject(WlzGetSectionFromObject(globals.orig_obj,
					    view_struct->wlzViewStr,
					    WLZ_INTERPOLATION_NEAREST,
					    NULL),
		    NULL);
  warpSetXImage(&(warpGlobals.dst));
  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback, NULL);

  /* reset the overlay object */
  cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
  if( cobj->o[0] ){
    WlzFreeObj(cobj->o[0]);
  }
  cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);

  /* check for a standard-named bibfile and read tie-points if found */
  dstVtxs = srcVtxs = NULL;
  nPoints = warpGetBibfileTiePoints(tpTrackGetBibfileName(wlzViewStr->dist),
				    &dstVtxs, &srcVtxs);
  if( nPoints > 0 ){
    if( HGU_XmUserConfirm(widget,
		      "Previous tie-points found, install?",
		      "Yes", "No", 1) ){
      warpGlobals.num_vtxs = nPoints;
      for(i=0; i < nPoints; i++){
	warpGlobals.dst_vtxs[i].vtX = dstVtxs[i].vtX
	  - warpGlobals.dst.obj->domain.i->kol1;
	warpGlobals.dst_vtxs[i].vtY = dstVtxs[i].vtY
	  - warpGlobals.dst.obj->domain.i->line1;
	warpGlobals.src_vtxs[i].vtX = srcVtxs[i].vtX
	  + warpGlobals.srcXOffset;
	warpGlobals.src_vtxs[i].vtY = srcVtxs[i].vtY
	  + warpGlobals.srcYOffset;
      }
    }
    AlcFree(dstVtxs);
    AlcFree(srcVtxs);
  }

  /* calculate the warp transform and display */	
  warpRedisplayOvly();

  /* set warp dialog to the top */
  paint_key = view_struct;

  warpGlobals.tpTrackingInit = 1;
  win = XtWindow(XtParent(warpGlobals.warp2DInteractDialog));
  XRaiseWindow(dpy, win);
  XFlush(dpy);
  return;
}

void tpTrackNextCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Display		*dpy=XtDisplay(view_struct->canvas);
  Window		win=XtWindow(view_struct->canvas);
  WlzCompoundArray	*cobj;
  int			i, nPoints, vtxIdx;
  WlzDVertex2		*dstVtxs, *srcVtxs;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check if set up for tracking */
  if( !warpGlobals.tpTrackingInit ){
    return;
  }

  /* save current bibfile & tie-points */
  tpTrackSaveCb(widget, client_data, call_data);

  /* set tie-points to absolute values of source */
  for(i=0; i < warpGlobals.num_vtxs; i++){
    warpGlobals.src_vtxs[i].vtX -= warpGlobals.srcXOffset;
    warpGlobals.src_vtxs[i].vtY -= warpGlobals.srcYOffset;
    warpGlobals.dst_vtxs[i].vtX = warpGlobals.src_vtxs[i].vtX;
    warpGlobals.dst_vtxs[i].vtY = warpGlobals.src_vtxs[i].vtY;
  }  

  /* set target plane to distance+1 i.e. source plane */
  wlzViewStr->dist += 1.0;
  reset_view_struct( view_struct );

  /* redisplay */
  XClearWindow(dpy, win);
  display_view_cb(view_struct->dialog, (XtPointer) view_struct, NULL);
  view_feedback_cb(view_struct->dialog, (XtPointer) view_struct, NULL);

  /* re-acquire the view domains for painting */
  getViewDomains(view_struct);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  /* get the next section, i.e. distance+1 and set it as the source image */
  tpTrackSetSrc(view_struct);

  /* reset the destination image */
  /* set the destination object and reset the ximage */
  if( warpGlobals.dst.obj ){
    WlzFreeObj(warpGlobals.dst.obj);
  }
  warpGlobals.dst.obj =
    WlzAssignObject(WlzGetSectionFromObject(globals.orig_obj,
					    view_struct->wlzViewStr,
  					    WLZ_INTERPOLATION_NEAREST,
					    NULL),
		    NULL);
  warpSetXImage(&(warpGlobals.dst));
  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback, NULL);

  /* reset the overlay object */
  cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
  if( cobj->o[0] ){
    WlzFreeObj(cobj->o[0]);
  }
  cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);

  /* check for bibfile and read tie-points if found */
  dstVtxs = srcVtxs = NULL;
  nPoints = warpGetBibfileTiePoints(tpTrackGetBibfileName(wlzViewStr->dist),
				    &dstVtxs, &srcVtxs);

  /* ask if tie-points are to be merged */
  if( nPoints > 0 ){
    switch( HGU_XmUserConfirm3(widget,
			  "Previous tie-points found,\n"
			  "please select action:",
			   "merge", "ignore", "replace", 0) ){
    default:
    case 0:
      for(i=0; i < nPoints; i++){
	/* check against existing */
	if((vtxIdx = warpCloseDstVtx(dstVtxs[i].vtX,	
				     dstVtxs[i].vtY)) > -1 ){
	  /* don't add to the list */
	  continue;
	}
	else {
	  warpGlobals.dst_vtxs[warpGlobals.num_vtxs].vtX = dstVtxs[i].vtX;
	  warpGlobals.dst_vtxs[warpGlobals.num_vtxs].vtY = dstVtxs[i].vtY;
	  warpGlobals.src_vtxs[warpGlobals.num_vtxs].vtX = srcVtxs[i].vtX;
	  warpGlobals.src_vtxs[warpGlobals.num_vtxs].vtY = srcVtxs[i].vtY;
	  warpGlobals.num_vtxs++;
	}
      }
      break;

    case 1:
      break;

    case 2:
      warp2DInteractDeleteAllCb(view_struct->dialog,
			    (XtPointer) view_struct, NULL);
      for(i=0; i < nPoints; i++){
	warpGlobals.dst_vtxs[warpGlobals.num_vtxs].vtX = dstVtxs[i].vtX;
	warpGlobals.dst_vtxs[warpGlobals.num_vtxs].vtY = dstVtxs[i].vtY;
	warpGlobals.src_vtxs[warpGlobals.num_vtxs].vtX = srcVtxs[i].vtX;
	warpGlobals.src_vtxs[warpGlobals.num_vtxs].vtY = srcVtxs[i].vtY;
	warpGlobals.num_vtxs++;
      }
      break;

    }
    AlcFree(dstVtxs);
    AlcFree(srcVtxs);
  }
    
  /* Set tie-point off-sets */
  for(i=0; i < warpGlobals.num_vtxs; i++){
    warpGlobals.dst_vtxs[i].vtX -= warpGlobals.dst.obj->domain.i->kol1;
    warpGlobals.dst_vtxs[i].vtY -= warpGlobals.dst.obj->domain.i->line1;
    warpGlobals.src_vtxs[i].vtX += warpGlobals.srcXOffset;
    warpGlobals.src_vtxs[i].vtY += warpGlobals.srcYOffset;
  }  

  /* calculate the warp transform and display */
  warpRedisplayOvly();
  win = XtWindow(XtParent(warpGlobals.warp2DInteractDialog));
  XRaiseWindow(dpy, win);
  XFlush(dpy);

  return;
}

void tpTrackBackCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Display		*dpy=XtDisplay(view_struct->canvas);
  Window		win=XtWindow(view_struct->canvas);  
  WlzCompoundArray	*cobj;
  int			i, nPoints;
  WlzDVertex2		*dstVtxs, *srcVtxs;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check if set up for tracking */
  if( !warpGlobals.tpTrackingInit ){
    return;
  }

  /* save current bibfile & tie-points */
  tpTrackSaveCb(widget, client_data, call_data);

  /* remove tie-points */
  warp2DInteractDeleteAllCb(view_struct->dialog,
			    (XtPointer) view_struct, NULL);

  /* set target plane to distance-1 */
  wlzViewStr->dist -= 1.0;
  reset_view_struct( view_struct );

  /* redisplay */
  XClearWindow(dpy, win);
  display_view_cb(view_struct->dialog, (XtPointer) view_struct, NULL);
  view_feedback_cb(view_struct->dialog, (XtPointer) view_struct, NULL);

  /* re-acquire the view domains for painting */
  getViewDomains(view_struct);

  /* clear previous domains */
  view_struct_clear_prev_obj( view_struct );

  /* get the next section, i.e. distance+1 and set it as the source image */
  tpTrackSetSrc(view_struct);

  /* reset the destination image */
  /* set the destination object and reset the ximage */
  if( warpGlobals.dst.obj ){
    WlzFreeObj(warpGlobals.dst.obj);
  }
  warpGlobals.dst.obj =
    WlzAssignObject(WlzGetSectionFromObject(globals.orig_obj,
					    view_struct->wlzViewStr,
  					    WLZ_INTERPOLATION_NEAREST,
					    NULL),
		    NULL);
  warpSetXImage(&(warpGlobals.dst));
  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback, NULL);

  /* reset the overlay object */
  cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
  if( cobj->o[0] ){
    WlzFreeObj(cobj->o[0]);
  }
  cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);

  /* check for bibfile and read tie-points if found */
  dstVtxs = srcVtxs = NULL;
  nPoints = warpGetBibfileTiePoints(tpTrackGetBibfileName(wlzViewStr->dist),
				    &dstVtxs, &srcVtxs);
  if( nPoints > 0 ){	
    warpGlobals.num_vtxs = nPoints;
    for(i=0; i < nPoints; i++){
      warpGlobals.dst_vtxs[i].vtX = dstVtxs[i].vtX
	- warpGlobals.dst.obj->domain.i->kol1;
      warpGlobals.dst_vtxs[i].vtY = dstVtxs[i].vtY
	- warpGlobals.dst.obj->domain.i->line1;
      warpGlobals.src_vtxs[i].vtX = srcVtxs[i].vtX
	+ warpGlobals.srcXOffset;
      warpGlobals.src_vtxs[i].vtY = srcVtxs[i].vtY
	+ warpGlobals.srcYOffset;
    }
    AlcFree(dstVtxs);
    AlcFree(srcVtxs);
  }

  /* calculate the warp transform and display */
  warpRedisplayOvly();
  win = XtWindow(XtParent(warpGlobals.warp2DInteractDialog));
  XRaiseWindow(dpy, win);
  XFlush(dpy);

  return;
}


void tpTrackSaveCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  char		*file;
  XmString	xmstr;
  FILE		*fp;
  int		confirmFlg;

  /* just save current bibfile, this has a fixed naming convention,
     and it assumed that the current working directory is appropriate.
     However check for overwrite. */
  if( !warpGlobals.tpTrackingInit ){
    return;
  }

  /* save current warp params bibfile - create backup file */
  if( file = tpTrackGetBibfileName(view_struct->wlzViewStr->dist) ){
    if( xmstr = XmStringCreateSimple(file) ){
      confirmFlg = 0;
      if( fp = HGU_XmGetFilePointerBckCnfm(globals.topl, xmstr, NULL, "w",
					   ".bak", &confirmFlg) ){
	warpBibfileWrite(fp, view_struct);
	fclose(fp);
      }
      XmStringFree(xmstr);
    }
    AlcFree(file);
  }

  return;
}

static ActionAreaItem   warp_controls_actions[] = {
{"src_read",	NULL,		NULL},
{"sgnl_read",	NULL,		NULL},
{"re_mesh",	NULL,		NULL},
{"i_o",		NULL,           NULL},
{"import",	NULL,           NULL},
};

static ActionAreaItem   warp_rapid_actions[] = {
{"next",	NULL,		NULL},
{"select",	NULL,		NULL},
{"save",	NULL,		NULL},
{"save as",	NULL,		NULL},
};

static ActionAreaItem   tp_tracking_actions[] = {
{"start",	NULL,		NULL},
{"next",	NULL,		NULL},
{"back",	NULL,		NULL},
{"save",	NULL,		NULL},
};

static MenuItem mesh_menu_itemsP[] = {		/* mesh_menu items */
  {"gradient", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshMethodCb, (XtPointer) WLZ_MESH_GENMETHOD_GRADIENT,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"block", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshMethodCb, (XtPointer) WLZ_MESH_GENMETHOD_BLOCK,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem interpFunctionItemsP[] = { /* interp_function_menu items */
  {"multiquadric", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshFunctionCb, (XtPointer) WLZ_FN_BASIS_2DMQ,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"thin-plate spline", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshFunctionCb, (XtPointer) WLZ_FN_BASIS_2DTPS,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"polynomial", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshFunctionCb, (XtPointer) WLZ_FN_BASIS_2DPOLY,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
/*{"gaussian", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshFunctionCb, (XtPointer) WLZ_FN_BASIS_2DGAUSS,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},*/
  NULL,
};

static MenuItem affineTypeItemsP[] = { /* interp_function_menu items */
  {"noshear", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpAffineTypeCb, (XtPointer) WLZ_TRANSFORM_2D_NOSHEAR,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"rigid", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpAffineTypeCb, (XtPointer) WLZ_TRANSFORM_2D_REG,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"affine", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpAffineTypeCb, (XtPointer) WLZ_TRANSFORM_2D_AFFINE,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

Widget createWarpStandardControlsPage(
  Widget		notebook,
  ThreeDViewStruct	*view_struct)
{
  Widget	form, button, option_menu, slider;
  Widget	buttons, widget;
  float		fval, fmin, fmax;

  form = XtVaCreateWidget("warp_input_2d_form",
			  xmFormWidgetClass, 	notebook,
			  XmNnotebookChildType, XmPAGE,
			  NULL);  

  button = XtVaCreateManagedWidget("Standard",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, standardPageSelectCb, view_struct);

  /* now the controls */
  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "mesh_method", 0,
				XmTEAR_OFF_DISABLED, mesh_menu_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild(option_menu);

  /* set block by default */
  if( widget = XtNameToWidget(option_menu, "*.block") ){
    XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
    warpGlobals.meshMthd = WLZ_MESH_GENMETHOD_BLOCK;
  }
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

  XtManageChild(form);

  return form;
}

Widget createWarpRapidControlsPage(
  Widget		notebook,
  ThreeDViewStruct	*view_struct)
{
  Widget	form, button;
  Widget	label, buttons, widget;
  XmString	xmstr;

  form = XtVaCreateWidget("warp_input_rapid_form",
			  xmFormWidgetClass, 	notebook,
			  XmNnotebookChildType, XmPAGE,
			  NULL);  

  button = XtVaCreateManagedWidget("Rapid Map",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, rapidMapPageSelectCb, view_struct);

  /* label for warning message */
  label = XtVaCreateManagedWidget("rapidmap_label",
				  xmLabelWidgetClass, form,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNborderWidth,	1,
				  XmNshadowThickness,	2,
				  XmNalignment,		XmALIGNMENT_BEGINNING,
				  NULL);
  xmstr = XmStringCreateLtoR(
    "In rapid-map mode it is assumed that the bibfile parameters are correct and that\n"
    "all files should be installed without checking. Also all domains will be saved to\n"
    "the current directory and the current directory changed to that of the new bibfile.\n\n"
    "next - save, clear domains then select next bibfile from the input list\n"
    "select - save, clear domains then select next bibfile from browser\n"
    "save - save current warp transform file and all domains\n\n",
    XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(label, XmNlabelString, xmstr, NULL);
  XmStringFree(xmstr);

  /* now some buttons */
  buttons = HGU_XmCreateActionArea(form,
				    warp_rapid_actions,
				    XtNumber(warp_rapid_actions));

  /* set the buttons attachments */
  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		label,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  /* now the callbacks */
  if( widget = XtNameToWidget(buttons, "*.next") ){
    XtAddCallback(widget, XmNactivateCallback, rapidMapNextCb,
		  view_struct);
  }
  if( widget = XtNameToWidget(buttons, "*.select") ){
    XtAddCallback(widget, XmNactivateCallback, rapidMapSelectCb,
		  view_struct);
  }
  if( widget = XtNameToWidget(buttons, "*.save") ){
    XtAddCallback(widget, XmNactivateCallback, rapidMapSaveCb,
		  view_struct);
  }
  if( widget = XtNameToWidget(buttons, "*.save as") ){
    XtAddCallback(widget, XmNactivateCallback, rapidMapSaveAsCb,
		  view_struct);
  }

  XtManageChild(form);

  return form;
}

Widget createTiePointTrackingControlsPage(
  Widget		notebook,
  ThreeDViewStruct	*view_struct)
{
  Widget	form, button;
  Widget	label, buttons, widget;
  XmString	xmstr;

  form = XtVaCreateWidget("warptie_point_tracking_form",
			  xmFormWidgetClass, 	notebook,
			  XmNnotebookChildType, XmPAGE,
			  NULL);  

  button = XtVaCreateManagedWidget("TP Tracking",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, tpTrackPageSelectCb, view_struct);

  /* label for warning message */
  label = XtVaCreateManagedWidget("tp_track_label",
				  xmLabelWidgetClass, form,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNborderWidth,	1,
				  XmNshadowThickness,	2,
				  XmNalignment,		XmALIGNMENT_BEGINNING,
				  NULL);
  xmstr = XmStringCreateLtoR(
    "Tie-point tracking mode:\n\n"
    "  Tie-point tracks can only be started in the target window.\n"
    "  Tie-points can only be deleted in the source window.\n"
    "  Target tie-points can only be adjusted before the source vertex has been defined.\n"
    "  Only the source vertex can be edited when both source and target are defined\n"
    "\n",
    XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(label, XmNlabelString, xmstr, NULL);
  XmStringFree(xmstr);

  /* now some buttons */
  buttons = HGU_XmCreateActionArea(form,
				   tp_tracking_actions,
				   XtNumber(tp_tracking_actions));

  /* set the buttons attachments */
  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		label,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  /* now the callbacks */
  if( widget = XtNameToWidget(buttons, "*.start") ){
    XtAddCallback(widget, XmNactivateCallback, tpTrackStartCb,
		  view_struct);
  }
  if( widget = XtNameToWidget(buttons, "*.next") ){
    XtAddCallback(widget, XmNactivateCallback, tpTrackNextCb,
		  view_struct);
  }
  if( widget = XtNameToWidget(buttons, "*.back") ){
    XtAddCallback(widget, XmNactivateCallback, tpTrackBackCb,
		  view_struct);
  }
  if( widget = XtNameToWidget(buttons, "*.save") ){
    XtAddCallback(widget, XmNactivateCallback, tpTrackSaveCb,
		  view_struct);
  }

   
  XtManageChild(form);

  return form;
}
