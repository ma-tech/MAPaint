#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	options_menu.c						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <math.h>

#include <MAPaint.h>

/* menu item structures */

static MenuItem interact_tool_itemsP[] = {	/* select menu items */
  {"Draw", &xmToggleButtonGadgetClass, 0, NULL, NULL, True,
   select_interact_tool_cb, (XtPointer) MAPAINT_DRAW_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Paint ball", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_PAINT_BALL_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Fill", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_FILL_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Geometry object", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_GEOMETRY_OBJ_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Threshold", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_THRESHOLD_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Erode/Dilate", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_MORPHOLOGICAL_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Affine", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_AFFINE_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Separator", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) MAPAINT_NO_PAINT_ACTION, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Image Tracking", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_IMAGE_TRACKING_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Edge Tracking", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_EDGE_TRACKING_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
#if !defined (LINUX2) && !defined (DARWIN)
  {"Tablet", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_TABLET_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
#endif /* LINUX2 */
  NULL,
};

static MenuItem options_menu_itemsP[] = {	/* option_menu items */
  {"paint_tools_2D", &xmCascadeButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_ENABLED, True, True, &(interact_tool_itemsP[0])},
  {"paint_tools_3D", &xmCascadeButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_ENABLED, True, True, NULL},
  {"tool_controls", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   tool_controls_cb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"domain_review", &xmPushButtonWidgetClass, 0, NULL, NULL, False,
   domainReviewCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"domain_surgery", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   domainSurgeryCb, NULL, NULL, NULL,
   XmTEAR_OFF_ENABLED, True, True, NULL},
  {"auto_segment", &xmPushButtonWidgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"realignment", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   realignmentCb, NULL, NULL, NULL,
   XmTEAR_OFF_ENABLED, True, True, NULL},
  {"warp_input_2d", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   warpInput2DCb, NULL, NULL, NULL,
   XmTEAR_OFF_ENABLED, True, True, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"colormap", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   colormap_cb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"autosave_opts", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   autosave_opts_cb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"save_seq_opts", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   save_seq_opts_cb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

MenuItem	*options_menu_items = &(options_menu_itemsP[0]);
Widget		tool_controls_dialog = NULL;

extern Widget	reviewDialog;
extern Widget	surgeryDialog;
extern Widget	colormap_dialog;
extern Widget	autosave_dialog;
extern Widget	save_seq_dialog;

static XtIntervalId	domain_review_timeoutId;

static void domain_review_timeout_proc(XtPointer client_data,
				       XtIntervalId *id)
{
  if( paint_key != NULL ){

    /* set a timer event to wait until paint_key == NULL */
    domain_review_timeoutId = XtAppAddTimeOut(globals.app_con, 100,
					      domain_review_timeout_proc,
					      client_data);
  }
  else {
    XtSetSensitive((Widget) client_data, False);
/*    domain_review_func();*/
    XtSetSensitive((Widget) client_data, True);
  }

  return;
}

void domain_review_cb(Widget widget,
		      XtPointer client_data,
		      XtPointer call_data)
{
  XmPushButtonCallbackStruct *cbs =
    (XmPushButtonCallbackStruct *) call_data;

  /* check if painting in progress */
  if( paint_key != NULL ){
/*    globals.paint_action_quit_flag = 1;*/

    /* set a timer event to wait until paint_key == NULL */
    domain_review_timeoutId = XtAppAddTimeOut(globals.app_con, 100,
					      domain_review_timeout_proc,
					      (XtPointer) widget);
  }
  else {
    XtSetSensitive(widget, False);
/*    domain_review_func();*/
    XtSetSensitive(widget, True);
  }

  return;
}

/* tool menu manipulation procedure to mimic accelerators from a
   painting view window */
void selectNext2DTool(int downFlag)
{
  int		currIndex, newIndex, numTools;
  Widget	toggle, cascadeB;
  XmToggleButtonCallbackStruct	cbs;
  String	strBuf;

  /* run throught the tool menu to find the names of
     the currently selected toggle and the new toggle */
  numTools = 0;
  currIndex = -1;
  while( interact_tool_itemsP[numTools].name ){
    if( (PaintActionType) interact_tool_itemsP[numTools].callback_data
       == globals.currentPaintAction ){
      currIndex = numTools;
    }
    /* only go as far as the separator to cut out of the sequence
       tracking and tablet which must be selected directly */
    if( !strcmp(interact_tool_itemsP[numTools].name, "Separator") ){
      break;
    }
    numTools++;
  }
  if( currIndex == -1 ){
    return;
  }
  newIndex = currIndex + ((downFlag) ? (1) : (-1));
  if( newIndex < 0 ){
    newIndex += numTools;
  }
  newIndex = newIndex % numTools;

  /* get the menu and set/unset the toggles and reset the
     selected widget ID */
  cascadeB = XtNameToWidget(globals.topl,
			    "*menubar*options_menu*paint_tools_2D");

  /* unset the current */
  strBuf = (String) AlcMalloc(strlen(interact_tool_itemsP[currIndex].name)
			      + 4);
  sprintf(strBuf, "*%s", interact_tool_itemsP[currIndex].name);
  toggle = XtNameToWidget(cascadeB, strBuf);
  XtVaSetValues(toggle, XmNset, False, NULL);
  AlcFree((void *) strBuf);
  
  /* set the new */
  strBuf = (String) AlcMalloc(strlen(interact_tool_itemsP[newIndex].name)
			      + 4);
  sprintf(strBuf, "*%s", interact_tool_itemsP[newIndex].name);
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

/* action and callback procedures */

void select_interact_tool_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
  Widget	form[32], shell, newForm;
  int		i, nforms=9;
  XmToggleButtonCallbackStruct *cbs =
    (XmToggleButtonCallbackStruct *) call_data;
  Dimension	shellHeight, formHeight=0;

    if( !cbs->set )
	return;

    /* if painting then set paint_action_quit_flag */
/*    if( globals.currentPaintAction != (PaintActionType) client_data )
	globals.paint_action_quit_flag = 1;*/
    globals.currentPaintAction = (PaintActionType) client_data;

    shell = XtParent(tool_controls_dialog);
    XtVaGetValues(shell, XmNheight, &shellHeight, NULL);

    /* check for tool controls */
    form[0] = XtNameToWidget(tool_controls_dialog,
			     "*paint_draw_controls_form");
    form[1] = XtNameToWidget(tool_controls_dialog,
			     "*geometry_controls_form");
    form[2] = XtNameToWidget(tool_controls_dialog,
			     "*threshold_controls_form");
    form[3] = XtNameToWidget(tool_controls_dialog,
			     "*morph_controls_form");
    form[4] = XtNameToWidget(tool_controls_dialog,
			     "*affine_controls_form");
    form[5] = XtNameToWidget(tool_controls_dialog,
			     "*tracking_controls_form");
    form[6] = XtNameToWidget(tool_controls_dialog,
			     "*edge_tracking_controls_form");
#ifdef LINUX2
    form[7] = NULL;
#else
    form[7] = XtNameToWidget(tool_controls_dialog,
			     "*tablet_controls_form");
#endif /* LINUX2 */

    form[8] = XtNameToWidget(tool_controls_dialog,
			     "*fill_controls_form");
    for(i=0; i < nforms; i++){
      if( form[i] ){
	if( XtIsManaged(form[i]) ){
	  XtVaGetValues(form[i], XmNheight, &formHeight, NULL);
	  XtUnmanageChild(form[i]);
	}
      }
    }
    shellHeight -= formHeight;

    /* if currently painting then reset the paint tools */
    if( paint_key ){
      removeCurrentPaintTool(paint_key->canvas, paint_key);
    }

    switch( (PaintActionType) client_data ){
    case MAPAINT_DRAW_2D:
      globals.currentPaintActionCbFunc = MAPaintDraw2DCb;
      globals.currentPaintActionInitFunc = MAPaintDraw2DInit;
      globals.currentPaintActionQuitFunc = MAPaintDraw2DQuit;
      newForm = form[0];
      break;
    case MAPAINT_PAINT_BALL_2D:
      globals.currentPaintActionCbFunc = MAPaintPaintBall2DCb;
      globals.currentPaintActionInitFunc = MAPaintPaintBall2DInit;
      globals.currentPaintActionQuitFunc = MAPaintPaintBall2DQuit;
      newForm = form[0];
      break;
    case MAPAINT_GEOMETRY_OBJ_2D:
      globals.currentPaintActionCbFunc = MAPaintGeom2DCb;
      globals.currentPaintActionInitFunc = MAPaintGeom2DInit;
      globals.currentPaintActionQuitFunc = MAPaintGeom2DQuit;
      newForm = form[1];
      break;
    case MAPAINT_THRESHOLD_2D:
      globals.currentPaintActionCbFunc = MAPaintThreshold2DCb;
      globals.currentPaintActionInitFunc = MAPaintThreshold2DInit;
      globals.currentPaintActionQuitFunc = MAPaintThreshold2DQuit;
      newForm = form[2];
      break;
    case MAPAINT_FILL_2D:
      globals.currentPaintActionCbFunc = MAPaintFill2DCb;
      globals.currentPaintActionInitFunc = MAPaintFill2DInit;
      globals.currentPaintActionQuitFunc = MAPaintFill2DQuit;
      newForm = form[8];
      break;
    case MAPAINT_MORPHOLOGICAL_2D:
      globals.currentPaintActionCbFunc = MAPaintMorphological2DCb;
      globals.currentPaintActionInitFunc = MAPaintMorphological2DInit;
      globals.currentPaintActionQuitFunc = MAPaintMorphological2DQuit;
      newForm = form[3];
      break;
    case MAPAINT_AFFINE_2D:
      globals.currentPaintActionCbFunc = MAPaintAffine2DCb;
      globals.currentPaintActionInitFunc = MAPaintAffine2DInit;
      globals.currentPaintActionQuitFunc = MAPaintAffine2DQuit;
      newForm = form[4];
      break;
    case MAPAINT_IMAGE_TRACKING_2D:
      globals.currentPaintActionCbFunc = MAPaintTracking2DCb;
      globals.currentPaintActionInitFunc = MAPaintTracking2DInit;
      globals.currentPaintActionQuitFunc = MAPaintTracking2DQuit;
      newForm = form[5];
      break;
    case MAPAINT_EDGE_TRACKING_2D:
      globals.currentPaintActionCbFunc = MAPaintEdgeTracking2DCb;
      globals.currentPaintActionInitFunc = MAPaintEdgeTracking2DInit;
      globals.currentPaintActionQuitFunc = MAPaintEdgeTracking2DQuit;
      newForm = form[6];
      break;
#if !defined (LINUX2) && !defined (__ppc)
    case MAPAINT_TABLET_2D:
      globals.currentPaintActionCbFunc = MAPaintTabletCb;
      globals.currentPaintActionInitFunc = MAPaintTabletInit;
      globals.currentPaintActionQuitFunc = MAPaintTabletQuit;
      newForm = form[7];
      break;
#endif /* LINUX2 */
    default:
      newForm = form[0];
      break;
    }

    /* reset thte size of the tools dialog */
    XtVaGetValues(newForm, XmNheight, &formHeight, NULL);
    shellHeight += formHeight;
    XtVaSetValues(shell, XmNheight, shellHeight, NULL);
    XtManageChild(newForm);

    /* if currently painting then reset the paint tools */
    if( paint_key ){
      installCurrentPaintTool(paint_key->canvas, paint_key);
    }

    return;
}

void tool_controls_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  /* put up the tool controls dialog */
  XtManageChild(tool_controls_dialog);

  return;
}

static Widget create_tool_controls_dialog(
  Widget	topl)
{
    Widget	dialog, control, form;

    dialog = HGU_XmCreateStdDialog(topl, "tool_control_dialog",
				   xmFormWidgetClass, NULL, 0);

    control = XtNameToWidget( dialog, "*.Cancel" );
    XtRemoveAllCallbacks(control, XmNactivateCallback);
    XtAddCallback(control, XmNactivateCallback,
		  UnmanageChildCb, (XtPointer) dialog);

    control = XtNameToWidget( dialog, "*.control" );

    form = CreateDrawPaintBallControls( control );
    (void) CreateThresholdControls( control );
    (void) CreateGeometryObjectControls( control );
    (void) CreateFillControls( control );
    (void) CreateMorphologicalControls( control );
    (void) CreateAffineControls( control );
    (void) CreateTracking2DControls( control );
    (void) CreateEdgeTracking2DControls( control );
#if !defined (LINUX2) && !defined (__ppc)
    (void) CreateTabletControls( control );
#endif /* LINUX2 */

    XtManageChild( form );

  return( dialog );
}


static XtResource autosave_res[] = {
  {"autosaveTime", "AutosaveTime", XtRInt, sizeof(int),
   XtOffsetOf(PaintGlobals, autosave_time), XtRString, "300"},
  {"autosaveDir", "AutosaveDir", XtRString, sizeof(String),
   XtOffsetOf(PaintGlobals, autosave_dir),
   XtRString, "/tmp"},
  {"autosaveFile", "AutosaveFile", XtRString, sizeof(String),
   XtOffsetOf(PaintGlobals, autosave_file),
   XtRString, "MAPaintAutosave"}
};

/* options_menu initialisation procedure */
void options_menu_init(
Widget	topl)
{
    Widget	canvas = XtNameToWidget(topl, "*.work_area"), widget;
    XColor	xcolor;
    String	filestr, dirstr;

    /* create the tool controls dialog */
    tool_controls_dialog = create_tool_controls_dialog( topl );
    
    /* create the surgery controls dialog */
    surgeryDialog = createDomainSurgeryDialog( topl );
    XtManageChild( surgeryDialog );
    HGU_XmSaveRestoreAddWidget( surgeryDialog, NULL, NULL, NULL, NULL );

    /* create the review controls dialog */
    reviewDialog = createDomainReviewDialog( topl );
    XtManageChild( reviewDialog );
    HGU_XmSaveRestoreAddWidget( reviewDialog, NULL, NULL, NULL, NULL );

    /* create the colormap controls */
    colormap_dialog = create_colormap_dialog( topl );
    XtManageChild( colormap_dialog );
    HGU_XmSaveRestoreAddWidget( colormap_dialog, NULL, NULL, NULL, NULL );

    /* add an autosave timeout */
    XtGetApplicationResources(globals.topl, &globals,
			      autosave_res, XtNumber(autosave_res),
			      NULL, 0);
    filestr = globals.autosave_file;
    dirstr = globals.autosave_dir;
    if( strlen(dirstr) < 1 ){
      globals.autosave_file = (String)
	AlcMalloc(sizeof(char) * (strlen(filestr) + 16 ));
      sprintf(globals.autosave_file, "%s.%d", filestr, getpid());
    }
    else {
      globals.autosave_file = (String)
	AlcMalloc(sizeof(char) * (strlen(dirstr) + strlen(filestr) + 16 ));
      sprintf(globals.autosave_file, "%s/%s.%d", dirstr, filestr, getpid());
    }
    
    /* no autosave in sectioView mode */
    if( globals.sectViewFlg ){
      globals.autosavetimeoutID = 0;
    }
    else {
      globals.autosavetimeoutID =
	XtAppAddTimeOut(globals.app_con, globals.autosave_time*1000,
			autosavetimeout_cb, NULL);
    }

    /* create the autosave controls */
    autosave_dialog = create_autosave_dialog( topl );
    XtManageChild( autosave_dialog );
    HGU_XmSaveRestoreAddWidget( autosave_dialog, NULL, NULL, NULL, NULL );

    globals.stipple                = (Pixmap) 0;
    globals.paint_action_quit_flag = 0;
    globals.currentPaintAction = MAPAINT_DRAW_2D;
    globals.currentPaintActionCbFunc = MAPaintDraw2DCb;
    globals.currentPaintActionInitFunc = MAPaintDraw2DInit;
    globals.currentPaintActionQuitFunc = MAPaintDraw2DQuit;
    globals.review_domain_obj = NULL;

    /* remove the editing options in sectionView mode */
    if( globals.sectViewFlg ){
      char nameBuf[64];
      int i;
      for(i=0; options_menu_itemsP[i].name != NULL; i++){
	if( strstr(options_menu_itemsP[i].name, "colormap") ){
	  continue;
	}
	else if( options_menu_itemsP[i].name ){
	  sprintf(nameBuf, "*.options_menu*%s",
		  options_menu_itemsP[i].name);
	  if( widget = XtNameToWidget(topl, nameBuf) ){
	    XtSetSensitive(widget, False);
	  }
	}
      }
    }

    /* create the save sequence controls */
    save_seq_dialog = create_save_seq_dialog( topl );
    XtManageChild( save_seq_dialog );
    HGU_XmSaveRestoreAddWidget( save_seq_dialog, NULL, NULL, NULL, NULL );

    return;
}
 
