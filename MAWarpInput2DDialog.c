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

#define WARP_MAX_NUM_VTXS 128

extern Widget create_view_window_dialog(Widget topl,
					double theta,
					double phi,
					WlzDVertex3 *fixed);

static Widget create2DWarpDialog(Widget parent,
				 ThreeDViewStruct *view_struct);
typedef struct {
  WlzObject		*obj;
  Widget		canvas;
  XImage		*ximage;
  GC			gc;
  double		mag;
  int			rot;
  int			transpose;
  double		gamma;
} MAPaintImageWinStruct;
  
typedef struct {
  ThreeDViewStruct	*view_struct;
  Widget		warp2DInteractDialog;

  /* destination canvas and affine transform */
  MAPaintImageWinStruct	dst;

  /* source canvas and affine transform */
  MAPaintImageWinStruct	src;

  /* overlay canvas and affine transform */
  MAPaintImageWinStruct	ovly;
  
  /* tie-points */
  WlzDVertex2	dst_vtxs[WARP_MAX_NUM_VTXS];
  WlzDVertex2	src_vtxs[WARP_MAX_NUM_VTXS];
  int		num_vtxs;
  int		sel_vtx;
  int		tp_state;
  GC		green_gc;
  GC		red_gc;
  GC		blue_gc;
  GC		yellow_gc;
  WlzAffineTransform	*affine;
  WlzBasisFnTransform	*basisTr;
  WlzMeshTransform	*meshTr;
  int			meshErrFlg;
  WlzMeshGenMethod	meshMthd;
  int			meshMinDst;
  int			meshMaxDst;

} MAPaintWarp2DStruct;

typedef enum
{
  TP_INACTIVE,
  TP_DST_DEFINED,
  TP_SRC_DEFINED,
  TP_SELECTED
} WarpTiePointState;

static MAPaintWarp2DStruct warpGlobals;
static Widget warp_read_src_dialog=NULL;
static const char *warpErrStr;
static char warpErrBuf[256];

static WlzDVertex2 warpDisplayTransBack(
  WlzDVertex2	vtx,
  MAPaintImageWinStruct	*winStruct)
{
  WlzDVertex2	rtnVtx;
  float		x, y;
  int		width, height;	
  WlzCompoundArray	*cobj;

  if( winStruct->obj == NULL ){
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }

  switch( winStruct->obj->type ){
  case WLZ_2D_DOMAINOBJ:
    width = winStruct->obj->domain.i->lastkl -
      winStruct->obj->domain.i->kol1 + 1;
    height = winStruct->obj->domain.i->lastln -
      winStruct->obj->domain.i->line1 + 1;
    break;

  case WLZ_COMPOUND_ARR_1:
  case WLZ_COMPOUND_ARR_2:
    cobj = (WlzCompoundArray *) winStruct->obj;
    if( cobj->n > 0 ){
      width = (cobj->o[0])->domain.i->lastkl -
	(cobj->o[0])->domain.i->kol1 + 1;
      height = (cobj->o[0])->domain.i->lastln -
	(cobj->o[0])->domain.i->line1 + 1;
    }
    else {
      rtnVtx.vtX = -1;
      rtnVtx.vtY = -1;
      return rtnVtx;
    }
    break;

  default:
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }

  /* the back transform - floating point fashion */
  x = vtx.vtX / winStruct->mag;
  y = vtx.vtY / winStruct->mag;

  switch( winStruct->rot ){
  case 0:
    rtnVtx.vtX = x;
    rtnVtx.vtY = y;
    break;

  case 1:
    rtnVtx.vtX = width - y - 1;
    rtnVtx.vtY = x;
    break;

  case 2:
    rtnVtx.vtX = width - x - 1;
    rtnVtx.vtY = height - y - 1;
    break;

  case 3:
    rtnVtx.vtX = y;
    rtnVtx.vtY = height - x - 1;
    break;
  }

  if( winStruct->transpose ){
    rtnVtx.vtX = width - rtnVtx.vtX - 1;
  }

  return rtnVtx;
}
  
static WlzDVertex2 warpDisplayTransFore(
  WlzDVertex2	vtx,
  MAPaintImageWinStruct	*winStruct)
{
  WlzDVertex2	rtnVtx;
  float		x, y;
  int		width, height;
  WlzCompoundArray	*cobj;

  if( winStruct->obj == NULL ){
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }

  switch( winStruct->obj->type ){
  case WLZ_2D_DOMAINOBJ:
    width = winStruct->obj->domain.i->lastkl -
      winStruct->obj->domain.i->kol1 + 1;
    height = winStruct->obj->domain.i->lastln -
      winStruct->obj->domain.i->line1 + 1;
    break;

  case WLZ_COMPOUND_ARR_1:
  case WLZ_COMPOUND_ARR_2:
    cobj = (WlzCompoundArray *) winStruct->obj;
    if( cobj->n > 0 ){
      width = (cobj->o[0])->domain.i->lastkl -
	(cobj->o[0])->domain.i->kol1 + 1;
      height = (cobj->o[0])->domain.i->lastln -
	(cobj->o[0])->domain.i->line1 + 1;
    }
    else {
      rtnVtx.vtX = -1;
      rtnVtx.vtY = -1;
      return rtnVtx;
    }
    break;

  default:
    rtnVtx.vtX = -1;
    rtnVtx.vtY = -1;
    return rtnVtx;
  }
  width *= winStruct->mag;
  height *= winStruct->mag;

  /* the foreward transform - floating point fashion */
  x = vtx.vtX * winStruct->mag;
  y = vtx.vtY * winStruct->mag;

  if( winStruct->transpose ){
    x = width - x - winStruct->mag;
  }

  switch( winStruct->rot ){
  case 0:
    rtnVtx.vtX = x;
    rtnVtx.vtY = y;
    break;

  case 3:
    rtnVtx.vtX = height - y - winStruct->mag;
    rtnVtx.vtY = x;
    break;

  case 2:
    rtnVtx.vtX = width - x - winStruct->mag;
    rtnVtx.vtY = height - y - winStruct->mag;
    break;

  case 1:
    rtnVtx.vtX = y;
    rtnVtx.vtY = width - x - winStruct->mag;
    break;
  }

  return rtnVtx;
}
  
static void warpSetXImage(
  MAPaintImageWinStruct *winStruct)
{
  XWindowAttributes	win_att;
  Dimension		src_width, src_height, width, height;
  UBYTE			*data, *dst_data, gammaLUT[256];
  int			i, j;
  WlzGreyValueWSpace	*gVWSp = NULL;
  WlzObject		*obj = NULL;
  WlzCompoundArray	*cobj;
  int			cmpndFlg;

  if( winStruct->ximage ){
    AlcFree(winStruct->ximage->data);
    winStruct->ximage->data = NULL;
    XDestroyImage(winStruct->ximage);
    winStruct->ximage = NULL;
  }

  if( !winStruct->obj ){
    return;
  }
  else {
    switch( winStruct->obj->type ){
    case WLZ_2D_DOMAINOBJ:
      obj = WlzAssignObject(winStruct->obj, NULL);
      cmpndFlg = 0;
      break;

    case WLZ_COMPOUND_ARR_1:
    case WLZ_COMPOUND_ARR_2:
      cobj = (WlzCompoundArray *) winStruct->obj;
      if( cobj->n > 0 ){
	obj = WlzAssignObject((cobj->o)[0], NULL);
	cmpndFlg = 1;
/*	winStruct->gamma = 1.0;*/
      }
      break;

    default:
      return;
    }
  }
  if( obj == NULL ){
    return;
  }

  /* create the ximage using current mag and rotate */
  if( XGetWindowAttributes(XtDisplay(winStruct->canvas),
			   XtWindow(winStruct->canvas), &win_att) == 0 ){
    return;
  }
  src_width = obj->domain.i->lastkl - obj->domain.i->kol1 + 1; 
  src_height = obj->domain.i->lastln - obj->domain.i->line1 + 1;
  if( (winStruct->rot)%2 ){
    width = src_height*winStruct->mag;
    height = src_width*winStruct->mag;
  }
  else {
    width = src_width*winStruct->mag;
    height = src_height*winStruct->mag;
  }

  XtVaSetValues(winStruct->canvas,
		XmNwidth, width,
		XmNheight, height,
		NULL);

  gVWSp = WlzGreyValueMakeWSp(obj, NULL);
  dst_data = (UBYTE *) AlcMalloc(((win_att.depth == 8)?1:4)
			     *width*height*sizeof(char));
  data = dst_data;

  /* set up the gamma LUT */
  gammaLUT[0] = 0;
  for(i=1; i < 256; i++){
    double	val;
    val = i;
    gammaLUT[i] = (int) (pow(val/255, winStruct->gamma) * 255);
  }

  for(j=0; j < height; j++){
    for(i=0; i < width; i++, data++){
      WlzDVertex2 vtx1, vtx2;
      vtx1.vtX = i;
      vtx1.vtY = j;
      vtx2 = warpDisplayTransBack(vtx1, winStruct);
      WlzGreyValueGet(gVWSp, 0,
		      vtx2.vtY + obj->domain.i->line1,
		      vtx2.vtX + obj->domain.i->kol1);
      switch( gVWSp->gType ){
      default:
      case WLZ_GREY_INT:
	*data = *(gVWSp->gPtr[0].inp);
	break;
      case WLZ_GREY_SHORT:
	*data = *(gVWSp->gPtr[0].shp);
	break;
      case WLZ_GREY_UBYTE:
	*data = *(gVWSp->gPtr[0].ubp);
	break;
      case WLZ_GREY_FLOAT:
	*data = *(gVWSp->gPtr[0].flp);
	break;
      case WLZ_GREY_DOUBLE:
	*data = *(gVWSp->gPtr[0].dbp);
	break;
      }
      if( win_att.depth == 24 ){
	if( cmpndFlg ){
	  data[1] = data[0];
	  data[2] = data[0];
	  data[3] = 255;
	}
	else {
	  data[1] = gammaLUT[data[0]];
	  data[2] = gammaLUT[data[0]];
	  data[3] = gammaLUT[data[0]];
	}	  
	data += 3;
      }
    }
  }
  WlzGreyValueFreeWSp(gVWSp);
  WlzFreeObj(obj);

  winStruct->ximage = XCreateImage(XtDisplay(winStruct->canvas),
				   win_att.visual, win_att.depth,
				   ZPixmap, 0, (char *) dst_data,
				   width, height, 8, 0);

  return;
}

static void warpSetOvlyXImage(
  MAPaintImageWinStruct *winStruct)
{
  XWindowAttributes	win_att;
  Dimension		src_width, src_height, width, height;
  UBYTE			*data, *dst_data, dataVal;
  int			i, j, klOffset, lnOffset;
  WlzGreyValueWSpace	*gVWSp = NULL;
  WlzObject		*obj;
  WlzCompoundArray	*cobj;
  double		a, b;

  if( !winStruct->ximage ){
    return;
  }

  if( !winStruct->obj ){
    return;
  }
  else {
    switch( winStruct->obj->type ){

    case WLZ_COMPOUND_ARR_1:
    case WLZ_COMPOUND_ARR_2:
      cobj = (WlzCompoundArray *) winStruct->obj;
      if( cobj->n >= 2 ){
	obj = WlzAssignObject((cobj->o)[1], NULL);
	klOffset = (cobj->o)[0]->domain.i->kol1;
	lnOffset = (cobj->o)[0]->domain.i->line1;
      }
      else {
	obj = NULL;
      }
      break;

    default:
      return;
    }
  }
  if( obj == NULL ){
    return;
  }

  /* create the ximage using current mag and rotate */
  if( XGetWindowAttributes(XtDisplay(winStruct->canvas),
			   XtWindow(winStruct->canvas), &win_att) == 0 ){
    return;
  }
  if( win_att.depth != 24 ){
    WlzFreeObj(obj);
    return;
  }
  width = winStruct->ximage->width;
  height = winStruct->ximage->height;

  gVWSp = WlzGreyValueMakeWSp(obj, NULL);
  dst_data = (UBYTE *) winStruct->ximage->data;
  data = dst_data;

  /* set up mixing factors */
  if( winStruct->gamma > 1.0){
    a = (winStruct->gamma - 1.0) / 9.0 * 255;
    b = (10.0 - winStruct->gamma) / 9.0;
  }
  else {
    a = (1.0 - winStruct->gamma) / 0.9 * 255;
    b = (winStruct->gamma - 0.1) / 0.9;
  }

  for(j=0; j < height; j++){
    for(i=0; i < width; i++, data++){
      WlzDVertex2 vtx1, vtx2;
      vtx1.vtX = i;
      vtx1.vtY = j;
      vtx2 = warpDisplayTransBack(vtx1, winStruct);
      WlzGreyValueGet(gVWSp, 0,
		      vtx2.vtY + lnOffset,
		      vtx2.vtX + klOffset);
      switch( gVWSp->gType ){
      default:
      case WLZ_GREY_INT:
	dataVal = *(gVWSp->gPtr[0].inp);
	break;
      case WLZ_GREY_SHORT:
	dataVal = *(gVWSp->gPtr[0].shp);
	break;
      case WLZ_GREY_UBYTE:
	dataVal = *(gVWSp->gPtr[0].ubp);
	break;
      case WLZ_GREY_FLOAT:
	dataVal = *(gVWSp->gPtr[0].flp);
	break;
      case WLZ_GREY_DOUBLE:
	dataVal = *(gVWSp->gPtr[0].dbp);
	break;
      }

      if( winStruct->gamma >= 1.0){
	data[3] = (int) (a + b * dataVal);
      }
      else {
	data[1] = (int) (a + b * data[0]);
	data[2] = data[1];
	data[3] = dataVal;
      }
      data += 3;
    }
  }
  WlzGreyValueFreeWSp(gVWSp);
  WlzFreeObj(obj);

  winStruct->ximage = XCreateImage(XtDisplay(winStruct->canvas),
				   win_att.visual, win_att.depth,
				   ZPixmap, 0, (char *) dst_data,
				   width, height, 8, 0);

  return;
}

static WlzErrorNum WlzSetMeshAffineProduct(
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
      vtx = WlzAffineTransformVertexD(affineTr, vtx, &errNum);
      vtx.vtX -= meshTr->nodes[i].position.vtX;
      vtx.vtY -= meshTr->nodes[i].position.vtY;
      meshTr->nodes[i].displacement = vtx;
    }
  }

  return errNum;
}

static void warpSetOvlyObject(void)
{
  WlzAffineTransform	*trans;
  WlzObject		*transObj;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  WlzCompoundArray	*cobj;
  WlzDVertex2		srcVtxs[WARP_MAX_NUM_VTXS];
  WlzDVertex2		dstVtxs[WARP_MAX_NUM_VTXS];
  int 			i, badElm;

  /* define the affine transform from the tie points */
  if( warpGlobals.num_vtxs < 1 ){
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
  if( (trans = WlzAffineTransformLSq(warpGlobals.num_vtxs, srcVtxs,
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

    return;
  }

  /* apply the inverse transform to the dst tie-points */
  if( trans = WlzAffineTransformInverse(warpGlobals.affine, &errNum) ){
    for(i=0; i < warpGlobals.num_vtxs; i++){
      dstVtxs[i] = WlzAffineTransformVertexD(trans, dstVtxs[i], &errNum);
    }
  }
  else {
    return;
  }

  /* calculate the warp transform */
  if( warpGlobals.basisTr ){
    WlzBasisFnFreeTransform(warpGlobals.basisTr);
  }
  if( !(warpGlobals.basisTr =
     WlzBasisFnTrFromCPts(WLZ_BASISFN_TPS, 0,
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
  WlzSetMeshAffineProduct(warpGlobals.meshTr, warpGlobals.affine);

  /* verify the mesh */
  if((errNum = WlzMeshTransformVerify(warpGlobals.meshTr, 1,
				      &badElm, NULL)) != WLZ_ERR_NONE){
    (void) WlzStringFromErrorNum(errNum, &warpErrStr);
    sprintf(warpErrBuf,
	    "2D Warp Object Input:\n"
	    "    The tie-points set generate an invalid\n"
	    "    mesh, please move some of the tie-points\n"
	    "    or reset the mesh parameters and try again.\n\n"
	    "Wlz error: %s", warpErrStr);
    HGU_XmUserError(globals.topl, warpErrBuf,
		    XmDIALOG_FULL_APPLICATION_MODAL);
    warpGlobals.meshErrFlg = 1;

    return;
  }
  warpGlobals.meshErrFlg = 0;

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

static int warpCloseDstVtx(
  int	x,
  int	y)
{
  int	dx, dy, i;
  WlzDVertex2	vtx1, vtx2;
  int	delta;

  vtx1.vtX = x;
  vtx1.vtY = y;
  vtx2 = 
    warpDisplayTransBack(vtx1, &(warpGlobals.dst));
  delta = 4 / warpGlobals.dst.mag;

  for(i=0; i < warpGlobals.num_vtxs; i++){
    dx = (int) (warpGlobals.dst_vtxs[i].vtX - vtx2.vtX);
    dy = (int) (warpGlobals.dst_vtxs[i].vtY - vtx2.vtY);
    dx = (dx < 0)?-dx:dx;
    dy = (dy < 0)?-dy:dy;
    if( (dx < delta) && (dy < delta) ){
      return i;
    }
  }

  return -1;
}

static int warpCloseSrcVtx(
  int	x,
  int	y)
{
  int	dx, dy, i;
  WlzDVertex2	vtx1, vtx2;
  int	delta;

  vtx1.vtX = x;
  vtx1.vtY = y;
  vtx2 = 
    warpDisplayTransBack(vtx1, &(warpGlobals.src));
  delta = 4 / warpGlobals.src.mag;

  for(i=0; i < warpGlobals.num_vtxs; i++){
    dx = (int) warpGlobals.src_vtxs[i].vtX - vtx2.vtX;
    dy = (int) warpGlobals.src_vtxs[i].vtY - vtx2.vtY;
    dx = (dx < 0)?-dx:dx;
    dy = (dy < 0)?-dy:dy;
    if( (dx < delta) && (dy < delta) ){
      return i;
    }
  }

  return -1;
}

static void warpUndisplayVtx(
  MAPaintImageWinStruct	*winStruct,
  WlzDVertex2	vtx)
{
  int	x, y, width, height;
  WlzDVertex2	vtx1;

  if( winStruct->ximage ){

    width = winStruct->ximage->width;
    height = winStruct->ximage->height;
    vtx1 = warpDisplayTransFore(vtx, winStruct);
    
    x = vtx1.vtX - 3;
    y = vtx1.vtY - 3;

    if( (x < -4) || (x >= width) || (y < -4) || (y >= height) ){
      return;
    }

    x = (x < 0) ? 0 : x;
    y = (y < 0) ? 0 : y;

    width = WLZ_MIN(width - x, 8);
    height = WLZ_MIN(height - y, 8);

    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage, x, y, x, y,
	      width, height);
    XFlush(XtDisplay(winStruct->canvas));
  }

  return;
}

static void warpDisplayTiePoints(void)
{
  Display	*dpy=XtDisplay(warpGlobals.dst.canvas);
  Window	dst_win = XtWindow(warpGlobals.dst.canvas);
  Window	src_win = XtWindow(warpGlobals.src.canvas);
  GC		gc;
  int		i, x, y;
  WlzDVertex2	vtx1, vtx2;

  /* check gc's */
  if( warpGlobals.red_gc == (GC) -1 ){
    XGCValues	values;

    values.foreground = 0xff;
    values.background = 0;
    warpGlobals.red_gc = XCreateGC(dpy, dst_win,
				   GCForeground|GCBackground,
				   &values);
    values.foreground = 0xff00;
    warpGlobals.green_gc = XCreateGC(dpy, dst_win,
				   GCForeground|GCBackground,
				   &values);
  }

  for(i=0; i < warpGlobals.num_vtxs; i++){
    if((warpGlobals.tp_state == TP_SELECTED) && (i == warpGlobals.sel_vtx)){
      gc = warpGlobals.red_gc;
    }
    else {
      gc = warpGlobals.green_gc;
    }
    vtx2 = warpDisplayTransFore(warpGlobals.dst_vtxs[i],
				&(warpGlobals.dst));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, dst_win, gc, x, y, 4, 4);

    vtx2 = warpDisplayTransFore(warpGlobals.src_vtxs[i],
				&(warpGlobals.src));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, src_win, gc, x, y, 4, 4);
  }

  if( warpGlobals.tp_state == TP_DST_DEFINED ){
    gc = warpGlobals.red_gc;

    vtx2 = warpDisplayTransFore(warpGlobals.dst_vtxs[warpGlobals.num_vtxs],
				&(warpGlobals.dst));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, dst_win, gc, x, y, 4, 4);
  }

  if( warpGlobals.tp_state == TP_SRC_DEFINED ){
    gc = warpGlobals.red_gc;
    vtx2 = warpDisplayTransFore(warpGlobals.src_vtxs[warpGlobals.num_vtxs],
				&(warpGlobals.src));
    x = (int) vtx2.vtX - 1;
    y = (int) vtx2.vtY - 1;
    XFillRectangle(dpy, src_win, gc, x, y, 4, 4);
  }

  XFlush(dpy);
  return;
}

void warpDisplayDstMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  WlzMeshTransform	*meshTr = warpGlobals.meshTr;
  int			elemIdx;
  WlzMeshElem		*elem;
  int			xOff, yOff;
  Display	*dpy=XtDisplay(warpGlobals.src.canvas);
  Window	dst_win = XtWindow(warpGlobals.dst.canvas);
  GC		gc;

  /* check there is a source image */
  if( !warpGlobals.src.obj || !warpGlobals.meshTr ){
    return;
  }
  xOff = warpGlobals.dst.obj->domain.i->kol1;
  yOff = warpGlobals.dst.obj->domain.i->line1;

  /* check the ximage */
  if( !winStruct->ximage ){
    warpSetXImage(winStruct);
    warpSetOvlyXImage(winStruct);
    if( !winStruct->ximage ){
      return;
    }
  }

  /* check the graphics context */
  if( warpGlobals.yellow_gc == (GC) -1 ){
    XGCValues	values;

    values.foreground = 0xffff;
    values.background = 0;
    warpGlobals.yellow_gc = XCreateGC(dpy, dst_win,
				      GCForeground|GCBackground,
				      &values);
  }
  gc = warpGlobals.yellow_gc;

  /* loop through the mesh displaying edges
     - note this covers each edge twice */
  elem = meshTr->elements;
  for(elemIdx=0; elemIdx < meshTr->nElem; elemIdx++, elem++){
    XPoint	points[4];
    WlzDVertex2	vtx;
    int		k;

    for(k=0; k < 3; k++){
      vtx = meshTr->nodes[elem->nodes[k]].position;
      vtx.vtX += meshTr->nodes[elem->nodes[k]].displacement.vtX;
      vtx.vtY += meshTr->nodes[elem->nodes[k]].displacement.vtY;
      vtx.vtX -= xOff;
      vtx.vtY -= yOff;
      vtx = warpDisplayTransFore(vtx, winStruct);
      points[k].x = vtx.vtX;
      points[k].y = vtx.vtY;
    }
    points[3] = points[0];
    
    /* check for duff elements */
    if( warpGlobals.meshErrFlg ){
      if( WlzMeshElemVerify(meshTr, 1, elem, NULL) != WLZ_ERR_NONE ){
	gc = warpGlobals.red_gc;
      }
      else {
	gc = warpGlobals.yellow_gc;
      }
    }
    XDrawLines(dpy, dst_win, gc, points, 4, CoordModeOrigin);
  }
  XFlush(dpy);

  return;
}

void warpDisplaySrcMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  WlzMeshTransform	*meshTr = warpGlobals.meshTr;
  int			elemIdx;
  WlzMeshElem		*elem;
  int			xOff, yOff;
  Display	*dpy=XtDisplay(warpGlobals.src.canvas);
  Window	src_win = XtWindow(warpGlobals.src.canvas);
  GC		gc;

  /* check there is a source image */
  if( !warpGlobals.src.obj || !warpGlobals.meshTr ){
    return;
  }
  xOff = warpGlobals.src.obj->domain.i->kol1;
  yOff = warpGlobals.src.obj->domain.i->line1;

  /* check the ximage */
  if( !winStruct->ximage ){
    warpSetXImage(winStruct);
    warpSetOvlyXImage(winStruct);
    if( !winStruct->ximage ){
      return;
    }
  }

  /* check the graphics context */
  if( warpGlobals.blue_gc == (GC) -1 ){
    XGCValues	values;

    values.foreground = 0xff0000;
    values.background = 0;
    warpGlobals.blue_gc = XCreateGC(dpy, src_win,
				   GCForeground|GCBackground,
				   &values);
  }
  gc = warpGlobals.blue_gc;

  /* loop through the mesh displaying edges
     - note this covers each edge twice */
  elem = meshTr->elements;
  for(elemIdx=0; elemIdx < meshTr->nElem; elemIdx++, elem++){
    XPoint	points[4];
    WlzDVertex2	vtx;
    int		k;

    for(k=0; k < 3; k++){
      vtx = meshTr->nodes[elem->nodes[k]].position;
      vtx.vtX -= xOff;
      vtx.vtY -= yOff;
      vtx = warpDisplayTransFore(vtx, winStruct);
      points[k].x = vtx.vtX;
      points[k].y = vtx.vtY;
    }
    points[3] = points[0];
    XDrawLines(dpy, src_win, gc, points, 4, CoordModeOrigin);
  }
  XFlush(dpy);

  return;
}

void warpCanvasExposeCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;

  if( !winStruct->ximage ){
    warpSetXImage(winStruct);
    warpSetOvlyXImage(winStruct);
    if( !winStruct->ximage ){
      return;
    }
  }

  /* check the graphics context */
  if( winStruct->gc == (GC) -1 ){
    XGCValues  gcvalues;
    winStruct->gc = XCreateGC(XtDisplay(winStruct->canvas),
			      XtWindow(winStruct->canvas),
			      0, &gcvalues);
  }

  /* display the image - if it is a real expose event just
     put the exposed rectangle */
  if( cbs && (cbs->event->type == Expose) ){
    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage,
	      cbs->event->xexpose.x, cbs->event->xexpose.y,
	      cbs->event->xexpose.x, cbs->event->xexpose.y,
	      cbs->event->xexpose.width, cbs->event->xexpose.height);
  }
  else {
    XPutImage(XtDisplay(winStruct->canvas), XtWindow(winStruct->canvas),
	      winStruct->gc, winStruct->ximage, 0, 0, 0, 0,
	      winStruct->ximage->width, winStruct->ximage->height);
  }
  XFlush(XtDisplay(winStruct->canvas));

  warpDisplayTiePoints();
  return;
}

void warpIncrGammaCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  winStruct->gamma *= 1.1;
  if( winStruct->gamma > 10.0 ){
    winStruct->gamma = 10.0;
  }

  warpSetXImage(winStruct);
  warpSetOvlyXImage(winStruct);
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpDecrGammaCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  winStruct->gamma /= 1.1;
  if( winStruct->gamma < 0.1 ){
    winStruct->gamma = 0.1;
  }

  warpSetXImage(winStruct);
  warpSetOvlyXImage(winStruct);
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasMagCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  if( cbs->event->xbutton.state & Mod1Mask ){
    if( winStruct->mag > 0.25 ){
      winStruct->mag /= 2.0;
    }
  }
  else {
    if( winStruct->mag < 16.0 ){
      winStruct->mag *= 2.0;
    }
  }

  warpSetXImage(winStruct);
  warpSetOvlyXImage(winStruct);
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasRotCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  if( cbs->event->xbutton.state & Mod1Mask ){
    winStruct->rot -= 1;
  }
  else {
    winStruct->rot += 1;
  }
  while( winStruct->rot < 0 ){
    winStruct->rot += 4;
  }
  winStruct->rot %= 4;

  warpSetXImage(winStruct);
  warpSetOvlyXImage(winStruct);
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasFlipCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  winStruct->transpose = (winStruct->transpose)?0:1;

  warpSetXImage(winStruct);
  warpSetOvlyXImage(winStruct);
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}

void warpCanvasResetCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;

  winStruct->rot = 0;
  winStruct->transpose = 0;
  winStruct->mag = 1.0;

  warpSetXImage(winStruct);
  warpSetOvlyXImage(winStruct);
  XtCallCallbacks(winStruct->canvas, XmNexposeCallback, call_data);

  return;
}


void warpCanvasMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  MAPaintImageWinStruct	*winStruct;
  XmToggleButtonCallbackStruct
    *cbs = (XmToggleButtonCallbackStruct *) call_data;
  int		winId = (int) client_data;

  switch( winId ){
  case 0 :
    winStruct = &(warpGlobals.dst);
    if( cbs->set == True ){
      XtAddCallback(winStruct->canvas, XmNexposeCallback,
		    warpDisplayDstMeshCb, (XtPointer) winStruct);
      warpDisplayDstMeshCb(winStruct->canvas,
			   (XtPointer) winStruct, call_data);
    }
    else {
      XtRemoveCallback(winStruct->canvas, XmNexposeCallback,
		       warpDisplayDstMeshCb, (XtPointer) winStruct);
      warpCanvasExposeCb(w, (XtPointer) winStruct, call_data);
    }
    break;
  case 1 :
    winStruct = &(warpGlobals.src);
    if( cbs->set == True ){
      XtAddCallback(winStruct->canvas, XmNexposeCallback,
		    warpDisplaySrcMeshCb, (XtPointer) winStruct);
      warpDisplaySrcMeshCb(winStruct->canvas,
			   (XtPointer) winStruct, call_data);
    }
    else {
      XtRemoveCallback(winStruct->canvas, XmNexposeCallback,
		       warpDisplaySrcMeshCb, (XtPointer) winStruct);
      warpCanvasExposeCb(w, (XtPointer) winStruct, call_data);
    }
    break;
  case 2 :
    winStruct = &(warpGlobals.ovly);
    break;
  }

  return;
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

void warpReadSourceCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzObject		*obj;
  FILE			*fp;
  WlzErrorNum		errNum;

  if( (fp = HGU_XmGetFilePointer(view_struct->dialog, cbs->value,
				 cbs->dir, "r")) == NULL ){
    return;
  }

  /* read the new source object - ready to extend to other types */
  obj = WlzEffReadObj(fp, NULL, WLZEFF_FORMAT_WLZ, &errNum);
  fclose(fp);

  /* check the object */
  if( errNum == WLZ_ERR_NONE ){
    if( obj ){
      switch( obj->type ){
      case WLZ_2D_DOMAINOBJ:
	/* set the source object */
	if( warpGlobals.src.obj ){
	  WlzFreeObj(warpGlobals.src.obj);
	}
	warpGlobals.src.obj = WlzAssignObject(obj, &errNum);
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
	warpSetOvlyXImage(&(warpGlobals.ovly));
	warpCanvasExposeCb(warpGlobals.ovly.canvas,
			   (XtPointer) &(warpGlobals.ovly),
			   call_data);
	break;

      default:
	WlzFreeObj(obj);
	HGU_XmUserError(globals.topl,
			"Read Source Object:\n"
			"    Wrong object type, please select\n"
			"    another image file.",
			XmDIALOG_FULL_APPLICATION_MODAL);
	break;
      }
    }
  }

  return;
}

void warpReadSourcePopupCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  Arg		arg[1];
  Visual	*visual;

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(globals.topl);
  XtSetArg(arg[0], XmNvisual, visual);

  if( warp_read_src_dialog == NULL ){
    warp_read_src_dialog =
      XmCreateFileSelectionDialog(view_struct->dialog,
				  "warp_read_src_dialog", arg, 1);

    XtAddCallback(warp_read_src_dialog, XmNokCallback,
		  warpReadSourceCb, client_data);
    XtAddCallback(warp_read_src_dialog, XmNokCallback,
		  PopdownCallback, NULL);
    XtAddCallback(warp_read_src_dialog, XmNcancelCallback, 
		  PopdownCallback, NULL);
  }

  XtManageChild(warp_read_src_dialog);
  PopupCallback(w, (XtPointer) XtParent(warp_read_src_dialog), NULL);

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
  }

  if( warpGlobals.src.obj ){
    WlzFreeObj(warpGlobals.src.obj);
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
  Widget		dialog;

  dialog = view_struct->dialog;

  if( cbs->set == True ){
    if( paint_key == view_struct ){
      if( !warpGlobals.warp2DInteractDialog ){
	warpGlobals.warp2DInteractDialog =
	  create2DWarpDialog(dialog, view_struct);
      }
      XtManageChild(warpGlobals.warp2DInteractDialog);
    }
  }
  else {
    if( warpGlobals.warp2DInteractDialog ){
      XtUnmanageChild( warpGlobals.warp2DInteractDialog );
    }
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
  int			wasManaged;
  Dimension		shellHeight, cntrlFormHeight;
  WlzCompoundArray	*cobj;

  /* get the section viewer frames and unmanage everything */
  dialog = view_struct->dialog;
  shell = XtParent(dialog);
  cntrlFrame = XtNameToWidget(dialog, "*.warp_input_2d_frame");
  cntrlForm = XtNameToWidget(dialog, "*.warp_input_2d_form");
  XtVaGetValues(shell, XmNheight, &shellHeight, NULL);
  XtVaGetValues(cntrlForm, XmNheight, &cntrlFormHeight, NULL);
  XtVaSetValues(dialog, XmNdefaultPosition, False, NULL);
  if( XtIsManaged(cntrlForm) ){
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
  XtVaSetValues(shell, XmNheight, shellHeight, NULL);
  if( wasManaged == False ){
    XtManageChild(cntrlForm);
  }
  XtManageChild(cntrlFrame);

  /* reset input callback and orientation controls sensitivity */
  XtSetSensitive(view_struct->controls, wasManaged);
  XtSetSensitive(view_struct->slider, wasManaged );
  setControlButtonsSensitive(view_struct, wasManaged);
  if( magFncForm = XtNameToWidget(dialog, "*.paint_function_row_col") ){
    XtSetSensitive(magFncForm, wasManaged);
  }

  if( wasManaged == False ){
    /* swap the callbacks to realignment mode */
    XtRemoveCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		     client_data);

    /* get the paint key */
    paint_key = view_struct;

    /* set the destination object and clear the ximage */
    if( warpGlobals.dst.obj ){
      WlzFreeObj(warpGlobals.dst.obj);
    }
    warpGlobals.dst.obj =
      WlzAssignObject(WlzGetSectionFromObject(globals.orig_obj,
					      view_struct->wlzViewStr, NULL),
		      NULL);

    if( warpGlobals.dst.ximage ){
      AlcFree(warpGlobals.dst.ximage->data);
      warpGlobals.dst.ximage->data = NULL;
      XDestroyImage(warpGlobals.dst.ximage);
      warpGlobals.dst.ximage = NULL;
    }

    /* set the overlay compound object */
    if( warpGlobals.ovly.obj ){
      cobj = (WlzCompoundArray *) warpGlobals.ovly.obj;
      cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);
    }
    else {
      cobj = WlzMakeCompoundArray(WLZ_COMPOUND_ARR_2, 1, 2, NULL,
				  WLZ_2D_DOMAINOBJ, NULL);
      cobj->n = 1;
      cobj->o[0] = WlzAssignObject(warpGlobals.dst.obj, NULL);
      warpGlobals.ovly.obj = (WlzObject *) cobj;
    }
    if( warpGlobals.ovly.ximage ){
      AlcFree(warpGlobals.ovly.ximage->data);
      warpGlobals.ovly.ximage->data = NULL;
      XDestroyImage(warpGlobals.ovly.ximage);
      warpGlobals.ovly.ximage = NULL;
    }
	
  }
  else {
    /* swap the callbacks to normal input mode */
    XtAddCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		  client_data);

    /* release the paint key */
    paint_key = NULL;
  }

  return;
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

static int	resetOvlyFlg=0;

static void warpDstCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  int		vtxIdx;
  WlzDVertex2	vtx;

  /* check there is and image */
  if( winStruct->ximage == NULL ){
    return;
  }

  switch( cbs->event->type ){
  case ButtonPress:
    vtx.vtX = cbs->event->xbutton.x;
    vtx.vtY = cbs->event->xbutton.y;

    switch( cbs->event->xbutton.button ){
    case Button1:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	/* set the dst vertex */
	warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	warpGlobals.tp_state = TP_DST_DEFINED;
	break;

      case TP_DST_DEFINED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.dst),
			 warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
	warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	break;

      case TP_SRC_DEFINED:
	/* set vertex, increment number of tie-points */
	warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	warpGlobals.tp_state = TP_SELECTED;
	warpGlobals.sel_vtx = warpGlobals.num_vtxs;
	warpGlobals.num_vtxs++;
	resetOvlyFlg = 1;
	break;

      case TP_SELECTED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.dst),
			 warpGlobals.dst_vtxs[warpGlobals.sel_vtx]);
	warpGlobals.dst_vtxs[warpGlobals.sel_vtx] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	resetOvlyFlg = 1;
	break;
      }
      warpDisplayTiePoints();
      break;

    case Button2:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	break;

      case TP_DST_DEFINED:
	/* delete vertex */
	warpUndisplayVtx(&(warpGlobals.dst),
			 warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
	warpGlobals.tp_state = TP_INACTIVE;
	break;

      case TP_SRC_DEFINED:
	break;

      case TP_SELECTED:
	/* delete vertex */
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
      break;

    case Button3:
      break;
    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){
    case Button1:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	break;
      case TP_DST_DEFINED:
	break;
      case TP_SRC_DEFINED:
	break;
      case TP_SELECTED:
	break;
      }
      warpDisplayTiePoints();
      break;

    case Button2:
    case Button3:
      break;
    }
    if( resetOvlyFlg ){
      /* update the overlay image and display */
      warpSetOvlyObject();
      warpSetOvlyXImage(&(warpGlobals.ovly));
      warpCanvasExposeCb(warpGlobals.ovly.canvas,
			 (XtPointer) &(warpGlobals.ovly),
			 call_data);
      XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		      call_data);
      resetOvlyFlg = 0;
    }
    break;

  case EnterNotify:
    break;
  case LeaveNotify:
    break;

  case MotionNotify:
    vtx.vtX = cbs->event->xmotion.x;
    vtx.vtY = cbs->event->xmotion.y;

    if( cbs->event->xmotion.state & (Button1Mask) ){
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
      break;

      case TP_DST_DEFINED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.dst),
			 warpGlobals.dst_vtxs[warpGlobals.num_vtxs]);
	warpGlobals.dst_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	warpDisplayTiePoints();
	break;

      case TP_SRC_DEFINED:
	break;

      case TP_SELECTED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.dst),
		      warpGlobals.dst_vtxs[warpGlobals.sel_vtx]);
	warpGlobals.dst_vtxs[warpGlobals.sel_vtx] =
	  warpDisplayTransBack(vtx, &(warpGlobals.dst));
	warpDisplayTiePoints();
	resetOvlyFlg = 1;
	break;
      }
    }
    else {
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	if( (vtxIdx = warpCloseDstVtx(cbs->event->xmotion.x,
				      cbs->event->xmotion.y)) > -1 ){
	  warpGlobals.sel_vtx = vtxIdx;
	  warpGlobals.tp_state = TP_SELECTED;
	  warpDisplayTiePoints();
	}
	break;

      case TP_DST_DEFINED:
	break;
      case TP_SRC_DEFINED:
	break;
      case TP_SELECTED:
	if( (vtxIdx = warpCloseDstVtx(cbs->event->xmotion.x,
				      cbs->event->xmotion.y)) > -1 ){
	  warpGlobals.sel_vtx = vtxIdx;
	  warpGlobals.tp_state = TP_SELECTED;
	}
	else {
	  warpGlobals.tp_state = TP_INACTIVE;
	}
	warpDisplayTiePoints();
	break;
      }
    }
    break;
  case KeyPress:
    break;
  case KeyRelease:
    break;
  default:
    break;
  }

  return;
}

static void warpSrcCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintImageWinStruct	*winStruct = (MAPaintImageWinStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  int		vtxIdx;
  WlzDVertex2	vtx;

  /* check there is and image */
  if( winStruct->ximage == NULL ){
    return;
  }

  switch( cbs->event->type ){
  case ButtonPress:
    vtx.vtX = cbs->event->xbutton.x;
    vtx.vtY = cbs->event->xbutton.y;

    switch( cbs->event->xbutton.button ){
    case Button1:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	/* set the src vertex */
	warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	warpGlobals.tp_state = TP_SRC_DEFINED;
	break;

      case TP_SRC_DEFINED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.src),
		      warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
	warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	break;

      case TP_DST_DEFINED:
	/* set vertex, increment number of tie-points */
	warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	warpGlobals.tp_state = TP_SELECTED;
	warpGlobals.sel_vtx = warpGlobals.num_vtxs;
	warpGlobals.num_vtxs++;
	resetOvlyFlg = 1;
	break;

      case TP_SELECTED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.src),
			 warpGlobals.src_vtxs[warpGlobals.sel_vtx]);
	warpGlobals.src_vtxs[warpGlobals.sel_vtx] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	resetOvlyFlg = 1;
	break;
      }
      warpDisplayTiePoints();
      break;

    case Button2:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	break;

      case TP_SRC_DEFINED:
	/* delete vertex */
	warpUndisplayVtx(&(warpGlobals.src),
			 warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
	warpGlobals.tp_state = TP_INACTIVE;
	break;

      case TP_DST_DEFINED:
	break;

      case TP_SELECTED:
	/* delete vertex */
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
      break;

    case Button3:
      break;
    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){
    case Button1:
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	break;
      case TP_DST_DEFINED:
	break;
      case TP_SRC_DEFINED:
	break;
      case TP_SELECTED:
	break;
      }
      warpDisplayTiePoints();
      break;

    case Button2:
    case Button3:
      break;
    }
    if( resetOvlyFlg ){
      /* update the overlay image and display */
      warpSetOvlyObject();
      warpSetOvlyXImage(&(warpGlobals.ovly));
      warpCanvasExposeCb(warpGlobals.ovly.canvas,
			 (XtPointer) &(warpGlobals.ovly),
			 call_data);
      XtCallCallbacks(warpGlobals.dst.canvas, XmNexposeCallback,
		      call_data);
      resetOvlyFlg = 0;
    }
    break;

  case EnterNotify:
    break;
  case LeaveNotify:
    break;

  case MotionNotify:
    vtx.vtX = cbs->event->xmotion.x;
    vtx.vtY = cbs->event->xmotion.y;

    if( cbs->event->xmotion.state & (Button1Mask) ){
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
      break;

      case TP_SRC_DEFINED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.src),
			 warpGlobals.src_vtxs[warpGlobals.num_vtxs]);
	warpGlobals.src_vtxs[warpGlobals.num_vtxs] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	warpDisplayTiePoints();
	break;

      case TP_DST_DEFINED:
	break;

      case TP_SELECTED:
	/* reset vertex */
	warpUndisplayVtx(&(warpGlobals.src),
			 warpGlobals.src_vtxs[warpGlobals.sel_vtx]);
	warpGlobals.src_vtxs[warpGlobals.sel_vtx] =
	  warpDisplayTransBack(vtx, &(warpGlobals.src));
	warpDisplayTiePoints();
	resetOvlyFlg = 1;
	break;
      }
    }
    else {
      switch( warpGlobals.tp_state ){
      case TP_INACTIVE:
	if( (vtxIdx = warpCloseSrcVtx(cbs->event->xmotion.x,
				      cbs->event->xmotion.y)) > -1 ){
	  warpGlobals.sel_vtx = vtxIdx;
	  warpGlobals.tp_state = TP_SELECTED;
	  warpDisplayTiePoints();
	}
	break;

      case TP_DST_DEFINED:
	break;
      case TP_SRC_DEFINED:
	break;
      case TP_SELECTED:
	if( (vtxIdx = warpCloseSrcVtx(cbs->event->xmotion.x,
				      cbs->event->xmotion.y)) > -1 ){
	  warpGlobals.sel_vtx = vtxIdx;
	  warpGlobals.tp_state = TP_SELECTED;
	}
	else {
	  warpGlobals.tp_state = TP_INACTIVE;
	}
	warpDisplayTiePoints();
	break;
      }
    }
    break;

  case KeyPress:
    break;
  case KeyRelease:
    break;
  default:
    break;
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

static Widget createWarpDisplayFrame(
  Widget	parent,
  String	name,
  Visual	*visual,
  int		depth)
{
  Widget	frame, title, scrolled_window, canvas;
  Widget	button;

  /* create the frame and frame children */
  frame = XtVaCreateManagedWidget(name, xmFrameWidgetClass,
				  parent, NULL);
  title = XtVaCreateManagedWidget("title", xmRowColumnWidgetClass, frame,
				  XmNchildType, 	XmFRAME_TITLE_CHILD,
				  XmNorientation,	XmHORIZONTAL,
				  NULL);
  scrolled_window =
    XtVaCreateManagedWidget("scrolled_window", xmScrolledWindowWidgetClass,
			    frame,
			    XmNchildType, XmFRAME_WORKAREA_CHILD,
			    XmNscrollingPolicy, 	XmAUTOMATIC,
			    XmNborderWidth, 0,
			    NULL);

  /* create the buttons */
  button = XtVaCreateManagedWidget("b_1", xmPushButtonWidgetClass,
				   title, NULL);
  button = XtVaCreateManagedWidget("b_2", xmPushButtonWidgetClass,
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


  /* create the canvas */
  canvas = XtVaCreateManagedWidget("canvas", hgu_DrawingAreaWidgetClass,
				   scrolled_window,
				   XmNwidth, 512,
				   XmNheight, 512,
				   XmNshadowThickness, 0,
				   XtNdepth,	depth,
				   XtNvisual,	visual,
				   NULL);

  return frame;
}

static ActionAreaItem   warp_interact_actions[] = {
{"reset",	NULL,		NULL},
{"dismiss",	NULL,           NULL},
{"help",	NULL,           NULL},
};

static Widget create2DWarpDialog(
  Widget parent,
  ThreeDViewStruct *view_struct)
{
  Widget	dialog, control, child, button;
  Visual	*visual;

  /* create a dialog widget and get control form */
  dialog = HGU_XmCreateStdDialog(parent, "warp2DInteractDialog",
				 xmFormWidgetClass,
				 warp_interact_actions, 3);
  control = XtNameToWidget( dialog, "*.control" );
  if( !(visual = HGU_XGetVisual(globals.dpy, DefaultScreen(globals.dpy),
			      TrueColor, 24)) ){
    visual = HGU_XmWidgetToVisual(globals.topl);
  }
  button = XtNameToWidget( dialog, "*.dismiss" );
  XtAddCallback(button, XmNactivateCallback, warp2DInteractDismissCb, NULL);
		

  /* add children */
  child = createWarpDisplayFrame(control, "warp_dst_frame", visual, 24);
  XtVaSetValues(child,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	0,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	33,
		NULL);
  warpGlobals.dst.canvas = XtNameToWidget(child, "*canvas");
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
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagCb,
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
		XmNtopAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	33,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	67,
		NULL);
  warpGlobals.src.canvas = XtNameToWidget(child, "*canvas");
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
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagCb,
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
		XmNtopAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	67,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	100,
		NULL);
  warpGlobals.ovly.canvas = XtNameToWidget(child, "*canvas");
  warpGlobals.ovly.ximage = NULL;
  warpGlobals.ovly.mag = 1.0;
  warpGlobals.ovly.rot = 0;
  warpGlobals.ovly.transpose = 0;

  /* canvas callbacks and event handler */
  XtAddCallback(warpGlobals.ovly.canvas, XmNexposeCallback,
		warpCanvasExposeCb, (XtPointer) &(warpGlobals.ovly));

  /* button callbacks */
  button = XtNameToWidget(child, "*b_1");
  XtAddCallback(button, XmNactivateCallback, warpCanvasMagCb,
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

  return dialog;
}

static ActionAreaItem   warp_controls_actions[] = {
{"src_read",	NULL,		NULL},
{"src_transf",	NULL,		NULL},
{"i_o",		NULL,           NULL},
{"import",	NULL,           NULL},
};

static MenuItem mesh_menu_itemsP[] = {		/* mesh_menu items */
  {"block", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshMethodCb, (XtPointer) WLZ_MESH_GENMETHOD_BLOCK,
   HGU_XmHelpStandardCb, "paint/paint.html#view_menu",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"gradient", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpMeshMethodCb, (XtPointer) WLZ_MESH_GENMETHOD_GRADIENT,
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
  Widget	control, frame, form, title, controls_frame;
  Widget	option_menu, button, buttons, radio_box, label, widget;
  Widget	slider;
  float		fval, fmin, fmax;

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

  /* defaults for the mesh distances */
  warpGlobals.meshMinDst = 20.0;
  warpGlobals.meshMaxDst = 100.0;

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
  warp_controls_actions[0].callback = warpReadSourcePopupCb;
  warp_controls_actions[0].client_data = view_struct;

  buttons = HGU_XmCreateWActionArea(form,
				    warp_controls_actions,
				    XtNumber(warp_controls_actions),
				    xmPushButtonWidgetClass);

  /* set the buttons attachments */
  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);


  /* modify attachments for the orientation controls frame */
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
  warpGlobals.dst.obj = NULL;
  warpGlobals.src.obj = NULL;
  warpGlobals.ovly.obj = NULL;
  warpGlobals.dst.ximage = NULL;
  warpGlobals.src.ximage = NULL;
  warpGlobals.ovly.ximage = NULL;
  warpGlobals.dst.gc = (GC) -1;
  warpGlobals.src.gc = (GC) -1;
  warpGlobals.ovly.gc = (GC) -1;
  warpGlobals.dst.gamma = 1.0;
  warpGlobals.src.gamma = 1.0;
  warpGlobals.ovly.gamma = 1.0;

  warpGlobals.num_vtxs = 0;
  warpGlobals.sel_vtx = 0;
  warpGlobals.tp_state = TP_INACTIVE;
  warpGlobals.green_gc = (GC) -1;
  warpGlobals.red_gc = (GC) -1;
  warpGlobals.blue_gc = (GC) -1;
  warpGlobals.yellow_gc = (GC) -1;

  warpGlobals.affine = NULL;
  warpGlobals.basisTr = NULL;
  warpGlobals.meshTr = NULL;
  warpGlobals.meshErrFlg = 0;
  warpGlobals.meshMthd = WLZ_MESH_GENMETHOD_BLOCK;

  return( dialog );
}
