#ifndef MAPAINTTYPES_H
#define MAPAINTTYPES_H

#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Mouse Atlas Project					*
*   File       :   MAPaintTypes.h					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Tue Feb 24 14:18:01 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

typedef enum {
  GREYS_DOMAIN = 0,
  DOMAIN_1	 = 1,
  DOMAIN_2	 = 2,
  DOMAIN_3	 = 3,
  DOMAIN_4	 = 4,
  DOMAIN_5	 = 5,
  DOMAIN_6	 = 6,
  DOMAIN_7	 = 7,
  DOMAIN_8	 = 8,
  DOMAIN_9	 = 9,
  DOMAIN_10	 = 10,
  DOMAIN_11	 = 11,
  DOMAIN_12	 = 12,
  DOMAIN_13	 = 13,
  DOMAIN_14	 = 14,
  DOMAIN_15	 = 15,
  DOMAIN_16	 = 16,
  DOMAIN_17	 = 17,
  DOMAIN_18	 = 18,
  DOMAIN_19	 = 19,
  DOMAIN_20	 = 20,
  DOMAIN_21	 = 21,
  DOMAIN_22	 = 22,
  DOMAIN_23	 = 23,
  DOMAIN_24	 = 24,
  DOMAIN_25	 = 25,
  DOMAIN_26	 = 26,
  DOMAIN_27	 = 27,
  DOMAIN_28	 = 28,
  DOMAIN_29	 = 29,
  DOMAIN_30	 = 30,
  DOMAIN_31	 = 31,
  DOMAIN_32	 = 32,
  MASK_DOMAIN  = 100
} DomainSelection;

typedef enum {
    PRIORITY_1	 = 1,
    PRIORITY_2	 = 2,
    PRIORITY_3	 = 3,
    PRIORITY_4	 = 4,
    PRIORITY_5	 = 5,
    PRIORITY_6	 = 6,
    PRIORITY_7	 = 7,
    PRIORITY_8	 = 8,
    PRIORITY_9	 = 9,
    PRIORITY_10	 = 10,
    PRIORITY_11	 = 11,
    PRIORITY_12	 = 12,
    PRIORITY_13	 = 13,
    PRIORITY_14	 = 14,
    PRIORITY_15	 = 15,
    PRIORITY_16	 = 16,
    PRIORITY_17	 = 17,
    PRIORITY_18	 = 18,
    PRIORITY_19	 = 19,
    PRIORITY_20	 = 20,
    PRIORITY_21	 = 21,
    PRIORITY_22	 = 22,
    PRIORITY_23	 = 23,
    PRIORITY_24	 = 24,
    PRIORITY_25	 = 25,
    PRIORITY_26	 = 26,
    PRIORITY_27	 = 27,
    PRIORITY_28	 = 28,
    PRIORITY_29	 = 29,
    PRIORITY_30	 = 30,
    PRIORITY_31	 = 31,
    PRIORITY_32	 = 32
} PrioritySelection;

typedef enum {
  MAPAINT_SHOW_SOLID_SECTION	= 1,
  MAPAINT_HIGHLIGHT_SECTION	= 1<<1,
  MAPAINT_SHOW_FIXED_POINT	= 1<<2,
  MAPAINT_SHOW_FIXED_LINE	= 1<<3,
  MAPAINT_SHOW_UP_VECTOR	= 1<<4,
  MAPAINT_SHOW_VIEW_DIRECTION	= 1<<5,
  MAPAINT_FIXED_LINE_SET	= 1<<6,
  MAPAINT_POLY_VIEWFB_MODE	= 1<<7,
  MAPAINT_SOLID_VIEWFB_MODE	= 1<<8,
  MAPAINT_MASK_VIEWFB_MODE	= 1<<9,
  MAPAINT_TEMPLATE_VIEWFB_MODE	= 1<<10,
  MAPAINT_SWITCH_VIEWFB_MODE	= 1<<11,
  MAPAINT_IMAGE_VIEWFB_MODE	= 1<<12,
  MAPAINT_CONTROL_MAXIMUM	= 1<<13
} ThreeDViewControlMask;

typedef enum {
  MAPAINT_NO_PAINT_ACTION,
  MAPAINT_DRAW_2D,
  MAPAINT_PAINT_BALL_2D,
  MAPAINT_GEOMETRY_OBJ_2D,
  MAPAINT_THRESHOLD_2D,
  MAPAINT_FILL_2D,
  MAPAINT_MORPHOLOGICAL_2D,
  MAPAINT_AFFINE_2D,
  MAPAINT_IMAGE_TRACKING_2D,
  MAPAINT_EDGE_TRACKING_2D,
  MAPAINT_TRACKING_2D,
  MAPAINT_TABLET_2D,
  MAPAINT_SURGERY_SECTION_CUT,
  MAPAINT_SURGERY_LASER_CUT
} PaintActionType;

typedef enum {
  MAPAINT_GEOMETRY_OBJ_FCIRCLE,
  MAPAINT_GEOMETRY_OBJ_FSQUARE,
  MAPAINT_GEOMETRY_OBJ_VCIRCLE,
  MAPAINT_GEOMETRY_OBJ_VSQUARE,
  MAPAINT_GEOMETRY_OBJ_FSPHERE,
  MAPAINT_GEOMETRY_OBJ_FCUBE,
  MAPAINT_GEOMETRY_OBJ_VSPHERE,
  MAPAINT_GEOMETRY_OBJ_VCUBE
} PaintGeometryObjectType;


typedef enum {
  MAPAINT_DOMAIN_REVIEW_OBJ,
  MAPAINT_DOMAIN_MAPAINT_OBJ,
  MAPAINT_DOMAIN_READ
} PaintDomainSourceType;

typedef enum {
  MATRACK_PMSNAKE_SEARCH_METHOD
} MATrackDomainSearchMethod;

typedef enum {
  MATRACK_CORRELATION_COST_TYPE,
  MATRACK_EDGE_COST_TYPE
} MATrackDomainCostType;

typedef enum {
  MAPAINT_8BIT_ONLY_MODE,
  MAPAINT_8_24BIT_MODE,
  MAPAINT_24BIT_ONLY_MODE
} MAPaintVisualMode;

typedef struct {
  int		spacing;
  int		range;
} PMSnakeSearchParams, MATrackDomainSearchParams;

typedef struct {
  double	nu_dist;
} PMSnakeLCParams;

typedef struct {
  double	nu_alpha;
  double	nu_kappa;
} PMSnakeNLCParams;

typedef struct {
  PMSnakeNLCParams	*snakeParams;
  double	nu_direction;
} PMEdgeSnakeNLCParams;

typedef struct {
  int			size;
  PMSnakeLCParams	*LCParams;
  PMSnakeNLCParams	*NLCParams;
} CorrelationCostParams, MATrackDomainCostParams;

typedef struct {
  double		width;
  double		gradient;
  PMSnakeLCParams	*LCParams;
  PMEdgeSnakeNLCParams	*NLCParams;
} EdgeCostParams;

typedef struct {
  double	sintheta;
  double	costheta;
  int		index;
  int		x;
  int		y;
  int		x_off;
  int		y_off;
} MATrackMatchPointStruct;

typedef struct {
    Colormap		cmap;
    unsigned long	pixels[256];
    int			npixels;
    int			gmin;
    int			gmax;
    int			invert;
    float		gamma;
    int			num_overlays;
    int			num_solid_overlays;
    int			low_thresh;
    int			high_thresh;
    unsigned int	ovly_planes;
    unsigned int	ovly_cols[33], ovly_incr[6];
    unsigned char	ovly_red[33], ovly_green[33], ovly_blue[33];
    float		ovly_contrast[33];
} PaintCmapStruct;

typedef struct {
  Widget		dialog, controls, canvas, slider, text;
  Widget		control_buttons;
  unsigned int		controlFlag;
  XImage		*ximage;
  int			tablet_initialised;
  WlzDVertex3		fixed_2;
  WlzFVertex2		ref1, ref2;
  WlzDVertex3		norm2, norm3;
  WlzThreeDViewStruct	*wlzViewStr;
  WlzObject		*curr_domain[33];
  WlzObject		*prev_domain[33];
  WlzObject		*prev_view_obj;
  double		prev_dist;
  WlzObject		*view_object;
  WlzObject		*painted_object;
  WlzObject		*masked_object;
  GLuint		display_list;
  int			noPaintingFlag;
  int			viewLockedFlag;
  String		titleStr;
} ThreeDViewStruct;

typedef struct _ViewListEntry {
  Widget		dialog;
  ThreeDViewStruct 	*view_struct;
  struct _ViewListEntry	*next;
} ViewListEntry;

typedef void (*PaintActionInitProc)(ThreeDViewStruct	*view_struct);
typedef void (*PaintActionQuitProc)(ThreeDViewStruct	*view_struct);

/* global variables */
typedef struct _PaintGlobals{
  
  Widget		topl;
  XtAppContext		app_con;
  char			*app_name;
  int			sectViewFlg;
  Boolean		emageFlg;

  /* file_menu globals */
  char			*file;
  WlzObject		*obj;
  WlzObject		*orig_obj;
  WlzObject		*fb_obj;
  WlzObjectType		origObjType;
  WlzEffFormat		origObjExtType;
  int			num_lines, num_cols, num_planes;
  unsigned long		autosave_time;
  String		autosave_file;
  String		autosave_dir;
  XtIntervalId		autosavetimeoutID;
  GLuint		ref_display_list;
  WlzDVertex3		bbox_vtx, bbox_size;
  String		base_theiler_dir;
  String		theiler_stage;
  String		logfile;
  FILE			*logfileFp;
  String		resourceFile;
  AlcDLPList		*fileList;

  /* main_work_area globals */
  Widget		canvas;
  Display		*dpy;
  Window		win;
  MAPaintVisualMode	visualMode;
  int			toplDepth;
  Visual		*toplVisual;
  Visual		*warpVisual;

  /* main_button_bar globals */

  /* domain menu globals */
  GC			gc_greys, gc_reds, gc_blues;
  WlzObject		*mask_domain;
  PrioritySelection	current_priority;
  DomainSelection	current_domain;
  unsigned int		current_col;
  int			auto_increment;
  int			propogate;
  int			propogate_sel;
  int			domain_changed_since_saved[33];
  int			priority_to_domain_lut[33];
  String		domain_name[33];
  String		domain_filename[33];
  GLuint		domain_display_list[33];
  int			OpenGLDisplayDomainStep;

  /* view_menu globals */
  GC			gc_set;

  /* options menu globals */
  PaintCmapStruct	*cmapstruct;
  Colormap		cmap;
  unsigned char		colormap[3][256];
  Pixmap		stipple;
  int			paint_action_quit_flag;
  PaintActionType	currentPaintAction;
  XtCallbackProc	currentPaintActionCbFunc;
  PaintActionInitProc	currentPaintActionInitFunc;
  PaintActionQuitProc	currentPaintActionQuitFunc;
  WlzObject		*review_domain_obj;

} PaintGlobals;



/* do not add anything after this line */
#endif /* MAPAINTTYPES_H */
