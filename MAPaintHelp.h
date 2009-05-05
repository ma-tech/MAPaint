#ifndef MAPAINT_HELP_H
#define MAPAINT_HELP_H

#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MAPaintHelp.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MAPaintHelp.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:18:05 2009
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

#include <X11/Intrinsic.h>

#define SEC_GENERATING_PRINTED_VIEWS \
"paint_help/node57.html#sec_generating_printed_views"
#define SEC_MACRO_MENU \
"paint_help/node41.html#sec_macro_menu"
#define SEC_VIEW_DIALOG \
"paint_help/node48.html#sec_view_dialog"
#define SEC_COLORMAP_DIALOG \
"paint_help/node53.html#sec_colormap_dialog"
#define SEC_DOMAIN_MENU \
"paint_help/node37.html#sec_domain_menu"
#define SEC_ANATOMY_MENU \
"paint_help/node36.html#sec_anatomy_menu"
#define SEC_VIEW_MENU \
"paint_help/node38.html#sec_view_menu"
#define SEC_USING_GRAPHICS_TABLET \
"paint_help/node55.html#sec_using_graphics_tablet"
#define SEC_READ_WRITE_DOMAINS_DIALOGS \
"paint_help/node46.html#sec_read_write_domains_dialogs"
#define SEC_TOOLS_CONTROLS_DIALOG \
"paint_help/node49.html#sec_tools_controls_dialog"
#define SEC_FILE_MENU \
"paint_help/node35.html#sec_file_menu"
#define SEC_WRITE_PAINTED_VOLUME_DIALOG \
"paint_help/node47.html#sec_write_painted_volume_dialog"
#define SEC_AUTOSAVE_DIALOG \
"paint_help/node54.html#sec_autosave_dialog"
#define SEC_DOMAIN_SURGERY_DIALOG \
"paint_help/node51.html#sec_domain_surgery_dialog"
#define SEC_DOMAIN_REVIEW_DIALOG \
"paint_help/node50.html#sec_domain_review_dialog"
#define SEC_AUTOSEGMENT_DIALOG \
"paint_help/node52.html#sec_autosegment_dialog"
#define SEC_PULL_DOWN_MENUS \
"paint_help/node34.html#sec_pull_down_menus"
#define SEC_HELP_MENU \
"paint_help/node42.html#sec_help_menu"
#define SEC_OPTIONS_MENU \
"paint_help/node39.html#sec_options_menu"
#define SEC_MISC_MENU \
"paint_help/node40.html#sec_misc_menu"
#define SEC_CALIBRATING_TABLET_COORDS \
"paint_help/node58.html#sec_calibrating_tablet_coords"
#define SEC_INPUT_DATA_FORMATS \
"paint_help/node29.html#sec_input_data_formats"
#define SEC_OUTPUT_DATA_FORMATS \
"paint_help/node30.html#sec_output_data_formats"

typedef struct {
	String	sec_generating_printed_views;
	String	sec_macro_menu;
	String	sec_view_dialog;
	String	sec_colormap_dialog;
	String	sec_domain_menu;
	String	sec_anatomy_menu;
	String	sec_view_menu;
	String	sec_using_graphics_tablet;
	String	sec_read_write_domains_dialogs;
	String	sec_tools_controls_dialog;
	String	sec_file_menu;
	String	sec_write_painted_volume_dialog;
	String	sec_autosave_dialog;
	String	sec_domain_surgery_dialog;
	String	sec_domain_review_dialog;
	String	sec_autosegment_dialog;
	String	sec_pull_down_menus;
	String	sec_help_menu;
	String	sec_options_menu;
	String	sec_misc_menu;
	String	sec_calibrating_tablet_coords;
	String	sec_input_data_formats;
	String	sec_output_data_formats;
} MAPaintHelpResources;

extern MAPaintHelpResources helpResources;

#endif /* MAPAINT_HELP_H */
