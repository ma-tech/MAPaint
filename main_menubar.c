#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	main_menubar.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>

#include <MAPaint.h>

extern MenuItem		*HGU_XmMiscMenuItems;

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

static void test_activate_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  fprintf(stderr, "called test_activate_cb\n");
  return;
}

Widget create_main_menubar(
Widget	main_w)
{
    Widget	menubar, menu, widget;

    menubar = XmCreateMenuBar(main_w, "menubar", NULL, 0 );

    /* create pulldown menus */
    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "file_menu",
			     0, XmTEAR_OFF_DISABLED, file_menu_items);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "paint/paint.html#file_menu");

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "anatomy_menu",
			     0, XmTEAR_OFF_DISABLED, anatomy_menu_items);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "paint/paint.html#anatomy_menu");
    XtSetSensitive(widget, False);

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "domain_menu",
			     0, XmTEAR_OFF_ENABLED, domain_menu_items);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "paint/paint.html#domain_menu");
			  
    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "view_menu",
			     0, XmTEAR_OFF_DISABLED, view_menu_items);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "paint/paint.html#view_menu");

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "options_menu",
			     0, XmTEAR_OFF_ENABLED, options_menu_items);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "paint/paint.html#options_menu");
			  
    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "misc_menu",
			     0, XmTEAR_OFF_ENABLED,
			     HGU_XmMiscMenuItems);
/*    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "libhguXm/HGU_XmMacroMenu.html");*/

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "help_menu",
			     0, XmTEAR_OFF_DISABLED, HGU_XmHelpMenuItems);
    XtAddCallback(menubar, XtNdestroyCallback, HGU_XmHelpDestroyCb, NULL);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
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
