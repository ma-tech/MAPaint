#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MAPaintEvent_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MAPaintEvent.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:48:13 2009
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

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

/* table of accepted input events.
   For a given context and mode if an event is found it is re-mapped
   if not found then the event is unchanged but MAPaintRemapEvent()
   will return an error, this list therefore only need to hold events
   that we will want to re-map.

   Later this could include ALL allowed events to be more strict
   about input. */
/* Note this table must be ordered else the re-mapping pages will
   get more pages than are necessary. rder first on context then
   on mode */
static MAPaintEventMapping	mapaintEventMappingP[]=
{
  /* 3D view context, any mode */
  {MAPAINT_3D_VIEW_CONTEXT, MAPAINT_VIEW_MODE,
   ButtonPress, Button1, 0x0, ButtonPress, Button1, 0x0,
   "Select rotate"},
  {MAPAINT_3D_VIEW_CONTEXT, MAPAINT_VIEW_MODE,
   ButtonPress, Button1, ShiftMask, ButtonPress, Button1, ShiftMask,
   "Select translate along line of sight"},
  {MAPAINT_3D_VIEW_CONTEXT, MAPAINT_VIEW_MODE,
   ButtonPress, Button2, 0x0, ButtonPress, Button2, 0x0,
   "Select translate across line of sight"},

  /* section view context, view mode */
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_VIEW_MODE,
   ButtonPress, Button1, 0x0, ButtonPress, Button1, 0x0,
   "Select paint mode"},
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_VIEW_MODE,
   ButtonPress, Button2, 0x0, ButtonPress, Button2, 0x0,
   "Display pointer feedback"},

  /* section view context, paint mode */
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_PAINT_MODE,
   ButtonPress, Button1, ShiftMask, ButtonPress, Button1, ShiftMask,
   "Increase magnification"},
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_PAINT_MODE,
   ButtonPress, Button2, ShiftMask, ButtonPress, Button2, ShiftMask,
   "Decrease magnification"},
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_PAINT_MODE,
   ButtonPress, Button1, ControlMask, ButtonPress, Button1, ControlMask,
   "Lift paint domain"},
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_PAINT_MODE,
   ButtonPress, Button3, 0x0, ButtonPress, Button3, 0x0,
   "Select quit painting"},
  /* painting tools - all use the same re-mapping for sanity */
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_PAINT_MODE,
   ButtonPress, Button1, 0x0, ButtonPress, Button1, 0x0,
   "Initiate paint operation; Affine mode select region"},
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_PAINT_MODE,
   ButtonPress, Button2, 0x0, ButtonPress, Button2, 0x0,
   "Initiate paint operation - delete mode; Affine mode select domain"},
  {MAPAINT_SECT_VIEW_CONTEXT, MAPAINT_PAINT_MODE,
   ButtonPress, Button2, ControlMask, ButtonPress, Button2, ControlMask,
   "Affine mode select all domains"},

  /* warp contexts, warp mode */
  /* destination window */
  {MAPAINT_WARP_2D_CONTEXT, MAPAINT_WARP_MODE,
   ButtonPress, Button1, 0x0, ButtonPress, Button1, 0x0,
   "Set or move tie-point"},
  {MAPAINT_WARP_2D_CONTEXT, MAPAINT_WARP_MODE,
   ButtonPress, Button2, 0x0, ButtonPress, Button2, 0x0,
   "Delete tie-point if selected"},
  {MAPAINT_WARP_2D_CONTEXT, MAPAINT_WARP_MODE,
   ButtonPress, Button1, ShiftMask, ButtonPress, Button1, ShiftMask,
   "Multi select"},
  {MAPAINT_WARP_2D_CONTEXT, MAPAINT_WARP_MODE,
   ButtonPress, Button1, ControlMask, ButtonPress, Button1, ControlMask,
   "Rectangle select"},

  /* signal window */
  {MAPAINT_WARP_SGNL_CONTEXT, MAPAINT_THRESHOLD_MODE,
   ButtonPress, Button1, 0x0, ButtonPress, Button1, 0x0,
   "Set start value"},
  {MAPAINT_WARP_SGNL_CONTEXT, MAPAINT_THRESHOLD_MODE,
   ButtonPress, Button1, ControlMask, ButtonPress, Button1, ControlMask,
   "Lift signal domain"},
  {MAPAINT_WARP_SGNL_CONTEXT, MAPAINT_PICK_MODE,
   ButtonPress, Button1, 0x0, ButtonPress, Button1, 0x0,
   "Set start value"},
  {MAPAINT_WARP_SGNL_CONTEXT, MAPAINT_PICK_MODE,
   ButtonPress, Button2, 0x0, ButtonPress, Button2, 0x0,
   "Set finish value"},
  {MAPAINT_NONE_CONTEXT, MAPAINT_NONE_MODE,
   0x0, 0x0, 0x0, 0x0, 0x0, 0x0, NULL},
};

MAPaintEventMapping *mapaintEventMapping=&(mapaintEventMappingP[0]);

Widget	event_remap_dialog=NULL;

void eventRemapCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  if( event_remap_dialog == NULL ){
    event_remap_dialog = createEventRemapDialog(globals.topl);
  }
  XtManageChild(event_remap_dialog);
  PopupCallback(w, (XtPointer) XtParent(event_remap_dialog), NULL);
  return;
}

WlzErrorNum MAPaintEventRemap(
  MAPaintContext	context,
  MAPaintContextMode	mode,
  XEvent		*event)
{
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  MAPaintEventMapping	*eventMap;
  WlzUInt	modMask=ShiftMask|ControlMask|Mod1Mask|Mod2Mask;
  int	quitLoop;

  /* check every remapping and find match for context,
     mode, event, button, mask.
     Note we only check against the modifier bits. */
  /* ignore all modMask values except, shift, control, alt, meta */
  eventMap = mapaintEventMapping;
  quitLoop = 0;
  while( eventMap ){
    if((eventMap->context == context) &&
       (eventMap->mode == mode) &&
       (eventMap->srcEventType == event->type)){
      switch( event->type ){
      case ButtonPress:
      case ButtonRelease:
	if((eventMap->srcButton == event->xbutton.button) &&
	   (eventMap->srcModMask == (event->xbutton.state&modMask))){
	  quitLoop = 1;
	}
	break;

      default:
	break;
      }
    }
    if( quitLoop ){
      break;
    }
    else {
      eventMap++;
      if( eventMap->context == MAPAINT_NONE_CONTEXT ){
	eventMap = NULL;
      }
    }
  }

  if( eventMap ){
    switch( event->type ){
    case ButtonPress:
    case ButtonRelease:
      if( event->type == eventMap->dstEventType ){
	event->xbutton.type = eventMap->dstEventType;
	event->xbutton.button = eventMap->dstButton;
	event->xbutton.state &= ~modMask;
	event->xbutton.state |= eventMap->dstModMask;
      }
      break;

    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
    }
  }
  else {
    errNum = WLZ_ERR_PARAM_DATA;
  }

  return errNum;
}

String HGU_XEventTypeToString(
  unsigned int	type)
{
  switch( type ){
  case KeyPress:
    return "KeyPress";
  case KeyRelease:
    return "KeyRelease";
  case ButtonPress:
    return "ButtonPress";
  case ButtonRelease:
    return "ButtonRelease";
  case MotionNotify:
    return "MotionNotify";
  case EnterNotify:
    return "EnterNotify";
  case LeaveNotify:
    return "LeaveNotify";
  case FocusIn:
    return "FocusIn";
  case FocusOut:
    return "FocusOut";
  case KeymapNotify:
    return "KeymapNotify";
  case Expose:
    return "Expose";
  case GraphicsExpose:
    return "GraphicsExpose";
  case NoExpose:
    return "NoExpose";
  case VisibilityNotify:
    return "VisibilityNotify";
  case CreateNotify:
    return "CreateNotify";
  case DestroyNotify:
    return "DestroyNotify";
  case UnmapNotify:
    return "UnmapNotify";
  case MapNotify:
    return "MapNotify";
  case MapRequest:
    return "MapRequest";
  case ReparentNotify:
    return "ReparentNotify";
  case ConfigureNotify:
    return "ConfigureNotify";
  case ConfigureRequest:
    return "ConfigureRequest";
  case GravityNotify:
    return "GravityNotify";
  case ResizeRequest:
    return "ResizeRequest";
  case CirculateNotify:
    return "CirculateNotify";
  case CirculateRequest:
    return "CirculateRequest";
  case PropertyNotify:
    return "PropertyNotify";
  case SelectionClear:
    return "SelectionClear";
  case SelectionRequest:
    return "SelectionRequest";
  case SelectionNotify:
    return "SelectionNotify";
  case ColormapNotify:
    return "ColormapNotify";
  case ClientMessage:
    return "ClientMessage";
  case MappingNotify:
    return "MappingNotify";
  default:
    return "Unknown";
  }
}

String HGU_XButtonToString(
  unsigned int	button)
{
  switch(button){
  case Button1:
    return "Button1";
  case Button2:
    return "Button2";
  case Button3:
    return "Button3";
  case Button4:
    return "Button4";
  case Button5:
    return "Button5";
  default:
    return "Unknown";
  }
}

String HGU_XModifierMaskToString(
  unsigned int	mask)
{
  static char	rtnStr[64];

  rtnStr[0] = '\0';
  if( mask & ControlMask ){
    sprintf(rtnStr, "%s<Ctrl>", rtnStr);
  }
  if( mask & ShiftMask ){
    sprintf(rtnStr, "%s<Shift>", rtnStr);
  }
  if( mask & LockMask ){
    sprintf(rtnStr, "%s<Lock>", rtnStr);
  }
  if( mask & Mod1Mask ){
    sprintf(rtnStr, "%s<Alt>", rtnStr);
  }
  if( mask & Mod2Mask ){
    sprintf(rtnStr, "%s<Meta>", rtnStr);
  }
  if( rtnStr[0] == '\0' ){
    sprintf(rtnStr, "None");
  }

  return rtnStr;
}

static int eventMappingCompar(
  const void *item1,
  const void *item2)
{
  MAPaintEventMapping *e1=(MAPaintEventMapping *) item1;
  MAPaintEventMapping *e2=(MAPaintEventMapping *) item2;

  if( e1->context < e2->context ){
    return -1;
  }
  else if( e1->context > e2->context ){
    return 1;
  }
  else if( e1->mode < e2->mode ){
    return -1;
  }
  else if( e1->mode > e2->mode ){
    return 1;
  }
  else {
    return 0;
  }
}

static void createTitleRow(
  Widget	page,
  Widget	*col_rcs)
{
  Widget       	widget, label;

  /* for each colume create the row-column and the top widget */
  col_rcs[0] = XtVaCreateManagedWidget("action_col",
				       xmRowColumnWidgetClass, page,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_FORM,
				       XmNorientation, XmVERTICAL,
				       XmNpacking, XmPACK_TIGHT,
				       XmNnumColumns, 1,
				       XmNentryAlignment, XmALIGNMENT_CENTER,
				       NULL);
  label = XtVaCreateManagedWidget("Action", xmLabelWidgetClass,
				  col_rcs[0],
				  NULL);
  HGU_XmAddToolTip(globals.topl, label,
		   "Action invoked by this key/modifier combination"
		   " for this context and mode");
  widget = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass,
				   col_rcs[0], NULL);

  col_rcs[1] = XtVaCreateManagedWidget("event_col",
				       xmRowColumnWidgetClass, page,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_WIDGET,
				       XmNleftWidget, col_rcs[0],
				       XmNorientation, XmVERTICAL,
				       XmNpacking, XmPACK_TIGHT,
				       XmNnumColumns, 1,
				       XmNentryAlignment, XmALIGNMENT_CENTER,
				       NULL);
  label = XtVaCreateManagedWidget("Event", xmLabelWidgetClass,
				  col_rcs[1],
				  NULL);
  HGU_XmAddToolTip(globals.topl, label,
		   "GUI Event type");
  widget = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass,
				   col_rcs[1], NULL);

  col_rcs[2] = XtVaCreateManagedWidget("button_col",
				       xmRowColumnWidgetClass, page,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_WIDGET,
				       XmNleftWidget, col_rcs[1],
				       XmNorientation, XmVERTICAL,
				       XmNpacking, XmPACK_TIGHT,
				       XmNentryAlignment, XmALIGNMENT_CENTER,
				       NULL);
  label = XtVaCreateManagedWidget("Button", xmLabelWidgetClass,
				  col_rcs[2],
				  NULL);
  HGU_XmAddToolTip(globals.topl, label,
		   "Button");
  widget = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass,
				   col_rcs[2], NULL);

  col_rcs[3] = XtVaCreateManagedWidget("modifiers_col",
				       xmRowColumnWidgetClass, page,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_WIDGET,
				       XmNleftWidget, col_rcs[2],
				       XmNorientation, XmVERTICAL,
				       XmNpacking, XmPACK_TIGHT,
				       XmNentryAlignment, XmALIGNMENT_CENTER,
				       NULL);
  label = XtVaCreateManagedWidget("Modifiers", xmLabelWidgetClass,
				  col_rcs[3],
				  NULL);
  HGU_XmAddToolTip(globals.topl, label,
		   "Modifier combination");
  widget = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass,
				   col_rcs[3], NULL);

  col_rcs[4] = XtVaCreateManagedWidget("reset_col",
				       xmRowColumnWidgetClass, page,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_WIDGET,
				       XmNleftWidget, col_rcs[3],
				       XmNorientation, XmVERTICAL,
				       XmNpacking, XmPACK_TIGHT,
				       XmNentryAlignment, XmALIGNMENT_CENTER,
				       NULL);
  label = XtVaCreateManagedWidget("Reset", xmLabelWidgetClass,
				  col_rcs[4],
				  NULL);
  HGU_XmAddToolTip(globals.topl, label,
		   "Press required combination to reset Button and Modifier");
  widget = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass,
				   col_rcs[4], NULL);

  return;
}

static int eventUniqueSrc(
  MAPaintEventMapping	*eventMap)
{
  int	rtnFlg=1;
  MAPaintEventMapping	*eMapPtr;

  eMapPtr = mapaintEventMapping;
  while(eMapPtr->context != MAPAINT_NONE_CONTEXT){

    if( eventMap != eMapPtr ){
      if((eMapPtr->context == eventMap->context) &&
	 (eMapPtr->mode == eventMap->mode) &&
	 (eMapPtr->srcEventType == eventMap->srcEventType) &&
	 (eMapPtr->srcButton == eventMap->srcButton) &&
	 (eMapPtr->srcModMask == eventMap->srcModMask)){
	rtnFlg = 0;
      }
    }
    eMapPtr++;
  }

  return rtnFlg;
}
  
static void eventResetLabels(
  MAPaintEventMapping	*eventMap)
{
  String	str;
  XmString	xmstr;

  if( eventMap ){
    str = HGU_XButtonToString(eventMap->srcButton);
    xmstr = XmStringCreateSimple(str);
    XtVaSetValues(eventMap->buttonLabel,
		  XmNlabelString, xmstr, NULL);
    XmStringFree(xmstr);

    str = HGU_XModifierMaskToString(eventMap->srcModMask);
    xmstr = XmStringCreateSimple(str);
    XtVaSetValues(eventMap->modifierLabel,
		  XmNlabelString, xmstr, NULL);
    XmStringFree(xmstr);
  }

  return;
}

static void resetEventMapCb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  int	allFlg=(int) client_data;
  MAPaintEventMapping	*eventMap=mapaintEventMapping;

  while(eventMap->context != MAPAINT_NONE_CONTEXT){
    if( allFlg || 
       !(XmGetVisibility(eventMap->eventLabel) == XmVISIBILITY_FULLY_OBSCURED)){
      eventMap->srcEventType = eventMap->dstEventType;
      eventMap->srcButton = eventMap->dstButton;
      eventMap->srcModMask = eventMap->dstModMask;
      eventResetLabels(eventMap);
    }
    eventMap++;
  }

  return;
}

static int eventRemapTrigger=0;

static void eventRemapInputCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  MAPaintEventMapping	*eventMap=(MAPaintEventMapping *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs=(XmDrawingAreaCallbackStruct *) call_data;
  WlzUInt	modMask=ShiftMask|ControlMask|Mod1Mask|Mod2Mask;

  /* get event, if button press then reset event map */
  if( cbs->event ){
    switch(cbs->event->type){
    case ButtonPress:
      switch(cbs->event->xbutton.button){
      case Button1:
      case Button2:
      case Button3:
	eventMap->srcButton = cbs->event->xbutton.button;
	eventMap->srcModMask = (cbs->event->xbutton.state)&modMask;
	eventRemapTrigger = 1;
	break;

      default:
	break;
      }
      break;

    case ButtonRelease:
      switch(cbs->event->xbutton.button){
      case Button1:
      case Button2:
      case Button3:
	if( eventRemapTrigger ){
	  eventRemapTrigger = 0;
	  /* check uniqueness */
	  if( eventUniqueSrc(eventMap) ){
	    eventResetLabels(eventMap);
	  }
	  else {
	    if( HGU_XmUserConfirm(globals.topl,
				  "Warning: you have selected a key\n"
				  "  and modifier combination already\n"
				  "  in use.\n\n"
				  "Do you wish to continue?",
				  "Yes", "No", 0) ){
	      eventResetLabels(eventMap);
	    }
	    else {
	      eventMap->srcButton = eventMap->dstButton;
	      eventMap->srcModMask = eventMap->dstModMask;
	    }
	  }
	}
	break;

      default:
	break;
      }
      break;

    default:
      break;
    }
  }
  return;
}

void MAPaintTestEventHandler(
  Widget        w,
  XtPointer     client_data,
  XEvent        *event,
  Boolean       *continue_to_dispatch)
{
  XmDrawingAreaCallbackStruct	cbs;

  cbs.reason = XmCR_INPUT;
  cbs.window = XtWindow(w);
  cbs.event = event;
  switch( event->type ){
  case ButtonPress:
  case ButtonRelease:
    eventRemapInputCb(w, client_data, (XtPointer) &cbs);
    break;

  case KeyPress:
  case KeyRelease:
    break;

  default:
    break;
  }
    
  return;
}

static void createEventRow(
  Widget	page,
  Widget	widget,
  MAPaintEventMapping	*eventMap,
  Widget	*col_rcs)
{
  Widget       	label, da;
  String	str;
  Pixel		pixel;
  Dimension	height;

  pixel = WhitePixelOfScreen(XtScreen(globals.topl));
  str = (String) AlcStrDup(eventMap->helpStr);
  if( strlen(str) > 18 ){
    strcpy(&(str[15]), "...");
  }
  label = XtVaCreateManagedWidget(str,
				  xmLabelWidgetClass, col_rcs[0],
				  XmNbackground, pixel,
				  NULL);
  HGU_XmAddToolTip(globals.topl, label, eventMap->helpStr);
  AlcFree(str);

  str = HGU_XEventTypeToString(eventMap->srcEventType);
  label = XtVaCreateManagedWidget(str,
				  xmLabelWidgetClass, col_rcs[1],
				  XmNbackground, pixel,
				  NULL);
  eventMap->eventLabel = label;

  str = HGU_XButtonToString(eventMap->srcButton);
  label = XtVaCreateManagedWidget(str,
				  xmLabelWidgetClass, col_rcs[2],
				  XmNbackground, pixel,
				  NULL);
  eventMap->buttonLabel = label;

  str = HGU_XModifierMaskToString(eventMap->srcModMask);
  label = XtVaCreateManagedWidget(str,
				  xmLabelWidgetClass, col_rcs[3],
				  XmNbackground, pixel,
				  NULL);
  eventMap->modifierLabel = label;

  XtVaGetValues(label, XmNheight, &height, NULL);
  da = XtVaCreateManagedWidget("eventInput",
			       xmDrawingAreaWidgetClass, col_rcs[4],
			       XmNbackground, pixel,
			       XmNheight, height-4,
			       XmNborderWidth, 2,
			       NULL);
  XtAddCallback(da, XmNinputCallback, eventRemapInputCb,
    (XtPointer) eventMap);
  /* something weird here, need to include an event handler to get
     <Ctrl>Button1 event on the Mac */
  XtAddEventHandler(da, ButtonPressMask|KeyPressMask,
		    False, MAPaintTestEventHandler,
		    (XtPointer) eventMap);

  return;
}

static ActionAreaItem   event_remap_actions[] = {
{"Reset",	resetEventMapCb,		(XtPointer) 0},
{"Reset all",	resetEventMapCb,		(XtPointer) 1},
{"Dismiss",     NULL,           NULL},
{"Help",        NULL,           NULL},
};

Widget createEventRemapDialog(
  Widget	parent)
{
  Widget	dialog, control, button, notebook, page;
  Widget	widget, col_rcs[5];
  int		numMappings;
  MAPaintEventMapping	*eventMap;
  MAPaintContext	context;
  MAPaintContextMode	mode;
  String		contextPageStr, contextTabStr, modeTabStr;

  /* Sort the event mapping list first by context then by mode */
  numMappings = 0;
  /*eventMap = mapaintEventMapping;
  while(eventMap->context != MAPAINT_NONE_CONTEXT){
    numMappings++;
    eventMap++;
    }*/
  if( numMappings > 1 ){
    qsort(mapaintEventMapping, numMappings, sizeof(MAPaintEventMapping),
	  eventMappingCompar);
  }

  /* create a notebook dialog with a major page per
     context and minor page per mode.*/
  dialog = HGU_XmCreateStdDialog(parent, "event_remap_dialog",
				 xmFormWidgetClass,
				 event_remap_actions, 4);

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }
  control = XtNameToWidget( dialog, "*.control" );

  /* add the notebook */
  notebook = XtVaCreateManagedWidget("event_map_notebook",
				     xmNotebookWidgetClass, control,
				     XmNbindingType,	XmNONE,
				     XmNbackPagePlacement, XmTOP_RIGHT,
				     XmNbackPageSize, 3,
				     XmNorientation, XmVERTICAL,
				     XmNmajorTabSpacing, 0,
				     XmNminorTabSpacing, 0,
				     XmNtopAttachment, XmATTACH_FORM,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNrightAttachment, XmATTACH_FORM,
				     NULL);
  if((widget = XtNameToWidget(notebook, "*PageScroller"))){
    XtUnmanageChild(widget);
  }

  /* loop through event mapping creating pages and entries */
  eventMap = mapaintEventMapping;
  context = MAPAINT_NONE_CONTEXT;
  mode = MAPAINT_NONE_MODE;
  while(eventMap->context != MAPAINT_NONE_CONTEXT){
    /* get the strings */
    if(WlzValueMatchString(&contextPageStr, eventMap->context,
			   "None_page", MAPAINT_NONE_CONTEXT,
			   "3D View_page", MAPAINT_3D_VIEW_CONTEXT,
			   "Section_page", MAPAINT_SECT_VIEW_CONTEXT,
			   "Warp 2D_page", MAPAINT_WARP_2D_CONTEXT,
			   "Warp 3D_page", MAPAINT_WARP_3D_CONTEXT,
			   "Warp Sgnl_page", MAPAINT_WARP_SGNL_CONTEXT,
			   "Any_page", MAPAINT_ANY_CONTEXT,
			   NULL)){
    }
    else {
      contextPageStr = "Unknown_page";
    }      

    if(WlzValueMatchString(&contextTabStr, eventMap->context,
			   "None", MAPAINT_NONE_CONTEXT,
			   "3D View", MAPAINT_3D_VIEW_CONTEXT,
			   "Section", MAPAINT_SECT_VIEW_CONTEXT,
			   "Warp 2D", MAPAINT_WARP_2D_CONTEXT,
			   "Warp 3D", MAPAINT_WARP_3D_CONTEXT,
			   "Warp Sgnl", MAPAINT_WARP_SGNL_CONTEXT,
			   "Any", MAPAINT_ANY_CONTEXT,
			   NULL)){
    }
    else {
      contextTabStr = "Unknown";
    }      

    if(WlzValueMatchString(&modeTabStr, eventMap->mode,
			   "None", MAPAINT_NONE_MODE,
			   "View", MAPAINT_VIEW_MODE,
			   "Paint", MAPAINT_PAINT_MODE,
			   "Warp", MAPAINT_WARP_MODE,
			   "Thresh", MAPAINT_THRESHOLD_MODE,
			   "Pick", MAPAINT_PICK_MODE,
			   "Region", MAPAINT_PAINT_DOMAIN_MODE,
			   "Ball", MAPAINT_PAINT_BALL_MODE,
			   "Affine", MAPAINT_AFFINE_MODE,
			   "Fill", MAPAINT_FILL_MODE,
			   "Morph", MAPAINT_MORPHOLOGICAL_MODE,
			   "Geom", MAPAINT_GEOMETRY_MODE,
			   "Track", MAPAINT_TRACK_MODE,
			   "Any", MAPAINT_ANY_MODE,
			   NULL)){
    }
    else {
      modeTabStr = "Unknown";
    }      

    /* check for new major page */
    if( context != eventMap->context ){
      context = eventMap->context;
      mode = eventMap->mode;
      page = XtVaCreateManagedWidget(contextPageStr,
				     xmFormWidgetClass, notebook,
				     XmNnotebookChildType, XmPAGE,
				     XmNborderWidth,		0,
				     XmNhorizontalSpacing, 0,
				     NULL);
      button = XtVaCreateManagedWidget(contextTabStr,
				       xmPushButtonWidgetClass, notebook,
				       XmNnotebookChildType, XmMAJOR_TAB,
				       NULL);
      button = XtVaCreateManagedWidget(modeTabStr,
				       xmPushButtonWidgetClass, notebook,
				       XmNnotebookChildType, XmMINOR_TAB,
				       NULL);
      createTitleRow(page, col_rcs);
    }

    /* check for new minor page */
    if( mode != eventMap->mode ){
      mode = eventMap->mode;
      page = XtVaCreateManagedWidget(contextPageStr,
				     xmFormWidgetClass, notebook,
				     XmNnotebookChildType, XmPAGE,
				     XmNhorizontalSpacing, 0,
				     NULL);
      button = XtVaCreateManagedWidget(modeTabStr,
				       xmPushButtonWidgetClass, notebook,
				       XmNnotebookChildType, XmMINOR_TAB,
				       NULL);
      createTitleRow(page, col_rcs);
    }

    /* add new row to the page table */
    createEventRow(page, widget, eventMap, col_rcs);
    eventMap++;
  }

  return dialog;
}
