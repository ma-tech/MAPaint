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
				       WLZ_TRANSFORM_2D_NOSHEAR,
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
     WlzBasisFnTrFromCPts(warpGlobals.basisFnType, polyOrder,
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
  WlzSetMeshAffineProduct(warpGlobals.meshTr, warpGlobals.affine);

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


  /* get a filename for the warp parameters */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a filename\n"
				      "for the section, warp and tie-\n"
				      "point parameters to be saved\n"
				      "as plain text",
				      "OK", "cancel",
				      "MAPaintWarpParams.bib",
				      NULL, "*.bib") ){
    if( fp = fopen(fileStr, "w") ){
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
					      warpGlobals.basisFnType,
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

      /* write the processing transform parameters */
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
	  width,
	  warpGlobals.threshRangeLow,
	  warpGlobals.threshRangeHigh) != WLZ_ERR_NONE ){
	HGU_XmUserError(globals.topl,
	 		"Save Processing Parameters:\n"
		 	"    Error in writing the bibfile\n"
		 	"    Please check disk space or quotas\n"
		 	"    Section parameters not saved",
		 	XmDIALOG_FULL_APPLICATION_MODAL);
      }

      /* write the tie points */
      for(i=0; i < warpGlobals.num_vtxs; i++){
	WlzDVertex3	vtx1, vtx2;
	vtx1.vtX = warpGlobals.dst_vtxs[i].vtX;
	vtx1.vtY = warpGlobals.dst_vtxs[i].vtY;
	vtx1.vtZ = 0.0;
	vtx2.vtX = warpGlobals.src_vtxs[i].vtX;
	vtx2.vtY = warpGlobals.src_vtxs[i].vtY;
	vtx2.vtZ = 0.0;
	if( WlzEffBibWriteTiePointVtxsRecord(fp, "WlzTiePointVtxs", i, vtx1, vtx2)
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
  double		oldScale, newScale;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win  = XtWindow(view_struct->canvas);
  Widget		option_menu, widget, slider;
  WlzCompoundArray	*cobj;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  WlzObject		*obj;

  /* get a filename for the warp parameters */
  if( fileStr = HGU_XmUserGetFilename(globals.topl,
				      "Please type in a filename\n"
				      "for the section, warp and tie-\n"
				      "point parameters to be read in.\n"
				      "WARNING: this will delete all\n"
				      "current tie-points",
				      "OK", "cancel",
				      "MAPaintWarpParams.bib",
				      NULL, "*.bib") ){
    warp2DInteractDeleteAllCb(w, client_data, call_data);

    if( fp = fopen(fileStr, "r") ){
      BibFileRecord	*bibfileRecord;
      BibFileError	bibFileErr;
      char		*errMsg;

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
	  newScale = wlzViewStr->scale;
	  wlzViewStr->scale = oldScale;
	  setViewMode(view_struct, wlzViewStr->view_mode);
	  setViewScale(view_struct, newScale, 0, 0);

	  /* redisplay the section */
	  reset_view_struct( view_struct );
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
					      view_struct->wlzViewStr, NULL),
			    NULL);
	  warpSetXImage(&(warpGlobals.dst));
	  XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback, call_data);

	  /* reset the overlay object */
	  cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
	  if( cobj->o[0] ){
	    WlzFreeObj(cobj->o[0]);
	  }
	  cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);
	  warpSetXImage(&(warpGlobals.ovly));
	  XtCallCallbacks(warpGlobals.ovly.canvas, XmNexposeCallback, call_data);
	}

	/* check for warp transform parameters */
	if( !strncmp(bibfileRecord->name, "WlzWarpTransformParams", 22) ){
	  WlzBasisFnType	basisFnType;
	  WlzMeshGenMethod	meshMthd;
	  int			meshMinDst;
	  int	 		meshMaxDst;

	  WlzEffBibParseWarpTransformParamsRecord(bibfileRecord,
					      &basisFnType, &meshMthd,
					      &meshMinDst, &meshMaxDst);
	  /* set the parameters, note in this version the basisFnType
	     is not set */
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
	}

	/* read tie-points */
	if( !strncmp(bibfileRecord->name, "WlzTiePointVtxs", 15) ){
	  int		index;
	  WlzDVertex3	dstVtx;
	  WlzDVertex3	srcVtx;

	  WlzEffBibParseTiePointVtxsRecord(bibfileRecord, &index,
				       &dstVtx, &srcVtx);
	  /* only add if there is a source image */
	  if( warpGlobals.src.obj ){
	    warpGlobals.dst_vtxs[warpGlobals.num_vtxs].vtX = dstVtx.vtX;
	    warpGlobals.dst_vtxs[warpGlobals.num_vtxs].vtY = dstVtx.vtY;
	    warpGlobals.src_vtxs[warpGlobals.num_vtxs].vtX = srcVtx.vtX;
	    warpGlobals.src_vtxs[warpGlobals.num_vtxs].vtY = srcVtx.vtY;
	    warpGlobals.num_vtxs++;
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
	  if( obj = WlzEffReadObj(NULL, fileStr, fileType, &errNum) ){
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
	    }
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
	  if( obj = WlzEffReadObj(NULL, fileStr, fileType, &errNum) ){
	    warpGlobals.sgnlFile = fileStr;
	    warpGlobals.sgnlFileType = fileType;
	    if( warpGlobals.sgnl.obj ){
	      WlzFreeObj(warpGlobals.sgnl.obj);
	    }
	    warpGlobals.sgnl.obj = WlzAssignObject(obj, &errNum);
	  }
	}
	  
	/* check for segmentation transform parameters */
	if( !strncmp(bibfileRecord->name,
		     "MAPaintWarpInputSegmentationParams", 34) ){
	  int 		normFlg, histoFlg, shadeFlg, gaussFlg;
	  int		threshLow, threshHigh;
	  double	width;
	  Widget	toggle, slider;

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
	  if( toggle = XtNameToWidget(globals.topl,
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
	  if( slider = XtNameToWidget(globals.topl,
				      "*warp_sgnl_controls_form*thresh_range_low") ){
	    HGU_XmSetSliderValue(slider, (float) threshLow);
	    warpGlobals.threshRangeLow = threshLow;
	  }
	  if( slider = XtNameToWidget(globals.topl,
				      "*warp_sgnl_controls_form*thresh_range_high") ){
	    HGU_XmSetSliderValue(slider, (float) threshHigh);
	    warpGlobals.threshRangeHigh = threshHigh;
	  }
	}

	BibFileRecordFree(&bibfileRecord);
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
      }
      (void) fclose(fp);

      /* calculate the warp transform and display */
      if( warpGlobals.num_vtxs ){
	warpDisplayTiePoints();
	warpSetOvlyObject();
	warpSetOvlyXImage(&(warpGlobals.ovly));
	warpCanvasExposeCb(warpGlobals.ovly.canvas,
			 (XtPointer) &(warpGlobals.ovly),
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
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"warp_data_read", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpIORead, NULL,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"warped_image_write_interp", &xmPushButtonGadgetClass,
   0, NULL, NULL, False,
   warpIOWriteImage, (XtPointer) 1,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"warped_image_write_nn", &xmPushButtonGadgetClass,
   0, NULL, NULL, False,
   warpIOWriteImage, (XtPointer) 0,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
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
  XtAddEventHandler(widget, ButtonPressMask, True, PostIt, popup);

  return;
}

