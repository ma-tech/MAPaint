#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Woolz Library					*
*   File       :   MAWarpInputUtils.c					*
*************************************************************************
* This module has been copied from the original woolz library and       *
* modified for the public domain distribution. The original authors of  *
* the code and the original file headers and comments are in the        *
* HISTORY file.                                                         *
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Nov 29 13:30:38 1999				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <MAPaint.h>
#include <MAWarp.h>

extern Widget warp_read_src_dialog;
extern Widget warp_read_sgnl_dialog;
extern WlzObject *mapaintWarpObj(
  WlzObject	*obj,
  WlzInterpolationType	interpType);

WlzErrorNum WlzSetMeshAffineProduct(
  WlzMeshTransform	*meshTr,
  WlzAffineTransform	*affineTr)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;
  WlzDVertex2	vtx;

  /* check the mesh transform */
  if( meshTr ){
    if( meshTr->nodes == NULL ){
      errNum = WLZ_ERR_PARAM_NULL;
    }
  }
  else {
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* check the affine transform */
  if( errNum == WLZ_ERR_NONE ){
    if( affineTr == NULL ){
      errNum = WLZ_ERR_PARAM_NULL;
    }
  }

  /* loop through nodes, resetting the displacement */
  if( errNum == WLZ_ERR_NONE ){
    for(i=0; i < meshTr->nNodes; i++){
      vtx.vtX = meshTr->nodes[i].position.vtX +
	meshTr->nodes[i].displacement.vtX;
      vtx.vtY = meshTr->nodes[i].position.vtY +
	meshTr->nodes[i].displacement.vtY;
      vtx = WlzAffineTransformVertexD2(affineTr, vtx, &errNum);
      vtx.vtX -= meshTr->nodes[i].position.vtX;
      vtx.vtY -= meshTr->nodes[i].position.vtY;
      meshTr->nodes[i].displacement = vtx;
    }
  }

  return errNum;
}

void warpSetOvlyObject(void)
{
  WlzAffineTransform	*trans;
  WlzObject		*transObj, *tmpPoly;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  WlzCompoundArray	*cobj;
  WlzDVertex2		srcVtxs[WARP_MAX_NUM_VTXS];
  WlzDVertex2		dstVtxs[WARP_MAX_NUM_VTXS];
  int 			i, badElm, polyOrder=4;
  WlzDomain		domain;
  WlzValues		values;
  WlzDVertex2		*transVtxs, tmpVtx;
  double		delta;
  int			invertFlg;

  /* define the affine transform from the tie points */
  if( warpGlobals.num_vtxs < 1 ){
    if( warpGlobals.ovly.obj ){
      cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
      if( cobj->n > 1 ){
	if( cobj->o[1] ){
	  WlzFreeObj(cobj->o[1]);
	  cobj->o[1] = NULL;
	}
	cobj->n = 1;
      }
    }
    return;
  }
  if( !warpGlobals.src.obj ){
    return;
  }
  for(i=0; i < warpGlobals.num_vtxs; i++){
    srcVtxs[i] = warpGlobals.src_vtxs[i];
    srcVtxs[i].vtX += warpGlobals.src.obj->domain.i->kol1;
    srcVtxs[i].vtY += warpGlobals.src.obj->domain.i->line1;
    dstVtxs[i] = warpGlobals.dst_vtxs[i];
    dstVtxs[i].vtX += warpGlobals.dst.obj->domain.i->kol1;
    dstVtxs[i].vtY += warpGlobals.dst.obj->domain.i->line1;
  }
  if( (trans = WlzAffineTransformLSq2D(warpGlobals.num_vtxs, srcVtxs,
				       warpGlobals.num_vtxs, dstVtxs,
				       warpGlobals.affineType,
				       &errNum)) == NULL ){
    (void) WlzStringFromErrorNum(errNum, &warpErrStr);
    sprintf(warpErrBuf,
	    "2D Warp Object Input:\n"
	    "    Failed to determine the best fit affine.\n"
	    "    Please delete some of the tie-points\n"
	    "    and try again.\n\n"
	    "Wlz error: %s", warpErrStr);
    HGU_XmUserError(globals.topl, warpErrBuf,
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }
  if( warpGlobals.affine ){
    WlzFreeAffineTransform(warpGlobals.affine);
  }
  warpGlobals.affine = WlzAssignAffineTransform(trans, NULL);

  /* if < 4 tie-points, apply  transform to the source object */
  if( warpGlobals.num_vtxs < 4 ){
    if( transObj = WlzAffineTransformObj(warpGlobals.src.obj,
					 trans, WLZ_INTERPOLATION_NEAREST,
					 &errNum) ){
      if( warpGlobals.ovly.obj ){
	cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
	if( cobj->n > 1 ){
	  if( cobj->o[1] ){
	    WlzFreeObj(cobj->o[1]);
	  }
	}
	else {
	  cobj->n = 2;
	}
	cobj->o[1] = WlzAssignObject(transObj, &errNum);
      }
      else {
	WlzFreeObj(transObj);
      }
    }
    else {
      (void) WlzStringFromErrorNum(errNum, &warpErrStr);
      sprintf(warpErrBuf,
	      "2D Warp Object Input:\n"
	      "    Failed to affine transform the\n"
	      "    source object please move some\n"
	      "    of the tie points and try again.\n\n"
	      "Wlz error: %s", warpErrStr);
      HGU_XmUserError(globals.topl, warpErrBuf,
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }

    if( errNum != WLZ_ERR_NONE ){
      MAPaintReportWlzError(globals.topl, "warpSetOvlyObject", errNum);
    }
    return;
  }

  /* apply the inverse transform to the dst tie-points */
  if( trans = WlzAffineTransformInverse(warpGlobals.affine, &errNum) ){
    for(i=0; i < warpGlobals.num_vtxs; i++){
      dstVtxs[i] = WlzAffineTransformVertexD2(trans, dstVtxs[i], &errNum);
    }
    WlzFreeAffineTransform(trans);
  }
  else {
    MAPaintReportWlzError(globals.topl, "warpSetOvlyObject", errNum);
    return;
  }

  /* calculate the warp transform */
  if( warpGlobals.basisTr ){
    WlzBasisFnFreeTransform(warpGlobals.basisTr);
  }
  polyOrder = WLZ_MIN(4, warpGlobals.num_vtxs - 1);
  if( !(warpGlobals.basisTr =
     WlzBasisFnTrFromCPts2D(warpGlobals.wlzFnType, polyOrder,
			    warpGlobals.num_vtxs, srcVtxs,
			    warpGlobals.num_vtxs, dstVtxs,
			    &errNum)) ){

    (void) WlzStringFromErrorNum(errNum, &warpErrStr);
    sprintf(warpErrBuf,
	    "2D Warp Object Input:\n"
	    "    Failed to calculate the basis\n"
	    "    transform, please move some\n"
	    "    of the tie points and try again.\n\n"
	    "Wlz error: %s", warpErrStr);
    HGU_XmUserError(globals.topl, warpErrBuf,
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }
/*  delta = 0.0;
  for(i=0; i < warpGlobals.num_vtxs; i++){
    tmpVtx = WlzBasisFnTransformVertexD(warpGlobals.basisTr,
					    srcVtxs[i], NULL);
    delta += ((tmpVtx.vtX - dstVtxs[i].vtX) *
	      (tmpVtx.vtX - dstVtxs[i].vtX) +
	      (tmpVtx.vtY - dstVtxs[i].vtY) *
	      (tmpVtx.vtY - dstVtxs[i].vtY));
  }
  delta = sqrt(delta) / warpGlobals.num_vtxs;
  fprintf(stderr, "Warp mean RMS error, no-affine, basisFn = %f\n", delta);*/

  /* set the transform mesh and apply the affine transform */
  if((errNum = WlzBasisFnSetMesh(warpGlobals.meshTr, warpGlobals.basisTr))
     != WLZ_ERR_NONE){
    (void) WlzStringFromErrorNum(errNum, &warpErrStr);
    sprintf(warpErrBuf,
	    "2D Warp Object Input:\n"
	    "    Failed to apply the basis transform\n"
	    "    to the mesh, please move some of\n"
	    "    the tie points and try again.\n\n"
	    "Wlz error: %s", warpErrStr);
    HGU_XmUserError(globals.topl, warpErrBuf,
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }
/*  domain.poly = WlzMakePolyDmn(WLZ_POLYGON_DOUBLE,
			       (WlzIVertex2 *) &(srcVtxs[0]),
			       warpGlobals.num_vtxs, WARP_MAX_NUM_VTXS,
			       0, &errNum);
  values.core = NULL;
  tmpPoly = WlzMakeMain(WLZ_2D_POLYGON, domain, values,
			NULL, NULL, NULL);
  if( transObj = WlzMeshTransformObj(tmpPoly,
				     warpGlobals.meshTr,
				     WLZ_INTERPOLATION_NEAREST,
				     &errNum) ){
    transVtxs = (WlzDVertex2 *) transObj->domain.poly->vtx;
    delta = 0.0;
    for(i=0; i < warpGlobals.num_vtxs; i++){
      delta += ((transVtxs[i].vtX - dstVtxs[i].vtX) *
		(transVtxs[i].vtX - dstVtxs[i].vtX) +
		(transVtxs[i].vtY - dstVtxs[i].vtY) *
		(transVtxs[i].vtY - dstVtxs[i].vtY));
    }
    delta = sqrt(delta) / warpGlobals.num_vtxs;
    fprintf(stderr, "Warp mean RMS error, no-affine, mesh = %f\n", delta);
    WlzFreeObj(transObj);
  }
  WlzFreeObj(tmpPoly);*/
  /* test for inversion */
  delta = (warpGlobals.affine->mat[0][0] * warpGlobals.affine->mat[1][1])
    - (warpGlobals.affine->mat[0][1] * warpGlobals.affine->mat[1][0]);
  if( delta < 0.0 ){
    invertFlg = 1;
  }
  else {
    invertFlg = 0;
  }
  if( !invertFlg ){
    WlzSetMeshAffineProduct(warpGlobals.meshTr, warpGlobals.affine);
  }

  /* verify the mesh */
  if((errNum = WlzMeshTransformVerify(warpGlobals.meshTr, 1,
				      &badElm, NULL)) != WLZ_ERR_NONE){
    (void) WlzStringFromErrorNum(errNum, &warpErrStr);
    sprintf(warpErrBuf,
	    "2D Warp Object Input:\n"
	    "    The tie-points set generate an invalid\n"
	    "    mesh, select yes to attempt to correct\n"
	    "    the mesh automatically or no if you\n"
	    "    want to move some of the tie-points\n"
	    "    or reset the mesh parameters and try again.\n\n"
	    "Wlz error: %s", warpErrStr);
    if( HGU_XmUserConfirm(globals.topl, warpErrBuf,
			  "Yes", "No", 1) ){
      WlzMeshTransform *aMesh = NULL;
      const double minElmArea = 1.0;
      /* adapt mesh */
      if( !(aMesh = WlzMeshTransformAdapt(warpGlobals.meshTr,
					  minElmArea, &errNum)) ){
	(void) WlzStringFromErrorNum(errNum, &warpErrStr);
	sprintf(warpErrBuf,
		"2D Warp Object Input:\n"
		"    Failed to correct the mesh,\n"
		"    please move some of\n"
		"    the tie points and try again.\n\n"
		"Wlz error: %s", warpErrStr);
	HGU_XmUserError(globals.topl, warpErrBuf,
			XmDIALOG_FULL_APPLICATION_MODAL);
	return;
      }
      (void )WlzMeshFreeTransform(warpGlobals.meshTr);
      warpGlobals.meshTr = aMesh;
    }
    else {
      warpGlobals.meshErrFlg = 1;
      return;
    }
  }
  warpGlobals.meshErrFlg = 0;

  /* apply to the tie points to check errors */
/*  for(i=0; i < warpGlobals.num_vtxs; i++){
    dstVtxs[i] = warpGlobals.dst_vtxs[i];
    dstVtxs[i].vtX += warpGlobals.dst.obj->domain.i->kol1;
    dstVtxs[i].vtY += warpGlobals.dst.obj->domain.i->line1;
  }
  domain.poly = WlzMakePolyDmn(WLZ_POLYGON_DOUBLE,
			       (WlzIVertex2 *) &(srcVtxs[0]),
			       warpGlobals.num_vtxs, WARP_MAX_NUM_VTXS,
			       0, &errNum);
  values.core = NULL;
  tmpPoly = WlzMakeMain(WLZ_2D_POLYGON, domain, values,
			NULL, NULL, NULL);
  if( transObj = WlzMeshTransformObj(tmpPoly,
				     warpGlobals.meshTr,
				     WLZ_INTERPOLATION_NEAREST,
				     &errNum) ){
    transVtxs = (WlzDVertex2 *) transObj->domain.poly->vtx;
    delta = 0.0;
    for(i=0; i < warpGlobals.num_vtxs; i++){
      delta += ((transVtxs[i].vtX - dstVtxs[i].vtX) *
		(transVtxs[i].vtX - dstVtxs[i].vtX) +
		(transVtxs[i].vtY - dstVtxs[i].vtY) *
		(transVtxs[i].vtY - dstVtxs[i].vtY));
    }
    delta = sqrt(delta) / warpGlobals.num_vtxs;
    fprintf(stderr, "Warp mean RMS tie-point error after affine = %f\n\n", delta);
    WlzFreeObj(transObj);
  }
  WlzFreeObj(tmpPoly);*/

  /* apply to the original object and reset the overlay object */
  if( transObj = WlzMeshTransformObj(warpGlobals.src.obj,
				     warpGlobals.meshTr,
				     WLZ_INTERPOLATION_NEAREST,
				     &errNum) ){
    /* if inverted then need to do the affine transform here
       and transform the mesh so it will display properly */
    if( invertFlg ){
      WlzObject	*tmpObj;
      transObj = WlzAssignObject(transObj, &errNum);
      tmpObj = WlzAffineTransformObj(transObj, warpGlobals.affine, WLZ_INTERPOLATION_NEAREST, &errNum);
      WlzSetMeshAffineProduct(warpGlobals.meshTr, warpGlobals.affine);
      WlzFreeObj(transObj);
      transObj = tmpObj;
    }

    if( warpGlobals.ovly.obj ){
      cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
      if( cobj->n > 1 ){
	if( cobj->o[1] ){
	  WlzFreeObj(cobj->o[1]);
	}
      }
      else {
	cobj->n = 2;
      }
      cobj->o[1] = WlzAssignObject(transObj, &errNum);
    }
    else {
      WlzFreeObj(transObj);
    }
  }
  else {
    (void) WlzStringFromErrorNum(errNum, &warpErrStr);
    sprintf(warpErrBuf,
	    "2D Warp Object Input:\n"
	    "    Failed to apply the warp transform\n"
	    "    to the source object, please move\n"
	    "    some of the tie points and try again.\n\n"
	    "Wlz error: %s", warpErrStr);
    HGU_XmUserError(globals.topl, warpErrBuf,
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  return;
}

void warpInput2DDestroyCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget		widget;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  if( widget = XtNameToWidget(globals.topl, "*options_menu*warp_input_2d") ){
    XtSetSensitive(widget, True);
  }

  if( paint_key == view_struct ){
    paint_key = NULL;
  }

  if( warpGlobals.dst.obj ){
    WlzFreeObj(warpGlobals.dst.obj);
    warpGlobals.dst.obj = NULL;
  }

  if( warpGlobals.src.obj ){
    WlzFreeObj(warpGlobals.src.obj);
    warpGlobals.src.obj = NULL;
  }
  
  if( warpGlobals.sgnl.obj ){
    WlzFreeObj(warpGlobals.src.obj);
    warpGlobals.sgnl.obj = NULL;
  }

  warp_read_src_dialog=NULL;
  warp_read_sgnl_dialog=NULL;
  
  return;
}

void warpIOWrite(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  String		fileStr;
  FILE			*fp;
  int			i;
  BibFileRecord		*bibfileRecord;
  static char		unknownS[] = "unknown";
  time_t		tmpTime;
  char			*tmpS,
                        *idxS = NULL,
                        *dateS = NULL,
                        *hostS = NULL,
                        *userS = NULL,
                        *refFileS = NULL,
                        *srcFileS, *sgnlFileS;
  char			tmpBuf[256];
  Widget		toggle, slider;
  Boolean		normFlg=0, histoFlg=0, shadeFlg=0, gaussFlg=0;
  double		width=1.0;
  WlzEffBibWarpInputThresholdParamsStruct	threshParamStr;

  /* get a filename for the warp parameters */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a filename\n"
				      "for the section, warp and tie-\n"
				      "point parameters to be saved\n"
				      "as plain text",
				      "OK", "cancel",
				      warpGlobals.warpBibFile? 
				      warpGlobals.warpBibFile:"MAPaintWarpParams.bib",
				      NULL, "*.bib") ){
    if( fp = fopen(fileStr, "w") ){

      if( warpGlobals.warpBibFile ){
	AlcFree( warpGlobals.warpBibFile );
      }
      warpGlobals.warpBibFile = AlcStrDup(fileStr);

      /* write some sort of identifier */
      bibfileRecord = 
	BibFileRecordMake("Ident", "0",
			  BibFileFieldMakeVa("Text",
					     "MAPaint 2D warp input parameters",
					     "Version",	"1",
					     NULL));
      BibFileRecordWrite(fp, NULL, bibfileRecord);
      BibFileRecordFree(&bibfileRecord);

      /* now a comment with user, machine, date etc. */
      tmpS = getenv("USER");
      (void )sprintf(tmpBuf, "User: %s", tmpS?tmpS:unknownS);
      userS = AlcStrDup(tmpBuf);

      tmpTime = time(NULL);
      tmpS = ctime(&tmpTime);
      *(tmpS + strlen(tmpS) - 1) = '\0';
      (void )sprintf(tmpBuf, "Date: %s", tmpS?tmpS:unknownS);
      dateS = AlcStrDup(tmpBuf);

      tmpS = getenv("HOST");
      (void )sprintf(tmpBuf, "Host: %s", tmpS?tmpS:unknownS);
      hostS = AlcStrDup(tmpBuf);

      (void )sprintf(tmpBuf, "RefFile: %s", globals.file?globals.file:unknownS);
      refFileS = AlcStrDup(tmpBuf);

      (void )sprintf(tmpBuf, "SrcFile: %s",
		     warpGlobals.srcFile?warpGlobals.srcFile:unknownS);
      srcFileS = AlcStrDup(tmpBuf);

      (void )sprintf(tmpBuf, "SignalFile: %s",
		     warpGlobals.sgnlFile?warpGlobals.sgnlFile:unknownS);
      sgnlFileS = AlcStrDup(tmpBuf);

      bibfileRecord = 
	BibFileRecordMake("Comment", "0",
			  BibFileFieldMakeVa("Text", userS,
					     "Text", dateS,
					     "Text", hostS,
					     "Text", refFileS,
					     "Text", srcFileS,
					     "Text", sgnlFileS,
					     NULL));
      BibFileRecordWrite(fp, NULL, bibfileRecord);
      BibFileRecordFree(&bibfileRecord);

      /* if write a file record for the reference file */
      if( globals.file ){
	WlzEffBibWriteFileRecord(fp, "MAPaintReferenceFile",
			  globals.file,
			  globals.origObjExtType);
      }

      /* if defined write a file record for the source */
      if( warpGlobals.srcFile ){
	WlzEffBibWriteFileRecord(fp, "MAPaintWarpInputSourceFile",
			  warpGlobals.srcFile,
			  warpGlobals.srcFileType);
      }

      /* if defined write a file record for the signal */
      if( warpGlobals.sgnlFile ){
	WlzEffBibWriteFileRecord(fp, "MAPaintWarpInputSignalFile",
			  warpGlobals.sgnlFile,
			  warpGlobals.sgnlFileType);
      }


      /* write the section data */
      if( WlzEffBibWrite3DSectionViewParamsRecord(fp, "Wlz3DSectionViewParams", 
					      wlzViewStr) != WLZ_ERR_NONE ){
	HGU_XmUserError(globals.topl,
			"Save Warp Parameters:\n"
			"    Error in writing the bibfile\n"
			"    Please check disk space or quotas\n"
			"    Section parameters not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }

      /* write the warp transform parameters */
      if( WlzEffBibWriteWarpTransformParamsRecord(fp, "WlzWarpTransformParams",
						  warpGlobals.wlzFnType,
						  warpGlobals.affineType,
						  warpGlobals.meshMthd,
						  warpGlobals.meshMinDst,
						  warpGlobals.meshMaxDst)
	 != WLZ_ERR_NONE ){
	HGU_XmUserError(globals.topl,
			"Save Warp Parameters:\n"
			"    Error in writing the bibfile\n"
			"    Please check disk space or quotas\n"
			"    Section parameters not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }

      /* write the signal pre-processing and filter transform parameters */
      if( toggle = XtNameToWidget(globals.topl,
				  "*warp_sgnl_controls_form*normalise") ){
	XtVaGetValues(toggle, XmNset, &normFlg, NULL);
      }
      if( toggle = XtNameToWidget(globals.topl,
				  "*warp_sgnl_controls_form*histo_equalise") ){
	XtVaGetValues(toggle, XmNset, &histoFlg, NULL);
      }
      if( toggle = XtNameToWidget(globals.topl,
				  "*warp_sgnl_controls_form*shade_correction") ){
	XtVaGetValues(toggle, XmNset, &shadeFlg, NULL);
      }
      if( toggle = XtNameToWidget(globals.topl,
				  "*warp_sgnl_controls_form*gauss_smooth") ){
	XtVaGetValues(toggle, XmNset, &gaussFlg, NULL);
      }
      if( slider = XtNameToWidget(globals.topl,
				  "*warp_sgnl_controls_form*gauss_width") ){
	width = HGU_XmGetSliderValue(slider);
      }
      if( WlzEffBibWriteWarpInputSegmentationParamsRecord
	 (fp, "MAPaintWarpInputSegmentationParams",
	  normFlg==True?1:0,
	  histoFlg==True?1:0,
	  shadeFlg==True?1:0,
	  gaussFlg==True?1:0,
	  width) != WLZ_ERR_NONE ){
	HGU_XmUserError(globals.topl,
	 		"Save Pre-Processing Parameters:\n"
		 	"    Error in writing the bibfile\n"
		 	"    Please check disk space or quotas\n"
		 	"    Section parameters not saved",
		 	XmDIALOG_FULL_APPLICATION_MODAL);
      }

      /* write the signal thresholding parameters */
      threshParamStr.thresholdType = warpGlobals.thresholdType;
      threshParamStr.threshRGBSpace = warpGlobals.threshRGBSpace;
      threshParamStr.threshColorChannel = warpGlobals.threshColorChannel;
      threshParamStr.threshRangeLow = warpGlobals.threshRangeLow;
      threshParamStr.threshRangeHigh = warpGlobals.threshRangeHigh;
      for(i=0; i < 3; i++){
	threshParamStr.threshRangeRGBLow[i] = warpGlobals.threshRangeRGBLow[i];
	threshParamStr.threshRangeRGBHigh[i] = warpGlobals.threshRangeRGBHigh[i];
      }
      threshParamStr.threshRGBCombination = warpGlobals.threshRGBCombination;
      threshParamStr.lowRGBPoint = warpGlobals.lowRGBPoint;
      threshParamStr.highRGBPoint = warpGlobals.highRGBPoint;
      threshParamStr.colorEllipseEcc = warpGlobals.colorEllipseEcc;
      threshParamStr.globalThreshFlg = warpGlobals.globalThreshFlg;
      threshParamStr.globalThreshVtx = warpGlobals.globalThreshVtx;
      threshParamStr.incrThreshFlg = warpGlobals.incrThreshFlg;
      threshParamStr.pickThreshFlg = warpGlobals.pickThreshFlg;
      threshParamStr.distanceThreshFlg = warpGlobals.distanceThreshFlg;
      if( WlzEffBibWriteWarpInputThresholdParamsRecord
	 (fp, "MAPaintWarpInputThresholdParams",
	  &threshParamStr) != WLZ_ERR_NONE ){
	HGU_XmUserError(globals.topl,
	 		"Save Threshold Parameters:\n"
		 	"    Error in writing the bibfile\n"
		 	"    Please check disk space or quotas\n"
		 	"    Section parameters not saved",
		 	XmDIALOG_FULL_APPLICATION_MODAL);
      }

      /* write the signal post-processing parameters */

      /* write the tie points only if there is a src image */
      if( warpGlobals.src.obj ){
	for(i=0; i < warpGlobals.num_vtxs; i++){
	  WlzDVertex3	vtx1, vtx2;
	  vtx1.vtX = warpGlobals.dst_vtxs[i].vtX;
	  vtx1.vtY = warpGlobals.dst_vtxs[i].vtY;
	  vtx1.vtZ = 0.0;
	  if(warpGlobals.dst.obj != NULL){
	    vtx1.vtX += warpGlobals.dst.obj->domain.i->kol1;
	    vtx1.vtY += warpGlobals.dst.obj->domain.i->line1;
	  }
	  vtx2.vtX = warpGlobals.src_vtxs[i].vtX;
	  vtx2.vtY = warpGlobals.src_vtxs[i].vtY;
	  vtx2.vtZ = 0.0;
	  if(warpGlobals.src.obj != NULL){
	    vtx2.vtX -= warpGlobals.srcXOffset;
	    vtx2.vtY -= warpGlobals.srcYOffset;
	  }
	  if( WlzEffBibWriteTiePointVtxsRecord(fp, "WlzTiePointVtxs", i,
					       vtx1, vtx2, 0)
	     != WLZ_ERR_NONE ){
	    HGU_XmUserError(globals.topl,
			    "Save Warp Parameters:\n"
			    "    Error in writing the bibfile\n"
			    "    Please check disk space or quotas\n"
			    "    Section parameters not saved",
			    XmDIALOG_FULL_APPLICATION_MODAL);
	    break;
	  }
	}
      }

      /* close the file */
      if( fclose(fp) == EOF ){
	HGU_XmUserError(globals.topl,
			"Save Warp Parameters:\n"
			"    Error in closing the bibfile\n"
			"    Please check disk space or quotas\n"
			"    Section parameters not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }
    }
    else {
      HGU_XmUserError(globals.topl,
		      "Save Warp Parameters:\n"
		      "    Couldn't open the file for\n"
		      "    writing, please check\n"
		      "    permissions.\n"
		      "    Warp parameters not saved",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }
    AlcFree(fileStr);
  }

  return;
}

void warpIORead(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  String		fileStr;
  FILE			*fp;
  double		oldScale, newScale, newZeta;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win  = XtWindow(view_struct->canvas);
  Widget		option_menu, widget, slider;
  WlzCompoundArray	*cobj;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  WlzObject		*obj;
  WlzEffBibWarpInputThresholdParamsStruct	threshParamStr;
  int			resetOvlyFlg=0;

  /* get a filename for the warp parameters */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a filename\n"
				      "for the section, warp and tie-\n"
				      "point parameters to be read in.\n"
				      "WARNING: this will delete all\n"
				      "current tie-points",
				      "OK", "cancel",
				      warpGlobals.warpBibFile? 
				      warpGlobals.warpBibFile:"MAPaintWarpParams.bib",
				      NULL, "*.bib") ){
    warp2DInteractDeleteAllCb(w, client_data, call_data);

    if( fp = fopen(fileStr, "r") ){
      BibFileRecord	*bibfileRecord;
      BibFileError	bibFileErr;
      char		*errMsg;

      if( warpGlobals.warpBibFile ){
	AlcFree( warpGlobals.warpBibFile );
      }
      warpGlobals.warpBibFile = AlcStrDup(fileStr);

      /* read the bibfile */
      bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
      while( bibFileErr == BIBFILE_ER_NONE ){
	/* check for view parameters - reset dst image if necessary */
	if( !strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22) ){
	  oldScale = wlzViewStr->scale;
	  WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, wlzViewStr);
	  view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

	  /* reset the sliders and mode control */
	  slider = XtNameToWidget(view_struct->dialog, "*.theta_slider");
	  HGU_XmSetSliderValue
	    (slider, (float) (wlzViewStr->theta * 180.0 / WLZ_M_PI));
	  slider = XtNameToWidget(view_struct->dialog, "*.phi_slider");
	  HGU_XmSetSliderValue
	    (slider, (float) (wlzViewStr->phi * 180.0 / WLZ_M_PI));
	  slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
	  HGU_XmSetSliderValue
	    (slider, (float) (wlzViewStr->zeta * 180.0 / WLZ_M_PI));

	  /* extract the scale and the mode to reset the GUI */
	  newScale = wlzViewStr->scale;
	  newZeta = wlzViewStr->zeta;
	  wlzViewStr->scale = oldScale;
	  setViewMode(view_struct, wlzViewStr->view_mode);
	  setViewScale(view_struct, newScale, 0, 0);

	  /* check the section transform - trap mode/zeta bug
	     note setViewMode will reset zeta to whatever is
	     appropriate for the mode */
	  if( fabs(newZeta - wlzViewStr->zeta) > 0.01 ){
	    switch( wlzViewStr->view_mode ){
	    case WLZ_UP_IS_UP_MODE:
	      /* probably should have been in zeta-mode */
	      if( HGU_XmUserConfirm(
		    view_struct->dialog,
		    "Warning: The view-mode and roll angle are inconsistent\n"
		    "    Selecting absolute mode will probably fix the\n"
		    "    problem. To check, select Absolute below and check\n"
		    "    if the input tie-points are correct.\n\n"
		    "    If they are not then re-read the bibfile and\n"
		    "    select up-is-up.\n\n"
		    "    If this still fails then save any work and ask\n"
		    "    emap-tech@hgu.mrc.ac.uk\n"
		    "    If the tie-points are correct then save a new\n"
		    "    bibfile to record the correct values\n\n"
		    "Select Absolute or Up-is-Up below:",
		    "Absolute", "Up-is-Up", 0) ){
		wlzViewStr->view_mode = WLZ_ZETA_MODE;
		wlzViewStr->zeta = newZeta;
	      }
	      break;

	    case WLZ_STATUE_MODE:
	      /* probably should have been in zeta-mode */
	      if( HGU_XmUserConfirm(view_struct->dialog,
				    "Warning: The view-mode and roll angle\n"
				    "    are inconsistent. Selecting\n"
				    "    absolute mode will probably fix\n"
				    "    the problem. To check select\n"
				    "    absolute below and check if the\n"
				    "    input tie-points are correct.\n"
				    "    If they are not then re-read the\n"
				    "    bibfile and select statue\n"
				    "    If this still fails then ask\n"
				    "    emap-tech@hgu.mrc.ac.uk for help\n\n"
				    "    If the tie-points are correct then\n"
				    "    save the bibfile to record the\n"
				    "    correct settings\n\n"
				    "Select Absolute or Statue below:",
				    "Absolute", "Statue", 0) ){
		wlzViewStr->view_mode = WLZ_ZETA_MODE;
		wlzViewStr->zeta = newZeta;
	      }
	      break;

	    case WLZ_ZETA_MODE:
	      /* should not happen, silently just reset slider */
	      break;

	    default: /* set zeta mode - what else? */
	      wlzViewStr->view_mode = WLZ_ZETA_MODE;
	      wlzViewStr->zeta = newZeta;
	      break;
	    }

	    setViewMode(view_struct, wlzViewStr->view_mode);
	    slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
	    HGU_XmSetSliderValue
	      (slider, (float) (wlzViewStr->zeta * 180.0 / WLZ_M_PI));
	    reset_view_struct( view_struct );
	  }


	  /* redisplay */
	  XClearWindow(dpy, win);
	  display_view_cb(w, (XtPointer) view_struct, call_data);
	  view_feedback_cb(w, (XtPointer) view_struct, NULL);

	  /* clear previous domains */
	  view_struct_clear_prev_obj( view_struct );

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
	  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback, call_data);

	  /* reset the overlay object */
	  resetOvlyFlg = 1;
	  cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
	  if( cobj->o[0] ){
	    WlzFreeObj(cobj->o[0]);
	  }
	  cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);
	}

	/* check for warp transform parameters */
	if( !strncmp(bibfileRecord->name, "WlzWarpTransformParams", 22) ){
	  WlzFnType		wlzFnType;
	  WlzMeshGenMethod	meshMthd;
	  int			meshMinDst;
	  int	 		meshMaxDst;
	  WlzTransformType	affineType;

	  WlzEffBibParseWarpTransformParamsRecord(bibfileRecord,
						  &wlzFnType,
						  &affineType,
						  &meshMthd,
						  &meshMinDst, &meshMaxDst);
	  /* set the parameters, note in this version the basisFnType 
	     is not set  - now it is */
	  if( option_menu = XtNameToWidget(view_struct->dialog,
					   "*.mesh_function") ){
	    switch( wlzFnType ){
	    default:
	    case WLZ_FN_BASIS_2DMQ:
	      if( widget = XtNameToWidget(option_menu, "*.multiquadric") ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		XtCallCallbacks(widget, XmNactivateCallback, NULL);
	      }
	      break;

	    case WLZ_FN_BASIS_2DTPS:
	      if( widget = XtNameToWidget(option_menu, "*.thin-plate spline") ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		XtCallCallbacks(widget, XmNactivateCallback, NULL);
	      }
	      break;

	    case WLZ_FN_BASIS_2DPOLY:
	      if( widget = XtNameToWidget(option_menu, "*.polynomial") ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		XtCallCallbacks(widget, XmNactivateCallback, NULL);
	      }
	      break;

	    }
	  }
	
	  /* set the affine transform type */
	  if( option_menu = XtNameToWidget(view_struct->dialog,
					   "*.affine_type") ){
	    switch( affineType ){
	    default:
	    case WLZ_TRANSFORM_2D_NOSHEAR:
	      if( widget = XtNameToWidget(option_menu, "*.noshear") ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		XtCallCallbacks(widget, XmNactivateCallback, NULL);
	      }
	      break;

	    case WLZ_TRANSFORM_2D_REG:
	      if( widget = XtNameToWidget(option_menu, "*.rigid") ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		XtCallCallbacks(widget, XmNactivateCallback, NULL);
	      }
	      break;

	    case WLZ_TRANSFORM_2D_AFFINE:
	      if( widget = XtNameToWidget(option_menu, "*.affine") ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		XtCallCallbacks(widget, XmNactivateCallback, NULL);
	      }
	      break;

	    }
	  }
		
	  /* set the mesh generation method */
	  if( option_menu = XtNameToWidget(view_struct->dialog,
					   "*.mesh_method") ){
	    switch( meshMthd ){
	    default:
	    case WLZ_MESH_GENMETHOD_BLOCK:
	      if( widget = XtNameToWidget(option_menu, "*.block") ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		XtCallCallbacks(widget, XmNactivateCallback, NULL);
	      }
	      break;

	    case WLZ_MESH_GENMETHOD_GRADIENT:
	      if( widget = XtNameToWidget(option_menu, "*.gradient") ){
		XtVaSetValues(option_menu, XmNmenuHistory, widget, NULL);
		XtCallCallbacks(widget, XmNactivateCallback, NULL);
	      }
	      break;
	    }
	  }

	  if( slider = XtNameToWidget(view_struct->dialog,
					   "*.mesh_min_dist") ){
	    HGU_XmSetSliderValue(slider, (float) meshMinDst);
	  }
	  if( slider = XtNameToWidget(view_struct->dialog,
					   "*.mesh_max_dist") ){
	    HGU_XmSetSliderValue(slider, (float) meshMaxDst);
	  }
	  resetOvlyFlg = 1;
	}

	/* read tie-points */
	if( !strncmp(bibfileRecord->name, "WlzTiePointVtxs", 15) ){
	  int		index;
	  WlzDVertex3	dstVtx;
	  WlzDVertex3	srcVtx;
	  int		relFlg;

	  WlzEffBibParseTiePointVtxsRecord(bibfileRecord, &index,
				       &dstVtx, &srcVtx, &relFlg);
	  /* only add if there is a source image */
	  if( warpGlobals.src.obj ){
	    if( !relFlg ){
	      dstVtx.vtX -= warpGlobals.dst.obj->domain.i->kol1;
	      dstVtx.vtY -= warpGlobals.dst.obj->domain.i->line1;
	      srcVtx.vtX += warpGlobals.srcXOffset;
	      srcVtx.vtY += warpGlobals.srcYOffset;
	    }
	    warpGlobals.dst_vtxs[warpGlobals.num_vtxs].vtX = dstVtx.vtX;
	    warpGlobals.dst_vtxs[warpGlobals.num_vtxs].vtY = dstVtx.vtY;
	    warpGlobals.src_vtxs[warpGlobals.num_vtxs].vtX = srcVtx.vtX;
	    warpGlobals.src_vtxs[warpGlobals.num_vtxs].vtY = srcVtx.vtY;
	    warpGlobals.num_vtxs++;
	  }
	  resetOvlyFlg = 1;
	}

	/* read warp reference file - check against current
	   and query continue if they are different */
	if( !strncmp(bibfileRecord->name, "MAPaintReferenceFile", 20) ){
	  int		index;
	  char		*fileStr;
	  WlzEffFormat	fileType;

	  errNum = WlzEffBibParseFileRecord(bibfileRecord, &index,
					    &fileStr, &fileType);
	  if( errNum == WLZ_ERR_NONE ){
	    if( strcmp(fileStr, globals.file) ){
	      errMsg = (char *)
		AlcMalloc(sizeof(char) *
			  (strlen(fileStr) + strlen(globals.file) +
			   160));
	      sprintf(errMsg,
		      "The bibfile reference object is different"
		      " to the reference\n"
		      "object currently read into MAPaint:-\n"
		      "bibfile: %s\n"
		      "current: %s\n"
		      "Do you want to continue?", fileStr, globals.file);
	      if( !HGU_XmUserConfirm(globals.topl, errMsg,
				     "Yes", "No", 1) ){
		AlcFree(errMsg);
		return;
	      }
	      AlcFree(errMsg);
	    }
	  }
	}

	/* read warp source file */
	if( !strncmp(bibfileRecord->name, "MAPaintWarpInputSourceFile", 26) ){
	  int		index;
	  char		*fileStr;
	  WlzEffFormat	fileType;
	  WlzObject	*obj1;
	  WlzIBox2	cutBox;
	  WlzGreyType	greyType;

	  WlzEffBibParseFileRecord(bibfileRecord, &index,
			    &fileStr, &fileType);

	  /* read the image and install */
	  if( obj = WlzEffReadObj(NULL, fileStr, fileType, 0, &errNum) ){
	    if((obj->type == WLZ_2D_DOMAINOBJ) &&
	       (obj->values.core != NULL)){
	      warpGlobals.srcFile = fileStr;
	      warpGlobals.srcFileType = fileType;
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

	      warpSetXImage(&(warpGlobals.src));
	      warpCanvasExposeCb(warpGlobals.src.canvas,
				 (XtPointer) &(warpGlobals.src),
				 call_data);
	    }
	    resetOvlyFlg = 1;
	  }
	}
	  
	/* read warp signal file */
	if( !strncmp(bibfileRecord->name, "MAPaintWarpInputSignalFile", 26) ){
	  int		index;
	  char		*fileStr;
	  WlzEffFormat	fileType;

	  WlzEffBibParseFileRecord(bibfileRecord, &index,
			    &fileStr, &fileType);

	  /* read the image and install */
	  if( obj = WlzEffReadObj(NULL, fileStr, fileType, 0, &errNum) ){
	    warpGlobals.sgnlFile = fileStr;
	    warpGlobals.sgnlFileType = fileType;
	    if( warpGlobals.sgnl.obj ){
	      WlzFreeObj(warpGlobals.sgnl.obj);
	    }
	    warpGlobals.sgnl.obj = WlzAssignObject(obj, &errNum);
	  }
	}
	  
	/* check for segmentation pre-processing transform parameters.
	   Note redundantly get threshLow and threshHigh for
	   backward compatibility, likewise old name for the record */
	if( !strncmp(bibfileRecord->name,
		     "MAPaintWarpInputSegmentationParams", 34) ){
	  /* basic input parameters with defaults set */
	  int 		normFlg=0, histoFlg=0, shadeFlg=0, gaussFlg=0;
	  int		threshLow=256, threshHigh=256;
	  double	width=5.0;
	  Widget	toggle, slider;

	  /* do not check number of fields read. Note threshLow and threshHigh,
	     can be set but may be overriden by the following threshold
	     parameters */
	  WlzEffBibParseWarpInputSegmentationParamsRecord(bibfileRecord,
							  &normFlg,
							  &histoFlg,
							  &shadeFlg,
							  &gaussFlg,
							  &width,
							  &threshLow,
							  &threshHigh);
	  if( toggle = XtNameToWidget(globals.topl,
				      "*warp_sgnl_controls_form*normalise") ){
	    XtVaSetValues(toggle, XmNset, normFlg, NULL);
	  }
	  if( toggle = XtNameToWidget(globals.topl,
				      "*warp_sgnl_controls_form*histo_equalise") ){
	    XtVaSetValues(toggle, XmNset, histoFlg, NULL);
	  }
	  if( toggle =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*shade_correction") ){
	    XtVaSetValues(toggle, XmNset, shadeFlg, NULL);
	  }
	  if( toggle = XtNameToWidget(globals.topl,
				      "*warp_sgnl_controls_form*gauss_smooth") ){
	    XtVaSetValues(toggle, XmNset, gaussFlg, NULL);
	  }
	  if( slider = XtNameToWidget(globals.topl,
				      "*warp_sgnl_controls_form*gauss_width") ){
	    HGU_XmSetSliderValue(slider, width);
	  }
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_low") ){
	    HGU_XmSetSliderValue(slider, (float) threshLow);
	    warpGlobals.threshRangeLow = threshLow;
	  }
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_high") ){
	    HGU_XmSetSliderValue(slider, (float) threshHigh);
	    warpGlobals.threshRangeHigh = threshHigh;
	  }
	}

	/* check for signal thresholding parameters */
	if( !strncmp(bibfileRecord->name,
		     "MAPaintWarpInputThresholdParams", 31) ){
	  Widget	radio_box, toggle, slider;
	  Widget	notebook, option_menu, button;
	  char		*tmpStr;
	  int		i;
	  XmToggleButtonCallbackStruct cbs;

	  /* set defaults */
	  threshParamStr.thresholdType = WLZ_RGBA_THRESH_NONE;
	  threshParamStr.threshRGBSpace = WLZ_RGBA_SPACE_RGB;
	  threshParamStr.threshColorChannel = WLZ_RGBA_CHANNEL_GREY;
	  threshParamStr.threshRangeLow = 256;
	  threshParamStr.threshRangeHigh = 256;
	  for(i=0; i < 3; i++){
	    threshParamStr.threshRangeRGBLow[i] = 256;
	    threshParamStr.threshRangeRGBHigh[i] = 256;
	  }
	  threshParamStr.threshRGBCombination = 0x0;
	  threshParamStr.lowRGBPoint.type = WLZ_GREY_RGBA;
	  threshParamStr.lowRGBPoint.v.rgbv = 0xffffffff;
	  threshParamStr.highRGBPoint.type = WLZ_GREY_RGBA;
	  threshParamStr.highRGBPoint.v.rgbv = 0xffffffff;
	  threshParamStr.colorEllipseEcc = 1.0;
	  threshParamStr.globalThreshFlg = 1;
	  threshParamStr.globalThreshVtx.vtX = -10000;
	  threshParamStr.globalThreshVtx.vtY = -10000;
	  threshParamStr.incrThreshFlg = 0;
	  threshParamStr.pickThreshFlg = 0;
	  threshParamStr.distanceThreshFlg = 0;

	  /* do not check number of fields read */
	  WlzEffBibParseWarpInputThresholdParamsRecord(bibfileRecord,
						       &threshParamStr);

	  /* now set values */
	  warpGlobals.threshRangeLow = threshParamStr.threshRangeLow;
	  warpGlobals.threshRangeHigh = threshParamStr.threshRangeHigh;
	  for(i=0; i < 3; i++){
	    warpGlobals.threshRangeRGBLow[i] =
	      threshParamStr.threshRangeRGBLow[i];
	    warpGlobals.threshRangeRGBHigh[i] =
	      threshParamStr.threshRangeRGBHigh[i];
	  }
	  warpGlobals.threshRGBCombination = threshParamStr.threshRGBCombination;
	  warpGlobals.lowRGBPoint = threshParamStr.lowRGBPoint;
	  warpGlobals.highRGBPoint = threshParamStr.highRGBPoint;
	  warpGlobals.colorEllipseEcc = threshParamStr.colorEllipseEcc;
	  warpGlobals.globalThreshVtx = threshParamStr.globalThreshVtx;

	  /* reset simple controls */
	  /* do modes before threshold controls */
	  if( toggle = XtNameToWidget(globals.topl,
				      "*warp_sgnl_controls_form*global_thresh") ){
	    XtVaSetValues(toggle, XmNset, threshParamStr.globalThreshFlg, NULL);
	    cbs.set = threshParamStr.globalThreshFlg;
	    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
	  }
	  if( toggle = 
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*incremental_thresh") ){
	    XtVaSetValues(toggle, XmNset, threshParamStr.incrThreshFlg, NULL);
	    cbs.set = threshParamStr.incrThreshFlg;
	    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
	  }
	  if( toggle =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*pick_mode_thresh") ){
	    XtVaSetValues(toggle, XmNset, threshParamStr.pickThreshFlg, NULL);
	    cbs.set = threshParamStr.pickThreshFlg;
	    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
	  }
	  if( toggle =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*distance_mode_thresh") ){
	    XtVaSetValues(toggle, XmNset, threshParamStr.distanceThreshFlg, NULL);
	    cbs.set = threshParamStr.distanceThreshFlg;
	    XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
	  }
  
	  /* reset the threshold and interact controls */
	  /* single colour mode */
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_low") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeLow);
	  }
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_high") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeHigh);
	  }
	  
	  /* multi-colour mode */
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_red_low") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeRGBLow[0]);
	  }
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_red_high") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeRGBHigh[0]);
	  }
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_green_low") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeRGBLow[1]);
	  }
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_green_high") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeRGBHigh[1]);
	  }
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_blue_low") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeRGBLow[2]);
	  }
	  if( slider =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*thresh_range_blue_high") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.threshRangeRGBHigh[2]);
	  }

	  /* interactive mode */
	  if( slider =
	     XtNameToWidget(globals.topl,
  			    "*warp_sgnl_controls_form*thresh_eccentricity") ){
	    HGU_XmSetSliderValue(slider, (float) warpGlobals.colorEllipseEcc);
	  }
	  if( slider = XtNameToWidget(globals.topl,
				      "*warp_sgnl_controls_form*thresh_radius") ){
	    HGU_XmSetSliderValue(slider, (float) 10.0);
	  }
	  if( radio_box = 
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*threshold_interact_rc") ){
	    switch( threshParamStr.thresholdType ){
	    default:
	    case WLZ_RGBA_THRESH_SLICE:
	      toggle = XtNameToWidget(radio_box, "*box");
	      break;
	    case WLZ_RGBA_THRESH_BOX:
	      toggle = XtNameToWidget(radio_box, "*slice");
	      break;
	    case WLZ_RGBA_THRESH_SPHERE:
	      toggle = XtNameToWidget(radio_box, "*sphere");
	      break;
	    }
	    if( toggle ){
	      XtVaSetValues(radio_box, XmNmenuHistory, toggle, NULL);
	      XtCallCallbacks(toggle, XmNvalueChangedCallback, NULL);
	    }
	  }

	  /* threshold type - affects notebook page selected */
	  warpGlobals.thresholdType = threshParamStr.thresholdType;
	  switch( threshParamStr.thresholdType ){
	  case WLZ_RGBA_THRESH_NONE: /* select pre-proc page */
	    tmpStr = "pre_process_page";
	    break;
	  case WLZ_RGBA_THRESH_SINGLE:
	    tmpStr = "threshold_single_page";
	    break;
	  case WLZ_RGBA_THRESH_MULTI:
	    tmpStr = "threshold_multi_page";
	    break;
	  case WLZ_RGBA_THRESH_PLANE:
	  case WLZ_RGBA_THRESH_SLICE:
	  case WLZ_RGBA_THRESH_BOX:
	  case WLZ_RGBA_THRESH_SPHERE:
	    tmpStr = "threshold_interactive_page";
	    break;
	  }
	  if( notebook = XtNameToWidget(warpGlobals.sgnlControls,
				  "*warp_sgnl_notebook") ){
	    int	minPageNum, maxPageNum, pageNum;
	    XtVaGetValues(notebook,
			  XmNfirstPageNumber, &minPageNum,
			  XmNlastPageNumber, &maxPageNum,
			  NULL);
	    for(i=minPageNum; i <= maxPageNum; i++){
	      XmNotebookPageInfo	page_info;
	      if(XmNotebookGetPageInfo(notebook, i, &page_info) ==
		 XmPAGE_FOUND){
		if(strcmp(tmpStr, XtName(page_info.page_widget)) == 0){
		  if(strncmp(tmpStr, "threshold_", 10) == 0 ){
		    warpGlobals.lastThresholdPageNum = i;
		    thresholdMajorPageSelectCb(warpGlobals.sgnlControls,
					       (XtPointer) warpGlobals.thresholdType,
					       NULL);
		  }
		  break;
		}
	      }
	    }
	  }

	  /* colour space - affects channel selectors and callbacks */
	  warpGlobals.threshRGBSpace = threshParamStr.threshRGBSpace;
	  if( option_menu =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*color_space") ){
	    switch( warpGlobals.threshRGBSpace ){
	    default:
	    case WLZ_RGBA_SPACE_RGB:
	      button = XtNameToWidget(option_menu, "*RGB");
	      break;
	    case WLZ_RGBA_SPACE_HSB:
	      button = XtNameToWidget(option_menu, "*HSB");
	      break;
	    case WLZ_RGBA_SPACE_CMY:
	      button = XtNameToWidget(option_menu, "*CMY");
	      break;
	    }
	    if( button ){
	      XtVaSetValues(option_menu, XmNmenuHistory, button, NULL);
	      XtCallCallbacks(button, XmNactivateCallback, NULL);
	    }
	  }

	  /* colour channel - now recalculate the signal domain from scratch */
	  warpGlobals.threshColorChannel = threshParamStr.threshColorChannel;
	  if( radio_box =
	     XtNameToWidget(globals.topl,
			    "*warp_sgnl_controls_form*threshold_channel_rc") ){
	    switch( threshParamStr.threshColorChannel ){
	    default:
	    case WLZ_RGBA_CHANNEL_GREY:
	      toggle = XtNameToWidget(radio_box, "*grey");
	      break;
	    case WLZ_RGBA_CHANNEL_RED:
	    case WLZ_RGBA_CHANNEL_HUE:
	    case WLZ_RGBA_CHANNEL_CYAN:
	      toggle = XtNameToWidget(radio_box, "*red");
	      break;
	    case WLZ_RGBA_CHANNEL_GREEN:
	    case WLZ_RGBA_CHANNEL_SATURATION:
	    case WLZ_RGBA_CHANNEL_MAGENTA:
	      toggle = XtNameToWidget(radio_box, "*green");
	      break;
	    case WLZ_RGBA_CHANNEL_BLUE:
	    case WLZ_RGBA_CHANNEL_BRIGHTNESS:
	    case WLZ_RGBA_CHANNEL_YELLOW:
	      toggle = XtNameToWidget(radio_box, "*blue");
	      break;
	    }
	    if( toggle ){
	      XtVaSetValues(radio_box, XmNmenuHistory, toggle, NULL);
	      XtCallCallbacks(toggle, XmNvalueChangedCallback, NULL);
	    }
	  }
	}

	/* check for signal post-processing parameters */

	BibFileRecordFree(&bibfileRecord);
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
      }
      (void) fclose(fp);

      /* calculate the warp transform and display */
      if( warpGlobals.num_vtxs || resetOvlyFlg){
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
			call_data);
	XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		      call_data);
	XtCallCallbacks(warpGlobals.src.canvas, XmNexposeCallback,
		      call_data);
      }

      /* calculate the signal threshold object */
      if( warpGlobals.sgnl.obj ){
	recalcWarpProcObjCb(w, client_data, call_data);
      }

    }
  }

  return;
}

void warpIOWriteAffine(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  String		fileStr;
  FILE			*fp;

  /* check for source object */
  if((warpGlobals.src.obj == NULL)){
    return;
  }

  /* get a filename for the affine transform */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a filename\n"
				      "for the affine transform\n",
				      "OK", "cancel",
				      "MAPaintWarpAffine.wlz",
				      NULL, "*.wlz") ){

    if( fp = fopen(fileStr, "w") ){
      if( warpGlobals.affine ){
	WlzObject *obj;
	WlzDomain	domain;
	WlzValues	values;
	WlzAffineTransform	*t1, *t2;

	/* must take acount of source offset - yuk */
	t1 = WlzAffineTransformFromTranslation(WLZ_TRANSFORM_2D_AFFINE,
					       warpGlobals.srcXOffset,
					       warpGlobals.srcYOffset,
					       0, NULL);
	t2 = WlzAffineTransformProduct(t1, warpGlobals.affine, NULL);

/*	domain.t = warpGlobals.affine;*/
	domain.t = t2;
	values.core = NULL;
	if( obj = WlzMakeMain(WLZ_AFFINE_TRANS, domain, values,
			      NULL, NULL, NULL) ){
	  WlzWriteObj(fp, obj);
	  domain.core = NULL;
	  WlzFreeObj(obj);
	}
	WlzFreeAffineTransform(t2);
	WlzFreeAffineTransform(t1);
      }

      (void) fclose(fp);
    }
  }

  return;
}

void warpIOWriteImage(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		interpFlg = (int) client_data;
  String		fileStr;
  FILE			*fp;
  WlzObject		*obj;

  /* check for source object */
  if((warpGlobals.src.obj == NULL)){
    return;
  }

  /* get a filename for the warp parameters */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a filename\n"
				      "for the warped source image\n"
				      "point parameters to be read in.\n",
				      "OK", "cancel",
				      "MAPaintWarpedSource.wlz",
				      NULL, "*.wlz") ){

    if( fp = fopen(fileStr, "w") ){
      /* transform the object */
      if( interpFlg ){
	obj = mapaintWarpObj(warpGlobals.src.obj, WLZ_INTERPOLATION_LINEAR);
      }
      else {
	obj = mapaintWarpObj(warpGlobals.src.obj, WLZ_INTERPOLATION_LINEAR);
      }
      if( obj ){
	WlzWriteObj(fp, obj);
	WlzFreeObj(obj);
      }

      (void) fclose(fp);
    }
  }

  return;
}

static MenuItem warpIOMenuItemsP[] = {  /* controls values io menu */
  {"warp_data_write", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpIOWrite, NULL,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"warp_data_read", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpIORead, NULL,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"warped_image_write_interp", &xmPushButtonGadgetClass,
   0, NULL, NULL, False,
   warpIOWriteImage, (XtPointer) 1,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"warped_image_write_nn", &xmPushButtonGadgetClass,
   0, NULL, NULL, False,
   warpIOWriteImage, (XtPointer) 0,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"write_affine", &xmPushButtonGadgetClass,
   0, NULL, NULL, False,
   warpIOWriteAffine, (XtPointer) 0,
   myHGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

void setupWarpIOMenu(
  Widget		widget,
  ThreeDViewStruct	*view_struct)
{
  Widget	popup;

  /* set up the callback data */
  warpIOMenuItemsP[0].callback_data = (XtPointer) view_struct;
  warpIOMenuItemsP[1].callback_data = (XtPointer) view_struct;

  popup = HGU_XmBuildMenu(widget, XmMENU_POPUP, XtName(widget), '\0',
			  XmTEAR_OFF_ENABLED, warpIOMenuItemsP);
/*  XtAddEventHandler(widget, ButtonPressMask, True, PostIt, popup);*/
  XtVaSetValues(popup, XmNpopupEnabled, XmPOPUP_AUTOMATIC, NULL);

  return;
}

