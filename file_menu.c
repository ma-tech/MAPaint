#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	file_menu.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <netdb.h>

#include <MAPaint.h>

static void new_obj_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

static void image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

static void write_image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

void read_model_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

/* menu item structure */
static MenuItem file_type_menu_itemsP[] = {   /* file_menu items */
  {"woolz", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_WLZ,
   HGU_XmHelpStandardCb, SEC_INPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"ics", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_ICS,
   HGU_XmHelpStandardCb, SEC_INPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"den", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_DEN,
   HGU_XmHelpStandardCb, SEC_INPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_VFF,
   HGU_XmHelpStandardCb, SEC_INPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pic", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_PIC,
   HGU_XmHelpStandardCb, SEC_INPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vtk", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_VTK,
   HGU_XmHelpStandardCb, SEC_INPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"slc", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_SLC,
   HGU_XmHelpStandardCb, SEC_INPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"ipl", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_IPL,
   HGU_XmHelpStandardCb, SEC_INPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pgm", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_PNM,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"bmp", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_BMP,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem write_file_type_menu_itemsP[] = {/* write file_menu items */
  {"woolz", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_WLZ,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"ics", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_ICS,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"den", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_DEN,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_VFF,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pic", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_PIC,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vtk", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_VTK,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"slc", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_SLC,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pgm", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_PNM,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"bmp", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_BMP,
   HGU_XmHelpStandardCb, SEC_OUTPUT_DATA_FORMATS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem file_menu_itemsP[] = {		/* file_menu items */
  {"new_obj", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   new_obj_cb, NULL, HGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"read_obj", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   read_obj_cb, NULL,
   HGU_XmHelpStandardCb, SEC_READ_WRITE_DOMAINS_DIALOGS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"read_model", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   read_model_cb, NULL,
   HGU_XmHelpStandardCb, SEC_READ_WRITE_DOMAINS_DIALOGS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"write_obj", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_obj_cb, NULL,
   HGU_XmHelpStandardCb, SEC_READ_WRITE_DOMAINS_DIALOGS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"obj_props", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   obj_props_cb, NULL,
   HGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"theiler_stage", &xmCascadeButtonWidgetClass, 0, NULL, NULL, False,
   NULL, NULL, HGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"theiler_setup", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   theiler_stage_setup_cb, NULL, HGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"1", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 1,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"2", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 2,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"3", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 3,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"4", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 4,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"5", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 5,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"6", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 6,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"7", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 7,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"8", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 8,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"9", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 9,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"10", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   ReferenceFileListCb, (XtPointer) 10,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Quit", &xmPushButtonGadgetClass, 'Q', "Ctrl<Key>Q", "Ctrl-Q", False,
   quit_cb, NULL,
   HGU_XmHelpStandardCb, SEC_FILE_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

MenuItem	*file_menu_items = &(file_menu_itemsP[0]);

static MenuItem HGU_XmMiscMenuItemsP[] = {     /* misc_menu items */
  {"save_state", &xmPushButtonGadgetClass, 0,NULL, NULL, False,
   SaveStateCallback, NULL, HGU_XmHelpStandardCb, SEC_MISC_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"restore_state", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   RestoreStateCallback, NULL, HGU_XmHelpStandardCb, SEC_MISC_MENU,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL, XmTEAR_OFF_DISABLED, False, False, NULL},
  {"macro", &xmCascadeButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, HGU_XmHelpStandardCb, SEC_MACRO_MENU,
   XmTEAR_OFF_ENABLED, False, False, &(HGU_XmMacroMenuItemsP[0])},
  NULL,
};
MenuItem	*HGU_XmMiscMenuItems = &(HGU_XmMiscMenuItemsP[0]);

static Widget	new_obj_dialog, read_obj_dialog, read_model_dialog;
static Widget	write_obj_dialog;
static char 	*refFileList[]={NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL};
static WlzEffFormat refImageTypeList[]=
{WLZEFF_FORMAT_WLZ, WLZEFF_FORMAT_WLZ, WLZEFF_FORMAT_WLZ,
 WLZEFF_FORMAT_WLZ, WLZEFF_FORMAT_WLZ, WLZEFF_FORMAT_WLZ,
 WLZEFF_FORMAT_WLZ, WLZEFF_FORMAT_WLZ, WLZEFF_FORMAT_WLZ,
 WLZEFF_FORMAT_WLZ};

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
	      clockVal, value, code, widget);
    }
  }
  return;
}


/* reference file list manipulation procedures */
void ReferenceFileListPush(
  char 		*fileStr,
  WlzEffFormat	image_type)
{
  int	i;

  if( fileStr ){
    if( refFileList[9] ){
      AlcFree((void *) refFileList[9]);
    }
    for(i=9; i > 0; i--){
      refFileList[i] = refFileList[i-1];
      refImageTypeList[i] = refImageTypeList[i-1];
    }
    refFileList[0] = strdup(fileStr);
    refImageTypeList[0] = image_type;
  }

  return;
}

void ReferenceFileListWrite(
  FILE	*fp)
{
  int	i;

  fprintf(fp, "Reference file list:\n");
  for(i=0; i < 10; i++){
    if( refFileList[i] ){
      fprintf(fp, "%s, %d\n", refFileList[i], refImageTypeList[i]);
    }
    else {
      break;
    }
  }
  fprintf(fp, "End file list\n");

  return;
}

void ReferenceFileListCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int	fileIndex = ((int) client_data) - 1;
  FILE	*fp=NULL;
  WlzEffFormat	image_type = refImageTypeList[fileIndex];
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  if( refFileList[fileIndex] ){
    WlzObject 	*obj;

    /* get the file pointer or file name if ics format */
    if( image_type != WLZEFF_FORMAT_ICS )
    {
      if( (fp = fopen(refFileList[fileIndex], "r")) == NULL )
      {
	return;
      }
    }
  
    /* read the new reference object */
    HGU_XmSetHourGlassCursor(globals.topl);
    obj = WlzEffReadObj(fp, refFileList[fileIndex], image_type, &errNum);

    /* close the file pointer if non NULL */
    if( fp )
    {
      (void) fclose( fp );
    }

    if( obj ){
      switch( obj->type ){
	WlzDomain	domain;
	WlzValues	values;
	WlzObject	*newObj;

      case WLZ_2D_DOMAINOBJ:
	if( domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN, 0, 0,
					  obj->domain.i->line1,
					  obj->domain.i->lastln,
					  obj->domain.i->kol1,
					  obj->domain.i->lastkl,
					  &errNum) ){
	  domain.p->domains[0] = WlzAssignDomain(obj->domain, NULL);
	  if( values.vox = WlzMakeVoxelValueTb(WLZ_VOXELVALUETABLE_GREY,
					       0, 0, WlzGetBackground(obj, NULL),
					       NULL, &errNum) ){
	    values.vox->values[0] = WlzAssignValues(obj->values, NULL);
	    newObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
				 NULL, NULL, NULL);
	    WlzFreeObj(obj);
	    obj = newObj;
	  }
	}
	break;

      case WLZ_3D_DOMAINOBJ:
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
      if( fileIndex ){
	ReferenceFileListPush(refFileList[fileIndex], image_type);
      }
      MAPaintLogData("ReferenceFile", refFileList[0], 0, NULL);
      install_paint_reference_object( obj );
    }

    /* set the title of the top-level window */
    set_topl_title(refFileList[0]);
    globals.file = refFileList[0];

    HGU_XmUnsetHourGlassCursor(globals.topl);
  }

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

static XmStringCharSet charset = XmSTRING_DEFAULT_CHARSET;
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
      if( viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum) ){
	viewStr->theta = WLZ_M_PI / 2.0;
	viewStr->phi = WLZ_M_PI / 2.0;
	viewStr->dist = planedmn->line1 - step/2;
	errNum = WlzInit3DViewStruct(viewStr, globals.fb_obj);
	for(z=viewStr->dist+step; (errNum == WLZ_ERR_NONE) && (z <= planedmn->lastln);
	    z += step)
	{
	  Wlz3DSectionIncrementDistance(viewStr, (double) step);
	  if( obj1 = WlzGetSectionFromObject(globals.fb_obj, viewStr, &errNum) ){
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
      if( viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum) ){
	viewStr->theta = 0.0;
	viewStr->phi = WLZ_M_PI / 2.0;
	viewStr->dist = planedmn->kol1 - step/2;
	errNum = WlzInit3DViewStruct(viewStr, globals.fb_obj);
	for(z=viewStr->dist+step; (errNum == WLZ_ERR_NONE) && (z <= planedmn->lastkl);
	    z += step)
	{
	  Wlz3DSectionIncrementDistance(viewStr, (double) step);
	  if( obj1 = WlzGetSectionFromObject(globals.fb_obj, viewStr, &errNum) ){
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
	if( obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_INT, &errNum) ){
	  WlzGreySetRange(obj, min, max, Min, Max);
	  bckgrnd = WlzGetBackground(obj, &errNum);
	}
      }
      break;
    case WLZ_GREY_INT:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.inv < 0) || (max.v.inv > 255) ){
	obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_INT, &errNum);
	WlzGreySetRange(obj, min, max, Min, Max);
	bckgrnd = WlzGetBackground(obj, &errNum);
      }
      break;
    case WLZ_GREY_SHORT:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.shv < 0) || (max.v.shv > 255) ){
	if( obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_SHORT, &errNum) ){
	  WlzGreySetRange(obj, min, max, Min, Max);
	  bckgrnd = WlzGetBackground(obj, &errNum);
	}
      }
      break;
    case WLZ_GREY_FLOAT:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.flv < 0) || (max.v.flv > 255) ){
	if( obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_INT, &errNum) ){
	  WlzGreySetRange(obj, min, max, Min, Max);
	  bckgrnd = WlzGetBackground(obj, &errNum);
	}
      }
      break;
    case WLZ_GREY_DOUBLE:
      WlzGreyRange(globals.orig_obj, &min, &max);
      if( (min.v.dbv < 0) || (max.v.dbv > 255) ){
	if( obj = WlzConvertPix(globals.orig_obj, WLZ_GREY_INT, &errNum)){
	  WlzGreySetRange(obj, min, max, Min, Max);
	  bckgrnd = WlzGetBackground(obj, &errNum);
	}
      }
      break;

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
    errNum = WlzGreySetRange(globals.obj, min, max, Min, Max);
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
  char		str_buff[64];
  int		i, basename_start;

  if( globals.app_name == NULL ){
    /* get the top level dialog and application name */
    XtGetApplicationNameAndClass(XtDisplay(globals.topl),
				 &globals.app_name, &class_return);
  }
  
  if( name ){
    /* get file basename */
    for(i=0, basename_start=0; i < strlen(name); i++){
      if( name[i] == '/' ){
	basename_start = i+1;
      }
    }
    sprintf(str_buff, "%s: %s", globals.app_name, name+basename_start);
  }
  else {
    sprintf(str_buff, "%s", globals.app_name);
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
  WlzEffFormat		image_type = (WlzEffFormat) client_data;
  WlzObject		*obj;
  FILE			*fp;
  String		icsfile;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* get the file pointer or file name if ics format */
  if((image_type == WLZEFF_FORMAT_ICS) ||
     (image_type == WLZEFF_FORMAT_PNM) ||
     (image_type == WLZEFF_FORMAT_BMP))
  {
    if( (icsfile = HGU_XmGetFileStr(globals.topl, cbs->value,
				    cbs->dir)) == NULL )
    {
      return;
    }
    fp = NULL;
  }
  else
  {
    if( (fp = HGU_XmGetFilePointer(globals.topl, cbs->value,
				   cbs->dir, "r")) == NULL )
    {
      return;
    }
    icsfile = NULL;
  }

  /* set hour glass cursor */
  HGU_XmSetHourGlassCursor(globals.topl);

  /* read the new reference object */
  obj = WlzEffReadObj(fp, icsfile, image_type, NULL);

  /* close the file pointer if non NULL */
  if( fp )
  {
    (void) fclose( fp );
  }

  /* check the object */
  if( obj == NULL ){
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
    if( domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN, 0, 0,
				      obj->domain.i->line1,
				      obj->domain.i->lastln,
				      obj->domain.i->kol1,
				      obj->domain.i->lastkl,
				      &errNum) ){
      domain.p->domains[0] = WlzAssignDomain(obj->domain, NULL);
      if( values.vox = WlzMakeVoxelValueTb(WLZ_VOXELVALUETABLE_GREY,
					   0, 0, WlzGetBackground(obj, NULL),
					   NULL, &errNum) ){
	values.vox->values[0] = WlzAssignValues(obj->values, NULL);
	newObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
			     NULL, NULL, &errNum);
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

  /* set title and reference file list */
  if(icsfile ||
     (icsfile = HGU_XmGetFileStr(globals.topl, cbs->value, cbs->dir)) )
  {
    ReferenceFileListPush(icsfile, image_type);
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
    FILE		*fp;
    String		icsfile;

    /* get the file pointer or file name if ics format */
    if((image_type == WLZEFF_FORMAT_ICS) ||
       (image_type == WLZEFF_FORMAT_PNM) ||
       (image_type == WLZEFF_FORMAT_BMP))
    {
      if( (icsfile = HGU_XmGetFileStr(globals.topl, cbs->value,
				      cbs->dir)) == NULL )
      {
	return;
      }
      fp = NULL;
    }
    else
    {
      if( (fp = HGU_XmGetFilePointer(globals.topl, cbs->value,
				     cbs->dir, "w")) == NULL )
      {
	return;
      }
      icsfile = NULL;
    }

    /* set hour glass cursor */
    HGU_XmSetHourGlassCursor(globals.topl);

    /* write the reference object */
    if( globals.origObjType == WLZ_2D_DOMAINOBJ ){
      WlzObject *tmpObj;

      tmpObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, 
			   globals.orig_obj->domain.p->domains[0],
			   globals.orig_obj->values.vox->values[0],
			   NULL, NULL, NULL);
      if( fp ){
	WlzWriteObj(fp, tmpObj);
      }
      WlzFreeObj(tmpObj);
    }
    else {
      WlzEffWriteObj(fp, icsfile, globals.orig_obj, image_type);
    }

    /* close the file pointer if non NULL */
    if( fp )
    {
      if( fclose( fp ) == EOF ){
	HGU_XmUserError(globals.topl,
			"Write Reference Object:\n"
			"    Error closing the reference object file\n"
			"    Please check disk sapce or quotas.\n"
			"    Object not saved to disk.",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }
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
  int		i;
  char	str_buf[128];
  String	str;
  FILE	*fp;

  /* make sure we are mapped and at the top */
  XtMapWidget( globals.topl );

  /* use a dialog widget to confirm exit */
  if( !HGU_XmUserConfirm(globals.topl, "Really quit?", "Yes", "No", 1 ) )
    return;

  /* save domains */
  save_domains();
  clear_autosave();

  /* use a dialog widget to double confirm exit  - last possible cop out */
  if( !HGU_XmUserConfirm(globals.topl, "Really really quit?",
			 "Yes", "No", 1 ) )
    return;

  /* kill the help viewer */
  HGU_XmHelpDestroyViewer();

  XtDestroyWidget( globals.topl );

  /* write the mapaint config file */
  if( !globals.sectViewFlg ){
    sprintf(str_buf, "%s/%s", getenv("HOME"), ".mapaint");
    if( fp = fopen(str_buf, "w") ){
      fprintf(fp,
	      "###################################################\n"
	      "#                                                 #\n"
	      "#   This file is written automatically by MAPaint #\n"
	      "#   modifications may be lost and may cause the   #\n"
	      "#   program to crash. Please do not change.       #\n"
	      "#                                                 #\n"
	      "###################################################\n\n");
      ReferenceFileListWrite(fp);

      fclose(fp);
    }
  }
  exit( 0 );
}

static void image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzEffFormat	image_type = (WlzEffFormat) client_data;
  XmString	pattern_str;

  XtRemoveAllCallbacks(read_obj_dialog, XmNokCallback);

  switch( image_type )
  {
   default:
   case WLZEFF_FORMAT_WLZ:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_WLZ);
     pattern_str = XmStringCreateSimple( "*.wlz" );
     break;
     
   case WLZEFF_FORMAT_ICS:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_ICS);
     pattern_str = XmStringCreateSimple( "*.ics" );
     break;

   case WLZEFF_FORMAT_DEN:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_DEN);
     pattern_str = XmStringCreateSimple( "*.den" );
     break;

   case WLZEFF_FORMAT_VFF:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_VFF);
     pattern_str = XmStringCreateSimple( "*.vff" );
     break;

   case WLZEFF_FORMAT_PIC:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_PIC);
     pattern_str = XmStringCreateSimple( "*.pic" );
     break;

   case WLZEFF_FORMAT_VTK:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_VTK);
     pattern_str = XmStringCreateSimple( "*.vtk" );
     break;

   case WLZEFF_FORMAT_SLC:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_SLC);
     pattern_str = XmStringCreateSimple( "*.slc" );
     break;

   case WLZEFF_FORMAT_IPL:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_IPL);
     pattern_str = XmStringCreateSimple( "*.ipl" );
     break;
     
   case WLZEFF_FORMAT_PNM:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_PNM);
     pattern_str = XmStringCreateSimple( "*.p?m" );
     break;
     
   case WLZEFF_FORMAT_BMP:
     XtAddCallback(read_obj_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_BMP);
     pattern_str = XmStringCreateSimple( "*.bmp" );
     break;
  }

  /* XtAddCallback(read_obj_dialog, XmNokCallback, setup_ref_display_list_cb,
		NULL);
  XtAddCallback(read_obj_dialog, XmNokCallback, setup_obj_props_cb,
  NULL);*/
  XtAddCallback(read_obj_dialog, XmNokCallback, PopdownCallback, NULL);

  XtVaSetValues(read_obj_dialog, XmNpattern, pattern_str, NULL);
  XmStringFree( pattern_str );
  XmFileSelectionDoSearch( read_obj_dialog, NULL );

  return;
}

static void write_image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzEffFormat	image_type = (WlzEffFormat) client_data;
  XmString	pattern_str;

  XtRemoveAllCallbacks(write_obj_dialog, XmNokCallback);

  switch( image_type )
  {
   default:
   case WLZEFF_FORMAT_WLZ:
     pattern_str = XmStringCreateSimple( "*.wlz" );
     break;
     
   case WLZEFF_FORMAT_ICS:
     pattern_str = XmStringCreateSimple( "*.ics" );
     break;

   case WLZEFF_FORMAT_DEN:
     pattern_str = XmStringCreateSimple( "*.den" );
     break;
     
   case WLZEFF_FORMAT_VFF:
     pattern_str = XmStringCreateSimple( "*.vff" );
     break;
     
   case WLZEFF_FORMAT_PIC:
     pattern_str = XmStringCreateSimple( "*.pic" );
     break;
     
   case WLZEFF_FORMAT_VTK:
     pattern_str = XmStringCreateSimple( "*.vtk" );
     break;
     
   case WLZEFF_FORMAT_SLC:
     pattern_str = XmStringCreateSimple( "*.slc" );
     break;
     
   case WLZEFF_FORMAT_PNM:
     pattern_str = XmStringCreateSimple( "*.p?m" );
     break;
     
   case WLZEFF_FORMAT_BMP:
     pattern_str = XmStringCreateSimple( "*.bmp" );
     break;
     
  }

  XtAddCallback(write_obj_dialog, XmNokCallback,
		write_reference_object_cb, client_data);
  XtAddCallback(write_obj_dialog, XmNokCallback, PopdownCallback, NULL);

  XtVaSetValues(write_obj_dialog, XmNpattern, pattern_str, NULL);
  XmStringFree( pattern_str );
  XmFileSelectionDoSearch( write_obj_dialog, NULL );

  return;
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
    if( button = XtNameToWidget(menu, strBuf) ){
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
 set_att_offset(base_theiler_dir), XtRString, "./reconstructions"},
{"theilerStage", "TheilerStage", XtRString, sizeof(String),
 set_att_offset(theiler_stage), XtRString, NULL},
{"logfile", "Logfile", XtRString, sizeof(String),
 set_att_offset(logfile), XtRString, NULL},
};

void file_menu_init(
  Widget	topl)
{
  Widget	form, file_type_menu, toggle;
  XmString	xmstr;
  Visual	*visual;
  Arg		arg[1];
  char		fileStr[128], *tmpStr;
  FILE 		*fp;
  WlzEffFormat	image_type=WLZEFF_FORMAT_WLZ;

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
  read_obj_dialog = XmCreateFileSelectionDialog( topl,
						"read_obj_dialog", arg, 1);

  /* add a form to include file type and fill-blanks toggle */
  form = XtVaCreateManagedWidget("read_file_form", xmFormWidgetClass,
				 read_obj_dialog,
				 XmNborderWidth,	0,
				 NULL);

  /* add a file type selection menu */
  file_type_menu = HGU_XmBuildMenu(form, XmMENU_OPTION,
				   "file_type", 0, XmTEAR_OFF_DISABLED,
				   file_type_menu_itemsP);
  XtVaSetValues(file_type_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild( file_type_menu );

  /* add a fill-blanks toggles */
  toggle = XtVaCreateManagedWidget("fill_blanks", xmToggleButtonGadgetClass,
				   form,
				   XmNindicatorOn, 	True,
				   XmNindicatorType,	XmN_OF_MANY,
				   XmNset,		False,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	file_type_menu,
				   XmNleftAttachment,	XmATTACH_FORM,
				   NULL);

  toggle = XtVaCreateManagedWidget("min_domain", xmToggleButtonGadgetClass,
				   form,
				   XmNindicatorOn, 	True,
				   XmNindicatorType,	XmN_OF_MANY,
				   XmNset,		True,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	file_type_menu,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	toggle,
				   NULL);

  /* add the callbacks */
  image_type_cb( read_obj_dialog, (XtPointer) WLZEFF_FORMAT_WLZ,
		NULL);
  XtAddCallback( read_obj_dialog, XmNcancelCallback, 
		PopdownCallback, NULL);
  XtAddCallback(read_obj_dialog, XmNmapCallback,
		FSBPopupCallback, NULL);
  XtManageChild( read_obj_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( read_obj_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* create the write-obj file selection dialog */
  write_obj_dialog = XmCreateFileSelectionDialog( topl,
						 "write_obj_dialog",
						 arg, 1);

  /* add a file type selection menu */
  file_type_menu = HGU_XmBuildMenu(write_obj_dialog, XmMENU_OPTION,
				   "file_type", 0, XmTEAR_OFF_DISABLED,
				   write_file_type_menu_itemsP);
  XtManageChild( file_type_menu );

  /* add the callbacks */
  write_image_type_cb( write_obj_dialog, (XtPointer) WLZEFF_FORMAT_WLZ,
		      NULL);
  XtAddCallback( write_obj_dialog, XmNcancelCallback, 
		PopdownCallback, NULL);
  XtAddCallback(write_obj_dialog, XmNmapCallback,
		FSBPopupCallback, NULL);
  XtManageChild( write_obj_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( write_obj_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* create the object properties dialog */
  obj_props_dialog_init( topl );

  globals.file      = NULL;
  globals.obj       = NULL;
  globals.orig_obj  = NULL;
  globals.fb_obj    = NULL;

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
      time_t		tmpTime;
      char		timeBuf[16];
      struct hostent 	*hstnt;
      fprintf(stderr, "MAPaint: logging enabled to %s\n", globals.logfile);
      MAPaintLogData("User", getenv("USER"), 0, NULL);
#ifdef LINUX2
      strcpy(timeBuf, "00/00/00");
#else
      tmpTime = time(NULL);
      cftime(timeBuf, "%d/%m/%Y", &tmpTime);
#endif /* LINUX2 */
      MAPaintLogData("Date", timeBuf, 0, NULL);
#ifdef LINUX2
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
  if( globals.base_theiler_dir ){
    globals.base_theiler_dir = strdup( globals.base_theiler_dir );
  }
  if( globals.theiler_stage ){
    char *tStr;
    if( tStr = theilerString(globals.theiler_stage) ){
      globals.theiler_stage = strdup(tStr);
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
      if( obj = WlzReadObj( fp, NULL ) ){
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
			  "    The reference object must be a 2- or 3-D\n"
			  "    grey-level image. Please select an alternate\n"
			  "    object",
			  XmDIALOG_FULL_APPLICATION_MODAL);
	  WlzFreeObj( obj );
	  /* set hour glass cursor */
	  HGU_XmUnsetHourGlassCursor(globals.topl);
	  return;

	}
	ReferenceFileListPush(initial_reference_file, image_type);
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
    if( (fp = fopen(initial_domain_file, "r")) ){
      HGU_XmSetHourGlassCursor(topl);
      if( obj = WlzReadObj( fp, NULL ) ){
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

  /* initialise the reference file list */
  if( !globals.sectViewFlg ){
    sprintf(fileStr, "%s/%s", getenv("HOME"), ".mapaint");
    if( fp = fopen(fileStr, "r") ){
      char inBuf[128];
      int i=0, j;

      while( fgets(&(inBuf[0]), 127, fp) ){
	if( strncmp(inBuf, "Reference file list:", 20) ){
	  continue;
	}
	while((fgets(&(inBuf[0]), 127, fp)) &&
	      (strncmp(inBuf, "End file list", 13)) &&
	      (i < 10)){
	  refFileList[i] = strdup(inBuf);
	  for(j=0; j < strlen(refFileList[i]); j++){
	    if((refFileList[i][j] == ',') ||
	       (refFileList[i][j] == '\n')){
	      refFileList[i][j] = '\0';
	      break;
	    }
	  }
	  sscanf(&(inBuf[j]), ", %d", &(refImageTypeList[i]));
	  i++;
	}
	break;
      }
    }
  }

  return;
}
