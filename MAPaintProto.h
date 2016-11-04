#ifndef MAPAINTPROTO_H
#define MAPAINTPROTO_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAPaintProto_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAPaintProto.h
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

/* the release string defines the version name and version number
   defined in MAPaint.c */
extern char		*release_str;
extern char 		*initial_reference_file;
extern char 		*initial_domain_file;
extern PaintGlobals	globals;


/* misc */
extern void 		save_domains(void);
extern void 		clear_autosave(void);

extern unsigned long 	HGU_XGetColorPixel(Display	*dpy,
					   Colormap	cmap,
					   float	red,
					   float	green,
					   float	blue);

/* anatomy_menu.c */
extern MenuItem	*anatomy_menu_items;

extern void 	anatomy_menu_init(Widget topl);

extern void 	set_anatomy_menu(Widget topl);
extern String 	getAnatShortNameFromCoord(int k, int l, int p);
extern String 	getAnatFullNameFromCoord(int k, int l, int p);

/* domain_menu.c */
extern MenuItem		*domain_menu_items;
extern MenuItem		*select_menu_items;

extern int setDomain(WlzObject		*obj,
		     DomainSelection	domain,
		     String		name_str);

extern int clearDomain(DomainSelection	domain);

extern void addSelectDomainCallback(XtCallbackProc	callback,
				    XtPointer		clientData);

extern void removeSelectDomainCallback(XtCallbackProc	callback,
				       XtPointer	clientData);

extern void	select_domain_cb(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void	domain_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void	clear_domain_cb	(Widget 	w,
				 XtPointer 	client_data,
				 XtPointer 	call_data);

extern void 	int_toggle_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void 	save_domain_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void	threed_display_domain_cb(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern int	domain_menu_init(Widget	topl);

extern void	DD_StartDomainDominanceDrag(Widget	w,
					    XEvent	*event,
					    String	*params,
					    Cardinal	*num_params);

extern void DD_DomainDominanceDropSite(
  Widget	site);

extern void save_all_domains_cb(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void clear_all_domains_cb(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void set_domain_menu_entry(DomainSelection	domain,
				  String		name_str);

extern void setMenuLabelsAndColors(void);
extern void MAPaintDomainGetResourceValues(void);

/* domain_utils.c */
extern WlzObject *get_domain_from_object(WlzObject 	*obj,
					 DomainSelection 	domain);

extern int imageValueToDomain(unsigned int val);

extern void selectNextDomain(int downFlag);

extern void setGreysIncrement(WlzObject		*obj,
			      ThreeDViewStruct	*view_struct);

extern void setDomainIncrement(WlzObject	*obj,
			       ThreeDViewStruct	*view_struct,
			       DomainSelection	domain,
			       int		delFlag);

extern void setGreyValuesFromObject(WlzObject	*destObj,
				    WlzObject	*srcObj);

/* domainControls */
extern char *getEMAGE_Name(
  int	domain);
extern char *getEMAGE_Filename(
  int	domain);

extern void setEMAGEDomainsAndColoursCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

extern void setEMAPDomainsAndColoursCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

extern int domainChanged(void);

/* file_menu.c */
extern MenuItem		*file_menu_items;

extern void	MAPaintLogData(String	type,
			       String	value,
			       int	code,
			       Widget	widget);

extern void	file_menu_init(Widget	topl);

extern void ReferenceFileListCb(Widget	w,
				XtPointer	client_data,
				XtPointer	call_data);

extern void fileMenuPopupCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);

extern void	read_obj_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void	write_obj_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void	save_state_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void	restore_state_cb(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void	quit_cb		(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void install_paint_reference_object(WlzObject *obj);
extern void set_topl_title(String name);

extern void setup_ref_display_list_cb(Widget	w,
				      XtPointer	client_data,
				      XtPointer	call_data);

/* theilerStage.c */
extern char *theilerString(char	*str);
extern void theiler_menu_init(Widget	topl);

extern void theiler_stage_setup_cb(Widget	w,
				   XtPointer	client_data,
				   XtPointer	call_data);
extern void set_theiler_stage_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

/* options_menu.c and tools_*.c */
extern MenuItem		*options_menu_items;
extern Widget		tool_controls_dialog;

extern void		options_menu_init(
  Widget		topl);

extern void		tool_controls_cb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void		select_interact_tool_cb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void 		domain_review_cb(
  Widget 		widget,
  XtPointer	 	client_data,
  XtPointer 		call_data);

extern void selectNext2DTool(int downFlag);

/*tools_interactive_paint .c */
extern Widget           CreateDrawPaintBallControls( Widget );

extern void MAPaintDraw2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintDraw2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintDraw2DCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);

extern void MAPaintPaintBall2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintPaintBall2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintPaintBall2DCb(Widget	w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void MAPaintThreshold2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintThreshold2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintThreshold2DCb(Widget	w,
				 XtPointer	client_data,
				 XtPointer	call_data);


/* tools_tablet.c */
extern Widget		CreateTabletControls( Widget );
extern void MAPaintTabletInit(ThreeDViewStruct	*view_struct);
extern void MAPaintTabletQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintTabletCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);

/* tools_tracking.c */
extern Widget		CreateTracking2DControls( Widget );
extern void MAPaintTracking2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintTracking2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintTracking2DCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);

/* tools_edge_tracking.c */
extern Widget		CreateEdgeTracking2DControls( Widget );
extern void MAPaintEdgeTracking2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintEdgeTracking2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintEdgeTracking2DCb(Widget	w,
				    XtPointer	client_data,
				    XtPointer	call_data);

/* tools_affine.c */
extern Widget           CreateAffineControls( Widget );
extern void MAPaintAffine2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintAffine2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintAffine2DCb(Widget	w,
			      XtPointer	client_data,
			      XtPointer	call_data);

/* tools_morphological.c */
extern Widget           CreateMorphologicalControls( Widget );
extern void MAPaintMorphological2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintMorphological2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintMorphological2DCb(Widget	w,
				     XtPointer	client_data,
				     XtPointer	call_data);


extern Widget           CreateThresholdControls( Widget );

/* tools_fill_domain.c */
extern Widget           CreateFillControls( Widget );
extern void MAPaintFill2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintFill2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintFill2DCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);
extern DomainSelection getSelectedDomainType(int	x, 
					     int	y, 
					     ThreeDViewStruct	*view_struct);
extern WlzObject *getSelectedRegion(int		x, 
				    int		y, 
				    ThreeDViewStruct	*view_struct);

/* tools_interactive_geom.c */
extern Widget           CreateGeometryObjectControls( Widget );

extern void MAPaintGeom2DInit(ThreeDViewStruct	*view_struct);
extern void MAPaintGeom2DQuit(ThreeDViewStruct	*view_struct);
extern void MAPaintGeom2DCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);

/* MADomainSurgeryDialog.c */
extern Widget createDomainSelectOptionMenu(String	name,
					   Widget	parent,
					   XtCallbackProc	callbackproc);

extern void domainSurgeryCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);

extern Widget createDomainSurgeryDialog(Widget	topl);

/* MADomainReviewDialog.c */
extern void domainReviewCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);

extern Widget createDomainReviewDialog(Widget	topl);

/* MARealignmentDialog.c */
extern void realignmentCb(Widget	w,
			  XtPointer	client_data,
			  XtPointer	call_data);

extern Widget createRealignmentDialog(Widget	topl);

/* MAWarpInput2DDialog.c */
extern void warpInput2DCb(Widget	w,
			  XtPointer	client_data,
			  XtPointer	call_data);

extern Widget createWarpInput2DDialog(Widget	topl);

extern void recalcWarpProcObjCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

/* MAWarpInputPaintingCb.c */
extern void canvas_warp_painting_cb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data);

/* paint_utils.c, MAColormapUtils.c */
extern void myHGU_XmHelpStandardCb(Widget	w,
				   XtPointer	client_data,
				   XtPointer	call_data);

extern char *HGU_XRequestToString(
  char	request_code);

extern int set_backingX(Display	*dpy,
			Window	win);

extern int set_grey_values_from_view(WlzObject 		*obj,
				     ThreeDViewStruct 	*view_struct,
				     unsigned int 	col,
				     unsigned int 	planes);

extern int set_grey_values_from_domain(WlzObject 	*obj,
				       WlzObject	*destObj,
				       unsigned int	col,
				       unsigned int	planes);

extern int set_grey_values_from_ref_object(WlzObject	*obj,
					   WlzObject	*ref_obj);

extern WlzObject *get_domain_from_view(ThreeDViewStruct 	*view_struct,
				       DomainSelection 	domain);

extern WlzObject *get_mask_domain_from_object(WlzObject        *obj,
					      DomainSelection      domain);

extern WlzObject *get_mask_domain_from_view(ThreeDViewStruct  *view_struct,
					    DomainSelection   domain);

extern void HGU_XmSetHourGlassCursor(Widget	w);
extern void HGU_XmUnsetHourGlassCursor(Widget	w);

extern void MAPaintReportWlzError(Widget	w,
				  String	srcStr,
				  WlzErrorNum	wlzErr);

extern void install_cmap_cb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);
extern void UnmanageChildCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);

extern void destroy_cb(Widget          w,
		       XtPointer       client_data,
		       XtPointer       call_data);
extern void popup_dialog_cb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);


extern int InteractDispatchEvent(Display	*dpy,
				 Window		win,
				 XtPointer	client_data,
				 XEvent		*event);

extern int InteractHelpCb(Display	*dpy,
			  Window	win,
			  XtPointer	client_data,
			  String	help_str);

extern void HGU_XmCreatePrivateColormap(Display *dpy);

extern int set_paint_colormap(Widget		w,
			      Colormap		cmap,
			      PaintCmapStruct	*cmpstr);
extern void MAPaintColormapGetResourceValues(void);

/* MAColormapDialog.c */
extern void colormap_cb(Widget		w,
			XtPointer	client_data,
			XtPointer	call_data);

extern Widget create_colormap_dialog(Widget	topl);

/* MAAutosaveDialog.c */
extern void autosave_all_domains(void);
extern void autosave_opts_cb(Widget	w,
			     XtPointer	client_data,
			     XtPointer	call_data);
extern void autosavetimeout_cb(XtPointer	client_data,
			       XtIntervalId	*id);

extern Widget create_autosave_dialog(Widget	topl);

/* MASaveSequenceDialog.c */
extern void save_seq_opts_cb(Widget	w,
			     XtPointer	client_data,
			     XtPointer	call_data);

extern Widget create_save_seq_dialog(Widget	topl);

/* MAOpenGLUtils.c */
extern void MAOpenGLDisplayBoundList(WlzBoundList 	*bndlist,
				     float		z);

extern void MAOpenGLDisplayDomainIndex(WlzObject	*obj,
				       int		domain_index);

extern void MAOpenGLDrawScene(Widget canvasW);

extern void MAOpenGLDisplaySection(ThreeDViewStruct	*view_struct);

extern void MAOpenGLInitCb(Widget 	w, 
			   XtPointer	clientData,
			   XtPointer	callData);

/* view_menu.c */
extern MenuItem			*view_menu_items;
extern ThreeDViewStruct		*paint_key;
extern ViewListEntry		*global_view_list;
extern unsigned int		ButtonStateIgnoreMask;

extern void setControlButtonsSensitive(ThreeDViewStruct        *view_struct,
				       Boolean	sensitive);

extern void	view_cb			(Widget 	w,
					 XtPointer 	client_data,
					 XtPointer 	call_data);

extern void	ViewFeedback		(Widget 	w,
					 XEvent		*event,
					 String		*params,
					 Cardinal	*num_params);

extern int	view_menu_init		(Widget		w);

extern void	set_view_dialog_title(Widget	dialog,
				      double	theta,
				      double	phi);

extern void canvas_input_cb(Widget          w,
			    XtPointer	client_data,
			    XtPointer	call_data);

extern void display_pointer_feedback_information(
  ThreeDViewStruct	*view_struct,
  int			x, 
  int			y);

extern void display_pointer_feedback_informationV(
  ThreeDViewStruct	*view_struct,
  int			x, 
  int			y,
  int			sel_domain);

extern void popupDrawnButtonCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

/* view2DPaintingCb.c */
extern void canvas_2D_painting_cb(Widget          w,
				  XtPointer	client_data,
				  XtPointer	call_data);
extern void cleanDomainsCb(Widget          w,
			   XtPointer	client_data,
			   XtPointer	call_data);
extern void canvasMagPlusCb(Widget          w,
			    XtPointer	client_data,
			    XtPointer	call_data);
extern void canvasMagMinusCb(Widget          w,
			     XtPointer	client_data,
			     XtPointer	call_data);
extern void installCurrentPaintTool(Widget		w,
				    ThreeDViewStruct	*view_struct);
extern void removeCurrentPaintTool(Widget		w,
				   ThreeDViewStruct	*view_struct);

extern void setViewScale(
  ThreeDViewStruct	*viewStruct,
  double		newScale,
  int			fixedX,
  int			fixedY);

extern void setViewMode(
  ThreeDViewStruct	*viewStruct,
  WlzThreeDViewMode	newMode);

/* view_utils.c */
extern int init_view_struct		(ThreeDViewStruct 	*view_struct);
extern int reset_view_struct		(ThreeDViewStruct 	*view_struct);
extern int free_view_struct		(ThreeDViewStruct 	*view_struct);

extern void getViewDomains(ThreeDViewStruct	*view_struct);
extern void installViewDomains(ThreeDViewStruct	*view_struct);

extern void redisplay_view_cb		(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void redisplay_all_views_cb	(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void display_view_cb		(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void display_all_views_cb     	(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void view_feedback_cb		(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void destroy_view_cb		(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void distance_cb			(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void scale_cb			(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void theta_cb			(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void phi_cb			(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void zeta_cb			(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void psi_cb			(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void view_mode_cb		(Widget		w,
					 XtPointer	client_data,
					 XtPointer	call_data);

extern void view_struct_clear_prev_obj(ThreeDViewStruct	*view_struct);

/* viewFixedPointUtils */

extern void fixed_1_cb		(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void fixed_2_cb		(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void up_vector_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void controls_io_write_cb(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void controls_io_read_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern int controls_io_quiet_read(String	fileStr,
				  ThreeDViewStruct	*view_struct);

extern void setupFixed_1_Menu(Widget		widget,
			      ThreeDViewStruct	*view_struct);
extern void setupFixed_2_Menu(Widget		widget,
			      ThreeDViewStruct	*view_struct);
extern void setupUpVectorMenu(Widget		widget,
			      ThreeDViewStruct	*view_struct);
extern void setupIOVectorMenu(Widget		widget,
			      ThreeDViewStruct	*view_struct);
extern void setupUpIOMenu(Widget		widget,
			      ThreeDViewStruct	*view_struct);

extern void setViewSliderSensitivities(ThreeDViewStruct	*view_struct,
				       Boolean		bool);

extern void PostIt(
  Widget	pb,
  XtPointer	client_data,
  XEvent	*event,
  Boolean	*continue_to_dispatch);

/* from all over! */
extern Widget	create_main_buttonbar	(Widget w); /* from main_buttonbar.c */
extern Widget	create_main_menubar	(Widget w); /* from main_menubar.c   */
extern Widget	create_main_work_area	(Widget w); /* from main_work_area.c */

extern void	init_main_menubar	(Widget w); /* from main_menubar.c   */
extern void	init_main_work_area	(Widget w); /* from main_workarea.c  */
extern void	init_main_menubar	(Widget w); /* from main_menubar.c   */
extern void	init_main_buttonbar	(Widget w); /* from main_menubar.c   */

extern void	init_paint_cmapstruct	(Widget	canvas);

extern void 	ThreeDResetCb(Widget	w,
			      XtPointer	client_data,
			      XtPointer	call_data);

/* objPropsDialog.c */
extern void obj_props_dialog_init(Widget topl);

extern void obj_props_cb(Widget		w,
			 XtPointer	client_data,
			 XtPointer	call_data);

extern void setup_obj_props_cb(Widget		w,
			       XtPointer	client_data,
			       XtPointer	call_data);

/* NalgsDPSearch.c */
extern int NalgsDPSearch(int, int, double **, double **, int **,
			 double (*)(int, int, int, int **));

/* MATrackDomain.c */
extern WlzObject *HGU_TrackDomain(WlzObject	*refObj,
				  WlzObject	*dstObj,
				  WlzObject	*refDmn,
				  MATrackDomainSearchMethod	searchMethod,
				  MATrackDomainSearchParams	*searchParams,
				  MATrackDomainCostType		costType,
				  MATrackDomainCostParams	*costParams,
				  WlzErrorNum			*wlzErr);
				  

/* MAPMSnake.c */
extern void PMSnakeNlcSetup(PMSnakeNLCParams	*params);

extern double PMSnakeNlc(int nodeIndx, int pathIndx, int prevPathIndx,
		       int **optimalPath);

extern void PMEdgeSnakeNlcSetup(PMEdgeSnakeNLCParams	*params,
				double		**direction);

extern double PMEdgeSnakeNlc(int nodeIndx, int pathIndx, int prevPathIndx,
			   int **optimalPath);

extern void PMSnake(double		**localCost,
		    int			numNodes,
		    MATrackMatchPointStruct	*matchPts,
		    PMSnakeSearchParams	*searchParams,
		    PMSnakeLCParams	*LCParams,
		    PMSnakeNLCParams	*NLCParams,
		    int			endConstraint);

extern void PMEdgeSnake(double			**localCost,
			double			**direction,
			int			numNodes,
			MATrackMatchPointStruct	*matchPts,
			PMSnakeSearchParams	*params,
			PMSnakeLCParams		*LCParams,
			PMEdgeSnakeNLCParams	*NLCParams,
			int			endConstraint);

/* MAPaintUndoDomain.c */
extern void pushUndoDomains(ThreeDViewStruct	*view_struct);
extern void UndoDomains(ThreeDViewStruct	*view_struct);
extern void RedoDomains(ThreeDViewStruct	*view_struct);

extern void initUndoList(int depth);
extern void freeUndoList(void);
extern void clearUndoDomains(void);
extern void undoCb(Widget	widget,
		   XtPointer	client_data,
		   XtPointer	call_data);
extern void redoCb(Widget	widget,
		   XtPointer	client_data,
		   XtPointer	call_data);

/* MAPaintSocket.c */
extern void sockListenCb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void sockCloseCb		(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

/* MAPaintEvent.c */
extern void eventRemapCb(Widget	widget,
			 XtPointer	client_data,
			 XtPointer	call_data);
extern Widget createEventRemapDialog(Widget parent);

/* MAWarpInputXUtils.c */
extern int HGU_XGetColorIndexFromMask24(
  unsigned long mask,
  int		order);

/* MAWarpInputPages.c */
extern void expressMapSaveListCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data);

/* do not add anything after this line */
#endif /* MAPAINTPROTO_H */
