#ifndef MAWARP_H
#define MAWARP_H

#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MAWarp.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MAWarp.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:15:46 2009
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

#define WARP_MAX_NUM_VTXS 1024

extern Widget create_view_window_dialog(Widget topl,
					double theta,
					double phi,
					WlzDVertex3 *fixed);
typedef enum {
  MA_OVERLAY_MIXTYPE_RG1,	/* red-green no blue */
  MA_OVERLAY_MIXTYPE_RG2,	/* red-green avg blue */
  MA_OVERLAY_MIXTYPE_RB1,	/* red-blue no green */
  MA_OVERLAY_MIXTYPE_RB2,	/* red-blue avg green */
  MA_OVERLAY_MIXTYPE_GB1,	/* green-blue no red */
  MA_OVERLAY_MIXTYPE_GB2,	/* green-blue avg green */
  MA_OVERLAY_MIXTYPE_RNR,	/* red not-red(cyan) mixture */
  MA_OVERLAY_MIXTYPE_GNG,	/* green not-green (magenta) mixture */
  MA_OVERLAY_MIXTYPE_BNB,       /* blue not-blue (yellow) mixture */
  MA_OVERLAY_MIXTYPE_DITHER1,    /* dither/overlay image one */
  MA_OVERLAY_MIXTYPE_DITHER2,    /* dither/overlay image two */
  MA_OVERLAY_MIXTYPE_BLINK,	/* blink comparator */
    /* dummy entry to terminate list */
  MA_OVERLAY_MIXTYPE_LAST
} MAOverlayMixType;

typedef enum _MAPaintStructElemType
{
  MAPAINT_8_CONN_SE,
  MAPAINT_4_CONN_SE,
  MAPAINT_CIRCLE_SE,
  MAPAINT_SPHERE_SE,
  MAPAINT_SQUARE_SE,
  MAPAINT_CUBE_SE
} MAPaintStructElemType;

typedef struct {
  WlzObject		*obj;
  Widget		canvas, text, popup;
  XImage		*ximage;
  XImage		*ovlyImages[2];
  Pixmap		ovlyPixmaps[2];
  GC			gc;
  double		mag;
  int			rot;
  int			transpose;
  double		gamma;
  double		mixRatio;
  MAOverlayMixType	mixType;
  int			blinkTime[2];
  int			grey_invert;
} MAPaintImageWinStruct;

typedef struct {
  /* general */
  ThreeDViewStruct	*view_struct;
  Widget		warp2DInteractDialog;
  char			*srcFile;
  WlzEffFormat		srcFileType;
  char			*sgnlFile;
  WlzEffFormat		sgnlFileType;
  WlzObject		*sgnlObj;
  WlzObject		*sgnlProcObj;
  WlzObject		*sgnlThreshObj;
  String		warpBibFile;
  int			warpPanedFlg;

  /* destination canvas and affine transform */
  MAPaintImageWinStruct	dst;

  /* source canvas and affine transform */
  MAPaintImageWinStruct	src;
  int	srcXOffset;
  int	srcYOffset;

  /* overlay canvas and affine transform */
  MAPaintImageWinStruct	ovly;
  
  /* signal canvas and affine transform */
  MAPaintImageWinStruct	sgnl;

  /* tie-points */
  WlzDVertex2	dst_vtxs[WARP_MAX_NUM_VTXS];
  WlzDVertex2	src_vtxs[WARP_MAX_NUM_VTXS];
  int		num_vtxs;
  int		sel_vtx;
  int		sel_vtxs[WARP_MAX_NUM_VTXS];
  int		tp_state;
  GC		green_gc;
  GC		red_gc;
  GC		blue_gc;
  GC		yellow_gc;
  WlzAffineTransform	*affine;
  WlzTransformType	affineType;
  WlzBasisFnTransform	*basisTr;
  WlzFnType		wlzFnType;
  WlzMeshTransform	*meshTr;
  int			meshErrFlg;
  WlzMeshGenMethod	meshMthd;
  int			meshMinDst;
  int			meshMaxDst;

  /* signal processing controls */
  Widget	sgnlControls;

  /* threshold controls */
  WlzRGBAThresholdType	thresholdType;
  WlzRGBAColorChannel	threshColorChannel;
  int		threshRangeLow;		/* single threshold */
  int		threshRangeHigh;
  int		threshRangeRGBLow[3];	/* colour threshold */
  int		threshRangeRGBHigh[3];
  WlzRGBAColorSpace	threshRGBSpace;	/* which colour space */
  WlzUInt	threshRGBCombination;	/* logical combination */
  WlzPixelV	lowRGBPoint;	/* low-point for slice/box/ball */
  WlzPixelV	highRGBPoint;	/* high-point for slice/box/ball */
  double	colorEllipseEcc;	/* ball eccentricity */
  int		lastThresholdPageNum;
  int		globalThreshFlg; /* do global thresholding */
  WlzIVertex2	globalThreshVtx;
  int		incrThreshFlg; /* do incremental */
  WlzObject	*incrThreshObj;
  int		pickThreshFlg; /* pick mode for endpoint values */
  int		distanceThreshFlg; /* distance mode for endpoint values */

  /* post-processing controls */
  float		seElemRadius;
  MAPaintStructElemType	seType;

  /* rapid-map and express-map controls */
  int		bibfileSavedFlg;
  int		bibfileListCount;
  int		bibfileListIndex;
  char		*bibfileListCSVFile;
  char		**bibfileList;
  char		**statusList;
  char		**dateList;
  char		**authorList;
  char		**stageList;
  Widget	bibfileSelectButtonsFrame;
  Widget	matrix;

  /* tie-point tracking controls */
  int		tpTrackingFlg;
  int		tpTrackingInit;

} MAPaintWarp2DStruct;

typedef enum
{
  TP_INACTIVE,
  TP_DST_DEFINED,
  TP_SRC_DEFINED,
  TP_SELECTED
} WarpTiePointState;

extern MAPaintWarp2DStruct warpGlobals;
extern Widget warp_read_src_dialog;
extern Widget warp_read_sgnl_dialog;
extern const char *warpErrStr;
extern char warpErrBuf[256];

extern WlzDVertex2 warpDisplayTransBack(
  WlzDVertex2	vtx,
  MAPaintImageWinStruct	*winStruct);

extern WlzDVertex2 warpDisplayTransFore(
  WlzDVertex2	vtx,
  MAPaintImageWinStruct	*winStruct);

extern void warpSetXImage(
  MAPaintImageWinStruct *winStruct);

extern void warpSetOvlyXImage(
  MAPaintImageWinStruct *winStruct);

extern void warpSetOvlyXImages(
  MAPaintImageWinStruct *winStruct,
  int	imageIndex);

extern void warpRedisplayOvly(void);

extern WlzErrorNum WlzSetMeshAffineProduct(
  WlzMeshTransform	*meshTr,
  WlzAffineTransform	*affineTr);

extern void warpSetOvlyObject(void);

extern int warpCloseDstVtx(
  int	x,
  int	y);

extern int warpCloseSrcVtx(
  int	x,
  int	y);

extern void warpUndisplayVtx(
  MAPaintImageWinStruct	*winStruct,
  WlzDVertex2	vtx);

extern void warpDisplayTiePoints(void);

extern void warpDisplayDstMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpDisplaySrcMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpDisplayDomain(
  MAPaintImageWinStruct	*winStruct,
  WlzObject	*obj,
  int		displayFlg);

extern void warpCanvasExposeCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpSetXImageCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpSetXOvlyImageDstCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpSetXOvlyImageSrcCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpIncrGammaCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpDecrGammaCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpMixRatioCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpSetOvlyMethodCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpInvertGreyCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpCanvasMagCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpCanvasMagPlusCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpCanvasMagMinusCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpCanvasRotCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpCanvasFlipCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpCanvasMeshCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpCanvasResetCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warpDstCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data);

extern void warpSrcCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data);

extern void warpOvlyCanvasInputCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data);

extern void warpInput2DDestroyCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void setupWarpIOMenu(
  Widget		widget,
  ThreeDViewStruct	*view_struct);

extern void warp2DInteractDeleteLastCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern void warp2DInteractDeleteAllCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

extern Widget createWarpSgnlControlsFrame(
  Widget	parent,
  String	name,
  ThreeDViewStruct	*view_struct);

extern Widget createWarpSgnlDisplayFrame(
  Widget	parent,
  String	name,
  ThreeDViewStruct	*view_struct);

extern Widget createWarpDisplayFrame(
  Widget	parent,
  String	name,
  Visual	*visual,
  int		depth);

extern WlzObject *mapaintWarpObj(
  WlzObject	*obj,
  WlzInterpolationType	interpType);

extern void view_canvas_highlight(
  ThreeDViewStruct	*view_struct,
  Boolean		highlight);

extern MAPaintWarp2DStruct warpGlobals;
extern Widget warp_read_src_dialog;
extern Widget warp_read_sgnl_dialog;

extern void warpSetSignalDomain(
  WlzIVertex2	*selVtx);

extern void sgnlCanvasMotionInputCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data);

extern void sgnlIncrClear(void);
extern void sgnlIncrPush(
  WlzObject	*obj);
extern WlzObject *sgnlIncrPop(void);
extern WlzObject *sgnlIncrObj(void);

extern void sgnlCanvasInputCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data);

extern void warpSetSignalProcObj(void);
extern void warpSetSignalThreshObj(void);

extern void warpSetSignalDomain(
  WlzIVertex2	*selVtx);
extern void sgnlResetSgnlDomain(
  Widget	w,
  WlzIVertex2	*selVtx);
extern WlzPixelV warpThreshCentre(
  WlzPixelV	pix1,
  WlzPixelV	pix2);
extern void warpThreshSetLimitsFromDist(
  WlzPixelV	centreCol,	
  WlzDVertex2	dist,
  WlzPixelV	*sgnlStart,
  WlzPixelV	*sgnlFinish);
extern void sgnlInteractSetHighLowControls(
  WlzPixelV	*pix1,
  WlzPixelV	*pix2);

extern void warpSetThreshColorTypeSensitive(
  Boolean	sensitive);
extern void warpSetThreshColorTypeToggles(
  WlzRGBAColorSpace	colSpc);

extern void warpColorSpaceCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

extern void warpResetCWDCb(
  Widget		w,
  XtPointer		client_data,
  XtPointer		call_data);

/* misc callbacks */
extern void thresholdMajorPageSelectCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

/* pages for warp controls */
extern Widget createWarpStandardControlsPage(
  Widget		notebook,
  ThreeDViewStruct	*view_struct);

extern Widget createWarpRapidControlsPage(
  Widget		notebook,
  ThreeDViewStruct	*view_struct);

extern Widget createWarpExpressControlsPage(
  Widget		notebook,
  ThreeDViewStruct	*view_struct);

extern void expressMapStatusChange(
  int	status);

extern Widget createTiePointTrackingControlsPage(
  Widget		notebook,
  ThreeDViewStruct	*view_struct);

/* the various pages for signal pre-processing */
extern Widget createSignalPreProcPage(
  Widget	notebook);
extern Widget createSignalFilterPage(
  Widget	notebook);
extern Widget createSignalThresholdPage(
  Widget	notebook);
extern Widget createSignalAutoPage(
  Widget	notebook);
extern Widget createSignalPostProcPage(
  Widget	notebook);

/* do not add anything after this line */
#endif /* MAWARP_H */
