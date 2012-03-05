#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _file_menu_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         file_menu.c
* \author	Richard Baldock
* \date		April 2009
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <netdb.h>

#include <MAPaint.h>
#include <MAWarp.h>

static void new_obj_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

void read_model_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

static MenuItem file_menu_itemsP[] = {		/* file_menu items */
  {"new_obj", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   new_obj_cb, NULL, myHGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"read_obj", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   read_obj_cb, NULL,
   myHGU_XmHelpStandardCb, SEC_READ_WRITE_DOMAINS_DIALOGS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"read_model", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   read_model_cb, NULL,
   myHGU_XmHelpStandardCb, SEC_READ_WRITE_DOMAINS_DIALOGS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Recent", &xmCascadeButtonWidgetClass, 0, NULL, NULL, False,
   NULL, NULL, HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"write_obj", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_obj_cb, NULL,
   myHGU_XmHelpStandardCb, SEC_READ_WRITE_DOMAINS_DIALOGS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"obj_props", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   obj_props_cb, NULL,
   myHGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"theiler_stage", &xmCascadeButtonWidgetClass, 0, NULL, NULL, False,
   NULL, NULL, myHGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"theiler_setup", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   theiler_stage_setup_cb, NULL, myHGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Quit", &xmPushButtonGadgetClass, 'Q', "Ctrl<Key>Q", "Ctrl-Q", False,
   quit_cb, NULL,
   myHGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {NULL},
};

MenuItem	*file_menu_items = &(file_menu_itemsP[0]);

static MenuItem HGU_XmMiscMenuItemsP[] = {     /* misc_menu items */
  {"save_state", &xmPushButtonGadgetClass, 0,NULL, NULL, False,
   SaveStateCallback, NULL, myHGU_XmHelpStandardCb, SEC_MISC_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"restore_state", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   RestoreStateCallback, NULL, myHGU_XmHelpStandardCb, SEC_MISC_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL, XmTEAR_OFF_DISABLED, False, False, NULL},
  {"macro", &xmCascadeButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, myHGU_XmHelpStandardCb, SEC_MACRO_MENU,
   XmTEAR_OFF_ENABLED, False, False, &(HGU_XmMacroMenuItemsP[0])},
  {NULL},
};
MenuItem	*HGU_XmMiscMenuItems = &(HGU_XmMiscMenuItemsP[0]);

static Widget	read_obj_dialog, read_model_dialog;
static Widget	write_obj_dialog;
static char 	*refFileList[]={NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL};

/* logging functions */
void MAPaintLogData(
  String	type,
  String	value,
  int		code,
  Widget	widget)
{
  double clockVal;

  if( globals.logfileFp ){
    if(type && value ){
      if(*(value + strlen(value) - 1) == '\n'){
	*(value + strlen(value) - 1) = '\0';
      }
      clockVal = clock()/1000;
      fprintf(globals.logfileFp, "%-14s : %-12.0f : %s : %d : %x\n", type,
	      clockVal, value, code, (unsigned int) widget);
    }
  }
  return;
}


void referenceFileListCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmFileListCallbackStruct	*cbs=
    (HGU_XmFileListCallbackStruct *) client_data;
  WlzObject	*obj;
  Widget	cascade;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  HGU_XmSetHourGlassCursor(globals.topl);
  if( cbs == NULL ){
    /* clear list selection */
    HGU_XmFileListClearList(globals.fileList);
    HGU_XmFileListWriteResourceFile(globals.fileList,
				    globals.resourceFile);
    if((cascade = XtNameToWidget(globals.topl,
				 "*file_menu*_pulldown*Recent"))){
      HGU_XmFileListResetMenu(globals.fileList, cascade, referenceFileListCb);
    }
  }
  else if((obj = HGU_XmFileListReadObject(w, cbs, &errNum))){

    switch( obj->type ){
      WlzDomain	domain;
      WlzValues	values;
      WlzObject	*newObj;

    case WLZ_2D_DOMAINOBJ:
      if((domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN, 0, 0,
					obj->domain.i->line1,
					obj->domain.i->lastln,
					obj->domain.i->kol1,
					obj->domain.i->lastkl,
					&errNum))){
	domain.p->domains[0] = WlzAssignDomain(obj->domain, NULL);
	if((values.vox = WlzMakeVoxelValueTb(WLZ_VOXELVALUETABLE_GREY,
					     0, 0, WlzGetBackground(obj, NULL),
					     NULL, &errNum))){
	  values.vox->values[0] = WlzAssignValues(obj->values, NULL);
	  newObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
			       NULL, NULL, NULL);
	  WlzFreeObj(obj);
	  obj = newObj;
	}
      }
      globals.origObjType = WLZ_2D_DOMAINOBJ;
      break;

    case WLZ_3D_DOMAINOBJ:
      globals.origObjType = WLZ_3D_DOMAINOBJ;
      break;

    default:
      HGU_XmUserError(globals.topl,
		      "Read Reference Object:\n"
		      "    The reference object must be a 2- or 3-D\n"
		      "    grey-level image. Please select an alternate\n"
		      "    object",
		      XmDIALOG_FULL_APPLICATION_MODAL);
      WlzFreeObj( obj );
      /* set hour glass cursor */
      HGU_XmUnsetHourGlassCursor(globals.topl);
      return;
    }
    MAPaintLogData("ReferenceFile", refFileList[0], 0, NULL);
    install_paint_reference_object( obj );

    /* set the title of the top-level window */
    set_topl_title(cbs->file);
    globals.file = AlcStrDup(cbs->file);
    globals.origObjExtType = cbs->format;

    /* add to the file list and write file */
    HGU_XmFileListAddFile(globals.fileList, cbs->file, cbs->format);
    HGU_XmFileListWriteResourceFile(globals.fileList,
				    globals.resourceFile);
    if((cascade = XtNameToWidget(globals.topl,
				 "*file_menu*_pulldown*Recent"))){
      HGU_XmFileListResetMenu(globals.fileList, cascade, referenceFileListCb);
    }
  }

  HGU_XmUnsetHourGlassCursor(globals.topl);

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "ReferenceFileListCb", errNum);
  }
  return;
}

/* action and callback procedures */
void new_obj_cb(
Widget w, XtPointer client_data, XtPointer call_data)
{
/*  XtManageChild( new_obj_dialog );
  XtPopup( XtParent(obj_props_dialog), XtGrabNone );*/
  return;
}

void read_obj_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  if( read_obj_dialog ){
    XtManageChild( read_obj_dialog );
    XtPopup( XtParent(read_obj_dialog), XtGrabNone );
    /* kludge to update the file selections */
    XtCallCallbacks(read_obj_dialog, XmNmapCallback, call_data);
  }
  return;
}

void read_model_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  if( read_model_dialog ){
    XtManageChild( read_model_dialog );
    XtPopup( XtParent(read_model_dialog), XtGrabNone );
    /* kludge to update the file selections */
    XtCallCallbacks(read_model_dialog, XmNmapCallback, call_data);
  }
  return;
}

void write_obj_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  if( write_obj_dialog ){
    XtManageChild( write_obj_dialog );
    XtPopup( XtParent(write_obj_dialog), XtGrabNone );
    /* kludge to update the file selections */
    XtCallCallbacks(write_obj_dialog, XmNmapCallback, call_data);
  }
  return;
}

extern void MAOpenGLDisplayYBoundList(WlzBoundList *bndlist,
				      float		y);
extern void MAOpenGLDisplayXBoundList(WlzBoundList *bndlist,
				      float		x);

WlzErrorNum Wlz3DSectionTransformXPoly(
  WlzPolygonDomain	*poly,
  WlzThreeDViewStruct	*viewStr)
{
  WlzIVertex2	*vtxs;
  WlzDVertex3	vtx;
  int		i;

  if( poly == NULL ){
    return WLZ_ERR_NONE;
  }

  vtxs = poly->vtx;
  for(i=0; i < poly->nvertices; i++, vtxs++){
    vtx.vtX = vtxs->vtX;
    vtx.vtY = vtxs->vtY;
    vtx.vtZ = viewStr->dist;
    Wlz3DSectionTransformInvVtx(&vtx, viewStr);
    vtxs->vtX = vtx.vtZ;
    vtxs->vtY = vtx.vtY;
  }

  return WLZ_ERR_NONE;
}  

WlzErrorNum Wlz3DSectionTransformXBound(
  WlzBoundList		*bndlist,
  WlzThreeDViewStruct	*viewStr)
{
  if( bndlist == NULL ){
    return WLZ_ERR_NONE;
  }

  Wlz3DSectionTransformXBound(bndlist->next, viewStr);
  Wlz3DSectionTransformXBound(bndlist->down, viewStr);
  Wlz3DSectionTransformXPoly(bndlist->poly, viewStr);

  return WLZ_ERR_NONE;
}

WlzErrorNum Wlz3DSectionTransformYPoly(
  WlzPolygonDomain	*poly,
  WlzThreeDViewStruct	*viewStr)
{
  WlzIVertex2	*vtxs;
  WlzDVertex3	vtx;
  int		i;

  if( poly == NULL ){
    return WLZ_ERR_NONE;
  }

  vtxs = poly->vtx;
  for(i=0; i < poly->nvertices; i++, vtxs++){
    vtx.vtX = vtxs->vtX;
    vtx.vtY = vtxs->vtY;
    vtx.vtZ = viewStr->dist;
    Wlz3DSectionTransformInvVtx(&vtx, viewStr);
    vtxs->vtX = vtx.vtX;
    vtxs->vtY = vtx.vtZ;
  }

  return WLZ_ERR_NONE;
}  

WlzErrorNum Wlz3DSectionTransformYBound(
  WlzBoundList		*bndlist,
  WlzThreeDViewStruct	*viewStr)
{
  if( bndlist == NULL ){
    return WLZ_ERR_NONE;
  }

  Wlz3DSectionTransformYBound(bndlist->next, viewStr);
  Wlz3DSectionTransformYBound(bndlist->down, viewStr);
  Wlz3DSectionTransformYPoly(bndlist->poly, viewStr);

  return WLZ_ERR_NONE;
}

void setup_ref_display_list_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{

  WlzPlaneDomain	*planedmn;
  WlzThreeDViewStruct	*viewStr;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  if( globals.obj == NULL || globals.obj->type != WLZ_3D_DOMAINOBJ ||
     globals.ref_display_list == 0){
    return;
  }
  /* create a new display list */
  glDeleteLists(globals.ref_display_list, 1);
  globals.ref_display_list = glGenLists( (GLsizei) 1 );

  planedmn = globals.obj->domain.p;

  /* create the reference object 3D display  DisplayList */
  glNewList( globals.ref_display_list, GL_COMPILE );

  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glIndexi( HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 0.0, 0.0) );
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->lastpl);

  glColor3f(0.0, 1.0, 0.0);
  glIndexi( HGU_XGetColorPixel(globals.dpy, globals.cmap, 0.0, 1.0, 0.0) );
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->lastpl);

  glColor3f(0.0, 0.0, 1.0);
  glIndexi( HGU_XGetColorPixel(globals.dpy, globals.cmap, 0.0, 0.0, 1.0) );
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->lastpl);
  glEnd();

  if( globals.fb_obj && (globals.fb_obj->type == WLZ_3D_DOMAINOBJ) ){
    WlzObject		*obj1, *boundobj;
    WlzValues		values;
    int			z, step;

    /* establish the step size */
    planedmn = globals.fb_obj->domain.p;
    step = (planedmn->lastkl - planedmn->kol1) / 6;
    z = (planedmn->lastln - planedmn->line1) / 6;
    step = WLZ_MIN(step, z);
    z = (planedmn->lastpl - planedmn->plane1) / 6;
    step = WLZ_MIN(step, z);

    /* set up const z boundaries */
    glColor3f(1.0, 1.0, 1.0);
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 1.0));
    (void) glLineWidth( (GLfloat) 2.0 );
    for(z=planedmn->plane1+step/2; z <= planedmn->lastpl; z += step)
    {
      values.core = NULL;
      obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			 planedmn->domains[z - planedmn->plane1],
			 values, NULL, NULL, NULL);
      if( obj1->domain.core != NULL )
      {
	boundobj = WlzObjToBoundary(obj1, 1, &errNum);
	if( boundobj != NULL )
	{
	  MAOpenGLDisplayBoundList(boundobj->domain.b, (float) z );
	  WlzFreeObj( boundobj );
	}
	if( errNum != WLZ_ERR_NONE ){
	  break;
	}
      }
      WlzFreeObj( obj1 );
    }
    (void) glLineWidth( (GLfloat) 1.0 );

    /* set up const y boundaries */
    if( errNum == WLZ_ERR_NONE ){
      glColor3f(1.0, 1.0, 0.0);
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 0.0));
      if((viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum))){
	viewStr->theta = WLZ_M_PI / 2.0;
	viewStr->phi = WLZ_M_PI / 2.0;
	viewStr->dist = planedmn->line1 - step/2;
	errNum = WlzInit3DViewStruct(viewStr, globals.fb_obj);
	for(z=viewStr->dist+step; (errNum == WLZ_ERR_NONE) && (z <= planedmn->lastln);
	    z += step)
	{
	  Wlz3DSectionIncrementDistance(viewStr, (double) step);
	  if((obj1 = WlzGetSectionFromObject(globals.fb_obj, viewStr,
					     WLZ_INTERPOLATION_NEAREST,
					     &errNum))){
	    obj1 = WlzAssignObject(obj1, NULL);
	    boundobj = WlzObjToBoundary(obj1, 1, &errNum);
	    if( boundobj != NULL )
	    {
	      /* convert boundary coordinates to voxel coordinates */
	      Wlz3DSectionTransformYBound(boundobj->domain.b, viewStr);
	      MAOpenGLDisplayYBoundList(boundobj->domain.b, (float) z);
	      WlzFreeObj( boundobj );
	    }
	    WlzFreeObj( obj1 );
	  }
	}
	WlzFree3DViewStruct(viewStr);
      }
    }

    /* set up const x boundaries */
    if( errNum == WLZ_ERR_NONE ){
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 0.0));
      glColor3f(1.0, 1.0, 0.0);
      if((viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum))){
	viewStr->theta = 0.0;
	viewStr->phi = WLZ_M_PI / 2.0;
	viewStr->dist = planedmn->kol1 - step/2;
	errNum = WlzInit3DViewStruct(viewStr, globals.fb_obj);
	for(z=viewStr->dist+step; (errNum == WLZ_ERR_NONE) && (z <= planedmn->lastkl);
	    z += step)
	{
	  Wlz3DSectionIncrementDistance(viewStr, (double) step);
	  if((obj1 = WlzGetSectionFromObject(globals.fb_obj, viewStr,
					     WLZ_INTERPOLATION_NEAREST,
					     &errNum))){
	    obj1 = WlzAssignObject(obj1, NULL);
	    boundobj = WlzObjToBoundary(obj1, 1, &errNum);
	    if( boundobj != NULL )
	    {
	      /* convert boundary coordinates to voxel coordinates */
	      Wlz3DSectionTransformXBound(boundobj->domain.b, viewStr);
	      MAOpenGLDisplayXBoundList(boundobj->domain.b, (float) z);
	      WlzFreeObj( boundobj );
	    }
	    WlzFreeObj( obj1 );
	  }
	}
	WlzFree3DViewStruct(viewStr);
      }
    }
  }

  if( errNum == WLZ_ERR_NONE ){
    glIndexi( HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 1.0) );
    glColor3f(1.0, 1.0, 1.0);
    glEndList();

    WLZ_VTX_3_SET(globals.bbox_vtx, planedmn->kol1 - 2.0,
		  planedmn->line1 - 2.0, planedmn->plane1 - 2.0);
    WLZ_VTX_3_SET(globals.bbox_size,
		  planedmn->lastkl - planedmn->kol1 + 4.0,
		  planedmn->lastln - planedmn->line1 + 4.0,
		  planedmn->lastpl - planedmn->plane1 + 4.0);
    glFogf(GL_FOG_DENSITY, 0.25/globals.bbox_size.vtX);
    MAOpenGLDrawScene( globals.canvas );
  }
  else {
    MAPaintReportWlzError(globals.topl, "setup_ref_display_list_cb", errNum);
  }

  return;
}

void install_paint_reference_object(
  WlzObject	*obj)
{
  Widget		toggle;
  WlzPixelV		bckgrnd;
  WlzPixelV		min, max, Min, Max;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  if( globals.orig_obj != NULL ){
    WlzFreeObj( globals.orig_obj );
  }
  globals.orig_obj = WlzAssignObject(obj, &errNum);
  obj = NULL;

  /* check input object for grey-value type
     convert if necessary */
  if( errNum == WLZ_ERR_NONE ){
    bckgrnd = WlzGetBackground(globals.orig_obj, &errNum);
    Min.type = WLZ_GREY_INT;
    Max.type = WLZ_GREY_INT;
    Min.v.inv = 0;
    Max.v.inv = 255;
  }
  
  if( errNum == WLZ_ERR_NONE ){
    switch( bckgrnd.type ){
    case WLZ_GREY_LONG:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.lnv < 0) || (max.v.lnv > 255) ){
	if((obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_INT, &errNum))){
	  WlzGreySetRange(obj, min, max, Min, Max, 0);
	  bckgrnd = WlzGetBackground(obj, &errNum);
	}
      }
      break;
    case WLZ_GREY_INT:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.inv < 0) || (max.v.inv > 255) ){
	obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_INT, &errNum);
	WlzGreySetRange(obj, min, max, Min, Max, 0);
	bckgrnd = WlzGetBackground(obj, &errNum);
      }
      break;
    case WLZ_GREY_SHORT:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.shv < 0) || (max.v.shv > 255) ){
	if((obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_SHORT, &errNum))){
	  WlzGreySetRange(obj, min, max, Min, Max, 0);
	  bckgrnd = WlzGetBackground(obj, &errNum);
	}
      }
      break;
    case WLZ_GREY_FLOAT:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.flv < 0) || (max.v.flv > 255) ){
	if((obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_INT, &errNum))){
	  WlzGreySetRange(obj, min, max, Min, Max, 0);
	  bckgrnd = WlzGetBackground(obj, &errNum);
	}
      }
      break;
    case WLZ_GREY_DOUBLE:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.dbv < 0) || (max.v.dbv > 255) ){
	if((obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_INT, &errNum))){
	  WlzGreySetRange(obj, min, max, Min, Max, 0);
	  bckgrnd = WlzGetBackground(obj, &errNum);
	}
      }
      break;

    default:
    case WLZ_GREY_UBYTE:
      break;
    }
  }

  /* copy for later transform for display purposes,
     the original is kept for IP purposes note pixconvert loses
     the background value */
  if( errNum == WLZ_ERR_NONE ){
    if( globals.obj != NULL ){
      WlzFreeObj( globals.obj );
    }
    if( obj ){
      globals.obj =
	WlzAssignObject(WlzConvertPix(obj, WLZ_GREY_UBYTE, &errNum), NULL);
      WlzFreeObj(globals.orig_obj);
      globals.orig_obj = WlzAssignObject(obj, NULL);
    }
    else {
      globals.obj =
	WlzAssignObject(WlzConvertPix(globals.orig_obj,
				      WLZ_GREY_UBYTE, &errNum), NULL);
    }
  }

  if( errNum == WLZ_ERR_NONE ){
    min.type = WLZ_GREY_UBYTE;
    max.type = WLZ_GREY_UBYTE;
    Min.type = WLZ_GREY_UBYTE;
    Max.type = WLZ_GREY_UBYTE;
    min.v.ubv = 0;
    max.v.ubv = 255;
    Min.v.ubv = globals.cmapstruct->gmin;
    Max.v.ubv = globals.cmapstruct->gmax;
    errNum = WlzGreySetRange(globals.obj, min, max, Min, Max, 0);
  }

  /* also convert the background values */
  if( errNum == WLZ_ERR_NONE ){
/*  min = WlzGetBackground(globals.orig_obj, NULL);*/
    WlzValueConvertPixel(&bckgrnd, bckgrnd, WLZ_GREY_INT);
    max.type = WLZ_GREY_INT;
    max.v.inv = ((bckgrnd.v.inv * (Max.v.ubv - Min.v.ubv)) / 255) + Min.v.ubv;
    WlzSetBackground(globals.obj, max);

    /* fill blank planes here - should be a resource option */
    if( (toggle = XtNameToWidget(read_obj_dialog, "*.fill_blanks")) ){
      Boolean	fill_blanks, min_domain;
      XtVaGetValues(toggle, XmNset, &fill_blanks, NULL);
      if( fill_blanks ){
	if( (toggle = XtNameToWidget(read_obj_dialog, "*.min_domain")) ){
	  XtVaGetValues(toggle, XmNset, &min_domain, NULL);
	  if( min_domain ){
	    WlzFillBlankPlanes(globals.obj, 1);
	  } else {
	    WlzFillBlankPlanes(globals.obj, 0);
	  }
	}
	else {
	  WlzFillBlankPlanes(globals.obj, 1);
	}
      }
    }

    /* setup the display list and properties dialog */
    setup_ref_display_list_cb(read_obj_dialog, NULL, NULL);
    setup_obj_props_cb(read_obj_dialog, NULL, NULL);
  }

  /* log the object size */
  if( (errNum == WLZ_ERR_NONE) && globals.logfileFp ){
    char strBuf[64];
    sprintf(strBuf, "(%d, %d, %d, %d, %d, %d)",
	    globals.obj->domain.p->kol1, globals.obj->domain.p->line1,
	    globals.obj->domain.p->plane1, globals.obj->domain.p->lastkl,
	    globals.obj->domain.p->lastln, globals.obj->domain.p->lastpl);
    MAPaintLogData("BoundingBox", strBuf, 0, NULL);
    sprintf(strBuf, "%d", WlzVolume(globals.obj, NULL));
    MAPaintLogData("Volume", strBuf, 0, NULL);
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl,
			  "install_paint_reference_object", errNum);
  }
  return;
}

void set_topl_title(
  String	name)
{
  String	class_return;
  char		str_buff[64], app_name[32], *tmpStr;
  int		i, basename_start;

  if( globals.app_name == NULL ){
    /* get the top level dialog and application name */
    XtGetApplicationNameAndClass(XtDisplay(globals.topl),
				 &globals.app_name, &class_return);
  }

  /* check for emage flag 
     remove any trailing "_EMAGE" */
  strncpy(app_name, globals.app_name, 30);
  if((tmpStr = strstr(app_name, "_EMAGE"))){
    app_name[tmpStr - app_name] = '\0';
  }
  if( globals.emageFlg == True ){
    sprintf(app_name, "%s(EMAGE)", globals.app_name);
  }
  else {
    sprintf(app_name, "%s", globals.app_name);
  }
  
  /* use given name or globals.file or NULL */
  if( name == NULL ){
    name = globals.file;
  }
  if( name ){
    /* get file basename */
    for(i=0, basename_start=0; i < strlen(name); i++){
      if( name[i] == '/' ){
	basename_start = i+1;
      }
    }
    sprintf(str_buff, "%s: %s", app_name, name+basename_start);
  }
  else {
    sprintf(str_buff, "%s", app_name);
  }

  XtVaSetValues(globals.topl, XtNtitle, str_buff, NULL);

  return;
}
  

void read_reference_object_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  WlzEffFormat		image_type;
  WlzObject		*obj;
  String		icsfile;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* set hour glass cursor */
  HGU_XmSetHourGlassCursor(globals.topl);

  /* read the new reference object
     note the switch is to allow direct read
     given the image type to include the model
     input options */
  if( client_data ){
    image_type = (WlzEffFormat) client_data;
    if((icsfile = HGU_XmGetFileStr(globals.topl, cbs->value,
				   cbs->dir))){
      obj = WlzEffReadObj(NULL, icsfile, image_type, 0, &errNum);
      AlcFree(icsfile);
    }
    else {
      obj = NULL;
    }
  }
  else {
    obj = HGU_XmReadExtFFObject(read_obj_dialog, cbs,
			       &image_type, &errNum);
  }
  if( obj == NULL){
    HGU_XmUserError(globals.topl,
		    "Read Reference Object:\n"
		    "    No reference object read - either the\n"
		    "    selected file is empty or it is not the\n"
		    "    correct object type - please check the\n"
		    "    file or make a new selection",
   		    XmDIALOG_FULL_APPLICATION_MODAL);
    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
  }

  if( obj->values.core == NULL ){
    HGU_XmUserError(globals.topl,
		    "Read Reference Object:\n"
		    "    The reference object must have a grey-\n"
		    "    value table. Please select an alternate\n"
		    "    object",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    WlzFreeObj( obj );
    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
  }

  /* install the new reference object */
  switch( obj->type ){
    WlzDomain	domain;
    WlzValues	values;
    WlzObject	*newObj;

  case WLZ_2D_DOMAINOBJ:
    if((domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN, 0, 0,
				      obj->domain.i->line1,
				      obj->domain.i->lastln,
				      obj->domain.i->kol1,
				      obj->domain.i->lastkl,
				      &errNum))){
      domain.p->domains[0] = WlzAssignDomain(obj->domain, NULL);
      if((values.vox = WlzMakeVoxelValueTb(WLZ_VOXELVALUETABLE_GREY,
					   0, 0, WlzGetBackground(obj, NULL),
					   NULL, &errNum))){
	values.vox->values[0] = WlzAssignValues(obj->values, NULL);
	newObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
			     obj->plist, NULL, &errNum);
	WlzFreeObj(obj);
	obj = newObj;
      }
    }
    globals.origObjType = WLZ_2D_DOMAINOBJ;
    break;

  case WLZ_3D_DOMAINOBJ:
    globals.origObjType = WLZ_3D_DOMAINOBJ;
    break;

  default:
    HGU_XmUserError(globals.topl,
		    "Read Reference Object:\n"
		    "    The reference object must be a 2- or 3-D\n"
		    "    grey-level image. Please select an alternate\n"
		    "    object",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    WlzFreeObj( obj );
    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
  }
  globals.origObjExtType = image_type;

  /* set title and reference file list */
  if((icsfile = HGU_XmGetFileStr(globals.topl, cbs->value, cbs->dir)))
  {
    Widget	cascade;
    HGU_XmFileListAddFile(globals.fileList, icsfile, image_type);
    HGU_XmFileListWriteResourceFile(globals.fileList,
				    globals.resourceFile);
    if((cascade = XtNameToWidget(globals.topl,
				 "*file_menu*_pulldown*Recent"))){
      HGU_XmFileListResetMenu(globals.fileList, cascade, referenceFileListCb);
    }
    if( XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &icsfile) )
    {
      set_topl_title(icsfile);
      globals.file = icsfile;
    }
  }
  MAPaintLogData("ReferenceFile", globals.file, 0, NULL);

  /* clear feedback object and install new reference object */
  if( errNum == WLZ_ERR_NONE ){
    if( globals.fb_obj ){
      WlzFreeObj(globals.fb_obj);
      globals.fb_obj = NULL;
    }
    install_paint_reference_object( obj );
  }

  /* set hour glass cursor */
  HGU_XmUnsetHourGlassCursor(globals.topl);

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "read_reference_object_cb", errNum);
  }
  return;
}

void write_reference_object_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  WlzEffFormat	image_type = (WlzEffFormat) client_data;

  /* set hour glass cursor */
  HGU_XmSetHourGlassCursor(globals.topl);

  /* write the reference object */
  if( globals.origObjType == WLZ_2D_DOMAINOBJ ){
    WlzObject *tmpObj;

    tmpObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, 
			 globals.orig_obj->domain.p->domains[0],
			 globals.orig_obj->values.vox->values[0],
			 NULL, NULL, NULL);
    HGU_XmWriteExtFFObject(tmpObj, w, cbs, &image_type);
    WlzFreeObj(tmpObj);
  }
  else {
    HGU_XmWriteExtFFObject(globals.orig_obj, w, cbs, &image_type);
  }

  /* set hour glass cursor */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  return;
}

void quit_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  /* make sure we are mapped and at the top */
  XtMapWidget( globals.topl );

  /* use a dialog widget to confirm exit */
  if( !HGU_XmUserConfirm(globals.topl, "Really quit?", "Yes", "No", 1 ) ){
    return;
  }

  /* save domains */
  save_domains();
  clear_autosave();

  /* check if expressMap */
  if( globals.expressMapFlg ){
    if( !(warpGlobals.bibfileListCSVFileSavedFlg) ){
      expressMapSaveListCb(w, client_data, call_data);
    }
  }

  /* use a dialog widget to double confirm exit  - last possible cop out */
  if( !HGU_XmUserConfirm(globals.topl, "Really really quit?",
			 "Yes", "No", 1 ) ){
    return;
  }

  /* kill the help viewer */
  HGU_XmHelpDestroyViewer();

  XtDestroyWidget( globals.topl );

  exit( 0 );
}

void fileMenuPopupCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	menu=(Widget) client_data;
  Widget	button;
  int		i;

  /* put in current reference file list */
  for(i=1; i <= 10; i++){
    char	strBuf[128];
    XmString	xmStr;
    int		len;

    sprintf(strBuf, "*%d", i);
    if((button = XtNameToWidget(menu, strBuf))){
      if( refFileList[i-1] != NULL){
	/* reset the button label */
	if( (len = strlen(refFileList[i-1])) < 36 ){
	  sprintf(strBuf, "%d/ %s", i, refFileList[i-1]);
	}
	else {
	  while( len && (refFileList[i-1][len-1] != '/') ){
	    len--;
	  }
	  if( len ){
	    sprintf(strBuf, "%d/ %.16s .../%s", i, refFileList[i-1],
		    refFileList[i-1] + len);
	  }
	  else {
	    sprintf(strBuf, "%d/ %s", i, refFileList[i-1]);
	  }
	}
	xmStr = XmStringCreateSimple(strBuf);
	XtVaSetValues(button, XmNlabelString, xmStr, NULL);
	XmStringFree(xmStr);

	XtManageChild(button);
      }
      else {
	XtUnmanageChild(button);
      }
    }
  }

  return;
}

/* file_menu initialisation procedure */
#define set_att_offset(field)   XtOffsetOf(PaintGlobals, field)

static XtResource set_att_res[] = {
{"theilerDir", "TheilerDir", XtRString, sizeof(String),
 set_att_offset(base_theiler_dir), XtRString, NULL},
{"theilerStage", "TheilerStage", XtRString, sizeof(String),
 set_att_offset(theiler_stage), XtRString, NULL},
{"logfile", "Logfile", XtRString, sizeof(String),
 set_att_offset(logfile), XtRString, NULL},
};

void file_menu_init(
  Widget	topl)
{
  Widget	rc, form, toggle;
  Visual	*visual;
  Arg		arg[1];
  char		fileStr[128];
  FILE 		*fp;
  WlzEffFormat	image_type=WLZEFF_FORMAT_WLZ;

  /* set the top-level title */
  set_topl_title(NULL);

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(topl);
  XtSetArg(arg[0], XmNvisual, visual);

  /* create the read-model file selection dialog */
  read_model_dialog = XmCreateFileSelectionDialog(topl,
						  "read_model_dialog", arg, 1);
  XtAddCallback(read_model_dialog, XmNokCallback,
		read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_WLZ);
  XtAddCallback(read_model_dialog, XmNokCallback, PopdownCallback, NULL);
  XtAddCallback( read_model_dialog, XmNcancelCallback, 
		PopdownCallback, NULL);
  XtAddCallback(read_model_dialog, XmNmapCallback,
		FSBPopupCallback, NULL);
  XtManageChild( read_model_dialog );

  /* create the read-obj file selection dialog */
  read_obj_dialog = HGU_XmCreateExtFFObjectFSB(topl, "read_obj_dialog",
					      read_reference_object_cb, NULL);
  if((rc = XtNameToWidget(read_obj_dialog, "*.formatFormRC"))){

    /* add a form to include file type and fill-blanks toggle */
    form = XtVaCreateManagedWidget("read_file_form", xmFormWidgetClass,
				   rc,
				   XmNborderWidth,	0,
				   NULL);

    /* add a fill-blanks toggles */
    toggle = XtVaCreateManagedWidget("fill_blanks", xmToggleButtonGadgetClass,
				     form,
				     XmNindicatorOn, 	True,
				     XmNindicatorType,	XmN_OF_MANY,
				     XmNset,		False,
				     XmNtopAttachment,	XmATTACH_FORM,
				     XmNleftAttachment,	XmATTACH_FORM,
				     NULL);

    toggle = XtVaCreateManagedWidget("min_domain", xmToggleButtonGadgetClass,
				     form,
				     XmNindicatorOn, 	True,
				     XmNindicatorType,	XmN_OF_MANY,
				     XmNset,		True,
				     XmNtopAttachment,	XmATTACH_FORM,
				     XmNleftAttachment,	XmATTACH_WIDGET,
				     XmNleftWidget,	toggle,
				     NULL);
  }

  HGU_XmExtFFObjectFSBSetType(read_obj_dialog, WLZEFF_FORMAT_WLZ);
  XtManageChild( read_obj_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( read_obj_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* create the write-obj file selection dialog */
  write_obj_dialog = HGU_XmCreateExtFFObjectFSB(topl, "write_obj_dialog",
					       write_reference_object_cb,
					       NULL);
  HGU_XmExtFFObjectFSBSetType(write_obj_dialog, WLZEFF_FORMAT_WLZ);


  /* initialise the reference file list pulldown */
  if( !globals.sectViewFlg ){
    Widget	cascade;

    if((cascade = XtNameToWidget(globals.topl,
				 "*file_menu*_pulldown*Recent"))){
      globals.resourceFile = (String) 
	AlcMalloc(sizeof(char) * (strlen(getenv("HOME")) + 16));
      sprintf(globals.resourceFile, "%s/%s", getenv("HOME"), ".maRecentFiles");
      globals.fileList = HGU_XmFileListCreateList(globals.resourceFile, NULL);
      HGU_XmFileListResetMenu(globals.fileList, cascade, referenceFileListCb);
    }
  }

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( write_obj_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* create the object properties dialog */
  obj_props_dialog_init( topl );

  globals.file      = NULL;
  globals.obj       = NULL;
  globals.orig_obj  = NULL;
  globals.fb_obj    = NULL;
  globals.origObjExtType = image_type;

  /* setup the theiler directory and menu item - check for stage */
  XtGetApplicationResources(topl, &globals, set_att_res,
			    XtNumber(set_att_res), NULL, 0);

  /* check the logfile */
  if( globals.logfile ){
    if( (globals.logfileFp = fopen(globals.logfile, "w")) == NULL ){
      fprintf(stderr,
	      "MAPaint: something wrong with the logfile %s\n"
	      "Please check name and permissions\n"
	      "Logging not enabled\n\007", globals.logfile);
    }
    else {
      char		timeBuf[16];
      struct hostent 	*hstnt;
      fprintf(stderr, "MAPaint: logging enabled to %s\n", globals.logfile);
      MAPaintLogData("Filename", globals.logfile, 0, NULL);
      MAPaintLogData("User", getenv("USER"), 0, NULL);
#if defined (LINUX2) || defined (DARWIN)
      strcpy(timeBuf, "00/00/00");
#else
      tmpTime = time(NULL);
      cftime(timeBuf, "%d/%m/%Y", &tmpTime);
#endif /* LINUX2 */
      MAPaintLogData("Date", timeBuf, 0, NULL);
#if defined (LINUX2) || defined (DARWIN)
      strcpy(timeBuf, "00.00");
#else
      cftime(timeBuf, "%H.%M", &tmpTime);
#endif /* LINUX2 */
      MAPaintLogData("Time", timeBuf, 0, NULL);
      hstnt = gethostbyname(getenv("HOST"));
      MAPaintLogData("Host", getenv("HOST"), 0, NULL);
      MAPaintLogData("Hostname", hstnt->h_name, 0, NULL);
    }
  }
  else {
    globals.logfileFp = NULL;
  }

  /* check base directory - if the string has come from the resources then
     we need to duplicate it to allow it to be freed
     possibly some memory leakage here */
  /* note: only non-NULL if set by the user, if NULL then attempt to find
     the cdrom or copied data */
  if( globals.base_theiler_dir ){
    globals.base_theiler_dir = AlcStrDup( globals.base_theiler_dir );
  }
  else {
    FILE	*pp;
    
    /* search for the Theiler mode directory as per the CDROM 
       should search local disc first */
#if defined (LINUX2)
    if((pp = popen("find /mnt -maxdepth 4 -name Models", "r"))){
      while( fscanf(pp, "%s", fileStr) != EOF ){
	if( strstr(fileStr, "Models") ){
	  globals.base_theiler_dir = AlcStrDup(fileStr);
	  break;
	}
      }
      pclose(pp);
    }
#elif defined (DARWIN)
    if( pp = popen("find /Volumes -maxdepth 4 -name Models", "r") ){
      while( fscanf(pp, "%s", fileStr) != EOF ){
	if( strstr(fileStr, "Models") ){
	  globals.base_theiler_dir = AlcStrDup(fileStr);
	  break;
	}
      }
      pclose(pp);
    }
#elif defined (SUNOS4) || defined (SUNOS5)
    if( pp = popen("find /cdrom -maxdepth 4 -name Models", "r") ){
      while( fscanf(pp, "%s", fileStr) != EOF ){
	if( strstr(fileStr, "Models") ){
	  globals.base_theiler_dir = AlcStrDup(fileStr);
	  break;
	}
      }
      pclose(pp);
    }
#else
    globals.base_theiler_dir = NULL;
#endif
  }
  if( globals.theiler_stage ){
    char *tStr;
    if((tStr = theilerString(globals.theiler_stage))){
      globals.theiler_stage = AlcStrDup(tStr);
    }
    else {
      globals.theiler_stage = NULL;
    }
  }
  theiler_menu_init( topl );

  /* check for an initial reference file else check Theiler stage */
  if( initial_reference_file != NULL ){
    WlzObject 	*obj;

    /* open the reference object file and install */
    if( (fp = fopen(initial_reference_file, "r")) ){
      HGU_XmSetHourGlassCursor(topl);
      if((obj = WlzReadObj( fp, NULL ))){
	switch( obj->type ){
	  WlzDomain	domain;
	  WlzValues	values;
	  WlzObject	*newObj;

	case WLZ_2D_DOMAINOBJ:
	  domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN, 0, 0,
					obj->domain.i->line1,
					obj->domain.i->lastln,
					obj->domain.i->kol1,
					obj->domain.i->lastkl,
					NULL);
	  domain.p->domains[0] = WlzAssignDomain(obj->domain, NULL);
	  values.vox = WlzMakeVoxelValueTb(WLZ_VOXELVALUETABLE_GREY,
					   0, 0, WlzGetBackground(obj, NULL),
					   NULL, NULL);
	  values.vox->values[0] = WlzAssignValues(obj->values, NULL);
	  newObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
			       NULL, NULL, NULL);
	  WlzFreeObj(obj);
	  obj = newObj;
	  globals.origObjType = WLZ_2D_DOMAINOBJ;
	  break;

	case WLZ_3D_DOMAINOBJ:
	  globals.origObjType = WLZ_3D_DOMAINOBJ;
	  break;

	default:
	  HGU_XmUserError(globals.topl,
			  "Read Reference Object:\n"
			  "    The reference object must be a 2- or 3-D woolz\n"
			  "    grey-level image. Please select an alternate\n"
			  "    object",
			  XmDIALOG_FULL_APPLICATION_MODAL);
	  WlzFreeObj( obj );
	  /* set hour glass cursor */
	  HGU_XmUnsetHourGlassCursor(globals.topl);
	  return;

	}
	HGU_XmFileListAddFile(globals.fileList, initial_reference_file,
			      image_type);
	HGU_XmFileListWriteResourceFile(globals.fileList,
					globals.resourceFile);
	MAPaintLogData("ReferenceFile", initial_reference_file, 0, NULL);
	install_paint_reference_object( obj );

	/* set the globals reference file */
	globals.file = initial_reference_file;

	/* set the title of the top-level window */
	set_topl_title(globals.file);
      }
      else {
	/* if it fails to read, check the file name
	   or the file content for special options */
	if( strstr(initial_reference_file, "MAPaint") ){
	  /* standard MAPaint startup */
	  globals.app_name = "MAPaint";
	  /* set the title of the top-level window */
	  set_topl_title(globals.file);
	}
	else if( strstr(initial_reference_file, "SectionView") ){
	  /* restricted section view startup */
	  globals.app_name = "SectionView";
	  globals.sectViewFlg = 1;
	  /* set the title of the top-level window */
	  set_topl_title(globals.file);
	}
	else if( theilerString(initial_reference_file) ){
	  /* load in theiler stage anatomy etc. */
	  globals.app_name = "SectionView";
	  globals.sectViewFlg = 1;
	  set_theiler_stage_cb(topl, theilerString(initial_reference_file),
			       NULL);
	}
	else {
	  char	strBuf[33];
	  /* check the content */
	  rewind(fp);
	  fscanf(fp, "%32s", strBuf);
	  if( strstr(strBuf, "MAPaint") ){
	    /* standard MAPaint startup */
	    globals.app_name = "MAPaint";
	    /* set the title of the top-level window */
	    set_topl_title(globals.file);
	  }
	  else if( strstr(strBuf, "SectionView") ){
	    /* restricted section view startup */
	    globals.app_name = "SectionView";
	    globals.sectViewFlg = 1;
	    /* set the title of the top-level window */
	    set_topl_title(globals.file);
	  }
	  else if( theilerString(strBuf) ){
	    /* load in theiler stage anatomy etc. */
	    globals.app_name = "SectionView";
	    globals.sectViewFlg = 1;
	    set_theiler_stage_cb(topl,
				 theilerString(strBuf),
				 NULL);
	  }
	}

	/* set the globals reference file */
	globals.file = NULL;
      }
      (void) fclose( fp );

      HGU_XmUnsetHourGlassCursor(topl);
    }
  }
  else if( globals.theiler_stage ){
    globals.app_name = "MAPaint";
    set_theiler_stage_cb(topl, theilerString(globals.theiler_stage), NULL);
  }

  /* reset the colormap */
  if( globals.sectViewFlg == 1 ){
    init_paint_cmapstruct(globals.topl);
  }

  /* check for an initial domain file */
  if( initial_domain_file != NULL ){
    WlzObject 	*obj;

    /* open the domain object file and put it in as a 3D feedback option */
    if( (fp = fopen(initial_domain_file, "rb")) ){
      HGU_XmSetHourGlassCursor(topl);
      if((obj = WlzReadObj( fp, NULL ))){
	if( globals.fb_obj ){
	  WlzFreeObj(globals.fb_obj);
	}
	globals.fb_obj = WlzAssignObject(obj, NULL);
	setup_ref_display_list_cb(read_obj_dialog, NULL, NULL);
      }
      (void) fclose( fp );
      HGU_XmUnsetHourGlassCursor(topl);
    }
  }

  return;
}
