#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   							*
*   File       :   MAAutosaveDialog.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Mar  6 10:23:32 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

Widget	autosave_dialog=NULL;

void autosave_opts_cb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  /* put up the autosave dialog */
  PopupCallback(w, (XtPointer) XtParent(autosave_dialog), NULL);

  return;
}

void autosave_all_domains(void)
{
  FILE		*fp;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;

  /* get the autosave file */
  if( (fp = fopen(globals.autosave_file, "wb")) != NULL )
  {

    /* save the object data  - this could be more efficient (not much) */
    if( (errNum = WlzWriteObj(fp, globals.obj)) != WLZ_ERR_NONE ){
      char		*errstr;
      const char	*errMsg;
      (void )WlzStringFromErrorNum(errNum, &errMsg);
      errstr = (char *) AlcMalloc(strlen(errMsg) + 128);
      sprintf(errstr,
	      "Write Autosave File:\n"
	      "    woolz error detected:\n"
	      "    %s\n"
	      "    WARNING: data not saved\n"
	      "    - please check disc space or quotas",
	      errMsg);
      HGU_XmUserError(globals.topl, errstr,
		      XmDIALOG_FULL_APPLICATION_MODAL);
      AlcFree( (void *) errstr );
    }
      
    /* save supplementary information  - not required on read but useful */
    for(i=1; i < 33; i++){
      if( globals.domain_name[i] ){
	fprintf(fp, "%d:%s\n", i, globals.domain_name[i]);
      }
      else {
	fprintf(fp, "%d:no name\n", i);
      }
    }

    if( fclose( fp ) == EOF ){
      HGU_XmUserError(globals.topl,
		      "Write Autosave File:\n"
		      "    Error closing autosave file\n"
		      "    please check disc space or quotas\n"
		      "    WARNING: data not saved",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }

  }
  else
  {
    HGU_XmUserError(globals.topl,
		    "Autosave:\n"
		    "    Couldn't open the autosave file\n"
		    "     - please check pathname or permissions\n",
		    XmDIALOG_FULL_APPLICATION_MODAL);
  }

  return;
}

void autosavetimeout_cb(
  XtPointer		client_data,
  XtIntervalId	*id)
{
  Widget	dialog;
  XmString	xmstr1, xmstr2;
  XEvent	event;
  int		i, save_required = 0, finished;
  Visual	*visual;
  Arg		arg[1];

  /* check object */
  if( (*id != globals.autosavetimeoutID) ){
    fprintf(stderr, "autosavetimeout_cb: wrong Id\n");
    return;
  }

  /* check if save required */
  for(i=1; i < 33; i++){
    save_required |= globals.domain_changed_since_saved[i];
  }

  if( save_required ){

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(globals.topl);
    XtSetArg(arg[0], XmNvisual, visual);

    /* dump the current object */
    dialog = XmCreateWorkingDialog(globals.topl, "autosave_working_dialog",
				   arg, 1);

    XtUnmanageChild(XmMessageBoxGetChild(dialog,
					 XmDIALOG_OK_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog,
					 XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog,
					 XmDIALOG_HELP_BUTTON));
    xmstr1 = XmStringCreateSimple("Autosave - please wait.");
    xmstr2 = XmStringCreateSimple("Autosave");
    XtVaSetValues(dialog,
		  XmNmessageString, xmstr1,
		  XmNdialogTitle, xmstr2,
		  XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL);
    XmStringFree( xmstr1 );
    XmStringFree( xmstr2 );

    XtManageChild( dialog );

    /* popup widget and process events until fully exposed */
    PopupCallback( XtParent(dialog), (XtPointer) XtParent(dialog), NULL );
    finished = 0;
    while( !finished ){
      XtAppNextEvent( globals.app_con, &event );
      XtDispatchEvent( &event );
      XSync( XtDisplay(dialog), 0 );
      if( event.xany.type == Expose
	 && event.xany.window == XtWindow(dialog) )
      {
	finished = 1;
      }
    }

    autosave_all_domains();
 
    /* remove dialog, flush display and destroy resources */
    PopdownCallback(XtParent(dialog), (XtPointer)XtParent(dialog), NULL);
    XSync(XtDisplay(dialog), 0);
    XmUpdateDisplay(dialog);
 
    XtDestroyWidget(dialog);

  }

  /* set the next timeout */
  globals.autosavetimeoutID =
    XtAppAddTimeOut(globals.app_con, globals.autosave_time*1000,
		    autosavetimeout_cb, NULL);

  return;
}

static void autosave_on_cb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	control, dialog = (Widget) client_data;

  /* make the controls sensitive */
  if( (control = XtNameToWidget(dialog, "*.On")) )
  {
    XtSetSensitive(control, False);
  }
  if( (control = XtNameToWidget(dialog, "*.Off")) )
  {
    XtSetSensitive(control, True);
  }

  /* kill the existing timeout */
  if( globals.autosavetimeoutID )
  {
    XtRemoveTimeOut( globals.autosavetimeoutID );
  }

  /* set the next timeout */
  globals.autosavetimeoutID =
    XtAppAddTimeOut(globals.app_con, globals.autosave_time*1000,
		    autosavetimeout_cb, NULL);

  return;
}
  
static void autosave_off_cb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	control, dialog = (Widget) client_data;

  /* kill the existing timeout */
  if( globals.autosavetimeoutID )
  {
    XtRemoveTimeOut( globals.autosavetimeoutID );
  }
  globals.autosavetimeoutID = 0;

  /* make the controls insensitive */
  if( (control = XtNameToWidget(dialog, "*.Off")) )
  {
    XtSetSensitive(control, False);
  }
  if( (control = XtNameToWidget(dialog, "*.On")) )
  {
    XtSetSensitive(control, True);
  }

  return;
}
  
static void autosave_time_cb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	slider, dialog = (Widget) client_data;

  if( (slider = XtNameToWidget(dialog, "*.autosave_time")) == NULL )
    return;

  globals.autosave_time = (unsigned long) HGU_XmGetSliderValue( slider );

  autosave_on_cb(w, client_data, call_data);
  return;
}

static void autosave_file_cb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	widget, dialog = (Widget) client_data;

  if( (widget = XtNameToWidget(dialog, "*.autosave_file")) == NULL )
    return;

  globals.autosave_file = HGU_XmGetTextLineValue( widget );
  return;
}

static void autosave_recover_install_cb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  Widget	widget, dialog = (Widget) client_data;
  Widget	toggle=NULL;
  WlzObject	*domain;

  /* get the selected recovered domain */
  if( (widget = XtNameToWidget(dialog, "*.control")) == NULL )
    return;

  XtVaGetValues(widget, XmNmenuHistory, &toggle, NULL);
  if( toggle == NULL )
    return;

  XtVaGetValues(toggle, XmNuserData, &domain, NULL);

  /* add to the current domain */
  setDomain(domain, globals.current_domain, NULL);

  return;
}

static ActionAreaItem   autosave_recover_dialog_actions[] = {
  {"Install",		NULL,		NULL},
  {"Cancel",		PopdownCallback,		NULL},
  {"Help",        	NULL,           NULL},
};

static Widget create_autosave_domains_dialog(
  Widget	topl,
  WlzObject	**domains,
  String	*domain_names)
{
  Widget	dialog, control, toggle, install, cancel;
  String	name;
  int		i;

  dialog = HGU_XmCreateStdDialog(topl, "autosave_domains_dialog",
				 xmRowColumnWidgetClass,
				 autosave_recover_dialog_actions, 3);

  if( (install = XtNameToWidget(dialog, "*.Install")) != NULL ){
    XtAddCallback(install, XmNactivateCallback, autosave_recover_install_cb,
		  dialog);
    XtAddCallback(install, XmNactivateCallback, display_all_views_cb,
		  NULL);
    HGU_XmAddToolTip(globals.topl, install,
		     "Press to install the selected domain"
		     "into the current-domain");
  }

  control = XtNameToWidget( dialog, "*.control" );
  XtVaSetValues(control,
		XmNradioBehavior,	True,
		XmNradioAlwaysOne,	True,
		XmNorientation,		XmVERTICAL,
		XmNnumColumns,		4,
		XmNpacking,		XmPACK_COLUMN,
		NULL);
		  
  /* create the toggles */
  for(i=1; i < 33; i++)
  {
    /* only show defined domains */
    if( domains[i] == NULL ){
      continue;
    }

    if( domain_names[i] )
    {
      name = domain_names[i];
    }
    else
    {
      name = globals.domain_name[i];
    }
    toggle = XtVaCreateManagedWidget(name, xmToggleButtonGadgetClass,
				     control,
				     XmNuserData, (XtPointer) domains[i],
				     NULL);

    XtSetSensitive(toggle, (domains[i]==NULL)?False:True);
  }
				       
  return( dialog );
}

static Boolean	autosave_recover_flag=False;

static void autosave_recover_cb(
  Widget			w,
  XtPointer		client_data,
  XtPointer		call_data)
{
  WlzObject		*obj;
  WlzPlaneDomain	*new_pdom, *old_pdom;
  FILE			*fp;
  int			i, numDomains;
  WlzObject		*new_domains[33];
  WlzObject		*allDomainsObj, *tmpObj;
  String		new_domain_names[33];
  int			domain;
  Widget		widget, dialog = (Widget) client_data;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  WlzPixelV		thresh;
  char			lineBuf[128];


  /* check if an autosave is in progress - to catch double clicks */
  if( autosave_recover_flag == True )
  {
    return;
  }
  else
  {
    autosave_recover_flag = True;
  }

  /* get the file pointer */
  if( (widget = XtNameToWidget(dialog, "*.autosave_recover_file")) == NULL )
  {
    autosave_recover_flag = False;
    return;
  }

  if( (fp = fopen(HGU_XmGetTextLineValue( widget ), "rb")) == NULL )
  {
    HGU_XmUserError(globals.topl,
		    "Autosave recover:\n"
		    "    Couldn't open the autosave file\n"
		    "     - please check pathname or permissions\n",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    autosave_recover_flag = False;
    return;
  }

  /* read the required object */
  if( (obj = WlzReadObj(fp, &errNum)) == NULL )
  {
    char	*errstr;
    const char	*errMsg;
    (void )WlzStringFromErrorNum(errNum, &errMsg);
    errstr = (char *) AlcMalloc(strlen(errMsg) + 128);
    sprintf(errstr,
	    "Read Autosave File:\n"
	    "    woolz error detected:\n"
	    "    %s\n"
	    "    Please try an alternative file",
	    errMsg);
    HGU_XmUserError(globals.topl, errstr,
		    XmDIALOG_FULL_APPLICATION_MODAL);
    AlcFree( (void *) errstr );
    (void) fclose( fp );
    autosave_recover_flag = False;
    return;
  }

  /* read the save information and domain names */
  for(i=0; i < 33; i++)
  {
    new_domain_names[i] = NULL;
  }
  while( fgets(lineBuf, 127, fp) ){
    int		index;
    char	*nameStr=NULL;

    /* get the index */
    if( sscanf(lineBuf, "%d", &index) ){
      for(i=0; i < strlen(lineBuf); i++){
	if( lineBuf[i] == ':' ){
	  nameStr = &(lineBuf[i+1]);
	}
	if( lineBuf[i] == '\n' ){
	  lineBuf[i] == '\0';
	  break;
	}
      }
      if( nameStr ){
	new_domain_names[index] = AlcStrDup(nameStr);
      }
    }
    else {
      continue;
    }
  }

  (void) fclose( fp );

  /* check the object */
  if((obj->type != WLZ_3D_DOMAINOBJ) ||
     (obj->domain.core == NULL) ||
     (obj->domain.i->type != WLZ_PLANEDOMAIN_DOMAIN) ||
     (obj->values.core == NULL) )
  {
    HGU_XmUserError(globals.topl,
		    "Autosave recover:\n"
		    "    Wrong object type or NULL\n"
		    "    domain or valuetable\n"
		    "     - please check the filename",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    WlzFreeObj( obj );
    autosave_recover_flag = False;
    return;
  }
    

  /* check bounding box */
  new_pdom = obj->domain.p;
  old_pdom = globals.obj->domain.p;
  if( new_pdom->plane1 != old_pdom->plane1 ||
     new_pdom->lastpl != old_pdom->lastpl ||
     new_pdom->line1  != old_pdom->line1 ||
     new_pdom->lastln != old_pdom->lastln ||
     new_pdom->kol1   != old_pdom->kol1 ||
     new_pdom->lastkl != old_pdom->lastkl )
  {
    HGU_XmUserError(globals.topl,
		    "Autosave recover:\n"
		    "    Inconsistent bounding box\n"
		    "    in the the autosave object\n"
		    "     - please check the filename",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    WlzFreeObj( obj );
    autosave_recover_flag = False;
    return;
  }

  /* extract domains and add as required,
     this could be much more efficient - threshold incrementally
     and diff domain afterwards */
  thresh.type = WLZ_GREY_INT;
  thresh.v.inv = globals.cmapstruct->ovly_cols[DOMAIN_1];
  tmpObj = WlzThreshold(obj, thresh, WLZ_THRESH_HIGH, &errNum);
  
  if( tmpObj ){
    if( WlzVolume(tmpObj, &errNum) > 0 ){
      WlzPixelV	min, max;

      numDomains = globals.cmapstruct->num_overlays +
	globals.cmapstruct->num_solid_overlays;

      allDomainsObj = WlzAssignObject(tmpObj, NULL);
      WlzGreyRange(allDomainsObj, &min, &max);
      WlzValueConvertPixel(&max, max, WLZ_GREY_INT);
      while((numDomains > 0) &&
	    (globals.cmapstruct->ovly_cols[numDomains] > max.v.inv) ){
	numDomains--;
      }
    }
    else {
      WlzFreeObj(tmpObj);
      allDomainsObj = NULL;
    }
  }

  /* if only if domains found */
  if( allDomainsObj ){
  for(i=1; i < 33; i++)
  {
    domain = globals.priority_to_domain_lut[i];
    if( !allDomainsObj || (domain > numDomains) ){
      new_domains[i] = NULL;
    }
    else {
      new_domains[i] = get_domain_from_object(allDomainsObj, domain);
      if( WlzVolume(new_domains[i], &errNum) <= 0 )
      {
	WlzFreeObj( new_domains[i] );
	new_domains[i] = NULL;
      }
      else if( tmpObj = WlzDiffDomain(allDomainsObj, new_domains[i], &errNum) ){
	WlzFreeObj(allDomainsObj);
	allDomainsObj = WlzAssignObject(tmpObj, NULL);
      }
    }
  }

  dialog = create_autosave_domains_dialog(globals.topl, new_domains,
					  new_domain_names);

  /* make the button insensitive */
  XtSetSensitive(w, False);

  (void) HGU_XmDialogConfirm(dialog, XtNameToWidget(dialog, "*.Cancel"),
			     XtNameToWidget(dialog, "*.Cancel"), 0);
  XtDestroyWidget(dialog);

  /* make the button sensitive */
  XtSetSensitive(w, True);

  for(i=1; i < 33; i++)
  {
    if( new_domains[i] )
    {
      WlzFreeObj(new_domains[i]);
    }

    if( new_domain_names[i] )
    {
      AlcFree((void *) new_domain_names[i]);
    }
  }

  WlzFreeObj( obj );
  }
  else {
    /* put up help dialog to say nothing found */
    HGU_XmUserError(globals.topl,
		    "Autosave recover:\n"
		    "    No domains found in this autosave file.\n"
		    "    Possibly the file was not saved after domains\n"
		    "    had been defined or the painting was still\n"
		    "    in progress. You could read the autosave file\n"
		    "    in as a reference object to check if any\n"
		    "    domains are present then use threshold to\n"
		    "    extract any that are visible.\n",
		    XmDIALOG_FULL_APPLICATION_MODAL);
  }

  autosave_recover_flag = False;
  return;
}

static ActionAreaItem   autosave_dialog_actions[] = {
  {"On",		NULL,		NULL},
  {"Off",		NULL,		NULL},
  {"Recover",	NULL,		NULL},
  {"Dismiss",     NULL,           NULL},
  {"Help",        NULL,           NULL},
};

Widget create_autosave_dialog(
  Widget	topl)
{
  Widget	dialog, control, slider, text_line, widget;
  float		val, minval, maxval;

  dialog = HGU_XmCreateStdDialog(topl, "autosave_dialog", xmFormWidgetClass,
				 autosave_dialog_actions, 5);

  if( (widget = XtNameToWidget(dialog, "*.On")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, autosave_on_cb,
		  XtParent(dialog));
    XtSetSensitive(widget, False);
  }

  if( (widget = XtNameToWidget(dialog, "*.Off")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, autosave_off_cb,
		  XtParent(dialog));
  }

  if( (widget = XtNameToWidget(dialog, "*.Recover")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, autosave_recover_cb,
		  XtParent(dialog));
  }

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  control = XtNameToWidget( dialog, "*.control" );

  val = globals.autosave_time;
  minval = 5.0;
  maxval = 1000.0;
  slider = HGU_XmCreateHorizontalSlider("autosave_time", control,
					val, minval, maxval, 0,
					autosave_time_cb,
					(XtPointer) dialog);
  XtVaSetValues(slider,
		XmNtopAttachment,     XmATTACH_FORM,
		XmNleftAttachment,    XmATTACH_FORM,
		XmNrightAttachment,  	XmATTACH_FORM,
		NULL);

  text_line = HGU_XmCreateTextLine("autosave_file", control,
				   globals.autosave_file,
				   autosave_file_cb, (XtPointer) dialog);
  XtVaSetValues(text_line,
		XmNtopAttachment,     XmATTACH_WIDGET,
		XmNtopWidget,		slider,
		XmNleftAttachment,    XmATTACH_FORM,
		XmNrightAttachment,  	XmATTACH_FORM,
		NULL);
  widget = text_line;
		  
  text_line = HGU_XmCreateTextLine("autosave_recover_file", control,
				   globals.autosave_file, NULL, NULL);
  XtVaSetValues(text_line,
		XmNtopAttachment,     XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,    XmATTACH_FORM,
		XmNrightAttachment,  	XmATTACH_FORM,
		NULL);
		  

  return( dialog );
}
