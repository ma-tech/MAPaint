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
*   File       :   MAConformalPoly.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Mar 29 19:10:25 1999				*
*   $Revision$						*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <MAPaint.h>

static int	numCoords;
static int	x1, x2, y11, y2, z1, z2;
static int	specialTriggered;
static WlzPolygonDomain	*confPoly;
static int	numMeshPolys;
static WlzPolygonDomain *meshPolys[64];

#define MAXXPOINTS	512
static XPoint	xpoints[MAXXPOINTS];

static void DisplayPolyF(
  Display		*dpy,
  Window		win,
  GC			gc,
  WlzPolygonDomain	*poly)
{
  WlzFVertex2	*fvtxs;
  int		i;

  /* check the polygon */
  if( poly == NULL ){
    return;
  }

  /* copy to an X line list */
  fvtxs = (WlzFVertex2 *) poly->vtx;
  for(i=0; (i < poly->nvertices) && (i < MAXXPOINTS); i++, fvtxs++){
    xpoints[i].x = fvtxs->vtX;
    xpoints[i].y = fvtxs->vtY;
  }

  /* display the polyline */
  XDrawLines(dpy, win, gc, xpoints, i, CoordModeOrigin);
  XFlush( dpy );
  return;
}

static void DisplayBoundF(
  Display	*dpy,
  Window	win,
  GC		gc,
  WlzBoundList	*bound)
{
  /* check boundary */
  if( bound == NULL ){
    return;
  }

  /* display the polyline */
  DisplayPolyF(dpy, win, gc, bound->poly);

  /* display the next bound elements */
  DisplayBoundF(dpy, win, gc, bound->next);

  return;
}

WlzBasisFnTransform *WlzConfPolyFromCPts(
  WlzFnType		type,
  int 			order,
  int 			nDPts,
  WlzDVertex2 		*dPts,
  int 			nSPts,
  WlzDVertex2 		*sPts,
  WlzErrorNum 		*dstErr)
{
  int  		idM,
  		idN,
		idX,
  		idY,
		nCoef;
  double	thresh,
  		wMax;
  double	*bMx = NULL,
  		*wMx = NULL;
  double	**aMx = NULL,
  		**vMx = NULL;
  WlzDVertex2	powVx,
  		sVx;
  WlzBasisFnTransform *basis = NULL;
  WlzBasisFn	*basisFn;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  int		nPts = nDPts;
  const double	tol = 1.0e-06;

  /* allocate space */
  if((order < 0) || (nPts <= 0))
  {
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else if((basis = (WlzBasisFnTransform *)
	           AlcCalloc(sizeof(WlzBasisFnTransform), 1)) == NULL)
  {
    errNum = WLZ_ERR_MEM_ALLOC;
  }
  else
  {
    if((basisFn = (WlzBasisFn *)
	AlcCalloc(sizeof(WlzBasisFn), 1)) == NULL){
      AlcFree(basis);
      basis = NULL;
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    else {
     basis->type = WLZ_TRANSFORM_2D_BASISFN;
     basis->linkcount = 0;
     basis->freeptr = NULL;
     basisFn->type = WLZ_FN_BASIS_2DCONF_POLY;
     basisFn->nPoly = order;
     basisFn->nBasis = 0;
     basisFn->nVtx = 0;
     basisFn->param = AlcMalloc(sizeof(double));
     *((double *) basisFn->param) = 0.0;
    }
  }

  if(errNum == WLZ_ERR_NONE)
  {
    nCoef = (order + 1);
    if(((wMx = (double *)AlcCalloc(sizeof(double), nCoef)) == NULL) ||
       ((bMx = (double *)AlcMalloc(sizeof(double) * nPts)) == NULL) ||
       (AlcDouble2Malloc(&vMx, nPts, nCoef) != ALC_ER_NONE) ||
       (AlcDouble2Malloc(&aMx, nPts, nCoef) !=  ALC_ER_NONE) ||
       ((basisFn->poly.v = AlcMalloc(sizeof(WlzDVertex2) * nCoef)) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }

  if(errNum == WLZ_ERR_NONE)
  {
    /* Fill matrix A. */
    for(idM = 0; idM < nPts; ++idM)
    {
      idN = 0;
      powVx.vtX = 1.0;
      sVx = *(sPts + idM);
      for(idX = 0; idX <= basisFn->nPoly; ++idX)
      {
	*(*(aMx + idM) + idX) = powVx.vtX;
	powVx.vtX *= sVx.vtX;
      }
    }
    /* Perform singular value decomposition of matrix A. */
    errNum= WlzErrorFromAlg(AlgMatrixSVDecomp(aMx, nPts, nCoef, wMx, vMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Edit the singular values. */
    wMax = 0.0;
    for(idN = 0; idN < nCoef; ++idN)
    {
      if(*(wMx + idN) > wMax)
      {
	wMax = *(wMx + idN);
      }
    }
    thresh = tol * wMax;
    for(idN = 0; idN < nCoef; ++idN)
    {
      if(*(wMx + idN) < thresh)
      {
	*(wMx + idN) = 0.0;
      }
    }
    /* Fill matrix b for x coordinate */
    for(idM = 0; idM < nPts; ++idM)
    {
      *(bMx + idM) = (dPts + idM)->vtX;
    }
    /* Solve for x polynomial coefficients. */
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nPts, nCoef, wMx, vMx,
    					        bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Copy out the x polynomial coefficients, fill matrix b for
       y coordinate and re-solve. */
    for(idN = 0; idN < nCoef; ++idN)
    {
      (basisFn->poly.d2 + idN)->vtX = *(bMx + idN);
    }
    for(idM = 0; idM < nPts; ++idM)
    {
      *(bMx + idM) = (dPts + idM)->vtY;
    }
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nPts, nCoef, wMx, vMx,
    						bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Copy out the ypolynomial coefficients. */
    for(idN = 0; idN < nCoef; ++idN)
    {
      (basisFn->poly.d2 + idN)->vtY = *(bMx + idN);
    }
  }
  if(bMx)
  {
    AlcFree(bMx);
  }
  if(wMx)
  {
    AlcFree(wMx);
  }
  if(aMx)
  {
    (void )AlcDouble2Free(aMx);
  }
  if(vMx)
  {
    (void )AlcDouble2Free(vMx);
  }
  if(errNum != WLZ_ERR_NONE)
  {
    if(basisFn)
    {
      if(basisFn->poly.v)
      {
	AlcFree(basisFn->poly.v);
      }
      if(basisFn->param){
	AlcFree(basisFn->param);
      }
      AlcFree(basisFn);
    }
    if( basis ){
      AlcFree(basis);
      basis = NULL;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return basis;
}

WlzFVertex2	WlzConfPolyTransformVertexF(WlzBasisFnTransform *basis,
					   WlzFVertex2 srcVx,
					   WlzErrorNum *dstErr)
{
  int		i;
  ComplexD	z, w, powW, a, b;
  WlzFVertex2	dspVx;
  WlzDVertex2	*polyP;

  polyP = basis->basisFn->poly.d2;
  w.re = srcVx.vtX;
  w.im = srcVx.vtY;
  z.re = polyP[0].vtX;
  z.im = polyP[0].vtY;
  powW.re = 1.0;
  powW.im = 0.0;

  for(i=1; i <= basis->basisFn->nPoly; i++){
    powW = AlgCMult(powW, w);
    a.re = polyP[i].vtX;
    a.im = polyP[i].vtY;
    b = AlgCMult(a, powW);
    z.re += b.re;
    z.im += b.im;
  }
  dspVx.vtX = z.re;
  dspVx.vtY = z.im;

  return(dspVx);
}

void conformalCalculateMeshCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  WlzBasisFnTransform	*basis;
  WlzFVertex2		*fvtxs, fvtx;
  WlzDVertex2		*dPts, *sPts;
  int			nPts;
  int			i, j;
  double		dist, length;
  WlzErrorNum		wlzErr;

  /* check for polygon */
  if( confPoly == NULL ){
    return;
  }

  /* calculate the source and destination polylines hence
     the conformal transformation */
  nPts = confPoly->nvertices;
  dPts = (WlzDVertex2 *) AlcMalloc(sizeof(WlzDVertex2) * nPts);
  sPts = (WlzDVertex2 *) AlcMalloc(sizeof(WlzDVertex2) * nPts);
  length = 0.0;
  fvtxs = (WlzFVertex2 *) confPoly->vtx;
  dPts[0].vtX = fvtxs[0].vtX;
  dPts[0].vtY = fvtxs[0].vtY;
  sPts[0].vtX = length;
  sPts[0].vtY = 0.0;
  for(i=1; i < nPts; i++){
    dist = (fvtxs[i].vtX - fvtxs[i-1].vtX)*(fvtxs[i].vtX - fvtxs[i-1].vtX);
    dist += (fvtxs[i].vtY - fvtxs[i-1].vtY)*(fvtxs[i].vtY - fvtxs[i-1].vtY);
    dist = sqrt(dist);
    length += dist;
    dPts[i].vtX = fvtxs[i].vtX;
    dPts[i].vtY = fvtxs[i].vtY;
    sPts[i].vtX = length;
    sPts[i].vtY = 0.0;
  }
  /* normalise the transformation */
  for(i=0; i < nPts; i++){
/*    sPts[i].vtX /= length;*/
    sPts[i].vtX *= 2.0 * WLZ_M_PI / length;
  }
  nPts /= 4;
  for(i=0; i < nPts; i++){
    sPts[i] = sPts[i*4];
    sPts[i].vtX = cos(sPts[i].vtX);
    sPts[i].vtY = sin(sPts[i].vtX);
    dPts[i] = dPts[i*4];
  }
/*  basis = WlzConfPolyFromCPts(WLZ_BASISFN_CONF_POLY, 6,
			       nPts, dPts, nPts, sPts,
			       &wlzErr);*/
  basis = WlzBasisFnTrFromCPts2D(WLZ_FN_BASIS_2DCONF_POLY, 2,
				 nPts, dPts, nPts, sPts,
				 &wlzErr);

  /* calculate the mesh polylines */
  numMeshPolys = 20;
/*  for(i=0; i < 20; i++){
    meshPolys[i] = WlzMakePolyDmn(WLZ_POLYGON_FLOAT,
				  NULL, 0, 41, 1, NULL);
    fvtxs = (WlzFVertex2 *) meshPolys[i]->vtx;
    fvtx.vtX = ((float) i) / 19.0;
    for(j=0; j < 41; j++){
      fvtx.vtY = ((float) j - 20) / 160.0;
      fvtxs[j] = WlzConfPolyTransformVertexF(basis, fvtx, &wlzErr);
    }
    meshPolys[i]->nvertices = 40;
  }
  for(; i < 40; i++){
    meshPolys[i] = WlzMakePolyDmn(WLZ_POLYGON_FLOAT,
				  NULL, 0, 41, 1, NULL);
    fvtxs = (WlzFVertex2 *) meshPolys[i]->vtx;
    fvtx.vtY = ((float) i - 30.0) / 80.0;
    for(j=0; j < 41; j++){
      fvtx.vtX = ((float) j) / 39.0;
      fvtxs[j] = WlzConfPolyTransformVertexF(basis, fvtx, &wlzErr);
    }
    meshPolys[i]->nvertices = 40;
    }*/
  for(i=0; i < 20; i++){
    double	theta, radius;
    meshPolys[i] = WlzMakePolygonDomain(WLZ_POLYGON_FLOAT, 0,
					NULL, 41, 1, NULL);
    fvtxs = (WlzFVertex2 *) meshPolys[i]->vtx;
    theta = (double) i * 2.0 * WLZ_M_PI / 20.0;
    for(j=0; j < 41; j++){
      radius = (double) j / 20.0;
      fvtx.vtX = radius * cos(theta);
      fvtx.vtY = radius * sin(theta);
      fvtxs[j] = WlzBasisFnTransformVertexF(basis, fvtx, &wlzErr);
    }
    meshPolys[i]->nvertices = 40;
  }
/*  for(; i < 40; i++){
    meshPolys[i] = WlzMakePolyDmn(WLZ_POLYGON_FLOAT,
				  NULL, 0, 41, 1, NULL);
    fvtxs = (WlzFVertex2 *) meshPolys[i]->vtx;
    fvtx.vtY = ((float) i - 30.0) / 80.0;
    for(j=0; j < 41; j++){
      fvtx.vtX = ((float) j) / 39.0;
      fvtxs[j] = WlzConfPolyTransformVertexF(basis, fvtx, &wlzErr);
    }
    meshPolys[i]->nvertices = 40;
    }*/

  AlcFree((void *) dPts);
  AlcFree((void *) sPts);
  if( wlzErr != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "conformalCalculateMeshCb", wlzErr);
  }
  return;
}

void conformalDisplayPolysCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win = XtWindow(view_struct->canvas);
  GC			gc = globals.gc_set;
  int			i;

  /* check for polygon */
  if( confPoly == NULL ){
    return;
  }

  /* re-display the image */
  redisplay_view_cb(w, client_data, call_data);
  DisplayPolyF(dpy, win, gc, confPoly);

  for(i=0; i < numMeshPolys; i++){
    if( meshPolys[i] ){
      DisplayPolyF(dpy, win, gc, meshPolys[i]);
    }
  }
  
  return;
}

void conformal_input_cb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y, widthp, heightp;
  int			kol, line, plane;
  double		distance;
  int			i;
  Widget		widget;
  Boolean		toggleSet;
  WlzPolygonDomain	*startPoly;
  WlzFVertex2		fpVtx, *fvtxs;
  WlzObject		*polygon;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* switch on event type */
  switch( cbs->event->type ){

   case ButtonPress:
     switch( cbs->event->xbutton.button ){

      case Button1:
	specialTriggered = 1;

	/* get the polyline, rescale as required */
	fpVtx.vtX = cbs->event->xbutton.x;
	fpVtx.vtY = cbs->event->xbutton.y;
	startPoly = WlzAssignPolygonDomain(
	  WlzMakePolygonDomain(WLZ_POLYGON_FLOAT, 1,
			       (WlzIVertex2 *) &fpVtx,
			       1, 1, NULL), &errNum);
	if( confPoly ){
	  WlzFreePolyDmn(confPoly);
	}
	globals.paint_action_quit_flag = 0;
	confPoly = HGU_XGetPolyDomain(XtDisplay(view_struct->canvas),
				  XtWindow(view_struct->canvas),
				  0, NULL, startPoly);
	WlzFreePolyDmn(startPoly);
	if( confPoly == NULL ){
	  break;
	}

	/* convert to an 8-connected polyline */
	if( polygon = WlzPolyTo8Polygon(confPoly, 1, &errNum) ){
	  WlzFreePolyDmn(confPoly);
	  if( confPoly = WlzAssignPolygonDomain(
	    WlzMakePolygonDomain(WLZ_POLYGON_INT, 
			   polygon->domain.poly->nvertices,
			   polygon->domain.poly->vtx,
			   polygon->domain.poly->nvertices,
			   1, &errNum), NULL)){
	    confPoly->type = WLZ_POLYGON_FLOAT;
	    fvtxs = (WlzFVertex2 *) confPoly->vtx;
	    for(i=0; i < confPoly->nvertices; i++){
	      fvtxs[i].vtX = (float) confPoly->vtx[i].vtX;
	      fvtxs[i].vtY = (float) confPoly->vtx[i].vtY;
	    }
	  }
	}

	/* determine the conformal approximate coordinate mesh */
	conformalCalculateMeshCb(w, client_data, call_data);

	/* display the polyline and mesh */
	conformalDisplayPolysCb(w, client_data, call_data);

	break;

      case Button2:
	break;

      case Button3:		/* unused */
      default:
	break;

     }
     break;

   case ButtonRelease:
     switch( cbs->event->xbutton.button ){

      case Button1:
	break;

     case Button2:
       x = cbs->event->xbutton.x / wlzViewStr->scale;
       y = cbs->event->xbutton.y / wlzViewStr->scale;
       display_pointer_feedback_information(view_struct, x, y);
       break;

     case Button3:
       /* check if have been drawing */
       if( specialTriggered ){
	 specialTriggered = 0;
	 break;
       }

       /* revert to normal mode */
       view_struct->noPaintingFlag = 0;
       view_struct->titleStr = "Section view";
       set_view_dialog_title(view_struct->dialog,
			     wlzViewStr->theta,
			     wlzViewStr->phi);
       /* set sensitivities */
       XtSetSensitive(view_struct->controls, True);
       XtSetSensitive(view_struct->slider, True);
       setControlButtonsSensitive(view_struct, True);

       /* swap the callbacks to special mode */
       XtRemoveCallback(view_struct->canvas, XmNexposeCallback,
			conformalDisplayPolysCb, view_struct);
       XtRemoveCallback(view_struct->canvas, XmNinputCallback,
			conformal_input_cb, client_data);
       XtAddCallback(view_struct->canvas, XmNinputCallback,
		     canvas_input_cb, client_data);

       /* clear the polyline */
       if( confPoly ){
	 WlzFreePolyDmn(confPoly);
	 confPoly = NULL;
       }

       break;

     default:
       break;

     }
     break;

   case MotionNotify:
     if( cbs->event->xmotion.state & Button1Mask ){
     }

     if( cbs->event->xmotion.state & Button2Mask )
     {
	x = cbs->event->xmotion.x / wlzViewStr->scale;
	y = cbs->event->xmotion.y / wlzViewStr->scale;
	display_pointer_feedback_information(view_struct, x, y);
     }
     break;

  case KeyPress:
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_KP_Right:
    case XK_f:
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_KP_Left:
    case XK_b:
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
    MAPaintReportWlzError(globals.topl, "conformal_input_cb", errNum);
  }
  return;
}

void special_conf_display_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;

  /* call the disarm callbacks to remove any timeouts and avoid posting
     the menu */
  XtCallCallbacks(w, XmNdisarmCallback, call_data);

  /* if already painting do nothing */
  if( paint_key == view_struct ){
    return;
  }

  /* install a call back, don't allow painting */
  view_struct->noPaintingFlag = 1;
  view_struct->titleStr = "Conformal Polygon Special dialog";
  set_view_dialog_title(view_struct->dialog,
			wlzViewStr->theta,
			wlzViewStr->phi);

  /* set sensitivities */
  XtSetSensitive(view_struct->controls, False);
  XtSetSensitive(view_struct->slider, False);
  setControlButtonsSensitive(view_struct, False);

    /* swap the callbacks to special mode */
  XtRemoveCallback(view_struct->canvas, XmNinputCallback,
		   canvas_input_cb, client_data);
  XtAddCallback(view_struct->canvas, XmNinputCallback,
		conformal_input_cb, client_data);
  XtAddCallback(view_struct->canvas, XmNexposeCallback,
		  conformalDisplayPolysCb, view_struct);

  /* set the control globals */
  numCoords = 0;
  specialTriggered = 0;
  confPoly = NULL;

  return;
}

