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

static Widget	new_obj_dialog, read_obj_dialog;
static Widget	write_obj_dialog;
static char 	*refFileList[]={NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL};

/* reference file list manipulation procedures */
void ReferenceFileListPush(
  char *fileStr)
{
  int	i;

  if( fileStr ){
    for(i=9; i > 0; i--){
      refFileList[i] = refFileList[i-1];
    }
    refFileList[0] = strdup(fileStr);
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
      fprintf(fp, "%s\n", refFileList[i]);
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
  FILE	*fp;

  if( refFileList[fileIndex] ){
    WlzObject 	*obj;

    /* open the reference object file and install */
    if( (fp = fopen(refFileList[fileIndex], "r")) ){
      HGU_XmSetHourGlassCursor(globals.topl);
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
	  ReferenceFileListPush(refFileList[fileIndex]);
	}
	install_paint_reference_object( obj );
      }
      (void) fclose( fp );

      /* set the title of the top-level window */
      set_topl_title(refFileList[fileIndex]);
      globals.file = refFileList[fileIndex];

      HGU_XmUnsetHourGlassCursor(globals.topl);
    }
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
    XtManageChild( read_obj_dialog );
    XtPopup( XtParent(read_obj_dialog), XtGrabNone );
    return;
}

void write_obj_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
    XtManageChild( write_obj_dialog );
    XtPopup( XtParent(write_obj_dialog), XtGrabNone );
    return;
}

static XmStringCharSet charset = XmSTRING_DEFAULT_CHARSET;

void setup_ref_display_list_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{

  WlzPlaneDomain	*planedmn;

  if( globals.obj == NULL || globals.obj->type != WLZ_3D_DOMAINOBJ ||
     globals.ref_display_list == 0){
    return;
  }

  planedmn = globals.obj->domain.p;

  /* create the reference object 3D display  DisplayList */
  glNewList( globals.ref_display_list, GL_COMPILE );

  glBegin(GL_LINES);
  glIndexi( HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 0.0, 0.0) );
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->lastpl);

  glIndexi( HGU_XGetColorPixel(globals.dpy, globals.cmap, 0.0, 1.0, 0.0) );
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->kol1, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->kol1, planedmn->lastln, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->plane1);
  glVertex3i(planedmn->lastkl, planedmn->line1, planedmn->lastpl);
  glVertex3i(planedmn->lastkl, planedmn->lastln, planedmn->lastpl);

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
    WlzPlaneDomain	*planedmn;
    WlzValues		values;
    int			z;

    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 0.0));

    planedmn = globals.fb_obj->domain.p;
    for(z=planedmn->plane1; z <= planedmn->lastpl; z += 10)
    {
      values.core = NULL;
      obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			 planedmn->domains[z - planedmn->plane1],
			 values, NULL, NULL, NULL);
      if( obj1->domain.core != NULL )
      {
	boundobj = WlzObjToBoundary(obj1, 1, NULL);
	if( boundobj != NULL )
	{
	  MAOpenGLDisplayBoundList(boundobj->domain.b, (float) z );
	  WlzFreeObj( boundobj );
	}
      }
      WlzFreeObj( obj1 );
    }
  }

  glIndexi( HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 1.0) );
  glEndList();

  WLZ_VTX_3_SET(globals.bbox_vtx, planedmn->kol1 - 2.0,
		planedmn->line1 - 2.0, planedmn->plane1 - 2.0);
  WLZ_VTX_3_SET(globals.bbox_size,
		planedmn->lastkl - planedmn->kol1 + 4.0,
		planedmn->lastln - planedmn->line1 + 4.0,
		planedmn->lastpl - planedmn->plane1 + 4.0);

  MAOpenGLDrawScene( globals.canvas );
  return;
}

void install_paint_reference_object(
  WlzObject	*obj)
{
  Widget		toggle;
  WlzPixelV		min, max, Min, Max;

  if( globals.orig_obj != NULL )
    WlzFreeObj( globals.orig_obj );
  globals.orig_obj = WlzAssignObject(obj, NULL);

  /* check input object for grey-value type
     convert if necessary */

  /* copy for later transform for display purposes,
     the original is kept for IP purposes note pixconvert loses
     the background value */
  if( globals.obj != NULL ){
    WlzFreeObj( globals.obj );
  }
  globals.obj = WlzAssignObject(WlzConvertPix(globals.orig_obj,
					      WLZ_GREY_UBYTE, NULL),
				NULL);
  min.type = WLZ_GREY_UBYTE;
  max.type = WLZ_GREY_UBYTE;
  Min.type = WLZ_GREY_UBYTE;
  Max.type = WLZ_GREY_UBYTE;
  min.v.ubv = 0;
  max.v.ubv = 255;
  Min.v.ubv = globals.cmapstruct->gmin;
  Max.v.ubv = globals.cmapstruct->gmax;
  WlzGreySetRange(globals.obj, min, max, Min, Max);

  /* also convert the background values */
  min = WlzGetBackground(globals.orig_obj, NULL);
  WlzValueConvertPixel(&min, min, WLZ_GREY_INT);
  max.type = WLZ_GREY_INT;
  max.v.inv = ((min.v.inv * (Max.v.ubv - Min.v.ubv)) / 255) + Min.v.ubv;
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

  return;
}

void set_topl_title(
  String	name)
{
  String	name_return, class_return;
  char		str_buff[64];
  int		i, basename_start;

  /* get the top level dialog and application name */
  XtGetApplicationNameAndClass(XtDisplay(globals.topl),
			       &name_return, &class_return);
  /* get file basename */
  for(i=0, basename_start=0; i < strlen(name); i++){
    if( name[i] == '/' ){
      basename_start = i+1;
    }
  }
  (void) sprintf(str_buff, "%s: %s", name_return, name+basename_start);
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
    WlzEffFormat	image_type = (WlzEffFormat) client_data;
    WlzObject		*obj;
    FILE		*fp;
    String		icsfile;

    /* get the file pointer or file name if ics format */
    if( image_type == WLZEFF_FORMAT_ICS )
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

    if( icsfile = HGU_XmGetFileStr(globals.topl, cbs->value, cbs->dir) )
    {
      ReferenceFileListPush(icsfile);
      AlcFree((void *) icsfile);
    }
    install_paint_reference_object( obj );

    /* set the title of the top-level car */
    if( XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &icsfile) ){
       set_topl_title(icsfile);
       globals.file = icsfile;
    }

    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
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
    if( image_type == WLZEFF_FORMAT_ICS )
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
    WlzEffWriteObj(fp, icsfile, globals.orig_obj, image_type);

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
  }

  XtAddCallback(read_obj_dialog, XmNokCallback, setup_ref_display_list_cb,
		NULL);
  XtAddCallback(read_obj_dialog, XmNokCallback, setup_obj_props_cb,
		NULL);
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
	    sprintf(strBuf, "%d/ %.4s .../%s", i, refFileList[i-1],
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
};

void file_menu_init(
  Widget	topl)
{
  Widget	form, file_type_menu, toggle;
  XmString	xmstr;
  Visual	*visual;
  Arg		arg[1];
  char		fileStr[128];
  FILE 		*fp;

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(topl);
  XtSetArg(arg[0], XmNvisual, visual);

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

  /* setup the theiler directory and menu item */
  XtGetApplicationResources(topl, &globals, set_att_res,
			    XtNumber(set_att_res), NULL, 0);
  theiler_menu_init( topl );

  /* check for an initial reference file */
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
	ReferenceFileListPush(initial_reference_file);
	install_paint_reference_object( obj );
      }
      (void) fclose( fp );

      /* set the title of the top-level window */
      set_topl_title(initial_reference_file);
      globals.file = initial_reference_file;

      HGU_XmUnsetHourGlassCursor(topl);
    }
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
	globals.fb_obj = obj;
	setup_ref_display_list_cb(read_obj_dialog, NULL, NULL);
      }
      (void) fclose( fp );
      HGU_XmUnsetHourGlassCursor(topl);
    }
  }

  /* initialise the reference file list */
  sprintf(fileStr, "%s/%s", getenv("HOME"), ".mapaint");
  if( fp = fopen(fileStr, "r") ){
    char inBuf[128];
    int i=0;

    while( fgets(&(inBuf[0]), 127, fp) ){
      if( strncmp(inBuf, "Reference file list:", 20) ){
	continue;
      }
      while( fgets(&(inBuf[0]), 127, fp) &&
	     strncmp(inBuf, "End file list", 13) ){
	inBuf[strlen(inBuf)-1] = '\0';
	refFileList[i] = strdup(inBuf);
	i++;
      }
      break;
    }
  }

  return;
}
