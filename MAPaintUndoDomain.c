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
*   File       :   MAPaintUndoDomain.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Thu Oct 22 19:47:20 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

#define	MAPAINT_UNDO_DEPTH	32;

static int		undoListLength=0, undoDomainsFlag;
static int		undoListCurrent, undoListBase;
static int		redoListLength=0, redoDomainsFlag;
static int		redoListCurrent, redoListBase;

static WlzObject	***undoDomainList=NULL;
static WlzObject	***redoDomainList=NULL;

static void redisplayDomains(
  ThreeDViewStruct	*view_struct);

void initUndoList(
  int depth)
{
  int	i;

  /* if initialised do nothing */
  if( undoListLength ){
    return;
  }

  /* set the undo depth */
  if( depth > 0 ){
    undoListLength = depth;
  }
  else {
    undoListLength = MAPAINT_UNDO_DEPTH;
  }
  redoListLength = undoListLength;

  /* allocate space for object pointers - initialised to NULL */
  if( undoDomainList = (WlzObject ***) AlcCalloc(undoListLength,
						 sizeof(WlzObject **)) ){
    if( undoDomainList[0] = (WlzObject **) AlcCalloc(undoListLength * 33,
						     sizeof(WlzObject *)) ){
      for(i=1; i < undoListLength; i++){
	undoDomainList[i] = undoDomainList[i-1] + 33;
      }
    }
    else {
      AlcFree(undoDomainList);
      undoDomainList = NULL;
      undoListLength = 0;
      MAPaintReportWlzError(globals.topl, "initUndoList", WLZ_ERR_MEM_ALLOC);
      return;
    }
  }
  else {
    MAPaintReportWlzError(globals.topl, "initUndoList", WLZ_ERR_MEM_ALLOC);
    return;
  }

  if( redoDomainList = (WlzObject ***) AlcCalloc(redoListLength,
						 sizeof(WlzObject **)) ){
    if( redoDomainList[0] = (WlzObject **) AlcCalloc(redoListLength * 33,
						     sizeof(WlzObject *)) ){
      for(i=1; i < redoListLength; i++){
	redoDomainList[i] = redoDomainList[i-1] + 33;
      }
    }
    else {
      AlcFree(redoDomainList);
      redoDomainList = NULL;
      redoListLength = 0;
      MAPaintReportWlzError(globals.topl, "initUndoList", WLZ_ERR_MEM_ALLOC);
      return;
    }
  }
  else {
    MAPaintReportWlzError(globals.topl, "initUndoList", WLZ_ERR_MEM_ALLOC);
    return;
  }

  /* set the circular buffer pointers */
  undoListCurrent = 0;
  undoListBase = 0;
  undoDomainsFlag = 0;
  redoListCurrent = 0;
  redoListBase = 0;
  redoDomainsFlag = 0;

  return;
}

void freeUndoList(void)
{
  if( undoDomainList ){
    AlcFree((void *) undoDomainList[0]);
    AlcFree((void *) undoDomainList);
  }

  undoDomainList = NULL;
  undoListLength = 0;

  if( redoDomainList ){
    AlcFree((void *) redoDomainList[0]);
    AlcFree((void *) redoDomainList);
  }

  redoDomainList = NULL;
  redoListLength = 0;

  return;
}

static void _clearUndoDomains(void)
{
  int	i, j;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  for(i=0; i < undoListLength; i++){
    for(j=0; j < 33; j++){
      if( undoDomainList[i][j] ){
	if((errNum = WlzFreeObj(undoDomainList[i][j])) != WLZ_ERR_NONE){
	  break;
	}
	undoDomainList[i][j] = NULL;
      }
    }
  }
  undoListCurrent = 0;
  undoListBase = 0;
  undoDomainsFlag = 0;

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "_clearUndoDomains", errNum);
  }
  return;
}

static void _clearRedoDomains(void)
{
  int	i, j;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  for(i=0; i < redoListLength; i++){
    for(j=0; j < 33; j++){
      if( redoDomainList[i][j] ){
	if((errNum = WlzFreeObj(redoDomainList[i][j])) != WLZ_ERR_NONE){
	  break;
	}
	redoDomainList[i][j] = NULL;
      }
    }
  }
  redoListCurrent = 0;
  redoListBase = 0;
  redoDomainsFlag = 0;

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "_clearUndoDomains", errNum);
  }
  return;
}

void clearUndoDomains(void)
{
  _clearUndoDomains();
  _clearRedoDomains();

  return;
}

static void _pushUndoDomains(
  ThreeDViewStruct	*view_struct)
{
  int i, j;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* push current domains onto the undo list */
  /* calculate the next current index
     if current == base and check undoDomainsFlag
     else increment current, if new == base increment base
     */
  if( (undoListCurrent != undoListBase) || undoDomainsFlag ){
    undoListCurrent++;
    undoListCurrent %= undoListLength;
  }

  if( (undoListCurrent == undoListBase) && undoDomainsFlag ){
    undoListBase++;
    undoListBase %= undoListLength;
  }

  /* free any domains at current pointer and assign */
  for(j=0; (j < 33) && (errNum == WLZ_ERR_NONE); j++){
    if( undoDomainList[undoListCurrent][j] ){
      errNum = WlzFreeObj(undoDomainList[undoListCurrent][j]);
    }
    undoDomainList[undoListCurrent][j] = NULL;
    if( view_struct->curr_domain[j] ){
      undoDomainList[undoListCurrent][j] =
	WlzAssignObject(view_struct->curr_domain[j], NULL);
    }
  }
  undoDomainsFlag = 1;

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "_pushUndoDomains", errNum);
  }
  return;
}

void pushUndoDomains(
  ThreeDViewStruct	*view_struct)
{
  /* push the domains */
  _pushUndoDomains( view_struct );

  /* clear the redo list */
  _clearRedoDomains();

  return;
}

static void pushRedoDomains(
  ThreeDViewStruct	*view_struct)
{
  int i, j;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* push current domains onto the redo list */
  /* calculate the next current index
     if current == base and base domains == NULL - no increment
     else increment current, if new == base increment base
     */
  if( (redoListCurrent != redoListBase) || redoDomainsFlag ){
    redoListCurrent++;
    redoListCurrent %= redoListLength;
  }

  if( (redoListCurrent == redoListBase) && redoDomainsFlag ){
    redoListBase++;
    redoListBase %= redoListLength;
  }

  /* free any domains at current pointer and assign */
  for(j=0; (j < 33) && (errNum == WLZ_ERR_NONE); j++){
    if( redoDomainList[redoListCurrent][j] ){
      errNum = WlzFreeObj(redoDomainList[redoListCurrent][j]);
    }
    redoDomainList[redoListCurrent][j] = NULL;
    if( view_struct->curr_domain[j] ){
      redoDomainList[redoListCurrent][j] =
	WlzAssignObject(view_struct->curr_domain[j], NULL);
    }
  }
  redoDomainsFlag = 1;

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "pushRedoDomains", errNum);
  }
  return;
}

void UndoDomains(
  ThreeDViewStruct	*view_struct)
{
  int j;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* if no undo domains do nothing */
  if( !undoDomainsFlag ){
    return;
  }

  /* push current domains onto redo list */
  pushRedoDomains( view_struct );

  /* clear current and put top of undo list back as current */
  for(j=0; (j < 33) && (errNum == WLZ_ERR_NONE); j++){
    if( view_struct->curr_domain[j] ){
      errNum = WlzFreeObj(view_struct->curr_domain[j]);
    }
    view_struct->curr_domain[j] = undoDomainList[undoListCurrent][j];
    undoDomainList[undoListCurrent][j] = NULL;
  }

  /* decrement the undo pointer */
  if( errNum == WLZ_ERR_NONE ){
    if( undoListCurrent != undoListBase ){
      undoListCurrent--;
      if( undoListCurrent < 0 ){
	undoListCurrent = undoListLength - 1;
      }
    }
    else {
      undoDomainsFlag = 0;
    }
  }

  /* redisplay the view */
  redisplayDomains( view_struct );

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "UndoDomains", errNum);
  }
  return;
}


void RedoDomains(
  ThreeDViewStruct	*view_struct)
{
  int j;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* if no redo domains do nothing */
  if( !redoDomainsFlag ){
    return;
  }

  /* push current domains onto undo list */
  _pushUndoDomains( view_struct );

  /* clear current and put top of redo list back as current */
  for(j=0; (j < 33) && (errNum == WLZ_ERR_NONE); j++){
    if( view_struct->curr_domain[j] ){
      errNum = WlzFreeObj(view_struct->curr_domain[j]);
    }
    view_struct->curr_domain[j] = redoDomainList[redoListCurrent][j];
    redoDomainList[redoListCurrent][j] = NULL;
  }

  /* decrement the redo pointer */
  if( errNum == WLZ_ERR_NONE ){
    if( redoListCurrent != redoListBase ){
      redoListCurrent--;
      if( redoListCurrent < 0 ){
	redoListCurrent = redoListLength - 1;
      }
    }
    else {
      redoDomainsFlag = 0;
    }
  }

  /* redisplay the view */
  redisplayDomains( view_struct );

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "RedoDomains", errNum);
  }
  return;
}

static void redisplayDomains(
  ThreeDViewStruct	*view_struct)
{
  WlzObject	*obj;
  int 		j;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check the view object */
  if( view_struct->view_object == NULL ){
    if( obj = WlzGetSectionFromObject(globals.orig_obj,
				      view_struct->wlzViewStr,
				      WLZ_INTERPOLATION_NEAREST, &errNum) ){
      view_struct->view_object = WlzAssignObject(obj, NULL);
    }
    else {
      MAPaintReportWlzError(globals.topl, "redisplayDomains", errNum);
      return;
    }
  }
    
  /* clear the painted object */
  setGreyValuesFromObject(view_struct->painted_object,
			  view_struct->view_object);

  /* clear the view current domains and copy in the new */
  for(j=1; j < 33; j++){

    if( view_struct->curr_domain[j] == NULL ){
      continue;
    }

    /* paint in the domain */
    if( j > globals.cmapstruct->num_overlays ){
      set_grey_values_from_domain(view_struct->curr_domain[j],
				  view_struct->painted_object,
				  globals.cmapstruct->ovly_cols[j],
				  255);
    }
    else
    {
      set_grey_values_from_domain(view_struct->curr_domain[j],
				  view_struct->painted_object,
				  globals.cmapstruct->ovly_cols[j],
				  globals.cmapstruct->ovly_planes);
    }

  }

  /* redisplay the section */
  redisplay_view_cb(view_struct->canvas, (XtPointer) view_struct,
		    NULL);

  return;
}

void undoCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;

  if( paint_key == view_struct ){
    UndoDomains(view_struct);
  }
  
  return;
}

void redoCb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;

  if( paint_key == view_struct ){
    RedoDomains(view_struct);
  }
  
  return;
}

