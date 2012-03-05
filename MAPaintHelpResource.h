#ifndef MAPAINT_HELP_RESOURCE_H
#define MAPAINT_HELP_RESOURCE_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAPaintHelpResource_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAPaintHelpResource.h
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
*/

#include <X11/Intrinsic.h>

#include <MAPaintHelp.h>

#define help_res_offset(field)   XtOffsetOf(MAPaintHelpResources, field)

static XtResource help_res[] = {

	{"sec_generating_printed_views", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_generating_printed_views),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_macro_menu", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_macro_menu),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_view_dialog", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_view_dialog),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_colormap_dialog", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_colormap_dialog),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_domain_menu", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_domain_menu),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_anatomy_menu", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_anatomy_menu),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_view_menu", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_view_menu),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_using_graphics_tablet", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_using_graphics_tablet),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_read_write_domains_dialogs", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_read_write_domains_dialogs),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_tools_controls_dialog", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_tools_controls_dialog),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_file_menu", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_file_menu),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_write_painted_volume_dialog", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_write_painted_volume_dialog),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_autosave_dialog", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_autosave_dialog),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_domain_surgery_dialog", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_domain_surgery_dialog),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_domain_review_dialog", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_domain_review_dialog),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_autosegment_dialog", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_autosegment_dialog),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_pull_down_menus", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_pull_down_menus),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_help_menu", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_help_menu),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_options_menu", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_options_menu),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_misc_menu", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_misc_menu),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_calibrating_tablet_coords", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_calibrating_tablet_coords),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_input_data_formats", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_input_data_formats),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

	{"sec_output_data_formats", "MAPaintDefaultURL",
	 XtRString, sizeof(String),
	 help_res_offset(sec_output_data_formats),
	 XtRString, "http://genex.hgu.mrc.ac.uk/Software/Help"},

};

MAPaintHelpResources helpResources;

#endif /* MAPAINT_HELP_RESOURCE_H */
