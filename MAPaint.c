#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	MAPaint.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* $Revision$
* $Name$
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <MAPaint.h>
#include <MAPaintResources.h>
#include <MAPaintHelpResource.h>

/* global variables */
PaintGlobals	globals;
char		*initial_reference_file;
char		*initial_domain_file;
char		*release_str = "$Name$";

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
    char	butMap[10];
    int		numEl;

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

    /* get the button mapping */
    switch( numEl = XGetPointerMapping(XtDisplay(globals.topl), butMap, 10) ){
    case 1:
    case 3:
    default:
      break;

    case 2:
      HGU_XmUserMessage(globals.topl,
			"You have a two-button mouse, to get\n"
			"middle mouse-button events use the\n"
			" <alt> key with the left-button.\n"
			"E.g. to get anaomty and domain feedback\n"
			"press the <alt> key then press and drag\n"
			"the left mouse button over the image\n"
			"displayed in a view window\n",
			XmDIALOG_FULL_APPLICATION_MODAL);
      /*fprintf(stderr,
	      "Num map elements: %d\n"
	      "map[0] = %d\n"
	      "map[1] = %d\n"
	      "map[2] = %d\n"
	      "map[3] = %d\n"
	      "map[4] = %d\n", numEl, butMap[0], butMap[1],
	      butMap[2], butMap[3], butMap[4]);*/

      butMap[0] = 1;
      butMap[1] = 3;
      XSetPointerMapping(XtDisplay(globals.topl), butMap, numEl);
      break;
    }
      
    /* flush the X-request queue */
    XFlush(globals.dpy);
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
  char *errStr, *str;

  errStr = (char *) AlcMalloc(sizeof(char)*80);
  str = (char *) AlcMalloc(sizeof(char)*512);

  XGetErrorText(dpy, err_event->error_code, errStr, 80);
  sprintf(str,
	  "An X11 error has occurred:\n\n"
	  "X Error of failed request: %s\n"
	  "Value in failed request: 0x%x\n"
	  "Major opcode of failed request: %d (%s)\n"
	  "Minor opcode of failed request: %d\n"
	  "Serial number of request: %d\n\n"
	  "do you want to save domains\n"
	  "before attempting to continue?",
	  errStr, err_event->resourceid,
	  err_event->request_code,
	  HGU_XRequestToString(err_event->request_code),
	  err_event->minor_code,
	  err_event->serial);

  if( HGU_XmUserConfirm(globals.topl, str,
			"Yes", "No", 1) ){
    /* blah blah blah action here */
  }

  AlcFree(str);
  AlcFree(errStr);
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
    autosave_all_domains();
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
    autosave_all_domains();
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
  {"-listen",	"*view_dialog*listen.sensitive", XrmoptionNoArg, "True"},
  {"-24bit",	".24bit", XrmoptionNoArg, "True"},
  {"-cdrom",	".theilerDir", XrmoptionSepArg, NULL},
  {"-theilerStage",	".theilerStage", XrmoptionSepArg, NULL},
  {"-stage",	".theilerStage", XrmoptionSepArg, NULL},
  {"-help",	"*help_menu.sensitive", XrmoptionNoArg, "True"},
};

main(
  int	argc,
  char	**argv)
{
  Widget		topl, main_w, buttonbar, menubar, work_area;
  Widget		xmdisplay;
  XtAppContext		app_con;
  XtTranslations	translations;
  Atom			WM_DELETE_WINDOW;
  Display		*dpy;
  Visual		*visual;
  Arg			arg[3];
  Boolean		d24Flg=False;
  XrmValue		xrmValue;
  char			*rtnStrType;
  char			*nameStr, *depthRscStr;
  char			butMap[10];
  int			numEl;
  
  /* get the application name in case it is not MAPaint */
  if( argv[0] ){
    int i;
    nameStr = argv[0];
    for(i=strlen(argv[0]); i > 0; i--){
      if( argv[0][i-1] == '/' ){
	nameStr = (argv[0]) + i;
	break;
      }
    }
  }
  else {
    nameStr = "MAPaint";
  }
  depthRscStr = (char *) AlcMalloc(sizeof(char) * (strlen(nameStr) +10));
  sprintf(depthRscStr, "%s.24bit", nameStr);

  /* create the top level shell */
  XtToolkitInitialize();
  app_con = XtCreateApplicationContext();
  XtAppSetFallbackResources(app_con, fallback_resources);
  dpy = XtOpenDisplay(app_con, NULL, nameStr, "MAPaint", mapaint_options, 8,
		      &argc, argv);
  globals.dpy = dpy;

  /* now check what visuals are available to determine the mode */
  if( XrmGetResource(XtDatabase(dpy), depthRscStr, "MAPaint.24bit",
		     &rtnStrType, &xrmValue) == True ){
    if( !strcmp(xrmValue.addr, "True") ){
      d24Flg = True;
    }
  }
  if(!(visual = HGU_XGetVisual(dpy, DefaultScreen(dpy), PseudoColor, 8)) ||
     (d24Flg == True)){
    if( visual = HGU_XGetVisual(dpy, DefaultScreen(dpy), TrueColor, 24) ){
      globals.visualMode = MAPAINT_24BIT_ONLY_MODE;
      globals.toplDepth = 24;
      globals.toplVisual = visual;
      globals.warpVisual = visual;
    }
    else {
      fprintf(stderr,
	      "%s: can't get required 8-bit or 24-bit visual.\n"
	      "MAPaint requires either an 8-bit and/or a 24bit visual\n"
	      "for operation. Please check if your workstation can be set\n"
	      "to 8-bit or 24-bit mode (full operation requires 24-bit)\n"
	      "If only 8-bit is available then warp input will be\n"
	      "disabled\n",
	      argv[0]);
      return 1;
    }
  }
  else {
    globals.toplDepth = 8;
    globals.toplVisual = visual;
    globals.visualMode = MAPAINT_8BIT_ONLY_MODE;
    if( visual = HGU_XGetVisual(dpy, DefaultScreen(dpy), TrueColor, 24) ){
      globals.visualMode = MAPAINT_8_24BIT_MODE;
      globals.warpVisual = visual;
    }
  }
  
  /* also need to create and set the colormap */
  if( globals.toplDepth == 8 ){
    HGU_XmCreatePrivateColormap(dpy);
  }
  else {
    globals.cmap = XCreateColormap(dpy, DefaultRootWindow(dpy),
				   globals.toplVisual, AllocNone);
  }
  XtSetArg(arg[0], XtNdepth, globals.toplDepth);
  XtSetArg(arg[1], XmNvisual, globals.toplVisual);
  XtSetArg(arg[2], XtNcolormap, globals.cmap);

  /* finally create the top-level shell */
  topl = XtAppCreateShell(NULL, "MAPaint", applicationShellWidgetClass,
			  dpy, arg, 3);
  
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
  globals.app_name = "MAPaint";
  globals.sectViewFlg = 0;

  /* initialise the colormap */
  init_paint_cmapstruct( topl );

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
