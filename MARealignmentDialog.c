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
*   File       :   MARealignmentDialog.c				*
* $Revision$
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Sun Mar 14 16:43:06 1999				*
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

#include <Reconstruct.h>

extern Widget create_view_window_dialog(Widget topl,
					double theta,
					double phi,
					WlzDVertex3 *fixed);

static int			alignBufferSize=100;
static XPoint			*srcPoly=NULL, *dstPoly=NULL, *tmpPoly=NULL;
static WlzObject		*origPaintedObject=NULL;
static WlzObject		*origRefObj=NULL, *origObj=NULL;
static WlzObject		*transformsObj=NULL;

void realignmentCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	dialog, widget;

  if( dialog = createRealignmentDialog(globals.topl) ){
    if( widget = XtNameToWidget(globals.topl, "*options_menu*realignment") ){
      XtSetSensitive(widget, False);
    }
    XtManageChild(dialog);
  }

  return;
}

void realignmentDestroyCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget		widget;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;

  if( origRefObj ){
    WlzFreeObj(origRefObj);
    origRefObj = NULL;
  }
  if( origObj ){
    WlzFreeObj(origObj);
    origObj = NULL;
  }
  if( origPaintedObject ){
    WlzFreeObj(origPaintedObject);
    origPaintedObject = NULL;
  }

  if( srcPoly ){
    AlcFree((void *) srcPoly);
    srcPoly = NULL;
  }
  if( dstPoly ){
    AlcFree((void *) dstPoly);
    dstPoly = NULL;
  }

  if( transformsObj ){
    WlzFreeObj(transformsObj);
    transformsObj = NULL;
  }

  if( widget = XtNameToWidget(globals.topl, "*options_menu*realignment") ){
    XtSetSensitive(widget, True);
  }
  
  if( paint_key == view_struct ){
    paint_key = NULL;
  }
  
  return;
}

void realignSetImage(
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  int		width, height, oldWidth;
  int		i, offset;

  /* check offsets for each line and set the image */
  width = view_struct->ximage->width;
  oldWidth = origPaintedObject->values.r->width;
  height = view_struct->ximage->height;
  memset((void *) view_struct->ximage->data, 0, width*height);
  for(i=0; i < height; i++){
    offset = alignBufferSize + dstPoly[i].x - srcPoly[i].x;
    memcpy((void *) (view_struct->ximage->data + i * width + offset),
	   (void *) (origPaintedObject->values.r->values.ubp + i * oldWidth),
	   WLZ_MIN(oldWidth, width - offset));
  }

  return;
}

void realignDisplayPolysCb(
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

  /* re-display the image */
  redisplay_view_cb(w, client_data, call_data);

  /* need to check for scaling */
  if( wlzViewStr->scale > 1.0 ){
    for(i=0; i < view_struct->ximage->height; i++){
      tmpPoly[i].x = srcPoly[i].x * wlzViewStr->scale;
      tmpPoly[i].y = srcPoly[i].y * wlzViewStr->scale;
    }
    (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				 "red");
    XDrawLines(dpy, win, gc, tmpPoly, view_struct->ximage->height,
	       CoordModeOrigin);

    for(i=0; i < view_struct->ximage->height; i++){
      tmpPoly[i].x = dstPoly[i].x * wlzViewStr->scale;
      tmpPoly[i].y = dstPoly[i].y * wlzViewStr->scale;
    }
    (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				 "green");
    XDrawLines(dpy, win, gc, tmpPoly, view_struct->ximage->height,
	       CoordModeOrigin);
  }
  else {
    (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				 "red");
    XDrawLines(dpy, win, gc, srcPoly, view_struct->ximage->height,
	       CoordModeOrigin);

    (void) HGU_XColourFromNameGC(dpy, globals.cmap, globals.gc_set,
				 "green");
    XDrawLines(dpy, win, gc, dstPoly, view_struct->ximage->height,
	       CoordModeOrigin);
  }

  XFlush(dpy);
  
  return;
}

void resetRealignPolyCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  Widget		widget;
  Boolean		srcPolySet;
  int			i;

  if( widget = XtNameToWidget(view_struct->dialog,
			      "*.src_dest_select.src_poly" ) ){
    XtVaGetValues(widget, XmNset, &srcPolySet, NULL);
  }
  else {
    return;
  }

  if( srcPolySet ){
    for(i=0; i < view_struct->ximage->height; i++){
      srcPoly[i].x = view_struct->ximage->width/2;
      dstPoly[i].x = srcPoly[i].x;
    }
  }
  else {
    for(i=0; i < view_struct->ximage->height; i++){
      dstPoly[i].x = srcPoly[i].x;
    }
  }

  realignSetImage(view_struct);
  realignDisplayPolysCb(w, client_data, call_data);
  return;
}

WlzObject *WlzAffineTransformObj3D(
  WlzObject	*obj,
  WlzObject	*transObj,
  WlzErrorNum	*dstErr)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*rtnObj=NULL;
  WlzDomain	domain;
  WlzValues	values;
  int		p, nPlanes;
  WlzObject	*obj1, *obj2;

  /* make the new object */
  domain.p = WlzMakePlaneDomain(obj->domain.p->type,
				obj->domain.p->plane1,
				obj->domain.p->lastpl,
				obj->domain.p->line1,
				obj->domain.p->lastln,
				obj->domain.p->kol1,
				obj->domain.p->lastkl,
				&errNum);
  for(p=0; p < 3; p++){
    domain.p->voxel_size[p] = obj->domain.p->voxel_size[p];
  }

  if( obj->values.core ){
    values.vox = WlzMakeVoxelValueTb(obj->values.vox->type,
				     obj->values.vox->plane1,
				     obj->values.vox->lastpl,
				     obj->values.vox->bckgrnd,
				     NULL, &errNum);
  }
  else {
    values.vox = NULL;
  }
  rtnObj = WlzMakeMain(origRefObj->type, domain, values,
		       NULL, NULL, &errNum);

  /* apply the transform plane by plane */
  nPlanes = obj->domain.p->lastpl - obj->domain.p->plane1 + 1;
  for(p=0; p < nPlanes; p++){
    /* check if the reference object has empty domains */
    if((obj->domain.p->domains[p].core == NULL) ||
       (obj->domain.p->domains[p].core->type == WLZ_EMPTY_OBJ) ||
       (obj->domain.p->domains[p].core->type == WLZ_EMPTY_DOMAIN)){
      rtnObj->domain.p->domains[p].core = NULL;
      continue;
    }
      
    /* get the plane to be transformed */
    if( obj->values.core ){
      obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			 obj->domain.p->domains[p],
			 obj->values.vox->values[p],
			 NULL, NULL, NULL);
    }
    else {
      obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			 obj->domain.p->domains[p],
			 obj->values,
			 NULL, NULL, NULL);
    }
    obj1 = WlzAssignObject(obj1, NULL);

    /* now transform it */
    if(((p + obj->domain.p->plane1) >= transObj->domain.p->plane1) &&
       ((p + obj->domain.p->plane1) <= transObj->domain.p->lastpl) &&
       (transObj->domain.p->domains[p]).t ){
      obj2 = WlzAffineTransformObj(obj1,
				   (transObj->domain.p->domains[p]).t,
				   WLZ_INTERPOLATION_NEAREST, NULL);
      rtnObj->domain.p->domains[p] = WlzAssignDomain(obj2->domain, NULL);
      rtnObj->values.vox->values[p] = WlzAssignValues(obj2->values, NULL);
      WlzFreeObj(obj2);
    }
    else {
      rtnObj->domain.p->domains[p] = WlzAssignDomain(obj1->domain, NULL);
      rtnObj->values.vox->values[p] = WlzAssignValues(obj1->values, NULL);
    }
    WlzFreeObj(obj1);
  }

  /* standardise the plane domain and voxel table */
   WlzStandardPlaneDomain(rtnObj->domain.p, rtnObj->values.vox);

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

WlzObject *WlzMakeTransformObj3D(
  WlzObject	*obj,
  WlzErrorNum	*dstErr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  WlzDomain	domain;
  WlzValues	values;
  WlzObject	*rtnObj=NULL;
  int		i;

  if( domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_AFFINE,
				    obj->domain.p->plane1,
				    obj->domain.p->lastpl,
				    obj->domain.p->line1,
				    obj->domain.p->lastln,
				    obj->domain.p->kol1,
				    obj->domain.p->lastkl,
				    &errNum) ){
    for(i=0; i < 3; i++){
      domain.p->voxel_size[i] = obj->domain.p->voxel_size[i];
    }
    values.core = NULL;
    rtnObj = WlzMakeMain(WLZ_3D_DOMAINOBJ,
			 domain, values, NULL, NULL, NULL);
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

static void realignResetTransformsObj(void)
{
  int		i, p;

  /* set all transforms to the unit transform */
  if( transformsObj ){
    for(p=transformsObj->domain.p->plane1, i=0;
	p <=  transformsObj->domain.p->lastpl;
	p++, i++){
      if( (transformsObj->domain.p->domains[i]).t ){
	WlzFreeAffineTransform((transformsObj->domain.p->domains[i]).t);
      }
      (transformsObj->domain.p->domains[i]).t = 
	WlzAssignAffineTransform(
	  WlzAffineTransformFromPrim(WLZ_TRANSFORM_2D_AFFINE,
				     0.0, 0.0, 0.0, 1.0,
				     0.0, 0.0, 0.0, 0.0, 0.0,
				     0, NULL),
	  NULL);
    }
  }
  
  return;
}

static void realignUpdateTransformsObj(
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  int		i, p, nPlanes = view_struct->ximage->height;
  int		x, y, origWidth;
  int		kol1, kol2, line1, line2, plane1, plane2;
  WlzAffineTransform	*newTrans, *tmpTrans;

  /* fill transforms as required */
  for(i=0; i < nPlanes; i++){
    /* get true coordinates and build the new transform */
    origWidth = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
    x = srcPoly[i].x / wlzViewStr->scale - alignBufferSize;
    x = WLZ_MAX(0, x);
    x = WLZ_MIN(x, origWidth);
    y = i;
    kol1 = (int) (wlzViewStr->xp_to_x[x] + wlzViewStr->yp_to_x[y]);
    line1 = (int) (wlzViewStr->xp_to_y[x] + wlzViewStr->yp_to_y[y]);
    plane1 = (int) (wlzViewStr->xp_to_z[x] + wlzViewStr->yp_to_z[y]);

    x = dstPoly[i].x / wlzViewStr->scale - alignBufferSize;
    x = WLZ_MAX(0, x);
    x = WLZ_MIN(x, origWidth);
    kol2 = (int) (wlzViewStr->xp_to_x[x] + wlzViewStr->yp_to_x[y]);
    line2 = (int) (wlzViewStr->xp_to_y[x] + wlzViewStr->yp_to_y[y]);
    plane2 = (int) (wlzViewStr->xp_to_z[x] + wlzViewStr->yp_to_z[y]);

    if( plane1 != plane2 ){
      HGU_XmUserError(view_struct->dialog,
		      "Something wrong here, original planes\n"
		      "not coming out equal. Please check the\n"
		      "pitch angle which should be 90 degrees",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      return;
    }
    newTrans = WlzAffineTransformFromPrim(WLZ_TRANSFORM_2D_AFFINE,
					  (double) kol2 - kol1,
					  (double) line2 - line1,
					  0.0, 1.0, 0.0, 0.0,
					  0.0, 0.0, 0.0, 0, NULL);

    /* apply to existing transform */
    p = plane1 - transformsObj->domain.p->plane1;
    if( (transformsObj->domain.p->domains[p]).t ){
      tmpTrans = WlzAffineTransformProduct((transformsObj->domain.p->domains[p]).t,
					   newTrans, NULL);
      WlzFreeAffineTransform((transformsObj->domain.p->domains[p]).t);
      WlzFreeAffineTransform(newTrans);
      (transformsObj->domain.p->domains[p]).t =
	WlzAssignAffineTransform(tmpTrans, NULL);
    }
    else {
      (transformsObj->domain.p->domains[p]).t =
	WlzAssignAffineTransform(newTrans, NULL);
    }
  }

  return;
}

static int transformsUpdatedFlg=0;

void applyRealignTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  WlzObject	*newOrigObj, *newObj;
  Widget	widget;

  /* check we really want to do this */
  if( !HGU_XmUserConfirm(view_struct->dialog,
			 "Do you really want to transform\n"
			 "the whole 3D stack?", "Yes", "No", 1) ){
    return;
  }

  /* check the view is valid for this operation: pitch == 90 */
  if( fabs(wlzViewStr->phi - WLZ_M_PI_2) > 1.0e-2 ){
    HGU_XmUserError(view_struct->dialog,
		    "Invalid pitch value. For this\n"
		    "operation please set pitch = 90.0\n"
		    "and redefine the alignment",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* set hour glass */
  HGU_XmSetHourGlassCursor(globals.topl);
  HGU_XmSetHourGlassCursor(XtParent(view_struct->dialog));

  /* set up a transform object and update with current */
  if( transformsObj == NULL ){
    if( !(transformsObj =
	  WlzAssignObject(WlzMakeTransformObj3D(origObj, NULL), NULL)) ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to allocate memory for this\n"
		      "operation. Probably you need to restart\n"
		      "MAPaint and try again or make a smaller\n"
		      "reference image",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      return;
    }
  }
  if( !transformsUpdatedFlg ){
    realignUpdateTransformsObj(view_struct);
  }

  /* make a new reference object and painted volume */
  newOrigObj = WlzAffineTransformObj3D(origRefObj, transformsObj, NULL);
  newObj = WlzAffineTransformObj3D(origObj, transformsObj, NULL);

  /* install the new reference object and painted image */
  if( globals.orig_obj != NULL ){
    WlzFreeObj( globals.orig_obj );
  }
  globals.orig_obj = WlzAssignObject(newOrigObj, NULL);

  if( globals.obj != NULL ){
    WlzFreeObj( globals.obj );
  }
  globals.obj = WlzAssignObject(newObj, NULL);
  
  /* reset  displays */
  setup_ref_display_list_cb(NULL, NULL, NULL);
  setup_obj_props_cb(NULL, NULL, NULL);
    
  /* reset realignment controls */
  if( widget = XtNameToWidget(view_struct->dialog,
			      "*.realignment_frame_title") ){
    XmToggleButtonCallbackStruct cbs;

    XtVaSetValues(widget, XmNset, False, NULL);
    cbs.set = False;
    XtCallCallbacks(widget, XmNvalueChangedCallback, (XtPointer) &cbs);
  }

  /* unset hour glass */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));
  realignResetTransformsObj();

  return;
}

void undoRealignTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget		widget;
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;

  /* install the new reference object and painted image */
  if( globals.orig_obj != NULL ){
    WlzFreeObj( globals.orig_obj );
  }
  globals.orig_obj = WlzAssignObject(origRefObj, NULL);

  if( globals.obj != NULL ){
    WlzFreeObj( globals.obj );
  }
  globals.obj = WlzAssignObject(origObj, NULL);
  
  /* reset  displays */
  setup_ref_display_list_cb(NULL, NULL, NULL);
  setup_obj_props_cb(NULL, NULL, NULL);

  /* reset realignment controls */
  if( widget = XtNameToWidget(view_struct->dialog,
			      "*.realignment_frame_title") ){
    XmToggleButtonCallbackStruct cbs;

    XtVaSetValues(widget, XmNset, False, NULL);
    cbs.set = False;
    XtCallCallbacks(widget, XmNvalueChangedCallback, (XtPointer) &cbs);
  }

  /* clear the transform object */
  if( transformsObj ){
    WlzFreeObj(transformsObj);
    transformsObj = NULL;
  }

  return;
}

void writeRealignTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  String		fileStr;
  FILE			*fp;
  int			i, p;
  BibFileRecord	*record = NULL;
  BibFileField	*field0 = NULL,
		*field1 = NULL,
		*field2 = NULL;
  char		tmpBuf[256], *eMsg;
  char		*tmpS,
		*idxS = NULL,
		*dateS = NULL,
		*hostS = NULL,
		*userS = NULL,
                *fileS = NULL;
  char 		tTypeS[32],
		tTxS[32],
		tTyS[32],
		tTzS[32],
		tScaleS[32],
		tThetaS[32],
		tPhiS[32],
		tAlphaS[32],
		tPsiS[32],
		tXsiS[32],
		tInvertS[32];
  static char	unknownS[] = "unknown";
  time_t	tmpTime;

  /* get a bib-file filename */
  if( fileStr =
     HGU_XmUserGetFilename(globals.topl,
			   "Please type in a filename\n"
			   "for the transforms bib-file\n",
			   "OK", "cancel", "MAPaintRealign.bib",
			   NULL, "*.bib") ){
    if( (fp = fopen(fileStr, "w")) == NULL ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to open the bib-file. Please\n"
		      "check the filename and permissions.\n",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      AlcFree((void *) fileStr);
      return;
    }
  }
  else {
    return;
  }
  AlcFree((void *) fileStr);

  /* set hour glass */
  HGU_XmSetHourGlassCursor(globals.topl);
  HGU_XmSetHourGlassCursor(XtParent(view_struct->dialog));

  /* set up a transform object and update with current */
  if( transformsObj == NULL ){
    if( !(transformsObj =
	  WlzAssignObject(WlzMakeTransformObj3D(origObj, NULL), NULL)) ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to allocate memory for this\n"
		      "operation. Probably you need to restart\n"
		      "MAPaint and try again or make a smaller\n"
		      "reference image",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      return;
    }
  }
  realignUpdateTransformsObj(view_struct);

  /* run through the transform list writing a bibfile for each
     plane. Use correct plane numbers but do not define a
     file name (since its not known)
     Note the relative transform from the previous section is used.
  */

  /* an identifier record - who, when, how */
  if((((field0 = BibFileFieldMakeVa("Text", "MAPaint realignment section file",
				    "Version", "1",
				    "TransformType", "absolute",
				    NULL)) == NULL) ||
      ((record = BibFileRecordMake("Ident", "0", field0)) == NULL)))
  {
    HGU_XmUnsetHourGlassCursor(globals.topl);
    HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));
    fclose(fp);
    return;
  }
  BibFileRecordWrite(fp, &eMsg, record);
  BibFileRecordFree(&record);

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

  (void )sprintf(tmpBuf, "File: %s", globals.file?globals.file:unknownS);
  fileS = AlcStrDup(tmpBuf);

  if((((field0 = BibFileFieldMakeVa("Text", userS,
				    "Text", dateS,
				    "Text", hostS,
				    "Text", fileS,
				    NULL)) == NULL) ||
       ((record = BibFileRecordMake("Comment", "0", field0)) == NULL)))
  {
    HGU_XmUnsetHourGlassCursor(globals.topl);
    HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));
    fclose(fp);
    return;
  }
  BibFileRecordWrite(fp, &eMsg, record);
  BibFileRecordFree(&record);
  AlcFree(dateS);
  AlcFree(hostS);
  AlcFree(userS);

  /* now the section records */
  if( transformsObj ){
    for(p=transformsObj->domain.p->plane1, i=0;
	p <= transformsObj->domain.p->lastpl; p++, i++){
      WlzAffineTransform	*transf=transformsObj->domain.p->domains[i].t;
      if( transf == NULL ){
	continue;
      }
      sprintf(tmpBuf, "%d", p);
      idxS = AlcStrDup(tmpBuf);

      (void )sprintf(tTypeS, "%d", transf->type);
      (void )sprintf(tTxS, "%g", transf->tx);
      (void )sprintf(tTyS, "%g", transf->ty);
      (void )sprintf(tTzS, "%g", transf->tz);
      (void )sprintf(tScaleS, "%g", transf->scale);
      (void )sprintf(tThetaS, "%g", transf->theta);
      (void )sprintf(tPhiS, "%g", transf->phi);
      (void )sprintf(tAlphaS, "%g", transf->alpha);
      (void )sprintf(tPsiS, "%g", transf->psi);
      (void )sprintf(tXsiS, "%g", transf->xsi);
      (void )sprintf(tInvertS, "%d", transf->invert);
      field0 = BibFileFieldMakeVa(
	"TransformType", tTypeS,
	"TransformTx", tTxS,
	"TransformTy", tTyS,
	"TransformTz", tTzS,
	"TransformScale", tScaleS,
	"TransformTheta", tThetaS,
	"TransformPhi", tPhiS,
	"TransformAlpha", tAlphaS,
	"TransformPsi", tPsiS,
	"TransformXsi", tXsiS,
	"TransformInvert", tInvertS,
	NULL);

      record = BibFileRecordMake("Section", idxS, field0);
      BibFileRecordWrite(fp, &eMsg, record);
      BibFileRecordFree(&record);
      AlcFree(idxS);
    }
  }
  fclose(fp);


  /* unset hour glass */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));

  /* check for apply transform */
  if( HGU_XmUserConfirm(view_struct->dialog,
			"Apply this transform?\n",
			"Yes", "No", 1) ){
    transformsUpdatedFlg = 1;
    applyRealignTransCb(w, client_data, call_data);
    transformsUpdatedFlg = 0;
  }
  else {
    realignResetTransformsObj();
  }

  return;
}

void readRealignTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmPushButtonCallbackStruct
    *cbs = (XmPushButtonCallbackStruct *) call_data;
  String		fileStr;
  FILE			*fp;
  int			i, p;
  char			*errMsg;
  int			numParsedFields=0;
  BibFileRecord		*bibfileRecord;
  BibFileField		*bibfileField;
  BibFileError		bibFileErr=BIBFILE_ER_NONE;
  double		tx=0.0, ty=0.0, theta=0.0;
  WlzAffineTransform	*inTrans, *tmpTrans;

  /* get a bib-file filename */
  if( fileStr =
     HGU_XmUserGetFilename(globals.topl,
			   "Please type in a filename\n"
			   "for the transforms bib-file\n",
			   "OK", "cancel", "MAPaintRealign.bib",
			   NULL, "*.bib") ){
    if( (fp = fopen(fileStr, "r")) == NULL ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to open the bib-file. Please\n"
		      "check the filename and permissions.\n",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      AlcFree((void *) fileStr);
      return;
    }
  }
  else {
    return;
  }
  AlcFree((void *) fileStr);

  /* set hour glass */
  HGU_XmSetHourGlassCursor(globals.topl);
  HGU_XmSetHourGlassCursor(XtParent(view_struct->dialog));

  /* set up a transform object and update with current */
  if( transformsObj == NULL ){
    if( !(transformsObj =
	  WlzAssignObject(WlzMakeTransformObj3D(origObj, NULL), NULL)) ){
      HGU_XmUserError(view_struct->dialog,
		      "Failed to allocate memory for this\n"
		      "operation. Probably you need to restart\n"
		      "MAPaint and try again or make a smaller\n"
		      "reference image",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      return;
    }
  }

  /* check for transform on top of current or to replace it */
  if( HGU_XmUserConfirm(view_struct->dialog,
			"Do you want the transforms\n"
			"read in to overwrite the current\n"
			"transform (i.e. the current line"
			"offsets) or to extend the current\n"
			"transforms?",
			"Overwrite", "Extend", 1) ){
    realignResetTransformsObj();
  }
  else {
    realignUpdateTransformsObj(view_struct);
  }

  /* now read the bibfile applying any transform with a matching
     plane number to the current transform.
     In this version assume the bibfile is from MAPaint therefore a 
     set of absolute transforms */
  bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
  while( bibFileErr == BIBFILE_ER_NONE ) 
  {
    /* should check what type of bibfile here */
    if( strncmp(bibfileRecord->name, "Section", 7) ){
      BibFileRecordFree(&bibfileRecord);
    }
    else {
      /* check if this corresponds to a plane in the reference object */
      p = atoi(bibfileRecord->id);
      if((p < transformsObj->domain.p->plane1) ||
	 (p > transformsObj->domain.p->lastpl)){
	BibFileRecordFree(&bibfileRecord);
	BibFileRecordRead(&bibfileRecord, &errMsg, fp);
	continue;
      }
      p -= transformsObj->domain.p->plane1;

      /* parse the record */
      numParsedFields = BibFileFieldParseFmt
	(bibfileRecord->field,
	 (void *) &tx, "%lg", "TransformTx",
	 (void *) &ty, "%lg", "TransformTy",
	 (void *) &theta, "%lg", "TransformTheta",
	 NULL);

      /* make the transform for this record */
      inTrans = WlzAffineTransformFromPrim(WLZ_TRANSFORM_2D_AFFINE,
					   tx, ty, 0.0, 1.0,
					   theta, 0.0, 0.0, 0.0, 0.0, 0, NULL);

      /* if concatenate with the existing transforms */
      if( transformsObj->domain.p->domains[p].t ){
	tmpTrans =
	  WlzAffineTransformProduct(transformsObj->domain.p->domains[p].t,
				    inTrans, NULL);
	WlzFreeAffineTransform(transformsObj->domain.p->domains[p].t);
	WlzFreeAffineTransform(inTrans);
	transformsObj->domain.p->domains[p].t =
	  WlzAssignAffineTransform(tmpTrans, NULL);
      }
      else {
	transformsObj->domain.p->domains[p].t =
	  WlzAssignAffineTransform(inTrans, NULL);
      }

      BibFileRecordFree(&bibfileRecord);
    }
    bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
  }
  fclose(fp);

  /* now apply the transform read in - this is the only way to get the
     image to change since resetting the source and destination lines might
     be too painful because each transform would have to projected into
     translations othogonal and parallel to the current view. */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  HGU_XmUnsetHourGlassCursor(XtParent(view_struct->dialog));
  transformsUpdatedFlg = 1;
  applyRealignTransCb(w, client_data, call_data);
  transformsUpdatedFlg = 0;

  return;
}

static int	lastX, lastY;
static int	lineModeFlg;
static int	setSrcPolyFlg;

void realignment_input_cb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y, origWidth, domain;
  int			i;
  Widget		widget;
  Boolean		toggleSet;

  /* switch on event type */
  switch( cbs->event->type ){

   case ButtonPress:
     switch( cbs->event->xbutton.button ){

      case Button1:
	x = cbs->event->xbutton.x / wlzViewStr->scale;
	y = cbs->event->xbutton.y / wlzViewStr->scale;
	if( (y < 0) || (y >= view_struct->ximage->height) ){
	  break;
	}

	/* establish the modes */
	lineModeFlg = 0;
	if( widget = XtNameToWidget(view_struct->dialog,
				    "*.straight_line") ){
	  XtVaGetValues(widget, XmNset, &toggleSet, NULL);
	  if( toggleSet ){
	    lineModeFlg = 1;
	  }
	}
	setSrcPolyFlg = 0;
	if( widget = XtNameToWidget(view_struct->dialog,
				    "*.src_poly") ){
	  XtVaGetValues(widget, XmNset, &toggleSet, NULL);
	  if( toggleSet ){
	    setSrcPolyFlg = 1;
	  }
	}

	dstPoly[y].x = x;
	if( setSrcPolyFlg ){
	  srcPoly[y].x = x;
	}
	realignSetImage(view_struct);
	realignDisplayPolysCb(w, client_data, call_data);
	lastX = x;
	lastY = y;
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
	domain = imageValueToDomain
	  ((unsigned int) *((UBYTE *) (view_struct->ximage->data + x +
				       y * view_struct->ximage->bytes_per_line)));
	origWidth = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
	x -= alignBufferSize;
	x = WLZ_MAX(0, x);
	x = WLZ_MIN(x, origWidth);
	display_pointer_feedback_informationV(view_struct, x, y, domain);
	break;

      default:
	break;

     }
     break;

   case MotionNotify:
     if( cbs->event->xmotion.state & Button1Mask ){
	x = cbs->event->xmotion.x / wlzViewStr->scale;
	y = cbs->event->xmotion.y / wlzViewStr->scale;
	if( y == lastY ){
	  break;
	}
	if( (y < 0) || (y >= view_struct->ximage->height) ){
	  break;
	}
	i=lastY;
	while( i != y ){
	  i += (lastY < y) ? 1 : -1;
	  dstPoly[i].x = ((y-i)*lastX*1024 + (i-lastY)*x*1024)/(y-lastY)/1024;
	  if( setSrcPolyFlg ){
	    srcPoly[i].x = dstPoly[i].x;
	  }
	}
	realignSetImage(view_struct);
	realignDisplayPolysCb(w, client_data, call_data);
	if( !lineModeFlg ){
	  lastX = x;
	  lastY = y;
	}
     }

     if( cbs->event->xmotion.state & Button2Mask )
     {
	x = cbs->event->xmotion.x / wlzViewStr->scale;
	y = cbs->event->xmotion.y / wlzViewStr->scale;
	domain = imageValueToDomain
	  ((unsigned int) *((UBYTE *) (view_struct->ximage->data + x +
				       y * view_struct->ximage->bytes_per_line)));
	origWidth = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
	x -= alignBufferSize;
	x = WLZ_MAX(0, x);
	x = WLZ_MIN(x, origWidth);
	display_pointer_feedback_informationV(view_struct, x, y, domain);
     }
     break;

  case KeyPress:
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
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

  return;
}

static void realignment_setup_cb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmToggleButtonCallbackStruct 
    *cbs=(XmToggleButtonCallbackStruct *) call_data;
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  WlzObject	*obj;
  WlzIBox2	newSize;
  int		width, height;
  int		i;

  if( cbs->set == True ){
    /* install a new painted_image and redisplay */
    newSize.xMin = view_struct->painted_object->domain.i->kol1 -
      alignBufferSize;
    newSize.xMax = view_struct->painted_object->domain.i->lastkl +
      alignBufferSize;
    newSize.yMin = view_struct->painted_object->domain.i->line1;
    newSize.yMax = view_struct->painted_object->domain.i->lastln;
    width = newSize.xMax - newSize.xMin + 1;
    height = newSize.yMax - newSize.yMin + 1;
    obj = WlzCutObjToBox2D(view_struct->painted_object, newSize,
			   WLZ_GREY_UBYTE, 0, 0.0, 0.0, NULL);
    origPaintedObject = WlzAssignObject(view_struct->painted_object, NULL);
    WlzFreeObj(view_struct->painted_object);
    view_struct->painted_object = WlzAssignObject(obj, NULL);

    view_struct->ximage->width = width;
    view_struct->ximage->height = height;
    view_struct->ximage->bytes_per_line = width;
    view_struct->ximage->data = (char *) obj->values.r->values.ubp;
    XtVaSetValues(view_struct->canvas,
		  XmNwidth, (Dimension) (width*wlzViewStr->scale),
		  XmNheight, (Dimension) (height*wlzViewStr->scale),
		  NULL);

    /* set default source polyline */
    srcPoly = (XPoint *) AlcMalloc(sizeof(XPoint) * height);
    for(i=0; i < height; i++){
      srcPoly[i].x = width/2;
      srcPoly[i].y = i;
    }

    /* set default destination polyline */
    dstPoly = (XPoint *) AlcMalloc(sizeof(XPoint) * height);
    for(i=0; i < height; i++){
      dstPoly[i].x = width/2;
      dstPoly[i].y = i;
    }
    tmpPoly = (XPoint *) AlcMalloc(sizeof(XPoint) * height);

    /* set the image, display the polyline and redisplay */
    realignSetImage(view_struct);
    realignDisplayPolysCb(view_struct->canvas, client_data, call_data);

    XtAddCallback(view_struct->canvas, XmNexposeCallback,
		  realignDisplayPolysCb, view_struct);
  }
  else {
    /* clear stored data */
    XtRemoveCallback(view_struct->canvas, XmNexposeCallback,
		     realignDisplayPolysCb, view_struct);
    reset_view_struct(view_struct);
    display_view_cb(widget, client_data, call_data);
    AlcFree((void *) srcPoly);
    AlcFree((void *) dstPoly);
    AlcFree((void *) tmpPoly);
    WlzFreeObj(origPaintedObject);
    srcPoly = dstPoly = tmpPoly = NULL;
    origPaintedObject = NULL;
  }
  return;
}

static void realignment_controls_cb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct=(ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr=view_struct->wlzViewStr;
  Widget		shell, dialog, cntrlFrame, cntrlForm, magFncForm;
  int			wasManaged;
  Dimension		shellHeight, cntrlFormHeight;

  /* get the section viewer frames and unmanage everything */
  dialog = view_struct->dialog;
  shell = XtParent(dialog);
  cntrlFrame = XtNameToWidget(dialog, "*.realignment_frame");
  cntrlForm = XtNameToWidget(dialog, "*.realignment_form");
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
    XtAddCallback(view_struct->canvas, XmNinputCallback, realignment_input_cb,
		  client_data);

    /* get the paint key */
    paint_key = view_struct;
  }
  else {
    /* swap the callbacks to normal input mode */
    XtRemoveCallback(view_struct->canvas, XmNinputCallback, realignment_input_cb,
		     client_data);
    XtAddCallback(view_struct->canvas, XmNinputCallback, canvas_input_cb,
		  client_data);

    /* release the paint key */
    paint_key = NULL;
  }

  return;
}

static ActionAreaItem   realign_controls_actions[] = {
{"reset",	NULL,		NULL},
{"apply",	NULL,		NULL},
{"undo",	NULL,		NULL},
{"read",	NULL,           NULL},
{"write",	NULL,           NULL},
};

static MenuItem poly_menu_itemsP[] = {		/* poly_menu items */
  {"1", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
/*  {"2", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"3", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"4", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},*/
  NULL,
};

Widget createRealignmentDialog(
  Widget	topl)
{
  Widget	dialog=NULL;
  WlzDVertex3		fixed;
  ThreeDViewStruct	*view_struct;
  Widget	control, frame, form, title, controls_frame;
  Widget	option_menu, button, buttons, radio_box, label, widget;

  /* check for reference object */
  if( !globals.obj ){
    return NULL;
  }

  /* should call reset fixed point for this */
  fixed.vtX = (globals.obj->domain.p->kol1 + globals.obj->domain.p->lastkl)/2;
  fixed.vtY = (globals.obj->domain.p->line1 + globals.obj->domain.p->lastln)/2;
  fixed.vtZ = (globals.obj->domain.p->plane1 + globals.obj->domain.p->lastpl)/2;
  dialog = create_view_window_dialog(topl, 0.0, WLZ_M_PI/2, &fixed);

  /* prevent use for painting */
  XtVaGetValues(dialog, XmNuserData, &view_struct, NULL);
  view_struct->noPaintingFlag = 1;
  view_struct->titleStr = "Realigment dialog";

  /* reset the dialog title */
  set_view_dialog_title(dialog, 0.0, WLZ_M_PI/2);

  /* get controls form and controls frame to add realignment controls */
  control = XtNameToWidget( dialog, "*.control" );
  controls_frame = XtNameToWidget( control, "*.controls_frame");

  /* add a new frame */
  frame = XtVaCreateManagedWidget("realignment_frame",
				  xmFrameWidgetClass, control,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  XmNbottomAttachment,	XmATTACH_FORM,
				  NULL);
  form = XtVaCreateWidget("realignment_form",
			  xmFormWidgetClass, 	frame,
			  XmNleftAttachment,	XmATTACH_FORM,
			  XmNrightAttachment,	XmATTACH_FORM,
			  XmNtopAttachment,	XmATTACH_FORM,
			  XmNchildType, XmFRAME_WORKAREA_CHILD,
			  NULL);  
  title = XtVaCreateManagedWidget("realignment_frame_title",
				  xmToggleButtonGadgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  XtAddCallback(title, XmNvalueChangedCallback, realignment_controls_cb,
		view_struct);
  XtAddCallback(title, XmNvalueChangedCallback, realignment_setup_cb,
		view_struct);

  /* now the controls */
  option_menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "poly_select", 0,
				XmTEAR_OFF_DISABLED, poly_menu_itemsP);
  XtVaSetValues(option_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);
  widget = option_menu;
  XtManageChild(widget);

  radio_box = XmCreateRadioBox(form, "src_dest_select", NULL, 0);
  XtVaSetValues(radio_box,
		XmNorientation, XmHORIZONTAL,
		XmNtopAttachment,  XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, widget,
		XmNspacing, 0,
		XmNborderWidth,	0,
		XmNmarginHeight, 0,
		XmNmarginWidth, 0,
		XmNpacking, XmPACK_TIGHT,
		NULL);
  button = XtVaCreateManagedWidget("src_poly",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   NULL);
  button = XtVaCreateManagedWidget("dst_poly",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   XmNset, True,
				   NULL);
  XtVaSetValues(radio_box, XmNmenuHistory, button, NULL);
  widget = radio_box;
  XtManageChild(widget);

  label = XtVaCreateManagedWidget("poly_mode_select_label",
				  xmLabelWidgetClass, form,
				  XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
				  XmNtopWidget, widget,
				  XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
				  XmNbottomWidget, widget,
				  XmNleftAttachment, XmATTACH_WIDGET,
				  XmNleftWidget, widget,
				  XmNborderWidth,	0,
				  NULL);
  widget = label;

  radio_box = XmCreateRadioBox(form, "poly_mode_select", NULL, 0);
  XtVaSetValues(radio_box,
		XmNorientation, XmHORIZONTAL,
		XmNtopAttachment,  XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, widget,
		XmNspacing, 0,
		XmNborderWidth,	0,
		XmNmarginHeight, 0,
		XmNmarginWidth, 0,
		XmNpacking, XmPACK_TIGHT,
		NULL);
  button = XtVaCreateManagedWidget("polyline",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   XmNset, True,
				   NULL);
  XtVaSetValues(radio_box, XmNmenuHistory, button, NULL);
  button = XtVaCreateManagedWidget("straight_line",
				   xmToggleButtonGadgetClass, radio_box,
				   XmNindicatorOn, False,
				   XmNborderWidth, 1,
				   XmNhighlightThickness, 0,
				   XmNshadowThickness, 3,
				   NULL);
  widget = radio_box;
  XtManageChild(widget);

  /* now some buttons */
  realign_controls_actions[0].callback = resetRealignPolyCb;
  realign_controls_actions[0].client_data = view_struct;
  realign_controls_actions[1].callback = applyRealignTransCb;
  realign_controls_actions[1].client_data = view_struct;
  realign_controls_actions[2].callback = undoRealignTransCb;
  realign_controls_actions[2].client_data = view_struct;
  realign_controls_actions[3].callback = readRealignTransCb;
  realign_controls_actions[3].client_data = view_struct;
  realign_controls_actions[4].callback = writeRealignTransCb;
  realign_controls_actions[4].client_data = view_struct;
  buttons = HGU_XmCreateWActionArea(form,
				    realign_controls_actions,
				    XtNumber(realign_controls_actions),
				    xmPushButtonWidgetClass);

  /* set the buttons attachments */
  XtVaSetValues(buttons,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		option_menu,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  /* modify attachments for the orientation controls frame */
  XtVaSetValues(controls_frame,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,	frame,
		NULL);

  /* copy the original reference object and painted volume
     and add a callback to the destroy callback list */
  origRefObj = WlzAssignObject(globals.orig_obj, NULL);
  origObj = WlzAssignObject(globals.obj, NULL);
  XtAddCallback(dialog, XmNdestroyCallback, realignmentDestroyCb,
		view_struct);

  return( dialog );
}
