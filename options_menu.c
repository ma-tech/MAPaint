#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _options_menu_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         options_menu.c
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
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <math.h>

#include <MAPaint.h>
#include <MAWarp.h>

#include <Xbae/Matrix.h>

/* menu item structures */

static MenuItem interact_tool_itemsP[] = {	/* select menu items */
  {"Draw", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_DRAW_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Paint ball", &xmToggleButtonGadgetClass, 0, NULL, NULL, True,
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
#if !defined (LINUX2) && !defined (DARWIN) && !defined (SUNOS5)
  {"Tablet", &xmToggleButtonGadgetClass, 0, NULL, NULL, False,
   select_interact_tool_cb, (XtPointer) MAPAINT_TABLET_2D,
   NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
#endif /* LINUX2 */
  {NULL},
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
  {"event_remap", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   eventRemapCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"autosave_opts", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   autosave_opts_cb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"save_seq_opts", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   save_seq_opts_cb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {NULL},
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
#if defined (LINUX2) || defined (DARWIN) || defined (SUNOS5)
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
#if !defined (LINUX2) && !defined (DARWIN) && !defined (SUNOS5)
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
#if !defined (LINUX2) && !defined (DARWIN) && !defined (SUNOS5)
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
    Widget	widget;
    String	filestr, dirstr;
    char	lineBuf[1024], fileBuf[512], dateBuf[64];
    char	authorBuf[64], stageBuf[16];
    MAPaintExpressMapStatus	status;
    MAPaintExpressMapQuality	quality;
    int		bibfileStartIndex=0;

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

    /* get option menu resources */
    XtGetApplicationResources(globals.topl, &globals,
			      autosave_res, XtNumber(autosave_res),
			      NULL, 0);

    /* add an autosave timeout */
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
    
    /* no autosave in sectionView mode */
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
    globals.currentPaintAction = MAPAINT_PAINT_BALL_2D;
    globals.currentPaintActionCbFunc = MAPaintPaintBall2DCb;
    globals.currentPaintActionInitFunc = MAPaintPaintBall2DInit;
    globals.currentPaintActionQuitFunc = MAPaintPaintBall2DQuit;
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
	  if((widget = XtNameToWidget(topl, nameBuf))){
	    XtSetSensitive(widget, False);
	  }
	}
      }
    }

    /* create the save sequence controls */
    save_seq_dialog = create_save_seq_dialog( topl );
    XtManageChild( save_seq_dialog );
    HGU_XmSaveRestoreAddWidget( save_seq_dialog, NULL, NULL, NULL, NULL );

    /* get the current working directory */
    if((dirstr = getcwd(lineBuf, 256))){
      globals.origDir = AlcStrDup(dirstr);
    }
    else {
      globals.origDir = NULL;
    }

    /* check for rapid-map or express-map */
    if( globals.rapidMapFlg || globals.expressMapFlg ){

      /* check for bibfile list file */
      if( globals.bibfileListFile ){
	FILE	*fp;
	int		i, j, linecount, csvCount;
	WlzObject	*dummyObj;
	WlzPixelV	bgdV;
	Widget		toggle, notebook, matrix, textField;
	XmNotebookPageInfo	pageInfo;
	XmToggleButtonCallbackStruct	cbs;

	/* set the default csv file */
	if( globals.bibfileListFile[0] == '/' ){
	  if( strstr(globals.bibfileListFile, ".csv") ){
	    sprintf(lineBuf, "%s", globals.bibfileListFile);
	  }
	  else {
	    sprintf(lineBuf, "%s.csv", globals.bibfileListFile);
	  }
	}
	else {
	  if( strstr(globals.bibfileListFile, ".csv") ){
	    sprintf(lineBuf, "%s/%s", globals.origDir, globals.bibfileListFile);
	  }
	  else {
	    sprintf(lineBuf, "%s/%s.csv", globals.origDir, globals.bibfileListFile);
	  }
	}
	warpGlobals.bibfileListCSVFile = AlcStrDup(lineBuf);

	/* read file and create lists */
	if((fp = fopen(globals.bibfileListFile, "r"))){
	  /* count lines */
	  linecount = 1;
	  while( fgets(lineBuf, 1024, fp) ){
	    linecount++;
	  }
	  warpGlobals.bibfileList = (char **) AlcCalloc(linecount, sizeof(char *));
	  warpGlobals.statusList = (MAPaintExpressMapStatus *)
	    AlcCalloc(linecount, sizeof(MAPaintExpressMapStatus));
	  warpGlobals.dateList = (char **) AlcCalloc(linecount, sizeof(char *));
	  warpGlobals.authorList = (char **) AlcCalloc(linecount, sizeof(char *));
	  warpGlobals.stageList = (char **) AlcCalloc(linecount, sizeof(char *));
	  warpGlobals.qualityList = (MAPaintExpressMapQuality *)
	    AlcCalloc(linecount, sizeof(MAPaintExpressMapQuality));
	  rewind(fp);
	  linecount = 0;
	  while( fgets(lineBuf, 1024, fp) ){
	    int	statusTmp, qualityTmp;
	    csvCount = sscanf(lineBuf, "%[^,\n], %d, %[^,\n], %[^,\n], %[^,\n], %d", fileBuf,
			      &statusTmp, dateBuf, authorBuf, stageBuf, &qualityTmp);
	    status = statusTmp;
	    quality = qualityTmp;
	    if( csvCount > 0 ){
	      if((fileBuf[0] != '/') && globals.origDir ){
		warpGlobals.bibfileList[linecount] = 
		  (char *) AlcMalloc(sizeof(char)*
				     (strlen(fileBuf) +
				      strlen(globals.origDir) + 4));
		sprintf(warpGlobals.bibfileList[linecount],
			"%s/%s", globals.origDir, fileBuf);
	      }
	      else {
		warpGlobals.bibfileList[linecount] = AlcStrDup(fileBuf);
	      }
	    }
	    if( csvCount > 1 ){
		warpGlobals.statusList[linecount] = status;
	    }
	    else {
	      warpGlobals.statusList[linecount] = MA_EXPRESSMAP_STATUS_NONE;
	    }
	    if( csvCount > 2 ){
		warpGlobals.dateList[linecount] = AlcStrDup(dateBuf);
	    }
	    else {
	      warpGlobals.dateList[linecount] = AlcStrDup("Unknown");
	    }
	    if( csvCount > 3 ){
		warpGlobals.authorList[linecount] = AlcStrDup(authorBuf);
	    }
	    else {
	      warpGlobals.authorList[linecount] = AlcStrDup("Unknown");
	    }
	    if( csvCount > 4 ){
		warpGlobals.stageList[linecount] = AlcStrDup(stageBuf);
	    }
	    else {
	      warpGlobals.stageList[linecount] = AlcStrDup("TS23");
	    }
	    if( csvCount > 5 ){
		warpGlobals.qualityList[linecount] = quality;
	    }
	    else {
	      warpGlobals.qualityList[linecount] = MA_EXPRESSMAP_QUALITY_NONE;
	    }
	    linecount++;
	  }
	  warpGlobals.bibfileListCount = linecount;
	  fclose(fp);
	}

	/* create dummy reference object and install */
	bgdV.type = WLZ_GREY_UBYTE;
	bgdV.v.ubv = 255;
	dummyObj = WlzAssignObject(WlzMakeCuboid(0, 5, 0, 5, 0, 5,
						 WLZ_GREY_UBYTE, bgdV,
						 NULL, NULL, NULL), NULL);
	set_topl_title("dummy");
	globals.file = NULL;
	globals.origObjType = WLZ_3D_DOMAINOBJ;
	install_paint_reference_object(dummyObj);
	WlzFreeObj(dummyObj);

	/* create warp input dialog */
	warpInput2DCb(globals.topl, NULL, NULL);

	/* get the warp controls toggle */
	if((toggle = XtNameToWidget(globals.topl, "*warp_input_2d_frame_title"))){
	  XtVaSetValues(toggle, XmNset, True, NULL);
	  cbs.set = True;
	  XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);
	}

	/* set the files names etc in the table widget */
	if((matrix = XtNameToWidget(globals.topl, "*expressmap_matrix"))){
	  String	*rows, bibfileStr, tailStr, *labels;
	  int		numRows;
	  Boolean	*rowButtonLabels;

	  XtVaGetValues(matrix,
			XmNrows, &numRows,
			XmNtextField, &textField,
			NULL);
	  if( numRows > 0 ){
	    XbaeMatrixDeleteRows(matrix, 0, numRows);
	  }
	  rows = (String *) AlcMalloc(sizeof(String) * warpGlobals.bibfileListCount * 6);
	  labels = (String *) AlcMalloc(sizeof(String) * warpGlobals.bibfileListCount);
	  rowButtonLabels  = (Boolean *)AlcMalloc(sizeof(Boolean) *
						  warpGlobals.bibfileListCount);
       
	  bibfileStartIndex = -1;
	  for(i=0; i < warpGlobals.bibfileListCount; i++){
	    bibfileStr = tailStr = warpGlobals.bibfileList[i];
	    for(j=0; j < strlen(bibfileStr); j++){
	      if( bibfileStr[j] == '/' ){
		tailStr = bibfileStr + (j + 1);
	      }
	    }
	    rows[i*6 + 0] = tailStr;
	    rows[i*6 + 1] = expressMapStatusStrs[warpGlobals.statusList[i]];
	    rows[i*6 + 2] = warpGlobals.dateList[i];
	    rows[i*6 + 3] = warpGlobals.authorList[i];
	    rows[i*6 + 4] = warpGlobals.stageList[i];
	    rows[i*6 + 5] = expressMapQualityStrs[warpGlobals.qualityList[i]];
	    labels[i] = "->";
	    rowButtonLabels[i] = True;

	    /* check for first "None" */
	    if((bibfileStartIndex == -1) &&
	       (warpGlobals.statusList[i] == MA_EXPRESSMAP_STATUS_NONE)){
	      bibfileStartIndex = i;
	    }
	  }
	  XbaeMatrixAddRows(matrix, 0, rows, labels, NULL, i);
	  for(i=0; i < warpGlobals.bibfileListCount; i++){
	    XbaeMatrixSetRowUserData(matrix, i, (XtPointer) (long) i);
	  }
	  XtVaSetValues(matrix,
			XmNbuttonLabels, True,
			XmNrowButtonLabels, rowButtonLabels,
			XmNrowLabelAlignment, XmALIGNMENT_CENTER,
			NULL);
	  AlcFree(rows);
	  AlcFree(labels);
	  AlcFree(rowButtonLabels);
	  XtVaSetValues(textField, XmNeditable, False, NULL);
	}
	

	/* set rapidMap mode */
	if( globals.rapidMapFlg ){
	  if((notebook = XtNameToWidget(globals.topl,
					"*warp_cntrl_notebook"))){
	    XtVaSetValues(notebook,
			  XmNcurrentPageNumber, 2,
			  NULL);

	    /* call the activate callback */
	    if( XmNotebookGetPageInfo(notebook, 2,
				      &pageInfo) == XmPAGE_FOUND ){
	      XmPushButtonCallbackStruct cbs;

	      cbs.reason = XmCR_ACTIVATE;
	      cbs.event = NULL;
	      cbs.click_count = 1;
	      XtCallCallbacks(pageInfo.major_tab_widget,
			      XmNactivateCallback, &cbs);
	    }
	  }
	}
	else {
	  if((notebook = XtNameToWidget(globals.topl,
					"*warp_cntrl_notebook"))){
	    XtVaSetValues(notebook,
			  XmNcurrentPageNumber, 3,
			  NULL);

	    /* call the activate callback */
	    if( XmNotebookGetPageInfo(notebook, 3,
				      &pageInfo) == XmPAGE_FOUND ){
	      XmPushButtonCallbackStruct cbs;

	      cbs.reason = XmCR_ACTIVATE;
	      cbs.event = NULL;
	      cbs.click_count = 1;
	      XtCallCallbacks(pageInfo.major_tab_widget,
			      XmNactivateCallback, &cbs);
	    }
	  }
	}

	/* read first bibfile */
	warpGlobals.bibfileSavedFlg = 1;
	if( bibfileStartIndex < 0 ){
	  bibfileStartIndex = 0;
	}
	warpGlobals.bibfileListIndex = bibfileStartIndex - 1;
	if( globals.rapidMapFlg ){
	  if((toggle = XtNameToWidget(globals.topl, "*warp_input_rapid_form*next"))){
	    XtCallCallbacks(toggle, XmNactivateCallback, NULL);
	  }
	}
	else {
	  if((toggle = XtNameToWidget(globals.topl, "*warp_input_express_form*next"))){
	    XtCallCallbacks(toggle, XmNactivateCallback, NULL);
	  }
	}
      }
      else {
	warpGlobals.bibfileListCSVFile = NULL;
	/* should report an error here */
      }
    }
    else {
      warpGlobals.matrix = NULL;
    }

    return;
}
 
