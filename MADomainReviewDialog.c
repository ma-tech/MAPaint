#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MADomainReviewDialog_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MADomainReviewDialog.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:50:27 2009
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par Copyright:
* Copyright (C) 2005 Medical research Council, UK.
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
*               
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

extern void HGU_XmUserContinue(
  Widget	w,
  String	question,
  String	str);

extern Widget create_view_window_dialog(
  Widget	topl,
  double	theta,
  double        phi,
  WlzDVertex3	*fixed);

void stopReviewCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

Widget reviewDialog=NULL, readReviewObjDialog=NULL;
static Widget viewDialog=NULL;
static ThreeDViewStruct	*reviewViewStr=NULL;

static PaintDomainSourceType reviewSrc=MAPAINT_DOMAIN_REVIEW_OBJ;
static DomainSelection 	sourceDomain=DOMAIN_1;
static int		reviewViewScale=1;

static WlzObject	*reviewDomain=NULL;
static WlzObject	*current_region=NULL;
static WlzObject	*flashing_region=NULL;

static int		current_region_plane;
static int		reviewInProgressFlg=0;
static int		flashing_region_onFlg=0;
static int		flashing_region_visibleFlg=0;
static XtIntervalId	flashing_region_timeoutId;
static GC		reviewGC=NULL;

static void Display2DObj(
  Display	*dpy,
  Window	win,
  GC		gc,
  WlzObject	*obj)
{
  WlzIntervalWSpace	iwsp;

  WlzInitRasterScan(obj, &iwsp, WLZ_RASTERDIR_ILIC);
  while(WlzNextInterval(&iwsp) == WLZ_ERR_NONE){
    XDrawLine(dpy, win, gc,
	      iwsp.lftpos, iwsp.linpos, iwsp.rgtpos, iwsp.linpos);
  }
  XFlush( dpy );
  return;
}


static void flashing_region_timeout_proc(
  XtPointer	client_data,
  XtIntervalId	*id)
{
  if( flashing_region == NULL ){
    return;
  }

  if( (id != NULL) && (*id != flashing_region_timeoutId) ){
    return;
  }

  if( !flashing_region_onFlg  ){
    return;
  }

  if( reviewGC == NULL ){
    reviewGC = HGU_XGetGC(XtDisplay(reviewViewStr->canvas),
			  XtWindow(reviewViewStr->canvas),
			  HGU_XGC_INTERACT);
  }

  /* display the boundary and toggle visibility flag */
  Display2DObj(XtDisplay(reviewViewStr->canvas),
	       XtWindow(reviewViewStr->canvas),
	       reviewGC, flashing_region);

  flashing_region_visibleFlg = !flashing_region_visibleFlg;

  flashing_region_timeoutId = XtAppAddTimeOut(globals.app_con, 500,
					      flashing_region_timeout_proc,
					      NULL);

  XFlush(XtDisplay(reviewViewStr->canvas));
  return;
}

static void set_review_region_flashing(void)
{
  WlzObject	*obj1=NULL, *obj2=NULL, *obj3=NULL;
  int		dx, dy;
  WlzDVertex3	vtx;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  if( current_region == NULL ){
    return;
  }
  if( flashing_region ){
    WlzFreeObj( flashing_region );
  }

  flashing_region_onFlg = 1;
  flashing_region_visibleFlg = 0;

  /* test the size in case it would be difficult to see */
  if( WlzArea(current_region, &errNum) < 30 ){
    double x, y, r;

    if( errNum == WLZ_ERR_NONE ){
      x = (current_region->domain.i->lastkl +
	   current_region->domain.i->kol1)/2.0;
      y = (current_region->domain.i->lastln +
	   current_region->domain.i->line1)/2.0;
      r = WLZ_MAX(
	current_region->domain.i->lastkl - current_region->domain.i->kol1,
	current_region->domain.i->lastln - current_region->domain.i->line1);
      r = WLZ_MAX(r+4.0, 20.0);

      if((obj1 = WlzMakeCircleObject(r, x, y, &errNum)) &&
	 (obj2 = WlzMakeCircleObject(r-2.0, x, y, NULL))){
	obj3 = WlzDiffDomain(obj1, obj2, &errNum);
	WlzFreeObj(obj1);
	WlzFreeObj(obj2);
	if( errNum == WLZ_ERR_NONE ){
	  obj1 = WlzUnion2(obj3, current_region, &errNum);
	  WlzFreeObj( obj3 );
	}
      }
    }
  }
  else {
    obj1 = WlzMakeMain(current_region->type, current_region->domain,
		       current_region->values, NULL, NULL, &errNum);
  }

  /* if review domain is 3D then transform to painted image coords */
  if( errNum == WLZ_ERR_NONE ){
    if( reviewDomain->type == WLZ_3D_DOMAINOBJ ){
      vtx.vtX = 0.0;
      vtx.vtY = 0.0;
      vtx.vtZ = (double) current_region_plane;
      Wlz3DSectionTransformVtx(&vtx, reviewViewStr->wlzViewStr);
      dx = (int) vtx.vtX;
      dy = (int) vtx.vtY;
    }
    else {
      dx = 0;
      dy = 0;
    }
    /* now to screen coords */
    dx -= reviewViewStr->painted_object->domain.i->kol1;
    dy -= reviewViewStr->painted_object->domain.i->line1;
    if((obj2 = WlzShiftObject(obj1, dx, dy, 0, &errNum))){

      if( reviewViewStr->wlzViewStr->scale > 0.95 ){
	obj3 = WlzIntRescaleObj(obj2, WLZ_NINT(reviewViewStr->wlzViewStr->scale),
				1, &errNum);
      }
      else {
	float invScale = 1.0 / reviewViewStr->wlzViewStr->scale;
	obj3 = WlzIntRescaleObj(obj2, WLZ_NINT(invScale), 0, &errNum);
      }
    }
  }

  /* create the flashing boundary */
  if( errNum == WLZ_ERR_NONE ){
  flashing_region = WlzAssignObject(obj3, NULL);
  WlzFreeObj(obj1);
  WlzFreeObj(obj2);
    
  flashing_region_timeout_proc(NULL, (XtIntervalId *) NULL);
  }
  else {
    MAPaintReportWlzError(globals.topl, "set_review_region_flashing", errNum);
  }
  return;
}

static void stop_review_region_flashing(void)
{
  flashing_region_onFlg = 0;
  if( reviewViewStr ){
    redisplay_view_cb(reviewViewStr->canvas,
		      (XtPointer) reviewViewStr, NULL);
  }
  flashing_region_visibleFlg = 0;

  return;
}

static void desensitizePlaneButton(
  int	plane)
{
  Widget	button;
  char		str_buf[32];

  /* get the button */
  sprintf(str_buf, "*review_planes_rc*%d", plane);
  if((button = XtNameToWidget(reviewDialog, str_buf))){
    XtSetSensitive(button, False);
  }

  return;
}

static void getNextReviewRegion(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  WlzObject		*obj1=NULL;
  WlzPlaneDomain	*planedmn;
  WlzValues		values;
  int			p, plane1;
  char			str_buf[32];
  Widget		toggle;
  Boolean		segDomainFlg=False;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* do nothing if review not in progress */
  if( !reviewInProgressFlg ){
    return;
  }

  /* clear the old */
  if( current_region ){
    errNum = WlzFreeObj( current_region );
    current_region = NULL;
  }

  /* if the review domain is NULL, stop review */
  if( reviewDomain == NULL ){
    stopReviewCb(w, client_data, call_data);
    return;
  }

  /* get the 2D object to be viewed */
  if( errNum == WLZ_ERR_NONE ){
    switch( reviewDomain->type ){
    case WLZ_2D_DOMAINOBJ:
      obj1 = WlzAssignObject(reviewDomain, &errNum);
      break;

    case WLZ_3D_DOMAINOBJ:
      /* check current plane else find next non-empty plane */
      if( (planedmn = reviewDomain->domain.p) == NULL ){
	errNum = WLZ_ERR_DOMAIN_NULL;
	break;
      }
      plane1 = planedmn->plane1;
      p = current_region_plane;

      /* find the next non-empty plane */
      while((p <= planedmn->lastpl) &&
	    ((planedmn->domains[p-plane1].core == NULL) ||
	     (planedmn->domains[p-plane1].core->type == WLZ_EMPTY_OBJ) ||
	     (planedmn->domains[p-plane1].core->type == WLZ_EMPTY_DOMAIN))
	){
	/* desensitize the plane button */
	desensitizePlaneButton(p);
	p++;
      }

      /* if at the end then stop - note passed over planes
	 can be gotten by restarting the review */
      if( p > planedmn->lastpl ){
	stopReviewCb(w, client_data, call_data);
	return;
      }

      /* set the current plane and reset the view */
      if( current_region_plane != p ){
	installViewDomains(reviewViewStr);
	current_region_plane = p;
	reviewViewStr->wlzViewStr->fixed.vtZ = current_region_plane;
	reviewViewStr->wlzViewStr->dist = 0.0;
	reset_view_struct( reviewViewStr );
	display_view_cb(w, (XtPointer) reviewViewStr, call_data);
	view_feedback_cb(w, (XtPointer) reviewViewStr, NULL);
	getViewDomains(reviewViewStr);
      }
      sprintf(str_buf, "Current review plane = %d", current_region_plane);
      XtVaSetValues(reviewViewStr->text, XmNvalue, str_buf, NULL);

      /* get the plane object */
      values.core = NULL;
      if((obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ, planedmn->domains[p-plane1],
			     values, NULL, NULL, &errNum))){
	obj1 = WlzAssignObject(obj1, &errNum);
      }
      break;

    case WLZ_EMPTY_OBJ:
    default:
      stopReviewCb(w, client_data, call_data);
      return;
    }
  }

  /* segment if required */
  if((toggle = XtNameToWidget(reviewDialog, "*segment_domain"))){
    XtVaGetValues(toggle, XmNset, &segDomainFlg, NULL);
  }
  if( (errNum == WLZ_ERR_NONE) && segDomainFlg ){
    WlzObject	**objs;
    int		nobjs, i;

    if( (errNum = WlzLabel(obj1, &nobjs, &objs, 256, 0, WLZ_4_CONNECTED))
       == WLZ_ERR_NONE ){
      if( nobjs ){
	WlzFreeObj(obj1);
	obj1 = objs[0];
	for(i=1; i < nobjs; i++){
	  WlzFreeObj( objs[i] );
	}
      }
      AlcFree( (void *) objs );
    }
  }
    
  /* assign the current review region */
  if( errNum == WLZ_ERR_NONE ){
    current_region = WlzAssignObject(obj1, NULL);
    WlzFreeObj(obj1);
  }
  else {
    current_region = NULL;
    MAPaintReportWlzError(globals.topl, "getNextReviewRegion", errNum);
  }
  return;
}

void readReviewObjCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  XtManageChild( readReviewObjDialog );
  XtPopup( XtParent(readReviewObjDialog), XtGrabNone );
  return;
}

void setCurrentRegionPlaneCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  char	str_buf[32];

  /* do nothing if review not in progress */
  if( !reviewInProgressFlg ){
    return;
  }

  /* clear the flashing region */
  stop_review_region_flashing();

  /* set the new plane */
  if( current_region_plane != (int) client_data ){
    installViewDomains(reviewViewStr);
    current_region_plane = (int) client_data;
    reviewViewStr->wlzViewStr->fixed.vtZ = current_region_plane;
    reviewViewStr->wlzViewStr->dist = 0.0;
    reset_view_struct( reviewViewStr );
    display_view_cb(w, (XtPointer) reviewViewStr, call_data);
    view_feedback_cb(w, (XtPointer) reviewViewStr, NULL);
    sprintf(str_buf, "Current review plane = %d", current_region_plane);
    XtVaSetValues(reviewViewStr->text, XmNvalue, str_buf, NULL);
    getViewDomains(reviewViewStr);
  }
  getNextReviewRegion(w, client_data, call_data);

  /* set it flashing */
  set_review_region_flashing();

  return;
}

static WlzErrorNum installReviewDomain(
  WlzObject	*obj)
{
  WlzValues	values;
  Widget	shell, rowcolumn, button;
  WidgetList	children;
  Cardinal	num_children;
  Dimension	shellHeight, rcHeight=0;
  WlzPlaneDomain	*planedmn;
  int		i, p;
  char		str_buf[16];
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* remove old domain */
  if( reviewDomain ){
    errNum = WlzFreeObj(reviewDomain);
    reviewDomain = NULL;
  }

  /* clear planes frame */
  if( (rowcolumn = XtNameToWidget(reviewDialog, "*review_planes_rc"))
     == NULL ){
    return WLZ_ERR_NONE;
  }

  shell = XtParent(reviewDialog);
  XtVaGetValues(shell, XmNheight, &shellHeight, NULL);
  XtVaGetValues(rowcolumn, XmNheight, &rcHeight, NULL);
  shellHeight -= rcHeight;

  XtUnmanageChild( rowcolumn );
  XtVaGetValues(rowcolumn,
		XmNchildren,	&children,
		XmNnumChildren,	&num_children,
		NULL);
  for(i=0; i < num_children; i++){
    XtDestroyWidget(children[i]);
  }

  /* check object */
  if( obj ){
    switch( obj->type ){
    case WLZ_2D_DOMAINOBJ:
      values.core = NULL;
      if((reviewDomain = WlzMakeMain(obj->type, obj->domain, values,
				     NULL, NULL, &errNum))){
	reviewDomain = WlzAssignObject(reviewDomain, &errNum);
      }
      break;

    case WLZ_3D_DOMAINOBJ:
      /* get intersection with the reference object */
      if( (reviewDomain = WlzIntersect2(globals.obj, obj, &errNum)) == NULL ){
	break;
      }
      if( reviewDomain->type == WLZ_EMPTY_OBJ ){
	break;
      }
      reviewDomain = WlzAssignObject(reviewDomain, &errNum);

      /* reset planes frame
	 create a button for each plane with a non-NULL domain */
      planedmn = reviewDomain->domain.p;
      for(p=planedmn->plane1; p <= planedmn->lastpl; p++){
	if((planedmn->domains[p-planedmn->plane1].core == NULL) ||
	   (planedmn->domains[p-planedmn->plane1].core->type ==
	    WLZ_EMPTY_OBJ) ||
	   (planedmn->domains[p-planedmn->plane1].core->type ==
	    WLZ_EMPTY_DOMAIN)
	  ){
	  continue;
	}
	sprintf(str_buf, "%d", p);
	button = XtVaCreateManagedWidget(str_buf, xmPushButtonWidgetClass,
					 rowcolumn, NULL);
	XtAddCallback(button, XmNactivateCallback,
		      setCurrentRegionPlaneCb, (XtPointer) p);
      }
      break;

    case WLZ_EMPTY_OBJ:
      break;

    case WLZ_TRANS_OBJ:
    case WLZ_2D_POLYGON:
    case WLZ_BOUNDLIST:
    case WLZ_COMPOUND_ARR_1:
    case WLZ_COMPOUND_ARR_2:
    default:
      break;
    }
  }

  XtVaGetValues(rowcolumn, XmNheight, &rcHeight, NULL);
  shellHeight += rcHeight;
  XtVaSetValues(shell, XmNheight, shellHeight, NULL);
  XtManageChild(rowcolumn);
  
  return errNum;
}

void readReviewObjectCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  WlzObject	*obj;
  FILE		*fp;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* get the file pointer */
  if( (fp = HGU_XmGetFilePointer(globals.topl, cbs->value,
				 cbs->dir, "r")) == NULL )
  {
    return;
  }

  /* read the new review object */
  obj = WlzReadObj(fp, &errNum);
  (void) fclose( fp );

  /* install it */
  if( obj ){
    obj = WlzAssignObject(obj, NULL);
    errNum = installReviewDomain(obj);
    WlzFreeObj(obj);
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "readReviewObjectCb", errNum);
  }
  return;
}

void domainReviewPopupCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	menu, selection=NULL;

  /* get the domain source selection and call its callbacks */
  if((menu = XtNameToWidget(reviewDialog, "*domainSource"))){
    XtVaGetValues(menu, XmNmenuHistory, &selection, NULL);
    if( selection ){
      XtCallCallbacks(selection, XmNactivateCallback, NULL);
    }
  }

  return;
}

void domainReviewCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  /* put up the domain review dialog */
  PopupCallback(w, (XtPointer) XtParent(reviewDialog), NULL);

  return;
}

static void sourceDomainCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  WlzObject	*srcDomainObj=NULL;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* set the source domain for future reference */
  sourceDomain = (DomainSelection) client_data;

  /* get selected domain */
  srcDomainObj = get_domain_from_object(globals.obj, sourceDomain);

  /* reset the planes frame */
  errNum = installReviewDomain(srcDomainObj);

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "sourceDomainCb", errNum);
  }
  return;
}

static void domainSourceCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  PaintDomainSourceType	domSrc = (PaintDomainSourceType) client_data;
  Widget	option, button;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  reviewSrc = domSrc;
  option = XtNameToWidget(reviewDialog, "*src_domain");
  button = XtNameToWidget(reviewDialog, "*domain_review_read");

  switch( domSrc ){
  case MAPAINT_DOMAIN_REVIEW_OBJ:
    if( option ){
      XtSetSensitive(option, False);
    }
    if( button ){
      XtSetSensitive(button, False);
    }
    errNum = installReviewDomain(globals.review_domain_obj);
    break;

  case MAPAINT_DOMAIN_MAPAINT_OBJ:
    if( option ){
      XtSetSensitive(option, True);
    }
    if( button ){
      XtSetSensitive(button, False);
    }
    sourceDomainCb(w, (XtPointer) sourceDomain, call_data);
    break;

  case MAPAINT_DOMAIN_READ:
    if( option ){
      XtSetSensitive(option, False);
    }
    if( button ){
      XtSetSensitive(button, True);
    }
    errNum = installReviewDomain(NULL);
    break;
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "domainSourceCb", errNum);
  }
  return;
}

void reviewDestDomainCb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  DomainSelection	destDomain=(DomainSelection) client_data;
  WlzObject		*obj1, *obj2;
  WlzValues		values;
  WlzPlaneDomain	*planedmn;
  int			p, plane1;
  Widget		widget;
  Boolean		overrideFlg;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* stop the flashing object */
  stop_review_region_flashing();

  /* assign current review region */
  /* if 3D then get the review region into painted image coords */
  if( (reviewDomain == NULL) || (reviewViewStr == NULL) ){
    return;
  }

  if( reviewDomain->type == WLZ_3D_DOMAINOBJ ){
    WlzDVertex3	vtx;
    int		dx, dy;

    vtx.vtX = 0.0;
    vtx.vtY = 0.0;
    vtx.vtZ = (double) current_region_plane;
    Wlz3DSectionTransformVtx(&vtx, reviewViewStr->wlzViewStr);
    dx = (int) vtx.vtX;
    dy = (int) vtx.vtY;
    obj1 = WlzShiftObject(current_region, dx, dy, 0, &errNum);
  }
  else {
    obj1 = WlzMakeMain(current_region->type, current_region->domain,
		       current_region->values, NULL, NULL, &errNum);
  }

  if( errNum == WLZ_ERR_NONE ){
    switch( destDomain ){
    case MASK_DOMAIN: /* do nothing */
      break;

    case GREYS_DOMAIN: /* return to greys */
      setGreysIncrement(obj1, reviewViewStr);
      break;

    default: /* the rest */
      overrideFlg = False;
      if((widget = XtNameToWidget(reviewDialog,
				  "*dominance_override_toggle"))){
	XtVaGetValues(widget, XmNset, &overrideFlg, NULL);
      }
      if( overrideFlg == False ){
	setDomainIncrement(obj1, reviewViewStr, destDomain, 0);
      }
      else {
	int domIdx = (int) destDomain;
	if( domIdx > globals.cmapstruct->num_overlays )
	{
	  set_grey_values_from_domain(obj1,
				      reviewViewStr->painted_object,
				      globals.cmapstruct->ovly_cols[domIdx],
				      255);
	}
	else
	{
	  setGreysIncrement(obj1, reviewViewStr);
	  set_grey_values_from_domain(obj1,
				      reviewViewStr->painted_object,
				      globals.cmapstruct->ovly_cols[domIdx],
				      globals.cmapstruct->ovly_planes);
	}
  
	/* redisplay this view */
	redisplay_view_cb(reviewViewStr->canvas, (XtPointer) reviewViewStr,
			  NULL);
      
      }
      break;
    }
  }

  /* remove it from the review domain */
  if( errNum == WLZ_ERR_NONE ){
    switch( reviewDomain->type ){
    case WLZ_2D_DOMAINOBJ:
      obj1 = WlzDiffDomain(reviewDomain, current_region, &errNum);
      WlzFreeObj(reviewDomain);
      reviewDomain = NULL;
      if( errNum == WLZ_ERR_NONE ){
	if( obj1 ){
	  if( WlzArea(obj1, &errNum) <= 0 ){
	    WlzFreeObj(obj1);
	  }
	  else {
	    reviewDomain = WlzAssignObject(obj1, &errNum);
	  }
	}
      }
      break;

    case WLZ_3D_DOMAINOBJ:
      planedmn = reviewDomain->domain.p;
      plane1 = planedmn->plane1;
      p = current_region_plane;
      values.core = NULL;
      if((obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ, planedmn->domains[p-plane1],
			     values, NULL, NULL, &errNum)) ){
	obj2 = WlzDiffDomain(obj1, current_region, &errNum);
	WlzFreeObj(obj1);
	WlzFreeDomain(planedmn->domains[p-plane1]);
	if( obj2 ){
	  if( WlzArea(obj2, &errNum) <= 0 ){
	    planedmn->domains[p-plane1].core = NULL;
	  }
	  else {
	    planedmn->domains[p-plane1] =
	      WlzAssignDomain(obj2->domain, &errNum);
	  }
	  WlzFreeObj(obj2);
	}
	else {
	  planedmn->domains[p-plane1].core = NULL;
	}
      }
      break;

    default:
      break;
    }
  }

  /* get the next region */
  if( errNum == WLZ_ERR_NONE ){
    getNextReviewRegion(w, client_data, call_data);

    /* set it flashing */
    set_review_region_flashing();
  }
  else {
    MAPaintReportWlzError(globals.topl, "reviewDestDomainCb", errNum);
  }
  return;
}

void reviewScaleCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  reviewViewScale = (int) client_data;

  /* if reviewing stop the flashing */
  stop_review_region_flashing();

  /* if it exists set the scale on the section view */
  if( viewDialog ){
    setViewScale(reviewViewStr, (double) reviewViewScale, 0, 0);
  }

  /* restart the flashing */
  set_review_region_flashing();

  return;
}

void stopReviewCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	button;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* stop the current region flashing */
  stop_review_region_flashing();

  /* mark review stopped */
  reviewInProgressFlg = 0;

  /* clear the current region */
  if( current_region ){
    errNum = WlzFreeObj(current_region);
    current_region = NULL;
  }

  if( reviewViewStr ){
    /* make the view sensitive */
    XtSetSensitive(reviewViewStr->canvas, True);
    XtSetSensitive(reviewViewStr->slider, True);
    XtSetSensitive(reviewViewStr->controls, True);

    /* install view domains */
    installViewDomains(reviewViewStr);

    /* popdown view and release the paint key */
    PopdownCallback(w, (XtPointer) XtParent(viewDialog), NULL);
    paint_key = NULL;

    /* remove the 3D display list */
    glDeleteLists(reviewViewStr->display_list, 1);
    reviewViewStr->display_list = 0;
    MAOpenGLDrawScene( globals.canvas );
  }

  /* set sensitivities - this button can't be pressed unless
     the review dialog has control - ergo this should be safe */
  button = XtNameToWidget(reviewDialog, "*Review");
  XtSetSensitive(button, True);
  button = XtNameToWidget(reviewDialog, "*Stop");
  XtSetSensitive(button, False);
  button = XtNameToWidget(reviewDialog, "*src_domain_frame");
  XtSetSensitive(button, True);

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "stopReviewCb", errNum);
  }
  return;
}

void startReviewCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  Widget		button;
  WlzDVertex3		fixed;
  WlzThreeDViewStruct	*wlzViewStr;

  /* do nothing if the review domain is NULL or
     if painting is in progress */
  if( !reviewDomain ){
    return;
  }

  if( paint_key ){
    HGU_XmUserMessage(reviewDialog,
		      "Start Review:\n"
		      "    There is a section view selected\n"
		      "    for painting. Please quit painting\n"
		      "    then restart the review.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* set sensitivities */
  button = XtNameToWidget(reviewDialog, "*Review");
  XtSetSensitive(button, False);
  button = XtNameToWidget(reviewDialog, "*Stop");
  XtSetSensitive(button, True);
  button = XtNameToWidget(reviewDialog, "*src_domain_frame");
  XtSetSensitive(button, False);

  /* create a section view and get the paint key */
  if( viewDialog == NULL ){
    fixed.vtX = (globals.obj->domain.p->kol1 +
		 globals.obj->domain.p->lastkl)/2;
    fixed.vtY = (globals.obj->domain.p->line1 +
		 globals.obj->domain.p->lastln)/2;
    fixed.vtZ = (globals.obj->domain.p->plane1 +
		 globals.obj->domain.p->lastpl)/2;
    viewDialog = create_view_window_dialog(globals.topl, 0.0, 0.0, &fixed);
/*    reviewViewStr = global_view_list->view_struct;*/
    XtVaGetValues(viewDialog, XmNuserData, &reviewViewStr, NULL);
    reviewViewStr->noPaintingFlag = 1;
    button = XtNameToWidget(viewDialog, "*dismiss");
    XtSetSensitive(button, False);
  }
  paint_key = reviewViewStr;
  XtManageChild( viewDialog );
  XtPopup(XtParent(viewDialog), XtGrabNone);

  /* if the review domain is 2D then request that the required
     section be set, then continue */
  if( reviewDomain->type == WLZ_2D_DOMAINOBJ ){
    HGU_XmUserContinue(viewDialog,
		       "The domain for review is 2D, please adjust\n"
		       "the Domain Review Section to the required\n"
		       "view then press Continue.", "Continue");
  }
  /* else if 3D reset the view and set the start plane */
  else {
    current_region_plane = reviewDomain->domain.p->plane1;
    reviewViewStr->controlFlag &= ~MAPAINT_FIXED_LINE_SET;
    wlzViewStr = reviewViewStr->wlzViewStr;
    wlzViewStr->initialised = 0;
    wlzViewStr->fixed.vtZ = current_region_plane;
    wlzViewStr->dist = 0.0;
    wlzViewStr->view_mode = WLZ_UP_IS_UP_MODE;
    wlzViewStr->theta = 0.0;
    wlzViewStr->phi = 0.0;
    wlzViewStr->up.vtX = 0.0;
    wlzViewStr->up.vtY = 0.0;
    wlzViewStr->up.vtZ = -1.0;
    /* process events to ensure viewer is visible and
       initialised */
    XSync(XtDisplay(viewDialog), False);
    while( XtAppPending(globals.app_con) ){
      XtAppProcessEvent(globals.app_con, XtIMAll);
      XSync(XtDisplay(viewDialog), False);
    }
    reset_view_struct( reviewViewStr );
    display_view_cb(w, (XtPointer) reviewViewStr, call_data);
    view_feedback_cb(w, (XtPointer) reviewViewStr, NULL);
    reviewScaleCb(w, (XtPointer) 1, NULL);
  }
  getViewDomains(reviewViewStr);

  /* make the view insensitive */
/*  XtSetSensitive(reviewViewStr->canvas, False);*/
  XtSetSensitive(reviewViewStr->slider, False);
  XtSetSensitive(reviewViewStr->controls, False);

  /* mark review in progress */
  reviewInProgressFlg = 1;

  /* get the next review region */
  getNextReviewRegion(w, client_data, call_data);

  /* start it flashing */
  set_review_region_flashing();

  return;
}

static MenuItem review_scale_items[] = {
{"1", &xmPushButtonGadgetClass, 0, NULL, NULL, True,
     reviewScaleCb, (XtPointer) 1,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"2", &xmPushButtonGadgetClass, 0, NULL, NULL, True,
     reviewScaleCb, (XtPointer) 2,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"4", &xmPushButtonGadgetClass, 0, NULL, NULL, True,
     reviewScaleCb, (XtPointer) 4,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"8", &xmPushButtonGadgetClass, 0, NULL, NULL, True,
     reviewScaleCb, (XtPointer) 8,
     myHGU_XmHelpStandardCb, "",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{NULL},
};

static MenuItem domainSourceItems[] = {
  {"review_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   domainSourceCb, (XtPointer) MAPAINT_DOMAIN_REVIEW_OBJ,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"mapaint_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   domainSourceCb, (XtPointer) MAPAINT_DOMAIN_MAPAINT_OBJ,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"read_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   domainSourceCb, (XtPointer) MAPAINT_DOMAIN_READ,
   myHGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {NULL},
};


static ActionAreaItem   review_dialog_actions[] = {
{"Review",	NULL,		NULL},
{"Stop",	NULL,		NULL},
{"Dismiss",     NULL,           NULL},
{"Help",        NULL,           NULL},
};

Widget createDomainReviewDialog(
  Widget	topl)
{
  Widget	dialog, control, form, frame, title, title_form;
  Widget	rowcolumn, button, option, toggle, widget;
  int		i;
  Visual	*visual;
  Arg		arg[1];

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(topl);
  XtSetArg(arg[0], XmNvisual, visual);

  dialog = HGU_XmCreateStdDialog(topl, "review_dialog",
				 xmFormWidgetClass,
				 review_dialog_actions, 4);
  XtAddCallback(XtParent(dialog), XmNpopupCallback,
		domainReviewPopupCb, NULL);

  if( (widget = XtNameToWidget(dialog, "*.Review")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, startReviewCb, NULL);
  }

  if( (widget = XtNameToWidget(dialog, "*.Stop")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, stopReviewCb, NULL);
    XtSetSensitive(widget, False);
  }

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, stopReviewCb, NULL);
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  control = XtNameToWidget( dialog, "*.control" );

  /* create the buttons for the source domain */
  frame = XtVaCreateManagedWidget("src_domain_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  title = XtVaCreateManagedWidget("src_domain_title", xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  form = XtVaCreateManagedWidget("src_domain_form", xmFormWidgetClass,
				 frame, NULL);

  /* create the domain source option menu */
  option = HGU_XmBuildMenu(form, XmMENU_OPTION, "domainSource", 0,
			   XmTEAR_OFF_DISABLED, domainSourceItems);
  XtManageChild(option);
  XtVaSetValues(option,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		NULL);
  widget = option;

  option = createDomainSelectOptionMenu("src_domain", form, sourceDomainCb);
  XtManageChild(option);
  XtVaSetValues(option,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, widget,
		NULL);
  widget = option;
  XtSetSensitive(option, False);

  button = XtVaCreateManagedWidget("domain_review_read",
				   xmPushButtonWidgetClass,
				   form,	
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNbottomAttachment, XmATTACH_FORM,
				   XmNleftAttachment, XmATTACH_WIDGET,
				   XmNleftWidget, widget,
				   XmNsensitive, False,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, readReviewObjCb, NULL);

  readReviewObjDialog = XmCreateFileSelectionDialog(globals.topl,
						    "read_review_obj_dialog",
						    arg, 1);
  XtAddCallback(readReviewObjDialog, XmNokCallback,
		readReviewObjectCb, (XtPointer) NULL);
  XtAddCallback(readReviewObjDialog, XmNokCallback,
		PopdownCallback, (XtPointer) NULL);
  XtAddCallback(readReviewObjDialog, XmNcancelCallback,
		PopdownCallback, NULL);
  XtAddCallback(readReviewObjDialog, XmNmapCallback, FSBPopupCallback, NULL);


  /* create buttons to select the planes */
  frame = XtVaCreateManagedWidget("review_planes_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget,	frame,
				  XmNleftAttachment, XmATTACH_FORM,
				  NULL);
  title = XtVaCreateManagedWidget("review_planes_title", xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  rowcolumn = XtVaCreateManagedWidget("review_planes_rc",
				      xmRowColumnWidgetClass,
				      frame,
				      XmNorientation, XmVERTICAL,
				      XmNnumColumns,	20,
				      XmNpacking,	XmPACK_COLUMN,
				      NULL);

  /* create scan control toggles */
  frame = XtVaCreateManagedWidget("review_scan_control_frame",
				  xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget,	frame,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  title = XtVaCreateManagedWidget("review_scan_control_title",
				  xmLabelWidgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  
  rowcolumn = XtVaCreateManagedWidget("review_scan_rc",
				      xmRowColumnWidgetClass,
				      frame,
				      XmNorientation, XmHORIZONTAL,
				      NULL);

  toggle = XtVaCreateManagedWidget("segment_domain",
				   xmToggleButtonGadgetClass,
				   rowcolumn, NULL);

  toggle = XtVaCreateManagedWidget("segment_planes",
				   xmToggleButtonGadgetClass,
				   rowcolumn, NULL);

  widget = HGU_XmBuildMenu(rowcolumn, XmMENU_OPTION, "review_scale", 0,
			   XmTEAR_OFF_DISABLED, review_scale_items);
  XtManageChild(widget);

  /* create the select buttons for the destination domain */
  frame = XtVaCreateManagedWidget("dest_domain_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget,	frame,
				  XmNleftAttachment, XmATTACH_FORM,
				  NULL);

  title_form = XtVaCreateManagedWidget("dest_domain_title_form",
				       xmFormWidgetClass, frame,
				       XmNchildType, XmFRAME_TITLE_CHILD,
				       NULL);
  title = XtVaCreateManagedWidget("dest_domain_title", xmLabelWidgetClass,
				  title_form,
				  XmNleftAttachment,	XmATTACH_FORM,
				  NULL);
  widget = XtVaCreateManagedWidget("dominance_override_toggle",
				   xmToggleButtonGadgetClass, title_form,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	title,
				   NULL);

  rowcolumn = XtVaCreateManagedWidget("dest_domain_rc",
				      xmRowColumnWidgetClass,
				      frame,
				      XmNorientation, XmVERTICAL,
				      XmNnumColumns,	5,
				      XmNpacking,	XmPACK_COLUMN,
				      XmNresizeWidth,	True,
				      XmNresizeHeight,	True,
				      NULL);
  widget = XtVaCreateManagedWidget("Discard", xmPushButtonWidgetClass,
				   rowcolumn, NULL);
  XtAddCallback(widget, XmNactivateCallback, reviewDestDomainCb,
		(XtPointer) MASK_DOMAIN);

  widget = XtVaCreateManagedWidget("Greys", xmPushButtonWidgetClass,
				   rowcolumn, NULL);
  XtAddCallback(widget, XmNactivateCallback, reviewDestDomainCb,
		(XtPointer) GREYS_DOMAIN);

  for(i=1; i < 33; i++){
    Pixel	pixel;
    char	domainStrBuf[16];

    sprintf(domainStrBuf, "domain %d", i);
    widget = XtVaCreateManagedWidget(domainStrBuf,
				     xmPushButtonWidgetClass,
				     rowcolumn, NULL);

    XtAddCallback(widget, XmNactivateCallback, reviewDestDomainCb,
		  (XtPointer) i);
    pixel = globals.cmapstruct->ovly_cols[i] +
      globals.cmapstruct->gmax - globals.cmapstruct->gmin;
    if( i > globals.cmapstruct->num_overlays )
    {
      pixel = globals.cmapstruct->ovly_cols[i];
    }
    pixel = HGU_XGetColorPixel(XtDisplay(topl), globals.cmap,
			       (float) globals.colormap[0][pixel]/255.0,
			       (float) globals.colormap[1][pixel]/255.0,
			       (float) globals.colormap[2][pixel]/255.0);
    XmChangeColor(widget, pixel);

    if( i > (globals.cmapstruct->num_overlays +
	     globals.cmapstruct->num_solid_overlays) )
    {
      XtUnmanageChild(widget);
    }

  }

  return( dialog );
}
