#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	domain_menu.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <MAPaint.h>

static void image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

/* menu item structures */

static MenuItem file_type_menu_itemsP[] = {	/* file_menu items */
  {"woolz", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_WLZ,
   HGU_XmHelpStandardCb, SEC_READ_WRITE_DOMAINS_DIALOGS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"ics", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_ICS,
   HGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"den", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_DEN,
   HGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_VFF,
   HGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pic", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_PIC,
   HGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vtk", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_VTK,
   HGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"slc", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_SLC,
   HGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pgm", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_PNM,
   HGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem select_menu_itemsP[] = {	/* select menu items */
{"domain_1", &xmToggleButtonWidgetClass, 0, NULL, NULL, True,
     select_domain_cb, (XtPointer) DOMAIN_1,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_2", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_2,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_3", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_3,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_4", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_4,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_5", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_5,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_6", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_6,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_7", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_7,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_8", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_8,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_9", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_9,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_10", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_10,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_11", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_11,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_12", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_12,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_13", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_13,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_14", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_14,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_15", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_15,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_16", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_16,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_17", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_17,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_18", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_18,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_19", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_19,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_20", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_20,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_21", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_21,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_22", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_22,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_23", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_23,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_24", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_24,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_25", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_25,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_26", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_26,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_27", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_27,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_28", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_28,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_29", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_29,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_30", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_30,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_31", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_31,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_32", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_32,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_mask", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) MASK_DOMAIN,
     HGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
NULL,
};

static MenuItem domain_menu_itemsP[] = {		/* file_menu items */
{"select", &xmCascadeButtonGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, HGU_XmHelpStandardCb, SEC_DOMAIN_MENU,
     XmTEAR_OFF_ENABLED, True, True, &(select_menu_itemsP[0])},
{"controls", &xmPushButtonGadgetClass, 0,  NULL, NULL, False,
     domain_cb, NULL, HGU_XmHelpStandardCb, "paint/paint.html#domain_controls",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"auto_increment", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
     int_toggle_cb, (caddr_t) &(globals.auto_increment),
     HGU_XmHelpStandardCb, "paint/paint.html#auto_increment",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"propogate", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
     int_toggle_cb, (caddr_t) &(globals.propogate),
     HGU_XmHelpStandardCb, "paint/paint.html#propogate",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"read_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, HGU_XmHelpStandardCb, "paint/paint.html#read_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
/*{"save_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     save_domain_cb, NULL, HGU_XmHelpStandardCb, "paint/paint.html#quick_save",
     XmTEAR_OFF_DISABLED, False, False, NULL},*/
{"write_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, HGU_XmHelpStandardCb, "paint/paint.html#write_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"clear_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     clear_domain_cb, NULL, HGU_XmHelpStandardCb,
     "paint/paint.html#clear_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"threed_display_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     threed_display_domain_cb, NULL, HGU_XmHelpStandardCb,
     "paint/paint.html#threed_display_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"clear_all_domains", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     clear_all_domains_cb, NULL, HGU_XmHelpStandardCb,
     "paint/paint.html#clear_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"write_paint_volume", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
NULL,
};

MenuItem	*domain_menu_items = &(domain_menu_itemsP[0]);
MenuItem	*select_menu_items = &(select_menu_itemsP[0]);
static Widget	read_domain_dialog, write_domain_dialog,
  write_paint_volume_dialog;
static Widget	domain_controls_dialog;

#define res_offset(field)   XtOffsetOf(PaintGlobals, field)

static XtResource domain_res[] = {
{"domain_name_0", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[0]), XtRString, "greys"},
{"domain_name_1", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[1]), XtRString, "domain_1.wlz"},
{"domain_name_2", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[2]), XtRString, "domain_2.wlz"},
{"domain_name_3", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[3]), XtRString, "domain_3.wlz"},
{"domain_name_4", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[4]), XtRString, "domain_4.wlz"},
{"domain_name_5", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[5]), XtRString, "domain_5.wlz"},
{"domain_name_6", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[6]), XtRString, "domain_6.wlz"},
{"domain_name_7", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[7]), XtRString, "domain_7.wlz"},
{"domain_name_8", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[8]), XtRString, "domain_8.wlz"},
{"domain_name_9", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[9]), XtRString, "domain_9.wlz"},
{"domain_name_10", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[10]), XtRString, "domain_10.wlz"},
{"domain_name_11", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[11]), XtRString, "domain_11.wlz"},
{"domain_name_12", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[12]), XtRString, "domain_12.wlz"},
{"domain_name_13", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[13]), XtRString, "domain_13.wlz"},
{"domain_name_14", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[14]), XtRString, "domain_14.wlz"},
{"domain_name_15", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[15]), XtRString, "domain_15.wlz"},
{"domain_name_16", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[16]), XtRString, "domain_16.wlz"},
{"domain_name_17", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[17]), XtRString, "domain_17.wlz"},
{"domain_name_18", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[18]), XtRString, "domain_18.wlz"},
{"domain_name_19", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[19]), XtRString, "domain_19.wlz"},
{"domain_name_20", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[20]), XtRString, "domain_20.wlz"},
{"domain_name_21", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[21]), XtRString, "domain_21.wlz"},
{"domain_name_22", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[22]), XtRString, "domain_22.wlz"},
{"domain_name_23", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[23]), XtRString, "domain_23.wlz"},
{"domain_name_24", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[24]), XtRString, "domain_24.wlz"},
{"domain_name_25", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[25]), XtRString, "domain_25.wlz"},
{"domain_name_26", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[26]), XtRString, "domain_26.wlz"},
{"domain_name_27", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[27]), XtRString, "domain_27.wlz"},
{"domain_name_28", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[28]), XtRString, "domain_28.wlz"},
{"domain_name_29", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[29]), XtRString, "domain_29.wlz"},
{"domain_name_30", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[30]), XtRString, "domain_30.wlz"},
{"domain_name_31", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[31]), XtRString, "domain_31.wlz"},
{"domain_name_32", "DomainName", XtRString, sizeof(String),
     res_offset(domain_name[32]), XtRString, "domain_32.wlz"},
{"domain_filename_1", "DomainFilename", XtRString, sizeof(String),
     res_offset(domain_filename[1]), XtRString, "domain_1.wlz"},
{"domain_filename_2", "DomainFilename", XtRString, sizeof(String),
     res_offset(domain_filename[2]), XtRString, "domain_2.wlz"},
{"domain_filename_3", "DomainFilename", XtRString, sizeof(String),
     res_offset(domain_filename[3]), XtRString, "domain_3.wlz"},
{"domain_filename_4", "DomainFilename", XtRString, sizeof(String),
     res_offset(domain_filename[4]), XtRString, "domain_4.wlz"},
{"domain_filename_5", "DomainFilename", XtRString, sizeof(String),
     res_offset(domain_filename[5]), XtRString, "domain_5.wlz"},
{"domain_filename_6", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[6]), XtRString, "domain_6.wlz"},
{"domain_filename_7", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[7]), XtRString, "domain_7.wlz"},
{"domain_filename_8", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[8]), XtRString, "domain_8.wlz"},
{"domain_filename_9", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[9]), XtRString, "domain_9.wlz"},
{"domain_filename_10", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[10]), XtRString, "domain_10.wlz"},
{"domain_filename_11", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[11]), XtRString, "domain_11.wlz"},
{"domain_filename_12", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[12]), XtRString, "domain_12.wlz"},
{"domain_filename_13", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[13]), XtRString, "domain_13.wlz"},
{"domain_filename_14", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[14]), XtRString, "domain_14.wlz"},
{"domain_filename_15", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[15]), XtRString, "domain_15.wlz"},
{"domain_filename_16", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[16]), XtRString, "domain_16.wlz"},
{"domain_filename_17", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[17]), XtRString, "domain_17.wlz"},
{"domain_filename_18", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[18]), XtRString, "domain_18.wlz"},
{"domain_filename_19", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[19]), XtRString, "domain_19.wlz"},
{"domain_filename_20", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[20]), XtRString, "domain_20.wlz"},
{"domain_filename_21", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[21]), XtRString, "domain_21.wlz"},
{"domain_filename_22", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[22]), XtRString, "domain_22.wlz"},
{"domain_filename_23", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[23]), XtRString, "domain_23.wlz"},
{"domain_filename_24", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[24]), XtRString, "domain_24.wlz"},
{"domain_filename_25", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[25]), XtRString, "domain_25.wlz"},
{"domain_filename_26", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[26]), XtRString, "domain_26.wlz"},
{"domain_filename_27", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[27]), XtRString, "domain_27.wlz"},
{"domain_filename_28", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[28]), XtRString, "domain_28.wlz"},
{"domain_filename_29", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[29]), XtRString, "domain_29.wlz"},
{"domain_filename_30", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[30]), XtRString, "domain_30.wlz"},
{"domain_filename_31", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[31]), XtRString, "domain_31.wlz"},
{"domain_filename_32", "DomainFileName", XtRString, sizeof(String),
     res_offset(domain_filename[32]), XtRString, "domain_32.wlz"},
{"OpenGLDisplayDomainStep", "OpenGLDisplayDomainStep", XtRInt, sizeof(int),
     res_offset(OpenGLDisplayDomainStep), XtRImmediate, (caddr_t) 2},
};

/* action and callback procedures */
void addSelectDomainCallback(
  XtCallbackProc	callback,
  XtPointer		clientData)
{
  Widget	toggle, cascadeB;
  int		i;
  String	strBuf;

  /* get the menu widget */
  cascadeB = XtNameToWidget(globals.topl,
			    "*menubar*domain_menu*select");

  /* get each toggle widget and add the callback */
  for(i=0; i < 32; i++){
    strBuf = (String) AlcMalloc(strlen(select_menu_itemsP[i].name)
				+ 4);
    sprintf(strBuf, "*%s", select_menu_itemsP[i].name);
    if( toggle = XtNameToWidget(cascadeB, strBuf) ){
      XtAddCallback(toggle, XmNvalueChangedCallback, callback, clientData);
    }
    AlcFree((void *) strBuf);
  }
  
  return;
}

/* action and callback procedures */
void removeSelectDomainCallback(
  XtCallbackProc	callback,
  XtPointer		clientData)
{
  Widget	toggle, cascadeB;
  int		i;
  String	strBuf;

  /* get the menu widget */
  cascadeB = XtNameToWidget(globals.topl,
			    "*menubar*domain_menu*select");

  /* get each toggle widget and add the callback */
  for(i=0; i < 32; i++){
    strBuf = (String) AlcMalloc(strlen(select_menu_itemsP[i].name)
				+ 4);
    sprintf(strBuf, "*%s", select_menu_itemsP[i].name);
    if( toggle = XtNameToWidget(cascadeB, strBuf) ){
      XtRemoveCallback(toggle, XmNvalueChangedCallback, callback, clientData);
    }
    AlcFree((void *) strBuf);
  }
  
  return;
}

void selectNextDomain(int downFlag)
{
  int		currIndex, newIndex, numDomains;
  Widget	toggle, cascadeB;
  XmToggleButtonCallbackStruct	cbs;
  String	strBuf;

  /* run throught the domains menu to find the names of
     the currently selected toggle and the new toggle */
  numDomains = 0;
  currIndex = 0;
  while( select_menu_itemsP[numDomains].name ){
    if( (PaintActionType) select_menu_itemsP[numDomains].callback_data
       == globals.current_domain ){
      currIndex = numDomains;
    }
    numDomains++;
  }
  numDomains = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;
  newIndex = currIndex + ((downFlag) ? (1) : (-1));
  if( newIndex < 0 ){
    newIndex += numDomains;
  }
  newIndex = newIndex % numDomains;

  /* get the menu and set/unset the toggles and reset the
     selected widget ID */
  cascadeB = XtNameToWidget(globals.topl,
			    "*menubar*domain_menu*select");

  /* unset the current */
  strBuf = (String) AlcMalloc(strlen(select_menu_itemsP[currIndex].name)
			      + 4);
  sprintf(strBuf, "*%s", select_menu_itemsP[currIndex].name);
  toggle = XtNameToWidget(cascadeB, strBuf);
  XtVaSetValues(toggle, XmNset, False, NULL);
  AlcFree((void *) strBuf);
  
  /* set the new */
  strBuf = (String) AlcMalloc(strlen(select_menu_itemsP[newIndex].name)
			      + 4);
  sprintf(strBuf, "*%s", select_menu_itemsP[newIndex].name);
  toggle = XtNameToWidget(cascadeB, strBuf);
  XtVaSetValues(toggle, XmNset, True, NULL);
  AlcFree((void *) strBuf);
  
  /* set the menu history */
  XtVaSetValues(XtParent(toggle), XmNmenuHistory, toggle, NULL);

  /* call the callbacks */
  cbs.reason = XmCR_VALUE_CHANGED;
  cbs.event = NULL;
  cbs.set = True;
  XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
  
  return;
}

void select_domain_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    XmToggleButtonCallbackStruct *cbs =
	(XmToggleButtonCallbackStruct *) call_data;
    int		new_domain;

    if( !cbs->set )
	return;

    globals.current_priority = (PrioritySelection) client_data;
    new_domain =
      globals.priority_to_domain_lut[globals.current_priority];

    /* check for domain change */
    if( new_domain != globals.current_domain )
    {
      globals.current_domain = new_domain;
    }

    /* check which domain is selected */
    switch( globals.current_domain ){
    case DOMAIN_1:
    case DOMAIN_2:
    case DOMAIN_3:
    case DOMAIN_4:
    case DOMAIN_5:
    case DOMAIN_6:
    case DOMAIN_7:
    case DOMAIN_8:
    case DOMAIN_9:
    case DOMAIN_10:
    case DOMAIN_11:
    case DOMAIN_12:
    case DOMAIN_13:
    case DOMAIN_14:
    case DOMAIN_15:
    case DOMAIN_16:
    case DOMAIN_17:
    case DOMAIN_18:
    case DOMAIN_19:
    case DOMAIN_20:
    case DOMAIN_21:
    case DOMAIN_22:
    case DOMAIN_23:
    case DOMAIN_24:
    case DOMAIN_25:
    case DOMAIN_26:
    case DOMAIN_27:
    case DOMAIN_28:
    case DOMAIN_29:
    case DOMAIN_30:
    case DOMAIN_31:
    case DOMAIN_32:
	globals.current_col =
	    globals.cmapstruct->ovly_cols[globals.current_domain];
	break;
    case MASK_DOMAIN:
    case GREYS_DOMAIN:
    default:
	break;
    }

    /* if painting then reset highlight colour */
    if( paint_key ){
      Pixel	pixel;
      Widget	widget;

      if( globals.current_domain > globals.cmapstruct->num_overlays ){
	pixel = globals.cmapstruct->ovly_cols[globals.current_domain];
      }
      else {
	pixel = globals.cmapstruct->ovly_cols[globals.current_domain] +
	  globals.cmapstruct->gmax - globals.cmapstruct->gmin;
      }
      widget = XtNameToWidget(paint_key->dialog, "*.scrolled_window");
      XtVaSetValues(widget, XmNborderColor, pixel, NULL);
    }
      
    return;
}

void domain_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
  /* put up the domain controls dialog */
  PopupCallback(w, (XtPointer) XtParent(domain_controls_dialog), NULL);

  return;
}

void int_toggle_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    XmToggleButtonCallbackStruct *cbs =
	(XmToggleButtonCallbackStruct *) call_data;

    int		*val = (int *) client_data;

    *val = cbs->set;

    return;
}

void set_domain_menu_entry(
    DomainSelection	domain,
    String		name_str)
{
    Widget	widget = NULL;
    XmString	name;
    int		i, num_overlays;
    char	str_buf[64];

    if( name_str == NULL ){
	return;
    }

    num_overlays = globals.cmapstruct->num_overlays +
      globals.cmapstruct->num_solid_overlays;

    /* get the corresponding priority and widget */
    for(i=1; i <= num_overlays; i++)
    {
      if( globals.priority_to_domain_lut[i] == domain )
      {
	sprintf(str_buf, "*menubar*domain_menu*select*domain_%d", i);
	widget = XtNameToWidget(globals.topl, str_buf);
	break;
      }
    }

    if( widget == NULL )
	return;

    /* strip directory element and set new name */
    for(i = strlen(name_str)-1; i >= 0; i--){
	if( name_str[i] == '/' ){
	    break;
	}
    }

    name = XmStringCreateSimple( name_str+i+1 );
    XtVaSetValues(widget, XmNlabelString, name, NULL);
    XmStringFree( name );

    /* copy into the stored domain names - lose some memory here possibly */
    globals.domain_name[domain] = (String) malloc(sizeof(char) *
						  (strlen(name_str+i+1) + 1));
    (void) strcpy(globals.domain_name[domain], name_str+i+1);

    return;
}

int setDomain(
  WlzObject		*obj,
  DomainSelection	domain,
  String		name_str)
{
  unsigned int	col;

  /* check object */
  if( (obj == NULL) || (obj->type != WLZ_3D_DOMAINOBJ) ){
    return 1;
  }

  /* check which domain is selected */
  switch( domain ){
   case DOMAIN_1:
   case DOMAIN_2:
   case DOMAIN_3:
   case DOMAIN_4:
   case DOMAIN_5:
   case DOMAIN_6:
   case DOMAIN_7:
   case DOMAIN_8:
   case DOMAIN_9:
   case DOMAIN_10:
   case DOMAIN_11:
   case DOMAIN_12:
   case DOMAIN_13:
   case DOMAIN_14:
   case DOMAIN_15:
   case DOMAIN_16:
   case DOMAIN_17:
   case DOMAIN_18:
   case DOMAIN_19:
   case DOMAIN_20:
   case DOMAIN_21:
   case DOMAIN_22:
   case DOMAIN_23:
   case DOMAIN_24:
   case DOMAIN_25:
   case DOMAIN_26:
   case DOMAIN_27:
   case DOMAIN_28:
   case DOMAIN_29:
   case DOMAIN_30:
   case DOMAIN_31:
   case DOMAIN_32:
     col = globals.cmapstruct->ovly_cols[domain];
     if( domain > globals.cmapstruct->num_overlays )
     {
       set_grey_values_from_domain( obj, globals.obj, col, 255);
     }
     else
     {
       set_grey_values_from_domain( obj, globals.obj, col,
				    globals.cmapstruct->ovly_planes );
     }

     /* reset the menu-entry label */
     if( name_str ){
       set_domain_menu_entry(domain, name_str);
     }
     break;

   case MASK_DOMAIN:
     if( globals.mask_domain )
       AlcFree( (void *) globals.mask_domain );
     globals.mask_domain = WlzAssignObject(obj, NULL);
     break;

   case GREYS_DOMAIN:
   default:
     break;
  }

  return 0;
}

void read_domain_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmFileSelectionBoxCallbackStruct	*cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  Boolean		check_overlap=False;
  FILE			*fp;
  WlzObject		*obj, *obj1, *obj2, *obj3;
  String		name_str;
  WlzPixelV		threshV;

  /* set hour glass cursor */
  HGU_XmSetHourGlassCursor(globals.topl);

  /* check the reference object */
  if( globals.obj == NULL )
    return;

  /* get the file pointer */
  if( (fp = HGU_XmGetFilePointer(globals.topl, cbs->value,
				 cbs->dir, "r")) == NULL )
    return;

  /* read the domain */
  if( (obj = WlzReadObj(fp, NULL)) == NULL ){
    (void) fclose( fp );
    HGU_XmUserError(globals.topl,
		    "Read Domain Object:\n"
		    "    No domain object read - either the\n"
		    "    selected file is empty or it is not a\n"
		    "    woolz object file - please check the file\n"
		    "    or make a new selection",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }
  (void) fclose( fp );

  /* check overlap with existing domains */
  if( client_data ){
    XtVaGetValues((Widget) client_data, XmNset, &check_overlap, NULL);
  }

  if( check_overlap == True ){
    threshV.type = WLZ_GREY_INT;
    threshV.v.inv = globals.cmapstruct->ovly_cols[1];
    if( obj1 = WlzThreshold(globals.obj, threshV, WLZ_THRESH_HIGH, NULL ) ){
      if( obj2 = WlzIntersect2(obj, obj1, NULL) ){
	int 		p, counter, str_len;
	char		*str_buf=NULL;
	WlzPlaneDomain 	*pdom = obj2->domain.p;
	int		p_offset = globals.obj->domain.p->plane1;
	WlzValues	values;

	values.core = NULL;

	for(p=pdom->plane1, counter=0; p <= pdom->lastpl; p++){
	  if( pdom->domains[p - pdom->plane1].core == NULL ){
	    continue;
	  }
	  obj3 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			     pdom->domains[p - pdom->plane1],
			     values,
			     NULL, NULL, NULL);
	  if( WlzArea( obj3, NULL ) ){
	    if( str_buf==NULL ){
	      str_len = 1024;
	      str_buf = (char *) AlcMalloc(str_len);
	      str_buf[0] = '\0';
	      strcat(str_buf,
		     "Warning: the object read in has overlap\n"
		     "with existing domains on planes:\n");
	    }
	    /* add comment to string */
	    if( (str_len - strlen(str_buf)) < 64 ){
	      str_len += 1024;
	      str_buf = (char *) AlcRealloc((void *) str_buf, str_len);
	    }
	    sprintf(&(str_buf[strlen(str_buf)]), "%d, ", p-p_offset);
	    counter++;
	    if( counter == 10 ){
	      strcat(str_buf, "\n");
	      counter = 0;
	    }
	  }
	  WlzFreeObj(obj3);
	}

	if( str_buf ){
	  strcat(str_buf, "\nContinue or review/delete overlap?");
	  /* put up warning dialog */
	  if( !HGU_XmUserConfirm(globals.topl, str_buf,
				 "continue", "review", 0) )
	  {
	    obj3 = WlzDiffDomain(obj, obj2, NULL);
	    WlzFreeObj(obj);
	    obj = obj3;
	    if( globals.review_domain_obj != NULL ){
	      WlzFreeObj( globals.review_domain_obj );
	    }
	    globals.review_domain_obj = obj2;
	    HGU_XmUserMessage(globals.topl,
			      "To review the overlap domain select\n"
			      "\"Domain Review\" from the Options\n"
			      "menu. Continue without review if you want\n"
			      "to ignore the overlap regions.",
			      XmDIALOG_FULL_APPLICATION_MODAL);
	  }
	  else {
	    WlzFreeObj( obj2 );
	  }
	  AlcFree( (void *) str_buf);
	}
	else {
	  WlzFreeObj( obj2 );
	}
      }
      WlzFreeObj( obj1 );
    }
  }
  if( obj == NULL ){
    return;
  }
  
  /* get the name string and set the domain */
  XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &name_str);
  obj = WlzAssignObject(obj, NULL);
  setDomain(obj, globals.current_domain, name_str);
  WlzFreeObj(obj);
  AlcFree( (void *) name_str );

  /* set hour glass cursor */
  HGU_XmUnsetHourGlassCursor(globals.topl);

  return;
}

void clear_all_domains_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
  WlzObject	*obj;

    /* check the reference object */
    if( globals.obj == NULL )
	return;

    /* operation irreversible therefore ask for confirmation */
    if( !HGU_XmUserConfirm(globals.topl, "Really clear domains?",
			   "yes", "no", 0) )
    {
	return;
    }

    /* set hour glass cursor */
    HGU_XmSetHourGlassCursor(globals.topl);

    /* copy in the original object and reset valueline pointers */
    obj = WlzAssignObject(globals.orig_obj, NULL);
    WlzFreeObj( globals.orig_obj );
    WlzFreeObj( globals.obj );
    globals.orig_obj = NULL;
    globals.obj = NULL;
    install_paint_reference_object(obj);
    WlzFreeObj(obj);

    /* redisplay and return */
    display_all_views_cb(w, client_data, call_data);

    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);

    return;
}

int clearDomain(
  DomainSelection	domain)
{
  WlzObject	*obj=NULL;

  /* check which domain is selected */
  switch( domain ){
  case DOMAIN_1:
  case DOMAIN_2:
  case DOMAIN_3:
  case DOMAIN_4:
  case DOMAIN_5:
  case DOMAIN_6:
  case DOMAIN_7:
  case DOMAIN_8:
  case DOMAIN_9:
  case DOMAIN_10:
  case DOMAIN_11:
  case DOMAIN_12:
  case DOMAIN_13:
  case DOMAIN_14:
  case DOMAIN_15:
  case DOMAIN_16:
  case DOMAIN_17:
  case DOMAIN_18:
  case DOMAIN_19:
  case DOMAIN_20:
  case DOMAIN_21:
  case DOMAIN_22:
  case DOMAIN_23:
  case DOMAIN_24:
  case DOMAIN_25:
  case DOMAIN_26:
  case DOMAIN_27:
  case DOMAIN_28:
  case DOMAIN_29:
  case DOMAIN_30:
  case DOMAIN_31:
  case DOMAIN_32:
    obj = get_domain_from_object(globals.obj, domain);
    break;
  case MASK_DOMAIN:
    if( globals.mask_domain )
      WlzFreeObj(globals.mask_domain ); 
    globals.mask_domain = NULL;
    obj = NULL;
    return 0;
  case GREYS_DOMAIN:
  default:
    return 1;
  }

  if( domain > globals.cmapstruct->num_overlays )
  {
    set_grey_values_from_ref_object( obj, globals.orig_obj);
  }
  else
  {
    set_grey_values_from_domain(obj, globals.obj,
				globals.cmapstruct->ovly_cols[0],
				globals.cmapstruct->ovly_planes );
  }

  if( obj ){
    WlzFreeObj( obj );
  }

  return 0;
}

void clear_domain_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    WlzObject	*obj;

    /* check the reference object */
    if( globals.obj == NULL )
	return;

    /* set hour glass cursor */
    HGU_XmSetHourGlassCursor(globals.topl);

    /* operation irreversible therefore ask for confirmation */
    if( !HGU_XmUserConfirm(globals.topl, "Really clear domain?",
			   "yes", "no", 0) )
	return;

    /* check which domain is selected */
    if( clearDomain(globals.current_domain) ){
      /* report an error */
    }
    display_all_views_cb(w, client_data, call_data);

    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);

    return;
}

void write_domain_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    XmFileSelectionBoxCallbackStruct	*cbs =
	(XmFileSelectionBoxCallbackStruct *) call_data;
    FILE		*fp;
    WlzObject		*obj;
    String		name_str;

    /* check the reference object */
    if( globals.obj == NULL )
	return;

    /* set hour glass cursor */
    HGU_XmSetHourGlassCursor(globals.topl);

    /* get the file pointer */
    if( (fp = HGU_XmGetFilePointerBck(globals.topl, cbs->value,
				      cbs->dir, "w", ".bak")) == NULL )
	return;

    /* check which domain is selected */
    switch( globals.current_domain ){
    case DOMAIN_1:
    case DOMAIN_2:
    case DOMAIN_3:
    case DOMAIN_4:
    case DOMAIN_5:
    case DOMAIN_6:
    case DOMAIN_7:
    case DOMAIN_8:
    case DOMAIN_9:
    case DOMAIN_10:
    case DOMAIN_11:
    case DOMAIN_12:
    case DOMAIN_13:
    case DOMAIN_14:
    case DOMAIN_15:
    case DOMAIN_16:
    case DOMAIN_17:
    case DOMAIN_18:
    case DOMAIN_19:
    case DOMAIN_20:
    case DOMAIN_21:
    case DOMAIN_22:
    case DOMAIN_23:
    case DOMAIN_24:
    case DOMAIN_25:
    case DOMAIN_26:
    case DOMAIN_27:
    case DOMAIN_28:
    case DOMAIN_29:
    case DOMAIN_30:
    case DOMAIN_31:
    case DOMAIN_32:
	obj = get_domain_from_object(globals.obj, globals.current_domain);

	/* reset the menu-entry label */
	XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &name_str);
	set_domain_menu_entry(globals.current_domain, name_str);
	free( name_str );

	break;
    case MASK_DOMAIN:
	obj = WlzMakeMain(globals.mask_domain->type,
			  globals.mask_domain->domain,
			  globals.mask_domain->values,
			  NULL, NULL, NULL);
	break;
    case GREYS_DOMAIN:
    default:
	(void) fclose( fp );
	/* set hour glass cursor */
	HGU_XmUnsetHourGlassCursor(globals.topl);
	return;
    }

    /* write the domain only */
    if( obj != NULL ){
	WlzValues	objVals = obj->values;
	obj->values.core = NULL;
	if( WlzWriteObj(fp, obj) == WLZ_ERR_NONE ){
	    globals.domain_changed_since_saved[globals.current_domain] = 0;
	    globals.domain_filename[globals.current_domain] =
		HGU_XmGetFileStr(w, cbs->value, cbs->dir);
	} else {
	    char		*errstr;
	    errstr = (char *) AlcMalloc(128);
	    sprintf(errstr, "Write Domain Object:\n"
		            "    woolz error detected:\n"
		            "    WARNING: domain not saved\n"
		            "    - please check disc space or quotas");
	    HGU_XmUserError(globals.topl, errstr,
			    XmDIALOG_FULL_APPLICATION_MODAL);
	    AlcFree( (void *) errstr );
	}
	obj->values = objVals;
	WlzFreeObj( obj );
    }
    if( fclose( fp ) == EOF ){
      HGU_XmUserError(globals.topl,
		      "Save Domain:\n"
		      "    Error closing the save domain file.\n"
		      "    Please check disk space or quotas\n"
		      "    Domain not saved.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }

    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
}


void save_domain_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    FILE	*fp;
    WlzObject	*obj;

    /* check the reference object */
    if( globals.obj == NULL )
	return;

    /* get the file pointer */
    if( (fp = fopen(globals.domain_filename[globals.current_domain], "w"))
       == NULL ){
	HGU_XmUserError(globals.topl,
			"Can't save domain to file\nplease check filename",
			XmDIALOG_FULL_APPLICATION_MODAL);
	return;
    }

    /* set hour glass cursor */
    HGU_XmSetHourGlassCursor(globals.topl);

    /* check which domain is selected */
    switch( globals.current_domain ){
    case DOMAIN_1:
    case DOMAIN_2:
    case DOMAIN_3:
    case DOMAIN_4:
    case DOMAIN_5:
    case DOMAIN_6:
    case DOMAIN_7:
    case DOMAIN_8:
    case DOMAIN_9:
    case DOMAIN_10:
    case DOMAIN_11:
    case DOMAIN_12:
    case DOMAIN_13:
    case DOMAIN_14:
    case DOMAIN_15:
    case DOMAIN_16:
    case DOMAIN_17:
    case DOMAIN_18:
    case DOMAIN_19:
    case DOMAIN_20:
    case DOMAIN_21:
    case DOMAIN_22:
    case DOMAIN_23:
    case DOMAIN_24:
    case DOMAIN_25:
    case DOMAIN_26:
    case DOMAIN_27:
    case DOMAIN_28:
    case DOMAIN_29:
    case DOMAIN_30:
    case DOMAIN_31:
    case DOMAIN_32:
	obj = get_domain_from_object(globals.obj, globals.current_domain);
	break;
    case MASK_DOMAIN:
	obj = globals.mask_domain;
	break;
    case GREYS_DOMAIN:
    default:
	(void) fclose( fp );
	/* set hour glass cursor */
	HGU_XmUnsetHourGlassCursor(globals.topl);
	return;
    }

    /* write the domain only */
    if( obj != NULL ){
	WlzValues	objVals = obj->values;
	obj->values.core = NULL;
	if( WlzWriteObj(fp, obj) == WLZ_ERR_NONE ){
	    globals.domain_changed_since_saved[globals.current_domain] = 0;
	} else {
	    char		*errstr;
	    errstr = (char *) AlcMalloc(128);
	    sprintf(errstr, "Save Domain Object:\n"
		            "    woolz error detected:\n"
		            "    Check disc space or quotas");
	    HGU_XmUserError(globals.topl, errstr,
			    XmDIALOG_FULL_APPLICATION_MODAL);
	    AlcFree( (void *) errstr );
	}
	obj->values = objVals;
	WlzFreeObj( obj );
    }
    if( fclose( fp ) == EOF ){
      HGU_XmUserError(globals.topl,
		      "Save Domain:\n"
		      "    Error closing the domain file.\n"
		      "    Please check disk space or quotas.\n"
		      "    Domain not saved",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }

    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
}

void threed_display_domain_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    WlzObject		*obj, *obj1, *boundobj;
    WlzPlaneDomain	*planedmn;
    int			z;
    float		red, green, blue;

    /* check the reference object */
    if( globals.obj == NULL )
	return;

    /* set hour glass cursor */
    HGU_XmSetHourGlassCursor(globals.topl);

    /* remove the display list if currently visible */
    if( globals.domain_display_list[globals.current_domain] != 0 )
    {
      glDeleteLists(globals.domain_display_list[globals.current_domain], 1);
      globals.domain_display_list[globals.current_domain] = 0;
      MAOpenGLDrawScene( globals.canvas );
      return;
    }

    /* check which domain is selected */
    switch( globals.current_domain ){
    case DOMAIN_1:
    case DOMAIN_2:
    case DOMAIN_3:
    case DOMAIN_4:
    case DOMAIN_5:
    case DOMAIN_6:
    case DOMAIN_7:
    case DOMAIN_8:
    case DOMAIN_9:
    case DOMAIN_10:
    case DOMAIN_11:
    case DOMAIN_12:
    case DOMAIN_13:
    case DOMAIN_14:
    case DOMAIN_15:
    case DOMAIN_16:
    case DOMAIN_17:
    case DOMAIN_18:
    case DOMAIN_19:
    case DOMAIN_20:
    case DOMAIN_21:
    case DOMAIN_22:
    case DOMAIN_23:
    case DOMAIN_24:
    case DOMAIN_25:
    case DOMAIN_26:
    case DOMAIN_27:
    case DOMAIN_28:
    case DOMAIN_29:
    case DOMAIN_30:
    case DOMAIN_31:
    case DOMAIN_32:
	obj = get_domain_from_object(globals.obj, globals.current_domain);
	break;
    case MASK_DOMAIN:
	obj = globals.mask_domain;
	break;
    case GREYS_DOMAIN:
    default:
      /* set hour glass cursor */
      HGU_XmUnsetHourGlassCursor(globals.topl);
	return;
    }

    if( obj == NULL )
    {
      /* set hour glass cursor */
      HGU_XmUnsetHourGlassCursor(globals.topl);
      return;
    }

    /* display in 3D */
    MAOpenGLDisplayDomainIndex(obj, globals.current_domain);
    WlzFreeObj( obj );

    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
}

static char dragTranslations[] = 
"#override <Btn2Down>: StartDragDrop(DomainDominance)";

static Boolean ConvertProc(
  Widget	w,
  Atom		*selection,
  Atom		*target,
  Atom		*type_return,
  XtPointer	*value_return,
  unsigned long	*length_return,
  int		*format_return)
{
  Atom		export_target, motif_drop;
  Widget	source_w;
  String	name;

  /* get export atom for final type check */
  export_target = XmInternAtom(XtDisplay(w), "HGU_PAINT_DOMAIN_NAME", False);
  motif_drop = XmInternAtom(XtDisplay(w), "_MOTIF_DROP", False);

  /* check selection */
  if( *selection != motif_drop )
  {
    return( False );
  }

  /* get the source widget and copy its name */
  XtVaGetValues(w, XmNclientData, (XtPointer) &source_w, NULL);
  if( source_w == NULL )
  {
    return( False );
  }
  name = (String) malloc(sizeof(char) * (strlen(XtName(source_w)) + 2));
  if( name == NULL )
  {
    return( False );
  }
  (void) strcpy( name, XtName(source_w) );

  /* set up the return values */
  *type_return = export_target;
  *value_return = (XtPointer) name;
  *length_return = strlen(name) + 1;
  *format_return = 8;

  return( True );
}

void RotateArray(
  char		*val,
  size_t	size,
  int		from_index,
  int		to_index)
{
  char		*tmp;
  int		i;

  tmp = (char *) AlcMalloc( size );
  memcpy( (void *) tmp, (const void *) (val + from_index*size), size );

  if( from_index < to_index )
  {
    for(i=from_index; i < to_index; i++)
    {
      memcpy((void *) (val + i*size), (const void *) (val + (i+1)*size), size);
    }
  }
  else if( from_index > to_index )
  {
    for(i=from_index; i > to_index; i--)
    {
      memcpy((void *) (val + i*size), (const void *) (val + (i-1)*size), size);
    }
  }

  memcpy( (void *) (val + to_index*size), (const void *) tmp, size );
  AlcFree( (void *) tmp );

  return;
}

static void TransferProc(
  Widget	w,
  XtPointer	client_data,
  Atom		*seltype,
  Atom		*type,
  XtPointer	value,
  unsigned long	*length,
  int		format)
{
  int		source_index, dest_index;
  ViewListEntry	*vl = global_view_list;
  int		i, num_overlays;

  /* extract the domain indices */
  if( sscanf((String) value, "%d", &source_index) < 1 )
  {
    return;
  }
  if( sscanf(XtName( (Widget) client_data ), "%d", &dest_index) < 1 )
  {
    return;
  }

  /* rotate the priority_to_domain_lut  */
  RotateArray((char *) &(globals.priority_to_domain_lut[0]),
	      sizeof(int), source_index, dest_index);

  /* reset the menu labels and background colours */
  num_overlays = globals.cmapstruct->num_overlays + 
    globals.cmapstruct->num_solid_overlays;
  for(i=1; i <= num_overlays; i++)
  {
    XmString	xmstr;
    Widget	widget;
    char	str_buf[64];
    Pixel	pixel;
    int		data_index;

    /* get new  label and background colour */
    data_index = globals.priority_to_domain_lut[i];
    xmstr = XmStringCreateSimple( globals.domain_name[data_index] );
    pixel = globals.cmapstruct->ovly_cols[data_index] +
      globals.cmapstruct->gmax - globals.cmapstruct->gmin;
    if( data_index > globals.cmapstruct->num_overlays )
    {
      pixel = globals.cmapstruct->ovly_cols[data_index];
    }

    /* get the widgets by priority and reset resources */
    sprintf(str_buf, "*menubar*domain_menu*select*domain_%d", i);
    widget = XtNameToWidget(globals.topl, str_buf);
    XtVaSetValues(widget, XmNlabelString, xmstr,
		  XmNbackground, pixel, NULL);

    sprintf(str_buf, "*domain_controls_dialog*dominance_form*%d", i);
    widget = XtNameToWidget(globals.topl, str_buf);
    XtVaSetValues(widget, XmNbackground, pixel, NULL);

    XmStringFree(xmstr);
  }

  return;
}

static void DD_FinishCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	icon = NULL;

  XtVaGetValues(w, XmNsourceCursorIcon, &icon, NULL);

  if( icon )
  {
    XtDestroyWidget( icon );
  }

  return;
}

#define domainbm_width 55
#define domainbm_height 17
static unsigned char domainbm_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x1f, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x10, 0xc2, 0x01, 0x03, 0x00, 0x00, 0x03, 0x10, 0xe2,
   0x03, 0x03, 0x00, 0x00, 0x00, 0x10, 0x72, 0xc7, 0x73, 0xfe, 0x38, 0x7b,
   0x10, 0x3a, 0xee, 0xfb, 0xfe, 0x7d, 0xfb, 0x10, 0x1a, 0x6c, 0xdb, 0xb6,
   0x65, 0xdb, 0x10, 0x3a, 0x6e, 0xdb, 0xb6, 0x79, 0xdb, 0x10, 0x72, 0x67,
   0xdb, 0xb6, 0x6d, 0xdb, 0x10, 0xe2, 0xe3, 0xfb, 0xb6, 0x7d, 0xdb, 0x10,
   0xc2, 0xc1, 0x73, 0xb6, 0xdd, 0xdb, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xfe, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void StartDD_DomainDominance(
Widget		w,
XEvent		*event,
String		*params,
Cardinal	*num_params)
{
  Atom		export_list[1];
  Pixmap	icon;
  int		n;
  Arg		args[12];
  Widget	drag_icon, drag_context;
  Pixel		fg, bg;

  /* if painting then ignore this request - should be fixed elsewhere */
  if( paint_key != NULL )
  {
    return;
  }

  /* get the atom for data target type */
  export_list[0] = XmInternAtom(XtDisplay(w), "HGU_PAINT_DOMAIN_NAME", False);
  XtVaGetValues(w, XmNforeground, &fg, XmNbackground, &bg, NULL);

  /* create pixmap for the drag icon */
  icon = XmGetPixmapByDepth(XtScreen(w), "domainbm.xbm", 1, 0, 1);
/*  icon = XCreatePixmapFromBitmapData(globals.dpy, XtWindow(w),
				     (char *) domainbm_bits,
				     domainbm_width, domainbm_height,
				     1, 0, 1);*/
  if( icon == XmUNSPECIFIED_PIXMAP )
  {
    fprintf(stderr, "Can't read drag bitmap file\n");
  }
  n = 0;
  XtSetArg(args[n], XmNpixmap, icon); n++;
  drag_icon = XmCreateDragIcon(w, "drag_icon", args, n);

  /* specify resources for the drag context */
  n = 0;
  fg = BlackPixelOfScreen(XtScreen(w));
  bg = WhitePixelOfScreen(XtScreen(w));
  XtSetArg(args[n], XmNblendModel, XmBLEND_ALL); n++;
  XtSetArg(args[n], XmNcursorForeground, fg); n++;
  XtSetArg(args[n], XmNcursorBackground, bg); n++;
  XtSetArg(args[n], XmNsourceCursorIcon, drag_icon); n++;
  XtSetArg(args[n], XmNexportTargets, export_list); n++;
  XtSetArg(args[n], XmNnumExportTargets, 1); n++;
  XtSetArg(args[n], XmNdragOperations, XmDROP_MOVE); n++;
  XtSetArg(args[n], XmNconvertProc, ConvertProc); n++;
  XtSetArg(args[n], XmNclientData, w); n++;

  /* start the drag and register a finish callback */
  drag_context = XmDragStart(w, event, args, n);
  XtAddCallback(drag_context, XmNdragDropFinishCallback, DD_FinishCb, NULL);

  return;
}

static void DD_DomainDominanceHandleCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmDropProcCallbackStruct	*cbs = (XmDropProcCallbackStruct *) call_data;
  int				i, n;
  Arg				args[8];
  Atom				import_target, *export_targets;
  Cardinal			num_export_targets;
  Widget			drag_context;
  Boolean			domain_name = False;
  XmDropTransferEntryRec	transfer_entries[1];

  /* get the required import target */
  import_target = XmInternAtom(XtDisplay(w), "HGU_PAINT_DOMAIN_NAME", False);

  drag_context = cbs->dragContext;

  /* get the export targets from the drag context and check type */
  n = 0;
  XtSetArg(args[n], XmNexportTargets, &export_targets); n++;
  XtSetArg(args[n], XmNnumExportTargets, &num_export_targets); n++;
  XtGetValues(drag_context, args, n);
  for(i=0; i < num_export_targets; i++)
  {
    if( export_targets[i] == import_target )
    {
      domain_name = True;
      break;
    }
  }

  /* if not HGU_PAINT_DOMAIN_NAME or not XmDROP_MOVE then fail */
  n = 0;
  if( (domain_name != True) || (cbs->dropAction != XmDROP) ||
      (cbs->operation != XmDROP_MOVE) )
  {
    XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
    XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
  }
  else
  {
    transfer_entries[0].target = import_target;
    transfer_entries[0].client_data = (XtPointer) w;
    XtSetArg(args[n], XmNdropTransfers, transfer_entries); n++;
    XtSetArg(args[n], XmNnumDropTransfers, 1); n++;
    XtSetArg(args[n], XmNtransferProc, TransferProc); n++;
  }

  XmDropTransferStart(cbs->dragContext, args, n);

  return;
}

static Widget create_domain_controls_dialog(
  Widget	topl)
{
    Widget		dialog, control, dominance, row_column,  form, widget;
    Atom		import_list[1];
    int			i, n, num_overlays;
    Arg			args[10];
    XtTranslations	trans_tb;
    Pixel		pixel;
    char		str_buf[16];


    /* create the controls dialog */
    dialog = HGU_XmCreateStdDialog(topl, "domain_controls_dialog",
				   xmFormWidgetClass, NULL, 0);

    control = XtNameToWidget( dialog, "*.control" );

    num_overlays = globals.cmapstruct->num_overlays +
      globals.cmapstruct->num_solid_overlays;

    /* add in frame for the dominance control */
    widget = XtVaCreateManagedWidget("dominance_frame", xmFrameWidgetClass,
				     control,
				     XmNtopAttachment,  	XmATTACH_FORM,
				     XmNleftAttachment, 	XmATTACH_FORM,
				     XmNrightAttachment,	XmATTACH_FORM,
				     NULL);

    form = XtVaCreateManagedWidget("dominance_form", xmFormWidgetClass,
				   widget,
				   XmNleftAttachment, 	XmATTACH_FORM,
				   XmNrightAttachment,	XmATTACH_FORM,
				   XmNborderWidth,	0,
				   XmNfractionBase,	num_overlays*10,
				   NULL);

    dominance = XtVaCreateManagedWidget("dominance", xmLabelWidgetClass,
					form,
					XmNtopAttachment,  	XmATTACH_FORM,
					XmNleftAttachment, 	XmATTACH_FORM,
					XmNrightAttachment,	XmATTACH_FORM,
					XmNborderWidth,	0,
					NULL);

    /* setup arguments for dominance drag and drop */
    trans_tb = XtParseTranslationTable(dragTranslations);
    n = 0;
    import_list[0] = XmInternAtom(XtDisplay(topl),
				  "HGU_PAINT_DOMAIN_NAME", False);
    XtSetArg(args[n], XmNimportTargets, import_list); n++;
    XtSetArg(args[n], XmNnumImportTargets, 1); n++;
    XtSetArg(args[n], XmNdropSiteOperations, XmDROP_MOVE); n++;
    XtSetArg(args[n], XmNdropProc, DD_DomainDominanceHandleCb); n++;

    /* create the dominance buttons */
    for(i=1; i <= num_overlays; i++)
    {
      int		data_index;

      sprintf(str_buf, "%d", i);
      data_index = globals.priority_to_domain_lut[i];

      pixel = globals.cmapstruct->ovly_cols[data_index] +
	globals.cmapstruct->gmax - globals.cmapstruct->gmin;
      if( data_index > globals.cmapstruct->num_overlays )
      {
	pixel = globals.cmapstruct->ovly_cols[data_index];
      }
      widget = XtVaCreateManagedWidget
	(str_buf, xmPushButtonWidgetClass,
	 form,
	 XmNborderWidth,	0,
	 XmNleftAttachment,	XmATTACH_POSITION,
	 XmNrightAttachment,	XmATTACH_POSITION,
	 XmNleftPosition,	i*10-9,
	 XmNrightPosition,	i*10-1,
	 XmNtopAttachment,	XmATTACH_WIDGET,
	 XmNtopWidget,		dominance,
	 XmNbackground,		pixel,
	 NULL);

      /* add drag and drop translations and register as a drop site */
      XtOverrideTranslations(widget, trans_tb);
      XmDropSiteRegister(widget, args, n);
    }
					 
    return( dialog );
}

void write_paint_object_cb(
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

    /* write the reference object */
    WlzEffWriteObj(fp, icsfile, globals.obj, image_type);

    /* close the file pointer if non NULL */
    if( fp )
    {
      if( fclose( fp ) == EOF ){
	HGU_XmUserError(globals.topl,
			"Write painted object:\n"
			"    Error in closing the object file\n"
			"    Please check disk space or quotas\n"
			"    Painted object not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }
    }

    return;
}

static void image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzEffFormat	image_type = (WlzEffFormat) client_data;
  XmString	pattern_str;

  XtRemoveAllCallbacks(write_paint_volume_dialog, XmNokCallback);

  switch( image_type )
  {
   default:
   case WLZEFF_FORMAT_WLZ:
     pattern_str = XmStringCreateSimple( "*.wlz*" );
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
     pattern_str = XmStringCreateSimple( "*.pgm" );
     break;
     
  }

  XtAddCallback(write_paint_volume_dialog, XmNokCallback,
		   write_paint_object_cb, client_data);

  XtAddCallback(write_paint_volume_dialog, XmNokCallback,
		PopdownCallback, NULL);

  XtVaSetValues(write_paint_volume_dialog, XmNpattern, pattern_str, NULL);
  XmStringFree( pattern_str );
  XmFileSelectionDoSearch( write_paint_volume_dialog, NULL );

  return;
}

int domain_menu_init(
Widget	topl)
{
    Widget		widget, file_type_menu;
    int			i, n;
    XtTranslations	trans_tb;
    Atom		import_list[1];
    Arg			args[8];
    Visual		*visual;

    globals.mask_domain    = NULL;
    globals.current_domain = DOMAIN_1;
    globals.current_col    = 64;
    globals.auto_increment = 0;
    globals.propogate      = 0;

    globals.gc_greys  = NULL;
    globals.gc_reds   = NULL;
    globals.gc_blues  = NULL;

    for(i=0; i < 33; i++)
    {
      globals.domain_display_list[i] = 0;
      globals.priority_to_domain_lut[i] = i;
      globals.domain_changed_since_saved[i] = 0;
    }

    /* create the domain controls dialog */
    domain_controls_dialog = create_domain_controls_dialog( topl );
    XtManageChild( domain_controls_dialog );
    HGU_XmSaveRestoreAddWidget( domain_controls_dialog,
				NULL, NULL, NULL, NULL );

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(topl);
    XtSetArg(args[0], XmNvisual, visual);

    /* create the read-domain file selection dialog */
    read_domain_dialog = XmCreateFileSelectionDialog( topl,
						     "read_domain_dialog",
						     args, 1);
    widget = XtVaCreateManagedWidget("Check overlap",
				     xmToggleButtonGadgetClass,
				     read_domain_dialog, NULL);
    XtAddCallback(read_domain_dialog, XmNcancelCallback, 
                  PopdownCallback, NULL);
    XtAddCallback(read_domain_dialog, XmNokCallback, read_domain_cb,
		  (XtPointer) widget);
    XtAddCallback(read_domain_dialog, XmNokCallback, display_all_views_cb,
		  NULL);
    XtAddCallback(read_domain_dialog, XmNokCallback, PopdownCallback, NULL);
    XtManageChild( read_domain_dialog );

    HGU_XmSaveRestoreAddWidget( read_domain_dialog, HGU_XmFSD_SaveFunc,
			       (XtPointer) XtName(topl), NULL, NULL );

    widget = XtNameToWidget( topl, "*domain_menu*read_domain");
    if( widget != NULL )
	XtAddCallback(widget, XmNactivateCallback, popup_dialog_cb,
		      (XtPointer) read_domain_dialog);

    /* create the write-domain file selection dialog */
    write_domain_dialog = XmCreateFileSelectionDialog( topl,
						     "write_domain_dialog",
						     args, 1);
    XtAddCallback(write_domain_dialog, XmNcancelCallback, 
                  PopdownCallback, NULL);
    XtAddCallback(write_domain_dialog, XmNokCallback, write_domain_cb, NULL);
    XtAddCallback(write_domain_dialog, XmNokCallback, PopdownCallback, NULL);
    XtManageChild( write_domain_dialog );

    HGU_XmSaveRestoreAddWidget( write_domain_dialog, HGU_XmFSD_SaveFunc,
			       (XtPointer) XtName(topl), NULL, NULL );

    widget = XtNameToWidget( topl, "*domain_menu*write_domain");
    if( widget != NULL )
	XtAddCallback(widget, XmNactivateCallback, popup_dialog_cb,
		      (XtPointer) write_domain_dialog);

    /* create the write-paint volume file selection dialog */
    write_paint_volume_dialog =
      XmCreateFileSelectionDialog( topl, "write_paint_volume_dialog", args, 1);

    /* add a file type selection menu */
    file_type_menu = HGU_XmBuildMenu(write_paint_volume_dialog, XmMENU_OPTION,
				     "file_type", 0, XmTEAR_OFF_DISABLED,
				     file_type_menu_itemsP);
    XtManageChild( file_type_menu );

    image_type_cb(write_paint_volume_dialog, (XtPointer) WLZEFF_FORMAT_WLZ,
		  NULL);
    XtAddCallback(write_paint_volume_dialog, XmNcancelCallback, 
                  PopdownCallback, NULL);
    XtManageChild( write_paint_volume_dialog );

    HGU_XmSaveRestoreAddWidget( write_paint_volume_dialog, HGU_XmFSD_SaveFunc,
			       (XtPointer) XtName(topl), NULL, NULL );

    widget = XtNameToWidget( topl, "*domain_menu*write_paint_volume");
    if( widget != NULL )
	XtAddCallback(widget, XmNactivateCallback, popup_dialog_cb,
		      (XtPointer) write_paint_volume_dialog);

    /* set up the domain names and files plus drag and drop actions */
    XtGetApplicationResources(topl, &globals, domain_res,
                              XtNumber(domain_res), NULL, 0);
    trans_tb = XtParseTranslationTable(dragTranslations);

    for(i=1; i < 33; i++){
	XmString	xmstr;
	char		str_buf[64];
	Pixel		pixel;
	int		data_index;

	/* get the widgets in priority order */
	sprintf(str_buf, "*menubar*domain_menu*select*domain_%d", i);
	widget = XtNameToWidget(topl, str_buf);
	if( i > (globals.cmapstruct->num_overlays +
		 globals.cmapstruct->num_solid_overlays) )
	{
	  XtUnmanageChild(widget);
	  continue;
	}

	/* get the data for each priority */
	data_index = globals.priority_to_domain_lut[i];
	xmstr = XmStringCreateSimple( globals.domain_name[data_index] );
	pixel = globals.cmapstruct->ovly_cols[data_index] +
	  globals.cmapstruct->gmax - globals.cmapstruct->gmin;
	if( data_index > globals.cmapstruct->num_overlays )
	{
	  pixel = globals.cmapstruct->ovly_cols[data_index];
	}
	XmChangeColor(widget, pixel);
	XtVaSetValues(widget,
		      XmNlabelString, xmstr,
		      NULL);
	XmStringFree(xmstr);

	/* add drag and drop translations */
	XtOverrideTranslations(widget, trans_tb);

	/* register as a drop site */
	n = 0;
	import_list[0] = XmInternAtom(XtDisplay(widget),
				     "HGU_PAINT_DOMAIN_NAME", False);
	XtSetArg(args[n], XmNimportTargets, import_list); n++;
	XtSetArg(args[n], XmNnumImportTargets, 1); n++;
	XtSetArg(args[n], XmNdropSiteOperations, XmDROP_MOVE); n++;
	XtSetArg(args[n], XmNdropProc, DD_DomainDominanceHandleCb); n++;
	XmDropSiteRegister(widget, args, n);
    }

    return( 0 );
}
