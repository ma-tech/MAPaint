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
#include <X11/Xproto.h>

void myHGU_XmHelpStandardCb(Widget	w,
			  XtPointer	client_data,
			  XtPointer	call_data)
{
  /* dummy routine to switch off help */
  HGU_XmUserMessage(w,
		    "Button and context help has been\n"
		    "disabled to switch to a more usable\n"
		    "system based on JavaHelp.\n\n"
		    "For now please see help pages on the\n"
		    "Mouse Atlas web site:\n"
		    "http://genex.hgu.mrc.ac.uk/Resources/"
		    "or email ma-tech@hgu.mrc.ac.uk.\n"
		    "Thank you",
		    XmDIALOG_FULL_APPLICATION_MODAL);

  return;
}



void popup_dialog_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
    XtManageChild( (Widget) client_data );
    XtPopup( XtParent((Widget) client_data), XtGrabNone );

    /* kludge to update the file selections */
    if( XmIsFileSelectionBox((Widget) client_data) ){
      XtCallCallbacks((Widget) client_data, XmNmapCallback, call_data);
    }
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
  WlzErrorNum		errNum=WLZ_ERR_NONE;

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
  new_obj = WlzIntersectN(3, objs, 0, &errNum);
  WlzFreeObj(objs[0]);
  WlzFreeObj(objs[1]);
  WlzFreeObj(objs[2]);

  if( errNum == WLZ_ERR_NONE ){
    switch( obj->type ){
    case WLZ_2D_DOMAINOBJ:
      if(objs[0] = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			       new_obj->domain,
			       globals.obj->values,
			       NULL, NULL, &errNum)){

	if(objs[1] = WlzMakeMain(WLZ_2D_DOMAINOBJ,
				 new_obj->domain,
				 ref_obj->values,
				 NULL, NULL, &errNum)){
	  errNum = WlzInitGreyScan( objs[0], &iwsp1, &gwsp1 );
	  errNum |= WlzInitGreyScan( objs[1], &iwsp2, &gwsp2 );
	  while((errNum == WLZ_ERR_NONE) &&
		((errNum = WlzNextGreyInterval(&iwsp1)) == WLZ_ERR_NONE) &&
		(WlzNextGreyInterval(&iwsp2) == WLZ_ERR_NONE) )
	  {
	    memcpy((void *) gwsp1.u_grintptr.ubp,
		   (const void *) gwsp2.u_grintptr.ubp,
		   (size_t) (iwsp1.rgtpos - iwsp1.lftpos + 1));
	  }
	  if( errNum == WLZ_ERR_EOO ){
	    errNum = WLZ_ERR_NONE;
	  }
	  WlzGreySetRange(objs[0], min, max, Min, Max);
	  WlzFreeObj( objs[1] );
	}
	WlzFreeObj( objs[0] );
      }
      break;

    case WLZ_3D_DOMAINOBJ:
      pdom = new_obj->domain.p;
      pdom1 = globals.obj->domain.p;
      pdom2 = ref_obj->domain.p;
      voxvals1 = globals.obj->values.vox;
      voxvals2 = ref_obj->values.vox;

      for(p=pdom->plane1; p <= pdom->lastpl; p++){

	if( pdom->domains[p - pdom->plane1].core == NULL ){
	  continue;
	}

	if( objs[0] = WlzMakeMain(WLZ_2D_DOMAINOBJ,
				  pdom->domains[p - pdom->plane1],
				  voxvals1->values[p - pdom1->plane1],
				  NULL, NULL, &errNum) ){

	  if( objs[1] = WlzMakeMain(WLZ_2D_DOMAINOBJ,
				    pdom->domains[p - pdom->plane1],
				    voxvals2->values[p - pdom2->plane1],
				    NULL, NULL, &errNum) ){

	    errNum = WlzInitGreyScan( objs[0], &iwsp1, &gwsp1 );
	    errNum |= WlzInitGreyScan( objs[1], &iwsp2, &gwsp2 );
	    while((errNum == WLZ_ERR_NONE) &&
		  ((errNum = WlzNextGreyInterval(&iwsp1)) == WLZ_ERR_NONE) &&
		  (WlzNextGreyInterval(&iwsp2) == WLZ_ERR_NONE) )
	    {
	      memcpy((void *) gwsp1.u_grintptr.ubp,
		     (const void *) gwsp2.u_grintptr.ubp,
		     (size_t) (iwsp1.rgtpos - iwsp1.lftpos + 1));
	    }
	    if( errNum == WLZ_ERR_EOO ){
	      errNum = WLZ_ERR_NONE;
	    }
	    WlzGreySetRange(objs[0], min, max, Min, Max);
	    WlzFreeObj( objs[1] );
	  }
	  WlzFreeObj( objs[0] );
	}
      }
      break;
    }
  }

  if( new_obj ){
    WlzFreeObj( new_obj );
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "set_grey_values_from_ref_object", errNum);
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
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check the object */
  if( obj == NULL || obj->type != WLZ_3D_DOMAINOBJ )
    return( -1 );

  /* scan through the new domain */
  if(((new_obj = WlzIntersect2(obj, destObj, &errNum)) == NULL) ||
     (new_obj->type == WLZ_EMPTY_OBJ) ){
    HGU_XmUserMessage(globals.topl,
		      "The domain object just read has no\n"
		      "intersection with the reference object",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    if( new_obj ){
      WlzFreeObj( new_obj );
    }
    if( errNum != WLZ_ERR_NONE ){
      MAPaintReportWlzError(globals.topl, "set_grey_values_from_domain3D",
			    errNum);
    }
    return( 0 );
  }
  pdom = new_obj->domain.p;
  pdom1 = destObj->domain.p;
  voxvaltb = destObj->values.vox;

  for(p=pdom->plane1; p <= pdom->lastpl; p++){

    if( pdom->domains[p - pdom->plane1].core == NULL )
      continue;

    if( obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			   pdom->domains[p - pdom->plane1],
			   voxvaltb->values[p - pdom1->plane1],
			   NULL, NULL, &errNum) ){

      errNum = WlzInitGreyScan( obj1, &iwsp, &gwsp );
      while((errNum == WLZ_ERR_NONE) &&
	    ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE) ){
	for( k=0; k <= (iwsp.rgtpos - iwsp.lftpos); k++ ){
	  gwsp.u_grintptr.ubp[k] &= ~planes;
	  gwsp.u_grintptr.ubp[k] |= col;
	}
      }
      if( errNum == WLZ_ERR_EOO ){
	errNum = WLZ_ERR_NONE;
      }
      WlzFreeObj( obj1 );
    }
  }
  WlzFreeObj( new_obj );

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "set_grey_values_from_domain3D", errNum);
  }
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
  WlzErrorNum		errNum=WLZ_ERR_NONE;

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
  if( new_obj = WlzIntersect2(obj, destObj, &errNum) ){

    new_obj = WlzAssignObject(new_obj, NULL);
    if( WlzArea(new_obj, &errNum) > 0 ){
      new_obj->values = WlzAssignValues(destObj->values, NULL);
      errNum = WlzInitGreyScan( new_obj, &iwsp, &gwsp );
      while((errNum == WLZ_ERR_NONE) &&
	    ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE) ){
	for( k=0; k <= (iwsp.rgtpos - iwsp.lftpos); k++ ){
	  gwsp.u_grintptr.ubp[k] &= ~planes;
	  gwsp.u_grintptr.ubp[k] |= col;
	}
      }
      if( errNum == WLZ_ERR_EOO ){
	errNum = WLZ_ERR_NONE;
      }
    }
    WlzFreeObj(new_obj);
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "set_grey_values_from_domain", errNum);
    return 1;
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

void MAPaintReportWlzError(
  Widget	w,
  String	srcStr,
  WlzErrorNum	wlzErr)
{
  char		msgBuf[256];
  const char	*errStr, *msgStr;

  /* get the error and message strings */
  errStr = WlzStringFromErrorNum(wlzErr, &msgStr);

  /* create the message string to display */
  sprintf(msgBuf,
	  "A Woolz error has occurred in %s:\n\n"
	  "\t%s: %s\n\n"
	  "Safest action is to try and save domains\n"
	  "and restart paint. If that fails you can\n"
	  "recover data from the autosave file.\n\n"
	  "Please report this error to:\n"
	  "ma-tech@hgu.mrc.ac.uk",
	  srcStr, errStr, msgStr);
  HGU_XmUserError(w, msgBuf, XmDIALOG_FULL_APPLICATION_MODAL);
  
  return;
}


char *HGU_XRequestToString(
  char	request_code)
{
  char	*str;

  switch( request_code ){
  case X_CreateWindow:
    str = "X_CreateWindow";
    break;

  case X_ChangeWindowAttributes:
    str = "X_ChangeWindowAttributes";
    break;

  case X_GetWindowAttributes:
    str = "X_GetWindowAttributes";
    break;

  case X_DestroyWindow:
    str = "X_DestroyWindow";
    break;

  case X_DestroySubwindows:
    str = "X_DestroySubwindows";
    break;

  case X_ChangeSaveSet:
    str = "X_ChangeSaveSet";
    break;

  case X_ReparentWindow:
    str = "X_ReparentWindow";
    break;

  case X_MapWindow:
    str = "X_MapWindow";
    break;

  case X_MapSubwindows:
    str = "X_MapSubwindows";
    break;

  case X_UnmapWindow:
    str = "X_UnmapWindow";
    break;

  case X_UnmapSubwindows:
    str = "X_UnmapSubwindows";
    break;

  case X_ConfigureWindow:
    str = "X_ConfigureWindow";
    break;

  case X_CirculateWindow:
    str = "X_CirculateWindow";
    break;

  case X_GetGeometry:
    str = "X_GetGeometry";
    break;

  case X_QueryTree:
    str = "X_QueryTree";
    break;

  case X_InternAtom:
    str = "X_InternAtom";
    break;

  case X_GetAtomName:
    str = "X_GetAtomName";
    break;

  case X_ChangeProperty:
    str = "X_ChangeProperty";
    break;

  case X_DeleteProperty:
    str = "X_DeleteProperty";
    break;

  case X_GetProperty:
    str = "X_GetProperty";
    break;

  case X_ListProperties:
    str = "X_ListProperties";
    break;

  case X_SetSelectionOwner:
    str = "X_SetSelectionOwner";
    break;

  case X_GetSelectionOwner:
    str = "X_GetSelectionOwner";
    break;

  case X_ConvertSelection:
    str = "X_ConvertSelection";
    break;

  case X_SendEvent:
    str = "X_SendEvent";
    break;

  case X_GrabPointer:
    str = "X_GrabPointer";
    break;

  case X_UngrabPointer:
    str = "X_UngrabPointer";
    break;

  case X_GrabButton:
    str = "X_GrabButton";
    break;

  case X_UngrabButton:
    str = "X_UngrabButton";
    break;

  case X_ChangeActivePointerGrab:
    str = "X_ChangeActivePointerGrab";
    break;

  case X_GrabKeyboard:
    str = "X_GrabKeyboard";
    break;

  case X_UngrabKeyboard:
    str = "X_UngrabKeyboard";
    break;

  case X_GrabKey:
    str = "X_GrabKey";
    break;

  case X_UngrabKey:
    str = "X_UngrabKey";
    break;

  case X_AllowEvents:
    str = "X_AllowEvents";
    break;

  case X_GrabServer:
    str = "X_GrabServer";
    break;

  case X_UngrabServer:
    str = "X_UngrabServer";
    break;

  case X_QueryPointer:
    str = "X_QueryPointer";
    break;

  case X_GetMotionEvents:
    str = "X_GetMotionEvents";
    break;

  case X_TranslateCoords:
    str = "X_TranslateCoords";
    break;

  case X_WarpPointer:
    str = "X_WarpPointer";
    break;

  case X_SetInputFocus:
    str = "X_SetInputFocus";
    break;

  case X_GetInputFocus:
    str = "X_GetInputFocus";
    break;

  case X_QueryKeymap:
    str = "X_QueryKeymap";
    break;

  case X_OpenFont:
    str = "X_OpenFont";
    break;

  case X_CloseFont:
    str = "X_CloseFont";
    break;

  case X_QueryFont:
    str = "X_QueryFont";
    break;

  case X_QueryTextExtents:
    str = "X_QueryTextExtents";
    break;

  case X_ListFonts:
    str = "X_ListFonts";
    break;

  case X_ListFontsWithInfo:
    str = "X_ListFontsWithInfo";
    break;

  case X_SetFontPath:
    str = "X_SetFontPath";
    break;

  case X_GetFontPath:
    str = "X_GetFontPath";
    break;

  case X_CreatePixmap:
    str = "X_CreatePixmap";
    break;

  case X_FreePixmap:
    str = "X_FreePixmap";
    break;

  case X_CreateGC:
    str = "X_CreateGC";
    break;

  case X_ChangeGC:
    str = "X_ChangeGC";
    break;

  case X_CopyGC:
    str = "X_CopyGC";
    break;

  case X_SetDashes:
    str = "X_SetDashes";
    break;

  case X_SetClipRectangles:
    str = "X_SetClipRectangles";
    break;

  case X_FreeGC:
    str = "X_FreeGC";
    break;

  case X_ClearArea:
    str = "X_ClearArea";
    break;

  case X_CopyArea:
    str = "X_CopyArea";
    break;

  case X_CopyPlane:
    str = "X_CopyPlane";
    break;

  case X_PolyPoint:
    str = "X_PolyPoint";
    break;

  case X_PolyLine:
    str = "X_PolyLine";
    break;

  case X_PolySegment:
    str = "X_PolySegment";
    break;

  case X_PolyRectangle:
    str = "X_PolyRectangle";
    break;

  case X_PolyArc:
    str = "X_PolyArc";
    break;

  case X_FillPoly:
    str = "X_FillPoly";
    break;

  case X_PolyFillRectangle:
    str = "X_PolyFillRectangle";
    break;

  case X_PolyFillArc:
    str = "X_PolyFillArc";
    break;

  case X_PutImage:
    str = "X_PutImage";
    break;

  case X_GetImage:
    str = "X_GetImage";
    break;

  case X_PolyText8:
    str = "X_PolyText8";
    break;

  case X_PolyText16:
    str = "X_PolyText16";
    break;

  case X_ImageText8:
    str = "X_ImageText8";
    break;

  case X_ImageText16:
    str = "X_ImageText16";
    break;

  case X_CreateColormap:
    str = "X_CreateColormap";
    break;

  case X_FreeColormap:
    str = "X_FreeColormap";
    break;

  case X_CopyColormapAndFree:
    str = "X_CopyColormapAndFree";
    break;

  case X_InstallColormap:
    str = "X_InstallColormap";
    break;

  case X_UninstallColormap:
    str = "X_UninstallColormap";
    break;

  case X_ListInstalledColormaps:
    str = "X_ListInstalledColormaps";
    break;

  case X_AllocColor:
    str = "X_AllocColor";
    break;

  case X_AllocNamedColor:
    str = "X_AllocNamedColor";
    break;

  case X_AllocColorCells:
    str = "X_AllocColorCells";
    break;

  case X_AllocColorPlanes:
    str = "X_AllocColorPlanes";
    break;

  case X_FreeColors:
    str = "X_FreeColors";
    break;

  case X_StoreColors:
    str = "X_StoreColors";
    break;

  case X_StoreNamedColor:
    str = "X_StoreNamedColor";
    break;

  case X_QueryColors:
    str = "X_QueryColors";
    break;

  case X_LookupColor:
    str = "X_LookupColor";
    break;

  case X_CreateCursor:
    str = "X_CreateCursor";
    break;

  case X_CreateGlyphCursor:
    str = "X_CreateGlyphCursor";
    break;

  case X_FreeCursor:
    str = "X_FreeCursor";
    break;

  case X_RecolorCursor:
    str = "X_RecolorCursor";
    break;

  case X_QueryBestSize:
    str = "X_QueryBestSize";
    break;

  case X_QueryExtension:
    str = "X_QueryExtension";
    break;

  case X_ListExtensions:
    str = "X_ListExtensions";
    break;

  case X_ChangeKeyboardMapping:
    str = "X_ChangeKeyboardMapping";
    break;

  case X_GetKeyboardMapping:
    str = "X_GetKeyboardMapping";
    break;

  case X_ChangeKeyboardControl:
    str = "X_ChangeKeyboardControl";
    break;

  case X_GetKeyboardControl:
    str = "X_GetKeyboardControl";
    break;

  case X_Bell:
    str = "X_Bell";
    break;

  case X_ChangePointerControl:
    str = "X_ChangePointerControl";
    break;

  case X_GetPointerControl:
    str = "X_GetPointerControl";
    break;

  case X_SetScreenSaver:
    str = "X_SetScreenSaver";
    break;

  case X_GetScreenSaver:
    str = "X_GetScreenSaver";
    break;

  case X_ChangeHosts:
    str = "X_ChangeHosts";
    break;

  case X_ListHosts:
    str = "X_ListHosts";
    break;

  case X_SetAccessControl:
    str = "X_SetAccessControl";
    break;

  case X_SetCloseDownMode:
    str = "X_SetCloseDownMode";
    break;

  case X_KillClient:
    str = "X_KillClient";
    break;

  case X_RotateProperties:
    str = "X_RotateProperties";
    break;

  case X_ForceScreenSaver:
    str = "X_ForceScreenSaver";
    break;

  case X_SetPointerMapping:
    str = "X_SetPointerMapping";
    break;

  case X_GetPointerMapping:
    str = "X_GetPointerMapping";
    break;

  case X_SetModifierMapping:
    str = "X_SetModifierMapping";
    break;

  case X_GetModifierMapping:
    str = "X_GetModifierMapping";
    break;

  case X_NoOperation:
    str = "X_NoOperation";
    break;

  default:
    str = "unknown code";
    break;
  }

  return str;
}
