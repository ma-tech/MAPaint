#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	MAPaint.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <MAPaint.h>
#include <MAPaintResources.h>
#include <MAPaintHelpResource.h>

/* global variables */
PaintGlobals	globals;
char		*initial_reference_file;
char		*initial_domain_file;

static int set_MainWindow_XSizeHints	(Widget	main_w);
static String translations_table =
	"<Map>: InitialiseTopl()";

static void StartDragDrop(
Widget		w,
XEvent		*event,
String		*params,
Cardinal	*num_params)
{
  if( *num_params )
  {
    if( strcmp(*params, "DomainDominance") == 0 )
    {
      StartDD_DomainDominance(w, event, params, num_params);
    }
  }

  return;
}

static void InitialiseTopl(
Widget		w,
XEvent		*event,
String		*params,
Cardinal	*num_params)
{
    Widget	widget;

    static	initialised=0;

    if( initialised )
	return;
    initialised = 1;

    /* set the WM size hints */
/*    set_MainWindow_XSizeHints( w );*/

    /* initialise the work area */
    (void) init_main_work_area( w );

    /* initialise the menu bar */
    (void) init_main_menubar( w );

    /* initialise the button bar */
    (void) init_main_buttonbar( w );
}

void clear_autosave(void)
{
    int		i, clean_file = 1;
    char	str_buf[128];
    String	str;

    /* check each domain */
    for(i=1; i < 33; i++){
      int	domain;

      domain = globals.priority_to_domain_lut[i];

      if( globals.domain_changed_since_saved[domain] )
      {
	if( HGU_XmUserConfirm(globals.topl,
			      "Domains defined but not saved do\n"
			      "you want to keep the autosave file\n"
			      "after quitting?",
			      "Yes", "No", 1) )
	{
	  clean_file = 0;
	}
	break;
      }
    }

    if( clean_file && globals.autosave_file )
    {
      sprintf(str_buf, "rm -f %s", globals.autosave_file);
      system((const char *) str_buf);
    }

    return;
}

void save_domains(void)
{
    int		i;
    char	str_buf[128];
    String	str;

    /* check each domain */
    for(i=1; i < 33; i++){
      int	domain;

      domain = globals.priority_to_domain_lut[i];

      if( !globals.domain_changed_since_saved[domain] )
	continue;

      sprintf(str_buf,
	      "%s has been modified\ndo you want to save domain to:\n"
	      "(modify filename as required)",
	      globals.domain_name[domain]);
      if( (str = HGU_XmUserGetFilename(globals.topl, str_buf, "Yes", "No",
				       globals.domain_filename[domain],
				       NULL, "*.wlz")) == NULL )
	continue;

      globals.current_priority = i;
      globals.current_domain = domain;
      globals.domain_filename[domain] = str;
      globals.current_col =
	globals.cmapstruct->ovly_cols[globals.current_domain];
      save_domain_cb(globals.topl, NULL, NULL);
    }

    return;
}

int XNonFatalErrorHandler(
Display		*dpy,
XErrorEvent	*err_event)
{
    if( HGU_XmUserConfirm(globals.topl,
			  "An X11 error has occurred, do\n"
			  "you want to try and save domains\n"
			  "before attempting to continue?",
			  "Yes", "No", 1) ){

	save_domains();
	clear_autosave();
    }

    return( 0 );
}

/* Warning - this is not consistent with the Xlib manual but
   as defined in Xlib.h
   */
int	XFatalErrorHandler(
Display		*dpy)
{
    if( HGU_XmUserConfirm(globals.topl,
			  "A fatal X11 error has occurred, do\n"
			  "you want to try and save domains\n"
			  "before quitting?",
			  "Yes", "No", 1) ){

	save_domains();
	clear_autosave();
    }

    /* kill the help viewer */
    HGU_XmHelpDestroyViewer();

    XtDestroyWidget( globals.topl );
    exit( 1 );
    return( 0 );
}

void non_abort_signal_handler(int sig)
{
    signal( sig, SIG_IGN );
    quit_cb(globals.topl, (XtPointer) globals.topl, NULL);
    signal( sig, non_abort_signal_handler );
    return;
}

void abort_signal_handler(int sig)
{
    if( HGU_XmUserConfirm(globals.topl,
			  "Signal SIGSEGV (fatal) received,\n"
			  "do you want to try and save domains?",
			  "Yes", "No", 1) )
    {
	save_domains();
	clear_autosave();
    }

    if( HGU_XmUserConfirm(globals.topl,
			  "Save a core file?",
			  "Yes", "No", 1) )
	abort();
	
    exit( 1 );
}

static XtActionsRec actions[] = {
{"InitialiseTopl",	InitialiseTopl},
{"ViewFeedback",	ViewFeedback},
{"StartDragDrop",	StartDragDrop},
{NULL,			NULL},
};

static XrmOptionDescRec mapaint_options[] = {
  {"-warpInput","*options_menu*warp_input_2d.sensitive",
   XrmoptionNoArg, "True"},
  {"-realign",	"*options_menu*realignment.sensitive",
   XrmoptionNoArg, "True"},
};

main(
int	argc,
char	**argv)
{
    Widget		topl, main_w, buttonbar, menubar, work_area;
    Widget		xmdisplay;
    XtAppContext	app_con;
    XtTranslations	translations;
    Atom		WM_DELETE_WINDOW;

    /* create the top level shell */
    topl = XtAppInitialize(&app_con, "MAPaint",
			   mapaint_options, XtNumber(mapaint_options),
			   &argc, argv, fallback_resources,
			   NULL, 0);

    /* check for command line reference image */
    if( argc > 1 ){
      initial_reference_file = argv[1];
    }
    else {
      initial_reference_file = NULL;
    }

    if( argc > 2 ){
      initial_domain_file = argv[2];
    }
    else {
      initial_domain_file = NULL;
    }

    /* add some additional translations to initialise the process when the
       top-level widget is mapped */
    translations = XtParseTranslationTable( translations_table );
    XtAugmentTranslations( topl, translations );

    globals.topl    = topl;
    globals.app_con = app_con;

    /* install a private colormap */
    HGU_XmCreatePrivateColormap( topl );

    /* initialise the save-restore facility */
    HGU_XmSaveRestoreInit( topl, &argc, argv );
    HGU_XmSaveRestoreHelpCallback(topl, HGU_XmHelpStandardCb,
				  "libhguXm/HGU_XmSaveRestore.html");

    /* create the motif main window */
    main_w = XtVaCreateManagedWidget("main_w", xmMainWindowWidgetClass,
				     topl,
				     XmNcommandWindowLocation,
				     XmCOMMAND_BELOW_WORKSPACE,
				     NULL);
    XtAddCallback(main_w, XmNhelpCallback, HGU_XmHelpStandardCb,
		  "paint/paint.html");

    /* create the menu bar */
    menubar = create_main_menubar( main_w );

    /* create the work area */
    work_area = create_main_work_area( main_w );
    
    /* create the command push buttons */
    buttonbar = create_main_buttonbar( main_w );
    XtVaSetValues(main_w, XmNcommandWindow, buttonbar, NULL);

    /* add application action procedures */
    XtAppAddActions(app_con, actions, XtNumber(actions));

    /* install signal and error handlers */
    signal(SIGHUP,  non_abort_signal_handler);
    signal(SIGINT,  non_abort_signal_handler);
    signal(SIGQUIT, abort_signal_handler);
    signal(SIGBUS,  abort_signal_handler);
    signal(SIGSEGV, abort_signal_handler);
    signal(SIGSYS,  abort_signal_handler);
    (void) XSetErrorHandler( XNonFatalErrorHandler );
    (void) XSetIOErrorHandler( XFatalErrorHandler );
    
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(topl),
				    "WM_DELETE_WINDOW", False);
    XmAddWMProtocols(topl, &WM_DELETE_WINDOW, 1);
    XmAddWMProtocolCallback(topl, WM_DELETE_WINDOW, quit_cb, (XtPointer) topl);

    XtRealizeWidget( topl );
    XtAppMainLoop( app_con );

    return( 0 );
}

static int set_MainWindow_XSizeHints(
Widget	main_w)
{
    Widget	widget;
    XSizeHints	xsizehints;
    Dimension	w, h;

    /* get the menubar */
    if( (widget = XtNameToWidget(main_w, "*.menubar")) == NULL )
    {
	xsizehints.max_width = 0;
	xsizehints.max_height = 0;
    } else
    {
	XtVaGetValues(widget, XmNwidth, &w, XmNheight, &h, NULL);
	xsizehints.max_width = w;
	xsizehints.max_height = h;
    }

    /* get the work_area */
    if( (widget = XtNameToWidget(main_w, "*.work_area")) != NULL )
    {
	XtVaGetValues(widget, XmNwidth, &w, XmNheight, &h, NULL);
	if( xsizehints.max_width < (int) w )
	    xsizehints.max_width = w;
	xsizehints.max_height += h;
    }
 
    /* get the buttonbar */
    if( (widget = XtNameToWidget(main_w, "*.buttonbar")) != NULL )
    {
	XtVaGetValues(widget, XmNwidth, &w, XmNheight, &h, NULL);
	if( xsizehints.max_width < (int) w )
	    xsizehints.max_width = w;
	xsizehints.max_height += h;
    }
 
    xsizehints.max_width += 8;
    xsizehints.max_height += 12;
    xsizehints.flags = PMaxSize;
    XSetWMNormalHints(XtDisplay(main_w), XtWindow(main_w), &xsizehints);
    return( 0 );
}
