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
*   File       :   MADomainSurgeryDialog.c				*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Thu Apr 30 14:48:34 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

Widget surgeryDialog=NULL;
static PaintActionType surgeryOption=MAPAINT_SURGERY_SECTION_CUT;
static DomainSelection sourceDomain=DOMAIN_1;
static WlzObject *surgeryObjs[2];
static WlzObject *flashingObj=NULL;
static int flashingObjFlag;
static XtIntervalId intervalId=0;

void domainSurgeryCb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  /* put up the domain surgery dialog */
  PopupCallback(w, (XtPointer) XtParent(surgeryDialog), NULL);

  return;
}

static void flashingObjTimeoutProc(
  XtPointer	client_data,
  XtIntervalId	*id)
{
  if( !flashingObj ){
    return;
  }

  if( flashingObjFlag ){
    if( globals.domain_display_list[sourceDomain] != 0 )
    {
      glDeleteLists(globals.domain_display_list[sourceDomain], 1);
      globals.domain_display_list[sourceDomain] = 0;
      MAOpenGLDrawScene( globals.canvas );
    }
    flashingObjFlag = 0;
  }
  else {
    MAOpenGLDisplayDomainIndex(flashingObj, sourceDomain);
    flashingObjFlag = 1;
  }

  intervalId = XtAppAddTimeOut(globals.app_con, 500,
			       flashingObjTimeoutProc, NULL);

  return;
}


void cutDomainSurgeryCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  WlzObject	*srcDomain;
  WlzObject	**rtnObjs;
  int		numObjs;

  /* check for a selected view and install the current view domains */
  if( !paint_key ){
    return;
  }
  installViewDomains(paint_key);

  /* get selected domain */
  if( (srcDomain = get_domain_from_object(globals.obj, sourceDomain)) == NULL ){
    return;
  }

  /* make the surgery domain application modal */
  PopdownCallback(w, (XtPointer) XtParent(surgeryDialog), NULL);
  XtVaSetValues(surgeryDialog,
		XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL,
		NULL);
  PopupCallback(w, (XtPointer) XtParent(surgeryDialog), NULL);
  
  /* if this domain is displayed in 3D then remove it */
  if( globals.domain_display_list[sourceDomain] != 0 )
  {
    glDeleteLists(globals.domain_display_list[sourceDomain], 1);
    globals.domain_display_list[sourceDomain] = 0;
    MAOpenGLDrawScene( globals.canvas );
  }

  /* cut the object and put them on the list */
  if( Wlz3DSectionSegmentObject(srcDomain, paint_key->wlzViewStr,
				&numObjs, &rtnObjs) == WLZ_ERR_NONE ){
    surgeryObjs[0] = rtnObjs[0];
    surgeryObjs[1] = rtnObjs[1];
    AlcFree((void *) rtnObjs);
  }
  else {
    surgeryObjs[0] = NULL;
    surgeryObjs[1] = NULL;
  }

  /* set the 3D display flashing */
  if( surgeryObjs[0] ){
    flashingObj = WlzAssignObject(surgeryObjs[0], NULL);
  }
  else if( surgeryObjs[1] ){
    flashingObj = WlzAssignObject(surgeryObjs[1], NULL);
  }
  else {
    flashingObj = NULL;
    return;
  }
  flashingObjFlag = 0;
  intervalId = XtAppAddTimeOut(globals.app_con, 500,
			       flashingObjTimeoutProc, NULL);

  return;
}

void surgeryDestDomainCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  DomainSelection	destDomain=(DomainSelection) client_data;
  int			col;
  WlzObject		*obj;

  /* remove the flashing timeout and displayed 3D object */
  if( intervalId ){
    XtRemoveTimeOut(intervalId);
    intervalId = 0;
  }
  if( flashingObj == NULL ){
    return;
  }
  if( flashingObjFlag ){
    if( globals.domain_display_list[sourceDomain] != 0 )
    {
      glDeleteLists(globals.domain_display_list[sourceDomain], 1);
      globals.domain_display_list[sourceDomain] = 0;
      MAOpenGLDrawScene( globals.canvas );
    }
    flashingObjFlag = 0;
  }

  /* add the flashing domain to the selected domain */
  col = globals.cmapstruct->ovly_cols[destDomain];
  switch( destDomain ){
  case MASK_DOMAIN:
    break;

  case GREYS_DOMAIN:
    if( destDomain > globals.cmapstruct->num_overlays )
    {
      set_grey_values_from_ref_object(flashingObj, globals.orig_obj);
    }
    else
    {
      set_grey_values_from_domain(flashingObj, globals.obj,
				  globals.cmapstruct->ovly_cols[0],
				  globals.cmapstruct->ovly_planes );
    }
    globals.domain_changed_since_saved[sourceDomain] = 1;
    break;

  default:
    if( destDomain > globals.cmapstruct->num_overlays )
    {
      set_grey_values_from_domain(flashingObj, globals.obj, col, 255);
    }
    else
    {
      set_grey_values_from_domain(flashingObj, globals.obj, col,
				  globals.cmapstruct->ovly_planes );
    }
    globals.domain_changed_since_saved[sourceDomain] = 1;
    globals.domain_changed_since_saved[destDomain] = 1;
    break;
  }

  /* update the view domains and undisplay the 3D view */
  display_view_cb(w, (XtPointer) paint_key, call_data);
  if( globals.domain_display_list[destDomain] != 0 )
  {
    glDeleteLists(globals.domain_display_list[destDomain], 1);
    globals.domain_display_list[destDomain] = 0;
    MAOpenGLDrawScene( globals.canvas );
  }

  /* remove the domain from top of the list reset for the remaining domains */
  WlzFreeObj(flashingObj);
  if( surgeryObjs[0] ){
    WlzFreeObj(surgeryObjs[0]);
    surgeryObjs[0] = NULL;
    flashingObj = WlzAssignObject(surgeryObjs[1], NULL);
  }
  else if( surgeryObjs[1] ){
    WlzFreeObj(surgeryObjs[1]);
    surgeryObjs[1] = NULL;
    flashingObj = NULL;
  }
  else {
    flashingObj = NULL;
  }

  if( flashingObj ){
    flashingObjFlag = 0;
    intervalId = XtAppAddTimeOut(globals.app_con, 500,
				 flashingObjTimeoutProc, NULL);
  }
  else {
    /* remove modality */
    PopdownCallback(w, (XtPointer) XtParent(surgeryDialog), NULL);
    XtVaSetValues(surgeryDialog,
		  XmNdialogStyle,	0,
		  NULL);
    PopupCallback(w, (XtPointer) XtParent(surgeryDialog), NULL);
  }

  return;
}

void surgeryOptionCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  surgeryOption = (PaintActionType) client_data;
  return;
}

static void sourceDomainCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  sourceDomain = (DomainSelection) client_data;
  return;
}

static MenuItem surgery_option_items[] = {	/* surgery options items */
  {"section cut", &xmPushButtonGadgetClass, 0, NULL, NULL, True,
   surgeryOptionCb, (XtPointer) MAPAINT_SURGERY_SECTION_CUT,
   HGU_XmHelpStandardCb, "",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"laser cut", &xmPushButtonGadgetClass, 0, NULL, NULL, True,
   surgeryOptionCb, (XtPointer) MAPAINT_SURGERY_LASER_CUT,
   HGU_XmHelpStandardCb, "",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static ActionAreaItem   surgery_dialog_actions[] = {
{"Cut",		cutDomainSurgeryCb,		NULL},
{"Stop",	NULL,		NULL},
{"Dismiss",     NULL,           NULL},
{"Help",        NULL,           NULL},
};

Widget createDomainSelectOptionMenu(
  String		name,
  Widget		parent,
  XtCallbackProc	callbackproc)
{
  Widget	option_menu, widget;
  MenuItem	*items;
  int		i, col;
  char		*c;
  String	str;
  int		num_overlays;

  num_overlays = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;

  items = (MenuItem *) AlcCalloc(num_overlays+1,sizeof(MenuItem));

  for(i=0; i < num_overlays; i++){
    items[i].name               = (String)
      AlcMalloc(strlen(globals.domain_name[i+1]) + 1);
    strcpy(items[i].name, globals.domain_name[i+1]);
    for(c=items[i].name; (*c) != '\0'; c++){
      if( (*c) == '.' ){
	*c = '_';
      }
    }
    items[i].wclass             = &xmPushButtonWidgetClass;
    items[i].mnemonic           = 0;
    items[i].accelerator        = NULL;
    items[i].accel_text         = NULL;
    items[i].item_set           = False;
    items[i].callback           = callbackproc;
    items[i].callback_data      = (XtPointer) (i+1);
    items[i].help_callback      = NULL;
    items[i].help_callback_data = NULL;
    items[i].tear_off_model     = XmTEAR_OFF_DISABLED;
    items[i].radio_behavior     = False;
    items[i].always_one         = False;
    items[i].subitems           = NULL;
  }

  option_menu = HGU_XmBuildMenu( parent, XmMENU_OPTION, name, 0,
				XmTEAR_OFF_DISABLED, items);

  for(i=1; i <= num_overlays; i++){
    if( i <= globals.cmapstruct->num_overlays )
    {
      col = globals.cmapstruct->ovly_cols[i] + globals.cmapstruct->gmax -
	globals.cmapstruct->gmin;
    }
    else
    {
      col = globals.cmapstruct->ovly_cols[i];
    }
    str = (String) AlcMalloc(strlen(items[i-1].name) + 4);
    (void) sprintf(str, "*%s", items[i-1].name);
    if( widget = XtNameToWidget(option_menu, str) ){
      XmChangeColor(widget, col);
    }
    AlcFree( str );
  }

  for(i=0; i < num_overlays; i++){
    AlcFree(items[i].name);
  }
  AlcFree( items );
  return( option_menu );
}

Widget createDomainSurgeryDialog(
  Widget	topl)
{
  Widget	dialog, control, form, frame, title;
  Widget	rowcolumn, button, option, widget;
  int		i;

  dialog = HGU_XmCreateStdDialog(topl, "surgery_dialog", xmFormWidgetClass,
				 surgery_dialog_actions, 4);

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  control = XtNameToWidget( dialog, "*.control" );

  /* create the buttons for the source domain */
  frame = XtVaCreateManagedWidget("src_domain_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  NULL);
  title = XtVaCreateManagedWidget("src_domain_title", xmLabelWidgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  form = XtVaCreateManagedWidget("src_domain_form", xmFormWidgetClass,
				 frame, NULL);

  option = createDomainSelectOptionMenu("src_domain", form, sourceDomainCb);
  XtManageChild(option);
  XtVaSetValues(option,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		NULL);

  /* option for the cut type */
  widget = HGU_XmBuildMenu(form, XmMENU_OPTION, "surgery_option", 0,
			   XmTEAR_OFF_DISABLED, surgery_option_items);
  XtManageChild(widget);
  XtVaSetValues(widget,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, option,
		NULL);
  

  /* create the select buttons for the destination domain */
  frame = XtVaCreateManagedWidget("dest_domain_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget,	frame,
				  XmNleftAttachment, XmATTACH_FORM,
				  NULL);
  title = XtVaCreateManagedWidget("dest_domain_title", xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  
  rowcolumn = XtVaCreateManagedWidget("dest_domain_rc", xmRowColumnWidgetClass,
				      frame,
				      XmNorientation, XmVERTICAL,
				      XmNnumColumns,	5,
				      XmNpacking,	XmPACK_COLUMN,
				      NULL);
  widget = XtVaCreateManagedWidget("Discard", xmPushButtonWidgetClass,
				   rowcolumn, NULL);
  XtAddCallback(widget, XmNactivateCallback, surgeryDestDomainCb,
		(XtPointer) MASK_DOMAIN);

  widget = XtVaCreateManagedWidget("Greys", xmPushButtonWidgetClass,
				   rowcolumn, NULL);
  XtAddCallback(widget, XmNactivateCallback, surgeryDestDomainCb,
		(XtPointer) GREYS_DOMAIN);

  for(i=1; i < 33; i++){
    Pixel	pixel;
    String	domain_str;;

    if( i > (globals.cmapstruct->num_overlays +
	     globals.cmapstruct->num_solid_overlays) )
    {
      continue;
    }

    domain_str = (String) AlcMalloc(strlen(globals.domain_name[i])+2);
    (void) strcpy(domain_str, globals.domain_name[i]);
    widget = XtVaCreateManagedWidget(domain_str,
				     xmPushButtonWidgetClass,
				     rowcolumn, NULL);
    AlcFree((void *) domain_str);
    XtAddCallback(widget, XmNactivateCallback, surgeryDestDomainCb,
		  (XtPointer) i);
    pixel = globals.cmapstruct->ovly_cols[i] +
      globals.cmapstruct->gmax - globals.cmapstruct->gmin;
    if( i > globals.cmapstruct->num_overlays )
    {
      pixel = globals.cmapstruct->ovly_cols[i];
    }
    XmChangeColor(widget, pixel);
  }

  /* initialise the object list */
  surgeryObjs[0] = NULL;
  surgeryObjs[1] = NULL;

  return( dialog );
}
