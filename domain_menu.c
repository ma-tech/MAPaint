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

extern Widget create_domain_controls_dialog(
  Widget	topl);

/* menu item structures */

static MenuItem file_type_menu_itemsP[] = {	/* file_menu items */
  {"woolz", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_WLZ,
   myHGU_XmHelpStandardCb, SEC_READ_WRITE_DOMAINS_DIALOGS,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"ics", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_ICS,
   myHGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"den", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_DEN,
   myHGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_VFF,
   myHGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pic", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_PIC,
   myHGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vtk", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_VTK,
   myHGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"slc", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_SLC,
   myHGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pgm", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_PNM,
   myHGU_XmHelpStandardCb, "paint/paint.html#read_object",
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem select_menu_itemsP[] = {	/* select menu items */
{"domain_1", &xmToggleButtonWidgetClass, 0, NULL, NULL, True,
     select_domain_cb, (XtPointer) DOMAIN_1,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_2", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_2,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_3", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_3,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_4", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_4,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_5", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_5,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_6", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_6,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_7", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_7,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_8", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_8,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_9", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_9,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_10", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_10,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_11", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_11,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_12", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_12,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_13", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_13,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_14", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_14,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_15", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_15,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_16", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_16,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_17", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_17,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_18", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_18,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_19", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_19,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_20", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_20,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_21", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_21,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_22", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_22,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_23", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_23,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_24", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_24,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_25", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_25,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_26", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_26,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_27", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_27,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_28", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_28,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_29", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_29,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_30", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_30,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_31", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_31,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_32", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) DOMAIN_32,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"domain_mask", &xmToggleButtonWidgetClass, 0, NULL, NULL, False,
     select_domain_cb, (XtPointer) MASK_DOMAIN,
     myHGU_XmHelpStandardCb, "paint/paint.html#select_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
NULL,
};

static MenuItem domain_menu_itemsP[] = {		/* file_menu items */
{"select", &xmCascadeButtonGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, myHGU_XmHelpStandardCb, SEC_DOMAIN_MENU,
     XmTEAR_OFF_ENABLED, True, True, &(select_menu_itemsP[0])},
{"controls", &xmPushButtonGadgetClass, 0,  NULL, NULL, False,
     domain_cb, NULL, myHGU_XmHelpStandardCb, "paint/paint.html#domain_controls",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"emage_domains", &xmPushButtonGadgetClass, 0,  NULL, NULL, False,
     setEMAGEDomainsAndColoursCb, NULL,
     myHGU_XmHelpStandardCb, "paint/paint.html#domain_controls",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"auto_increment", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
     int_toggle_cb, (caddr_t) &(globals.auto_increment),
     myHGU_XmHelpStandardCb, "paint/paint.html#auto_increment",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"propogate", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
     int_toggle_cb, (caddr_t) &(globals.propogate),
     myHGU_XmHelpStandardCb, "paint/paint.html#propogate",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"propogate_sel", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
     int_toggle_cb, (caddr_t) &(globals.propogate_sel),
     myHGU_XmHelpStandardCb, "paint/paint.html#propogate",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"read_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, myHGU_XmHelpStandardCb, "paint/paint.html#read_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
/*{"save_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     save_domain_cb, NULL, myHGU_XmHelpStandardCb, "paint/paint.html#quick_save",
     XmTEAR_OFF_DISABLED, False, False, NULL},*/
{"write_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, myHGU_XmHelpStandardCb, "paint/paint.html#write_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"clear_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     clear_domain_cb, NULL, myHGU_XmHelpStandardCb,
     "paint/paint.html#clear_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"threed_display_domain", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
     threed_display_domain_cb, NULL, myHGU_XmHelpStandardCb,
     "paint/paint.html#threed_display_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
     NULL, NULL, NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"clear_all_domains", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     clear_all_domains_cb, NULL, myHGU_XmHelpStandardCb,
     "paint/paint.html#clear_domain",
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"save_all_domains", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     save_all_domains_cb, NULL, myHGU_XmHelpStandardCb,
     "paint/paint.html#save_all_domains",
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

/* this resource list is used to re-establish the default
   domain names and filenames. Change with care */
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
{"currentDomain", "Domain", XtRInt, sizeof(int),
     res_offset(current_domain), XtRImmediate, (caddr_t) 1},
{"domain_priority_1", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[1]), XtRImmediate, (caddr_t) 1},
{"domain_priority_2", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[2]), XtRImmediate, (caddr_t) 2},
{"domain_priority_3", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[3]), XtRImmediate, (caddr_t) 3},
{"domain_priority_4", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[4]), XtRImmediate, (caddr_t) 4},
{"domain_priority_5", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[5]), XtRImmediate, (caddr_t) 5},
{"domain_priority_6", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[6]), XtRImmediate, (caddr_t) 6},
{"domain_priority_7", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[7]), XtRImmediate, (caddr_t) 7},
{"domain_priority_8", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[8]), XtRImmediate, (caddr_t) 8},
{"domain_priority_9", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[9]), XtRImmediate, (caddr_t) 9},
{"domain_priority_10", "DomainPriority", XtRInt, sizeof(int),
     res_offset(priority_to_domain_lut[10]), XtRImmediate, (caddr_t) 10},
};

void MAPaintDomainGetResourceValues(void)
{
  XtGetApplicationResources(globals.topl, &globals, domain_res,
			    XtNumber(domain_res), NULL, 0);
  return;
}


/* action and callback procedures */
void addSelectDomainCallback(
  XtCallbackProc	callback,
  XtPointer		clientData)
{
  Widget	toggle, cascadeB;
  int		i;
  String	strBuf;

  /* get the menu widget */
  if( cascadeB = XtNameToWidget(globals.topl,
				"*menubar*domain_menu*select") ){

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
  if( cascadeB = XtNameToWidget(globals.topl,
				"*menubar*domain_menu*select") ){

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
    if( globals.priority_to_domain_lut[
      (int) select_menu_itemsP[numDomains].callback_data]
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
  Widget	toggle;

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

    /* reset the 3D display toggle */
    if( toggle = XtNameToWidget(globals.topl,
				"*domain_menu*threed_display_domain") ){
      if( globals.domain_display_list[globals.current_domain] ){
	XtVaSetValues(toggle, XmNset, True, NULL);
      }
      else {
	XtVaSetValues(toggle, XmNset, False, NULL);
      }
    }
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
    pixel = HGU_XGetColorPixel(globals.dpy, globals.cmap,
			       (float) globals.colormap[0][pixel]/255.0,
			       (float) globals.colormap[1][pixel]/255.0,
			       (float) globals.colormap[2][pixel]/255.0);
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

  /* copy into the stored domain names - lose some memory here possibly */
  globals.domain_name[domain] = (String) malloc(sizeof(char) *
						(strlen(name_str+i+1) + 1));
  (void) strcpy(globals.domain_name[domain], name_str+i+1);

  /* change the name in the review and surgery dialogs */
  sprintf(str_buf, "*review_dialog*dest_domain_rc*domain %d", domain);
  if( widget = XtNameToWidget(globals.topl, str_buf) ){
    XtVaSetValues(widget, XmNlabelString, name, NULL);
  }
  sprintf(str_buf, "*review_dialog*src_domain*domain %d", domain);
  if( widget = XtNameToWidget(globals.topl, str_buf) ){
    XtVaSetValues(widget, XmNlabelString, name, NULL);
  }
  sprintf(str_buf, "*surgery_dialog*dest_domain_rc*domain %d", domain);
  if( widget = XtNameToWidget(globals.topl, str_buf) ){
    XtVaSetValues(widget, XmNlabelString, name, NULL);
  }
  sprintf(str_buf, "*surgery_dialog*src_domain*domain %d", domain);
  if( widget = XtNameToWidget(globals.topl, str_buf) ){
    XtVaSetValues(widget, XmNlabelString, name, NULL);
  }

  XmStringFree( name );

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

     /* reset the 3D display */
     if( globals.domain_display_list[domain] ){
       WlzObject	*domObj;
       glDeleteLists(globals.domain_display_list[globals.current_domain],
		     1);
       domObj = get_domain_from_object(globals.obj, domain);
       MAOpenGLDisplayDomainIndex(domObj, domain);
       MAOpenGLDrawScene( globals.canvas );
       WlzFreeObj(domObj);
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
  Boolean		replace_flg=False;
  Boolean		check_overlap=False;
  FILE			*fp;
  WlzObject		*obj, *obj1, *obj2, *obj3;
  String		name_str;
  WlzPixelV		threshV;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  Widget		widget;

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

  /* check for empty - do nothing then check type */
  switch( obj->type ){
    WlzDomain	domain;
    WlzValues	values;
    WlzObject	*tmpObj;

  case WLZ_2D_DOMAINOBJ:
    if( obj->type != globals.origObjType ){
      if(!HGU_XmUserConfirm(globals.topl,
			    "Read Domain Object:\n"
			    "    Invalid object type - the domain read\n"
			    "    in is 2D whereas the reference object\n"
			    "    is 3D. Please the check or convert\n"
			    "    the domain file. Do you want MAPaint\n"
			    "    to try and convert the data?",
			    "yes", "no", 0)){
	WlzFreeObj( obj );
	return;
      }
    }
    if( domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN,
				      globals.orig_obj->domain.p->plane1,
				      globals.orig_obj->domain.p->plane1,
				      obj->domain.i->line1,
				      obj->domain.i->lastln,
				      obj->domain.i->kol1,
				      obj->domain.i->lastkl,
				      &errNum) ){
      domain.p->domains[0] = WlzAssignDomain(obj->domain, NULL);
      values.core = NULL;
      tmpObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
			   NULL, NULL, &errNum);
      WlzFreeObj(obj);
      obj = tmpObj;
    }
    
    break;

  case WLZ_3D_DOMAINOBJ:
    if( obj->type != globals.origObjType ){
      if( HGU_XmUserConfirm(globals.topl,
			    "Read Domain Object:\n"
			    "    Invalid object type - the domain read\n"
			    "    in is 3D whereas the reference object\n"
			    "    is 2D. Please the check or convert\n"
			    "    the domain file. Do you want MAPaint\n"
			    "    to try and convert the data?",
			    "yes", "no", 0) ){
	obj->domain.p->lastpl -= obj->domain.p->plane1;
	obj->domain.p->plane1 = 0;
      }
      else {
	WlzFreeObj( obj );
	return;
      }
    }
    break;

  case WLZ_EMPTY_OBJ:
    break;

  default:
    HGU_XmUserError(globals.topl,
		    "Read Domain Object:\n"
		    "    Invalid object type - please check\n"
		    "    that the selected file has a woolz\n"
		    "    domain object of the same dimensions\n"
		    "    (2D or 3D) as the reference object",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    WlzFreeObj( obj );
    return;

  }

  /* check if replace existing required */
  if( widget =
     XtNameToWidget(globals.topl,
		    "*read_domain_dialog*Replace existing domain") ){
    XtVaGetValues(widget, XmNset, &replace_flg, NULL);
  }
  if( replace_flg == True ){
    /* could lose important data therefore ask for confirmation */
    if( HGU_XmUserConfirm(globals.topl,
			  "Replace existing domain?\n"
			  "(i.e. delete it!)",
			  "yes", "no", 0) ){
      if( clearDomain(globals.current_domain) ){
	/* report an error */
      }
    }
  }

  /* check overlap with existing domains */
  if( widget = XtNameToWidget(globals.topl,
			      "*read_domain_dialog*Check overlap") ){
    XtVaGetValues(widget, XmNset, &check_overlap, NULL);
  }

  if( (replace_flg != True) && (check_overlap == True) ){
    threshV.type = WLZ_GREY_INT;
    threshV.v.inv = globals.cmapstruct->ovly_cols[1];
    if( obj1 = WlzThreshold(globals.obj, threshV, WLZ_THRESH_HIGH, &errNum ) ){
      if( obj2 = WlzIntersect2(obj, obj1, &errNum) ){
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
  
  /* get the name string and set the domain */
  if( (errNum == WLZ_ERR_NONE) && obj ){
    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &name_str);
    obj = WlzAssignObject(obj, NULL);
    setDomain(obj, globals.current_domain, name_str);
    WlzFreeObj(obj);
    AlcFree( (void *) name_str );
  }

  /* set hour glass cursor */
  HGU_XmUnsetHourGlassCursor(globals.topl);

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "read_domain_cb", errNum);
  }
  return;
}

void clear_all_domains_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzObject		*obj;
  DomainSelection	domain;

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

  /* reset the menu and 3D view - use start-up names and files */
  MAPaintDomainGetResourceValues();

  for(domain=1; domain < 33; domain++){
    set_domain_menu_entry(domain, globals.domain_name[domain]);

    /* clear the 3D view */
    if( globals.domain_display_list[domain] ){
      Widget toggle;

      glDeleteLists(globals.domain_display_list[domain], 1);
      globals.domain_display_list[domain] = 0;
      MAOpenGLDrawScene( globals.canvas );
      if( toggle = XtNameToWidget(globals.topl,
				  "*domain_menu*threed_display_domain") ){
	XtVaSetValues(toggle, XmNset, False, NULL);
      }
    }
  }

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

    /* clear the domain label and filename to default
       this uses special knowledge of the resource list,
       should really get this from resources */
    /* now get it from resources - also requires special
       knowledge of the resourece list */
    XtGetApplicationResources(globals.topl, &globals, domain_res+domain,
			      1, NULL, 0);
    XtGetApplicationResources(globals.topl, &globals, domain_res+domain+32,
			      1, NULL, 0);
    set_domain_menu_entry(domain, globals.domain_name[domain]);
    /* clear the 3D view */
    if( globals.domain_display_list[domain] ){
      Widget toggle;

      glDeleteLists(globals.domain_display_list[globals.current_domain], 1);
      globals.domain_display_list[domain] = 0;
      MAOpenGLDrawScene( globals.canvas );
      if( toggle = XtNameToWidget(globals.topl,
				  "*domain_menu*threed_display_domain") ){
	XtVaSetValues(toggle, XmNset, False, NULL);
      }
    }
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
			 "yes", "no", 0) ){
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
  }

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

  /* check if any views selected for painting */
  if( paint_key ){
    if( !HGU_XmUserConfirm(globals.topl,
			   "Warning: There is a view window selected\n"
			   "\tfor painting. Any part of the domain\n"
			   "\tto be written that is in that view\n"
			   "\twill not be saved. To save the\n"
			   "\twhole domain select \"cancel\",\n"
			   "\tbelow, deselect the view from painting\n"
			   "\tand re-write the domain.\n\n"
			   "Do you want to continue writing?",
			   "yes", "cancel", 1) ){
      return;
    }
  }

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
    WlzObject	*tmpObj;
    WlzValues	objVals;
    objVals.core = NULL;
    if( globals.origObjType == WLZ_3D_DOMAINOBJ ){
      tmpObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, obj->domain, objVals,
			   NULL, NULL, NULL);
    }
    else {
      tmpObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, obj->domain.p->domains[0],
			   objVals, NULL, NULL, NULL);
    }
    if( WlzWriteObj(fp, tmpObj) == WLZ_ERR_NONE ){
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
    WlzFreeObj(tmpObj);
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

  /* unset hour glass cursor */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  return;
}

static int saveYesToAll=0;

void save_domain_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    FILE	*fp;
    WlzObject	*obj;
    DomainSelection	domainIndx;

    /* check the reference object */
    if( globals.obj == NULL )
	return;

    /* select domain */
    if( client_data == NULL ){
      domainIndx = globals.current_domain;
    }
    else {
      domainIndx = (DomainSelection) client_data;
    }

    /* set hour glass cursor */
    HGU_XmSetHourGlassCursor(globals.topl);

    /* check which domain is selected */
    switch( domainIndx ){
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
	obj = get_domain_from_object(globals.obj, domainIndx);
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
      WlzObject	*tmpObj;
      WlzValues	objVals;
      XmString	xmstr;

      /* check for empty */
      if( WlzIsEmpty(obj, NULL) ){
	WlzFreeObj(obj);
	HGU_XmUnsetHourGlassCursor(globals.topl);
	return;
      }

      /* get the file pointer */
      xmstr = XmStringCreateSimple(globals.domain_filename[domainIndx]);
      if((fp = HGU_XmGetFilePointerBckCnfm(globals.topl, xmstr, NULL,
					   "w", NULL, &saveYesToAll))
	 == NULL ){
	HGU_XmUserError(globals.topl,
			"Can't save domain to file\nplease check filename",
			XmDIALOG_FULL_APPLICATION_MODAL);
	WlzFreeObj( obj );
	HGU_XmUnsetHourGlassCursor(globals.topl);
	return;
      }
      XmStringFree(xmstr);

      objVals.core = NULL;
      if( globals.origObjType == WLZ_3D_DOMAINOBJ ){
	tmpObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, obj->domain, objVals,
			     NULL, NULL, NULL);
      }
      else {
	tmpObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, obj->domain.p->domains[0],
			     objVals, NULL, NULL, NULL);
      }
      if( WlzWriteObj(fp, tmpObj) == WLZ_ERR_NONE ){
	globals.domain_changed_since_saved[domainIndx] = 0;
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
      WlzFreeObj(tmpObj);
      WlzFreeObj( obj );
      if( fclose( fp ) == EOF ){
	HGU_XmUserError(globals.topl,
			"Save Domain:\n"
			"    Error closing the domain file.\n"
			"    Please check disk space or quotas.\n"
			"    Domain not saved",
			XmDIALOG_FULL_APPLICATION_MODAL);
      }
    }

    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
}

void save_all_domains_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
  int			i, num_overlays;

  num_overlays = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;

  saveYesToAll = -1;
  for(i=1; i <= num_overlays; i++)
  {
    save_domain_cb(w, (XtPointer) i, call_data);
  }

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
    Widget		toggle;

    /* check the reference object */
    if( globals.obj == NULL )
	return;

    /* set hour glass cursor */
    HGU_XmSetHourGlassCursor(globals.topl);

    /* get the toggle, this is probably w but we may wish to
       call this callback from elsewhere */
    toggle = XtNameToWidget(globals.topl, "*domain_menu*threed_display_domain");

    /* remove the display list if currently visible */
    if( globals.domain_display_list[globals.current_domain] != 0 )
    {
      glDeleteLists(globals.domain_display_list[globals.current_domain], 1);
      globals.domain_display_list[globals.current_domain] = 0;
      MAOpenGLDrawScene( globals.canvas );
      if( toggle ){
	XtVaSetValues(toggle, XmNset, False, NULL);
      }
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
    if( toggle ){
      XtVaSetValues(toggle, XmNset, True, NULL);
    }

    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
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

  /* update the FileSelectionBox */
  XmFileSelectionDoSearch( w, NULL );
  XtVaSetValues(w, XmNdirSpec, cbs->value, NULL);

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
     pattern_str = XmStringCreateSimple( "*.p?m" );
     break;
     
   case WLZEFF_FORMAT_BMP:
     pattern_str = XmStringCreateSimple( "*.bmp" );
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
  Widget		widget, form, file_type_menu;
  int			i, n;
  XtTranslations	trans_tb;
  Atom			import_list[1];
  Arg			args[8];
  Visual		*visual;

  /* set some defaults then get resources */
  globals.mask_domain    = NULL;
  globals.current_domain = DOMAIN_1;
  globals.current_col    = 64;
  globals.auto_increment = 0;
  globals.propogate      = 0;
  globals.propogate_sel  = 0;

  globals.gc_greys  = NULL;
  globals.gc_reds   = NULL;
  globals.gc_blues  = NULL;

  for(i=0; i < 33; i++)
  {
    globals.domain_display_list[i] = 0;
    globals.priority_to_domain_lut[i] = i;
    globals.domain_changed_since_saved[i] = 0;
  }
  MAPaintDomainGetResourceValues();

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

  /* add a form to hold checkboxes */
  form = XtVaCreateManagedWidget("read_domain_form", xmFormWidgetClass,
				 read_domain_dialog,
				 XmNborderWidth,	0,
				 NULL);
  widget = XtVaCreateManagedWidget("Check overlap",
				   xmToggleButtonGadgetClass, form,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_FORM,
				   NULL);
  widget = XtVaCreateManagedWidget("Replace existing domain",
				   xmToggleButtonGadgetClass, form,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	widget,
				   XmNleftAttachment,	XmATTACH_FORM,
				   NULL);
 
  XtAddCallback(read_domain_dialog, XmNcancelCallback, 
		PopdownCallback, NULL);
  XtAddCallback(read_domain_dialog, XmNokCallback, read_domain_cb, NULL);
  XtAddCallback(read_domain_dialog, XmNokCallback, display_all_views_cb,
		NULL);
  XtAddCallback(read_domain_dialog, XmNokCallback, PopdownCallback, NULL);
  XtAddCallback(read_domain_dialog, XmNmapCallback, FSBPopupCallback, NULL);
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
  XtAddCallback(write_domain_dialog, XmNmapCallback, FSBPopupCallback, NULL);
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
  XtAddCallback(write_paint_volume_dialog, XmNmapCallback,
		FSBPopupCallback, NULL);
  XtManageChild( write_paint_volume_dialog );

  HGU_XmSaveRestoreAddWidget( write_paint_volume_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  widget = XtNameToWidget( topl, "*domain_menu*write_paint_volume");
  if( widget != NULL )
    XtAddCallback(widget, XmNactivateCallback, popup_dialog_cb,
		  (XtPointer) write_paint_volume_dialog);

  /* Unmanage domain-menu buttons if not required */
  for(i=1; i < 33; i++){
    char	str_buf[64];

    /* get the widgets in priority order */
    sprintf(str_buf, "*menubar*domain_menu*select*domain_%d", i);
    if( widget = XtNameToWidget(topl, str_buf) ){
      if( i > (globals.cmapstruct->num_overlays +
	       globals.cmapstruct->num_solid_overlays) )
      {
	XtUnmanageChild(widget);
      }
    }
  }

  setMenuLabelsAndColors();

  return( 0 );
}
