#ifndef MAPAINT_EVENT_H
#define MAPAINT_EVENT_H
#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Mouse Atlas MAPaint					*
*   File       :   MAPaintEvent.h					*
*************************************************************************
*   Author Name :  richard						*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Thu Nov 27 17:54:50 2003				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

typedef enum _MAPaintContext {
  MAPAINT_NONE_CONTEXT		=(0x1<<0),
  MAPAINT_3D_VIEW_CONTEXT	=(0x1<<1),
  MAPAINT_SECT_VIEW_CONTEXT	=(0x1<<2),
  MAPAINT_WARP_2D_CONTEXT	=(0x1<<3),
  MAPAINT_WARP_3D_CONTEXT	=(0x1<<4),
  MAPAINT_WARP_SGNL_CONTEXT	=(0x1<<5),
  MAPAINT_ANY_CONTEXT	= 0xffffffff
} MAPaintContext;

typedef enum _MAPaintContextMode {
  MAPAINT_NONE_MODE		=(0x1<<0),
  MAPAINT_VIEW_MODE		=(0x1<<1),
  MAPAINT_PAINT_MODE		=(0x1<<2),
  MAPAINT_WARP_MODE		=(0x1<<3),
  MAPAINT_THRESHOLD_MODE	=(0x1<<4),
  MAPAINT_PICK_MODE		=(0x1<<5),
  MAPAINT_PAINT_DOMAIN_MODE	=(0x1<<6),
  MAPAINT_PAINT_BALL_MODE	=(0x1<<7),
  MAPAINT_AFFINE_MODE		=(0x1<<8),
  MAPAINT_FILL_MODE		=(0x1<<9),
  MAPAINT_MORPHOLOGICAL_MODE	=(0x1<<10),
  MAPAINT_GEOMETRY_MODE		=(0x1<<11),
  MAPAINT_TRACK_MODE		=(0x1<<12),
  MAPAINT_ANY_MODE		=(0xffffffff)
} MAPaintContextMode;

/* structure to hold context dependent mappings */
typedef struct _MAPaintEventMapping {
  MAPaintContext	context;
  MAPaintContextMode	mode;
  int			srcEventType;
  WlzUInt		srcButton;
  WlzUInt		srcModMask;
  int			dstEventType;
  WlzUInt		dstButton;
  WlzUInt		dstModMask;
  char			*helpStr;
  Widget		eventLabel;
  Widget		buttonLabel;
  Widget		modifierLabel;
} MAPaintEventMapping;

/* table of accepted input events.
   For a given context and mode if an event is found it is re-mapped
   if not found then the event is unchanged but MAPaintRemapEvent()
   will return an error, this list therefore only need to hold events
   that we will want to re-map.

   Later this could include ALL allowed events to be more strict
   about input.

   Actual table in MAPaintEvent.c */

extern MAPaintEventMapping	*mapaintEventMapping;



#endif /* MAPAINT_EVENT_H */
