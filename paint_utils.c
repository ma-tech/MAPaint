#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	paint_utils.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <MAPaint.h>
#include <X11/cursorfont.h>

void popup_dialog_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
    XtManageChild( (Widget) client_data );
    XtPopup( XtParent((Widget) client_data), XtGrabNone );
    return;
}

set_backingX(
  Display	*dpy,
  Window	win)
{
  unsigned long		valuemask;
  XSetWindowAttributes	setwinattr;

  setwinattr.backing_store = Always;
  valuemask = CWBackingStore;
  XChangeWindowAttributes( dpy, win, valuemask, &setwinattr);

  return( 0 );
}

void UnmanageChildCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  if( client_data != NULL ){
    XtUnmanageChild( (Widget) client_data );
  }
  else {
    XtUnmanageChild( w );
  }

  return;
}

void destroy_cb(
Widget          w,
XtPointer       client_data,
XtPointer       call_data)
{
    XtDestroyWidget( (Widget) client_data );
    return;
}
 
int InteractDispatchEvent(
Display			*dpy,
Window			win,
XtPointer		client_data,
XEvent			*event)
{
  XtAppContext	app_con;

  XtDispatchEvent( event );

  return( globals.paint_action_quit_flag );
}

int InteractHelpCb(
Display			*dpy,
Window			win,
XtPointer		client_data,
String			help_str)
{
    HGU_XmHelpShowString( globals.topl, (String) client_data, help_str );
    return( 0 );
}

set_grey_values_from_ref_object(
  WlzObject	*obj,
  WlzObject	*ref_obj)
{
  WlzObject		*objs[3], *new_obj;
  WlzPlaneDomain	*pdom, *pdom1, *pdom2;
  WlzVoxelValues	*voxvals1, *voxvals2;
  WlzIntervalWSpace	iwsp1, iwsp2;
  WlzGreyWSpace	gwsp1, gwsp2;
  int			x, y, z, l, k, p;
  WlzPixelV		min, max, Min, Max;

  /* check the object */
  if((obj == NULL) || (obj->type != ref_obj->type) ||
     ((obj->type != WLZ_2D_DOMAINOBJ) && (obj->type != WLZ_3D_DOMAINOBJ))){
    return( -1 );
  }

  /* set the required grey ranges */
  min.type = WLZ_GREY_UBYTE;
  max.type = WLZ_GREY_UBYTE;
  Min.type = WLZ_GREY_UBYTE;
  Max.type = WLZ_GREY_UBYTE;
  min.v.ubv = 0;
  max.v.ubv = 255;
  Min.v.ubv = globals.cmapstruct->gmin;
  Max.v.ubv = globals.cmapstruct->gmax;

  /* scan through the new domain */
  objs[0] = WlzAssignObject(obj, NULL);
  objs[1] = WlzAssignObject(globals.obj, NULL);
  objs[2] = WlzAssignObject(ref_obj, NULL);
  new_obj = WlzIntersectN(3, objs, 0, NULL);
  WlzFreeObj(objs[0]);
  WlzFreeObj(objs[1]);
  WlzFreeObj(objs[2]);
  switch( obj->type ){
  case WLZ_2D_DOMAINOBJ:
    objs[0] = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			  new_obj->domain,
			  globals.obj->values,
			  NULL, NULL, NULL);

    objs[1] = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			  new_obj->domain,
			  ref_obj->values,
			  NULL, NULL, NULL);
    WlzInitGreyScan( objs[0], &iwsp1, &gwsp1 );
    WlzInitGreyScan( objs[1], &iwsp2, &gwsp2 );
    while( WlzNextGreyInterval( &iwsp1 ) == WLZ_ERR_NONE &&
	  WlzNextGreyInterval( &iwsp2 ) == WLZ_ERR_NONE )
    {
      memcpy((void *) gwsp1.u_grintptr.ubp,
	     (const void *) gwsp2.u_grintptr.ubp,
	     (size_t) (iwsp1.rgtpos - iwsp1.lftpos + 1));
    }
    WlzGreySetRange(objs[0], min, max, Min, Max);
    WlzFreeObj( objs[0] );
    WlzFreeObj( objs[1] );
    break;

  case WLZ_3D_DOMAINOBJ:
    pdom = new_obj->domain.p;
    pdom1 = globals.obj->domain.p;
    pdom2 = ref_obj->domain.p;
    voxvals1 = globals.obj->values.vox;
    voxvals2 = ref_obj->values.vox;

    for(p=pdom->plane1; p <= pdom->lastpl; p++){

      if( pdom->domains[p - pdom->plane1].core == NULL )
	continue;

      objs[0] = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			    pdom->domains[p - pdom->plane1],
			    voxvals1->values[p - pdom1->plane1],
			    NULL, NULL, NULL);

      objs[1] = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			    pdom->domains[p - pdom->plane1],
			    voxvals2->values[p - pdom2->plane1],
			    NULL, NULL, NULL);

      WlzInitGreyScan( objs[0], &iwsp1, &gwsp1 );
      WlzInitGreyScan( objs[1], &iwsp2, &gwsp2 );
      while( WlzNextGreyInterval( &iwsp1 ) == WLZ_ERR_NONE &&
	    WlzNextGreyInterval( &iwsp2 ) == WLZ_ERR_NONE )
      {
	memcpy((void *) gwsp1.u_grintptr.ubp,
	       (const void *) gwsp2.u_grintptr.ubp,
	       (size_t) (iwsp1.rgtpos - iwsp1.lftpos + 1));
      }
      WlzGreySetRange(objs[0], min, max, Min, Max);
      WlzFreeObj( objs[0] );
      WlzFreeObj( objs[1] );
    }
    WlzFreeObj( new_obj );
    break;
  }

  return( 0 );
}

static int set_grey_values_from_domain3D(
  WlzObject		*obj,
  WlzObject		*destObj,
  unsigned int		col,
  unsigned int		planes)
{
  WlzObject		*obj1, *new_obj;
  WlzPlaneDomain	*pdom, *pdom1;
  WlzVoxelValues	*voxvaltb;
  WlzIntervalWSpace	iwsp;
  WlzGreyWSpace		gwsp;
  int			x, y, z, l, k, p;

  /* check the object */
  if( obj == NULL || obj->type != WLZ_3D_DOMAINOBJ )
    return( -1 );

  /* scan through the new domain */
  if( (new_obj = WlzIntersect2(obj, destObj, NULL)) == NULL ){
    HGU_XmUserMessage(globals.topl,
		      "The domain object just read has no\n"
		      "intersection with the reference object",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    return( 0 );
  }
  pdom = new_obj->domain.p;
  pdom1 = destObj->domain.p;
  voxvaltb = destObj->values.vox;

  for(p=pdom->plane1; p <= pdom->lastpl; p++){

    if( pdom->domains[p - pdom->plane1].core == NULL )
      continue;

    obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
		       pdom->domains[p - pdom->plane1],
		       voxvaltb->values[p - pdom1->plane1],
		       NULL, NULL, NULL);

    WlzInitGreyScan( obj1, &iwsp, &gwsp );
    while( WlzNextGreyInterval( &iwsp ) == WLZ_ERR_NONE )
    {
      for( k=0; k <= (iwsp.rgtpos - iwsp.lftpos); k++ )
      {
	gwsp.u_grintptr.ubp[k] &= ~planes;
	gwsp.u_grintptr.ubp[k] |= col;
      }
    }
    WlzFreeObj( obj1 );
  }
  WlzFreeObj( new_obj );

  return( 0 );
}

int set_grey_values_from_domain(
  WlzObject		*obj,
  WlzObject		*destObj,
  unsigned int		col,
  unsigned int		planes)
{
  WlzObject		*obj1, *new_obj;
  WlzPlaneDomain	*pdom, *pdom1;
  WlzVoxelValues	*voxvaltb;
  WlzIntervalWSpace	iwsp;
  WlzGreyWSpace		gwsp;
  int			x, y, z, l, k, p;

  /* check the object */
  if( obj == NULL ){
    return( -1 );
  }

  /* check the object type */
  if( obj->type != destObj->type ){
    return( -1 );
  }

  if( obj->type == WLZ_3D_DOMAINOBJ ){
    return set_grey_values_from_domain3D(obj, destObj, col, planes);
  }

  /* scan through the new domain */
  if( (new_obj = WlzIntersect2(obj, destObj, NULL)) == NULL ){
    return( 0 );
  }
  if( WlzArea(new_obj, NULL) <= 0 ){
    WlzFreeObj(new_obj);
    return 0;
  }

  new_obj->values = WlzAssignValues(destObj->values, NULL);
  WlzInitGreyScan( new_obj, &iwsp, &gwsp );
  while( WlzNextGreyInterval( &iwsp ) == WLZ_ERR_NONE )
  {
    for( k=0; k <= (iwsp.rgtpos - iwsp.lftpos); k++ )
    {
      gwsp.u_grintptr.ubp[k] &= ~planes;
      gwsp.u_grintptr.ubp[k] |= col;
    }
  }

  return 0;
}

void HGU_XmSetHourGlassCursor(
  Widget	w)
{
  static Cursor	cursor;

  /* create the cursor */
  if( !cursor ){
    cursor = XCreateFontCursor( XtDisplayOfObject(w), XC_watch );
  }

  /* if a shell widget, set the cursor */
  if( XtIsShell( w ) ){
    XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
  }

  /* get the children, call function on each */
  if( XtIsComposite(w) ){
    Cardinal	numChildren;
    WidgetList	children;

    XtVaGetValues(w,
		  XmNchildren, &children,
		  XmNnumChildren, &numChildren,
		  NULL);
    while( numChildren ){
      numChildren--;
      HGU_XmSetHourGlassCursor(children[numChildren]);
    }
  }
  XFlush(XtDisplayOfObject(w));

  return;
}

void HGU_XmUnsetHourGlassCursor(
  Widget	w)
{
  XEvent	event;

  /* if a shell widget, unset the cursor */
  if( XtIsShell( w ) ){
    XUndefineCursor(XtDisplay(w), XtWindow(w));
  }

  /* get the children, call function on each */
  if( XtIsComposite(w) ){
    Cardinal	numChildren=0;
    WidgetList	children=NULL;

    XtVaGetValues(w,
		  XmNchildren, &children,
		  XmNnumChildren, &numChildren,
		  NULL);
    while( numChildren ){
      numChildren--;
      HGU_XmUnsetHourGlassCursor(children[numChildren]);
    }
  }
  XFlush(XtDisplayOfObject(w));

  /* clear pending events */
  while( XCheckMaskEvent(XtDisplayOfObject(w),
			 ButtonPressMask|ButtonReleaseMask|
			 ButtonMotionMask|PointerMotionMask|
			 KeyPressMask, &event) )
  {
    /* do nothing */
  }

  return;
}
