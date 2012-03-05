#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _main_menubar_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         main_menubar.c
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

#include <MAPaint.h>

extern MenuItem		*HGU_XmMiscMenuItems;

void MAPaintVersionCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  String	versionStr;

  if((versionStr = AlcMalloc(sizeof(char)*(strlen(release_str) + 256)))){
    sprintf(versionStr, "Release:\n\%s\n\n%s", release_str,
	    "MAPaint has been developed at the MRC Human\n"
	    "Genetics Unit, Edinburgh as part of the Mouse\n"
	    "Atlas research programme. Menu, button and\n"
	    "context help will be provided in the next\n"
	    "release.\n\n"
	    "Copyright: Medical Research Council (UK), 2004");
    /* put version and text onto a simple widget */
    HGU_XmUserMessage(globals.topl, versionStr,
		      XmDIALOG_FULL_APPLICATION_MODAL);
    AlcFree(versionStr);
  }

  return;
}

static void HGU_XmPulldownAddHelp(
Widget		w,
XtCallbackProc	callback,
XtPointer	client_data)
{
    /* add the help callback to the button */
    XtAddCallback(w, XmNhelpCallback, callback, client_data);

    /* add the help callback to the menu itself */
    if( (w = XtNameToWidget(w, "*_pulldown")) != NULL )
	XtAddCallback(w, XmNhelpCallback, callback, client_data);

    return;
}

Widget create_main_menubar(
Widget	main_w)
{
    Widget	menubar, menu, widget;
    MenuItem	*helpMenuItem;

    menubar = XmCreateMenuBar(main_w, "menubar", NULL, 0 );

    /* create pulldown menus */
    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "file_menu",
			     0, XmTEAR_OFF_DISABLED, file_menu_items);
    HGU_XmPulldownAddHelp(widget, myHGU_XmHelpStandardCb,
			  "paint/paint.html#file_menu");
    if((menu = XtNameToWidget(widget, "*_pulldown"))){
      XtAddCallback(XtParent(menu), XmNpopupCallback,
		    fileMenuPopupCb, menu);
    }

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "anatomy_menu",
			     0, XmTEAR_OFF_DISABLED, anatomy_menu_items);
    HGU_XmPulldownAddHelp(widget, myHGU_XmHelpStandardCb,
			  "paint/paint.html#anatomy_menu");
    XtSetSensitive(widget, False);

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "domain_menu",
			     0, XmTEAR_OFF_ENABLED, domain_menu_items);
    HGU_XmPulldownAddHelp(widget, myHGU_XmHelpStandardCb,
			  "paint/paint.html#domain_menu");
			  
    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "view_menu",
			     0, XmTEAR_OFF_DISABLED, view_menu_items);
    HGU_XmPulldownAddHelp(widget, myHGU_XmHelpStandardCb,
			  "paint/paint.html#view_menu");

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "options_menu",
			     0, XmTEAR_OFF_ENABLED, options_menu_items);
    HGU_XmPulldownAddHelp(widget, myHGU_XmHelpStandardCb,
			  "paint/paint.html#options_menu");

    /* need to tune the help menu here
       change version item to pop-up a window with built-in
       version number */
    helpMenuItem = HGU_XmHelpMenuItems;
    while( helpMenuItem->name ){
      if( !strcmp(helpMenuItem->name, "version") ){
	helpMenuItem->callback = MAPaintVersionCb;
	helpMenuItem->callback_data = NULL;
	break;
      }
      helpMenuItem++;
    }
			  
    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "misc_menu",
			     0, XmTEAR_OFF_ENABLED,
			     HGU_XmMiscMenuItems);
/*    HGU_XmPulldownAddHelp(widget, myHGU_XmHelpStandardCb,
			  "libhguXm/HGU_XmMacroMenu.html");*/

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "help_menu",
			     0, XmTEAR_OFF_DISABLED, HGU_XmHelpMenuItems);
    XtAddCallback(menubar, XtNdestroyCallback, HGU_XmHelpDestroyCb, NULL);
    HGU_XmPulldownAddHelp(widget, myHGU_XmHelpStandardCb,
			  "libhguXm/HGU_XmHelpMenu.html");

    XtVaSetValues(menubar, XmNmenuHelpWidget, widget, NULL);

    XtManageChild( menubar );
    return( menubar );
}

void init_main_menubar(
Widget	topl)
{
    (void) file_menu_init( topl );
    (void) anatomy_menu_init( topl );
    (void) domain_menu_init( topl );
    (void) view_menu_init( topl );
    (void) options_menu_init( topl );
    (void) HGU_XmHelpMenuInit( topl );
}
