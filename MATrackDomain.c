#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MATrackDomain_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MATrackDomain.c
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
#include <math.h>

#include <MAPaint.h>


static double image_match_cost(
  WlzObject	*test_obj,
  WlzObject	*ref_obj,
  int		dx,
  int		dy)
{
  WlzIntervalWSpace	iwsp;
  WlzGreyWSpace		gwsp;
  WlzGreyValueWSpace	*gVWSp = NULL;
  double	cost;
  int		i, a;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* test object pointers, if NULL return very large cost
     note 1.0 is the theoretical maximum */
  if( (test_obj == NULL) || (ref_obj == NULL) ){
    return( 10.0 );
  }

  /* scan through obj2 comparing values with offset obj1 */
  errNum = WlzInitGreyScan(ref_obj, &iwsp, &gwsp);
  a = 0;
  cost = 0.0;
  if( errNum == WLZ_ERR_NONE ){
    if((gVWSp = WlzGreyValueMakeWSp(test_obj, &errNum))){
      while((errNum == WLZ_ERR_NONE) &&
	    ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE) )
      {
	for(i=0; i < iwsp.colrmn; i++)
	{
	  int	g1, g2;

	  WlzGreyValueGet(gVWSp, 0, (double) iwsp.linpos+dy,
			  (double) iwsp.colpos+i+dx);
	  g1 = (*(gVWSp->gVal)).ubv;
	  g2 = *gwsp.u_grintptr.ubp;
	  cost += (double) ((g1 > g2) ? g1 - g2 : g2 - g1);
	  gwsp.u_grintptr.ubp++;
	  a += 1;
	}
      }
      if( errNum == WLZ_ERR_EOO ){
	errNum = WLZ_ERR_NONE;
      }
      WlzGreyValueFreeWSp(gVWSp);
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "image_match_cost", errNum);
  }
  /* normalise by the object area and return */
  return( cost/a/256.0 );
}

static WlzObject *get_rect_region(
  WlzObject	*obj,
  int		line,
  int		kol,
  int		ln_size,
  int		kl_size)
{
  WlzObject	*rect_obj=NULL, *tmp_obj;
  WlzDomain	domain;
  WlzValues	values;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  if((domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_RECT,
				       line - ln_size, line + ln_size,
				       kol - kl_size, kol + kl_size,
				       &errNum))){
    values.core = NULL;
    
    if((tmp_obj = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain, values,
			      NULL, NULL, &errNum))){
      if( (rect_obj = WlzIntersect2(obj, tmp_obj, &errNum)) ){
	rect_obj->values = WlzAssignValues(obj->values, NULL);
      }
      WlzFreeObj( tmp_obj );
    }
    else {
      WlzFreeIntervalDomain(domain.i);
    }
  }

  if( errNum != WLZ_ERR_NONE ){
    MAPaintReportWlzError(globals.topl, "get_rect_region", errNum);
  }
  return( rect_obj );
}

static WlzPolygonDomain *HGU_TrackPolyline(
  WlzObject			*refObj,
  WlzObject			*dstObj,
  WlzPolygonDomain		*refPoly,
  MATrackDomainSearchMethod	searchMethod,
  MATrackDomainSearchParams	*searchParams,
  MATrackDomainCostType		costType,
  MATrackDomainCostParams	*costParams,
  WlzErrorNum			*wlzErr)
{
  WlzObject		*poly_obj, *ref_region, *testObj1=NULL, *testObj2=NULL, *tmpObj;
  WlzPolygonDomain	*new_polydmn=NULL;
  MATrackMatchPointStruct	*mpts=NULL;
  int			i, j, k, kmax, num_mpts=0;
  double		dx1, dx2, dx3, dy1, dy2, dy3, s1, s2, s3;
  int			x, y, dx, dy;
  WlzIVertex2		*vtxs=NULL;
  double		**local_cost, **direction;
  EdgeCostParams	*edgeCostParams=(EdgeCostParams *) costParams;
  WlzGreyValueWSpace	*gVWSp1=NULL, *gVWSp2=NULL;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* create an 8-connected polygon */
  if((poly_obj = WlzPolyTo8Polygon( refPoly, 1, &errNum ))){
    new_polydmn = WlzAssignPolygonDomain(poly_obj->domain.poly, NULL);
    WlzFreeObj( poly_obj );
  }

  /* allocate space for the match point */
  /* check for sufficient polyline points */
  if( errNum == WLZ_ERR_NONE ){
    if( (num_mpts = new_polydmn->nvertices / searchParams->spacing) < 3 )
    {
      return( new_polydmn );
    }

    /* make sure the last point is not matched because
       it is identical to the first */
    if( (new_polydmn->nvertices % searchParams->spacing) < 2 )
    {
      num_mpts--;
    }
    if((mpts = (MATrackMatchPointStruct *)
       AlcMalloc(sizeof(MATrackMatchPointStruct) *
		 (num_mpts+1)))){
      AlcErrno	alcErr;
      if( (alcErr = AlcDouble2Malloc(&local_cost, num_mpts+1,
				     2*searchParams->range+1)) != ALC_ER_NONE ){
	AlcFree(mpts);
	errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
    else {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }

  /* for correlation type the test image is the reference object,
     for edge type the test image is the vector gradient */
  if( errNum == WLZ_ERR_NONE ){
    switch( costType ){
    case MATRACK_CORRELATION_COST_TYPE:
      testObj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ, dstObj->domain, dstObj->values,
			     NULL, NULL, &errNum);
      break;

    case MATRACK_EDGE_COST_TYPE:
      if((tmpObj = WlzConvertPix(dstObj, WLZ_GREY_SHORT, &errNum))){
	if((testObj1 = WlzGauss2(tmpObj, edgeCostParams->width,
				 edgeCostParams->width, 1, 0, &errNum))){
	  if( !(gVWSp1 = WlzGreyValueMakeWSp(testObj1, &errNum)) ){
	    WlzFreeObj(testObj1);
	  }
	}
	
	if( errNum == WLZ_ERR_NONE ){
	  if((testObj2 = WlzGauss2(tmpObj, edgeCostParams->width,
				   edgeCostParams->width, 0, 1, &errNum))){
	    if( !(gVWSp2 = WlzGreyValueMakeWSp(testObj2, &errNum)) ){
	      WlzFreeObj(testObj1);
	      WlzFreeObj(testObj2);
	      WlzGreyValueFreeWSp(gVWSp1);
	    }
	  }
	  else {
	    WlzFreeObj(testObj1);
	    WlzGreyValueFreeWSp(gVWSp1);
	  }
	}
	
	if((errNum == WLZ_ERR_NONE) &&
	   AlcDouble2Malloc(&direction, num_mpts+1,
			    2*searchParams->range+1) != ALC_ER_NONE ){
	  WlzFreeObj(testObj1);
	  WlzFreeObj(testObj2);
	  WlzGreyValueFreeWSp(gVWSp1);
	  WlzGreyValueFreeWSp(gVWSp2);
	  errNum = WLZ_ERR_MEM_ALLOC;
	}
	WlzFreeObj(tmpObj);
      }
      if( errNum != WLZ_ERR_NONE ){
	AlcFree(mpts);
	AlcFree(local_cost);
      }
      break;
    }
  }

  /* scan through match points */
  if( errNum == WLZ_ERR_NONE ){
    for(i=0; i < num_mpts; i++)
    {
      mpts[i].index = i * searchParams->spacing;
      vtxs = new_polydmn->vtx;
      x = vtxs[mpts[i].index].vtX;
      y = vtxs[mpts[i].index].vtY;
      mpts[i].x = x;
      mpts[i].y = y;

      /* simple weighted angle, note eight_poly returns the last
	 vertex equal to the first when the polyline is closed */
      j = (mpts[i].index > 0) ? mpts[i].index :
	mpts[i].index + new_polydmn->nvertices - 1;

      dx1 = vtxs[mpts[i].index+1].vtX - vtxs[j-1].vtX;
      dx2 = vtxs[mpts[i].index+2].vtX - vtxs[j-2].vtX;
      dx3 = vtxs[mpts[i].index+3].vtX - vtxs[j-3].vtX;
      dy1 = vtxs[mpts[i].index+1].vtY - vtxs[j-1].vtY;
      dy2 = vtxs[mpts[i].index+2].vtY - vtxs[j-2].vtY;
      dy3 = vtxs[mpts[i].index+3].vtY - vtxs[j-3].vtY;
      s1 = sqrt(dx1*dx1 + dy1*dy1);
      s2 = sqrt(dx2*dx2 + dy2*dy2);
      s3 = sqrt(dx3*dx3 + dy3*dy3);

      /* note we want the perpendicular direction */
      mpts[i].sintheta =  ( dx1/s1 + dx2/s2 + dx3/s3 ) / 3.0;
      mpts[i].costheta = -( dy1/s1 + dy2/s2 + dy3/s3 ) / 3.0;

      /* scan through search region to generate the cost matrix */
      switch( costType ){
      case MATRACK_CORRELATION_COST_TYPE:
	/* get a region from the reference image */
	ref_region = get_rect_region(refObj, y, x,
				     costParams->size, costParams->size);

	for(j = -searchParams->range; j <= searchParams->range; j++)
	{
	  dx = WLZ_NINT(j * mpts[i].costheta);
	  dy = WLZ_NINT(j * mpts[i].sintheta);

	  /* calculate match cost */
	  local_cost[i][j+searchParams->range] =
	    image_match_cost(testObj1, ref_region,
			     dx, dy);
	}

	if( ref_region ){
	  WlzFreeObj( ref_region );
	}
	break;

      case MATRACK_EDGE_COST_TYPE:
	/* set up random access to the gradient images */

	/* calculate the edge strengths and direction strength */
	for(j = -searchParams->range; j <= searchParams->range; j++)
	{
	  double	val1, val2;
	  dx = WLZ_NINT(j * mpts[i].costheta);
	  dy = WLZ_NINT(j * mpts[i].sintheta);

	  /* get the gradient values */
	  WlzGreyValueGet(gVWSp1, 0, (double) (mpts[i].y + dy),
			  (double) (mpts[i].x + dx));
	  WlzGreyValueGet(gVWSp2, 0, (double) (mpts[i].y + dy),
			  (double) (mpts[i].x + dx));
	  switch( gVWSp1->gType )
	  {
	  default:
	  case WLZ_GREY_INT:
	    val1 = (*(gVWSp1->gVal)).inv;
	    break;
	  case WLZ_GREY_SHORT:
	    val1 = (*(gVWSp1->gVal)).shv;
	    break;
	  case WLZ_GREY_UBYTE:
	    val1 = (*(gVWSp1->gVal)).ubv;
	    break;
	  case WLZ_GREY_FLOAT:
	    val1 = (*(gVWSp1->gVal)).flv;
	    break;
	  case WLZ_GREY_DOUBLE:
	    val1 = (*(gVWSp1->gVal)).dbv;
	    break;
	  }
	  switch( gVWSp2->gType )
	  {
	  default:
	  case WLZ_GREY_INT:
	    val2 = (*(gVWSp2->gVal)).inv;
	    break;
	  case WLZ_GREY_SHORT:
	    val2 = (*(gVWSp2->gVal)).shv;
	    break;
	  case WLZ_GREY_UBYTE:
	    val2 = (*(gVWSp2->gVal)).ubv;
	    break;
	  case WLZ_GREY_FLOAT:
	    val2 = (*(gVWSp2->gVal)).flv;
	    break;
	  case WLZ_GREY_DOUBLE:
	    val2 = (*(gVWSp2->gVal)).dbv;
	    break;
	  }

	  /* edge strength cost */
	  /* inverse normalised version */
/*	local_cost[i][j+searchParams->range] =
	1.0 / (1.0 + sqrt(val1*val1 + val2*val2));*/
	  /* direct proportionality with adjustable strength */
	  local_cost[i][j+searchParams->range] = -edgeCostParams->gradient
	    * sqrt(val1*val1 + val2*val2);

	  /* direction - note cost is calculated as part of the
	     non-local cost */
	  direction[i][j+searchParams->range] = atan2(val2, val1);
	}

	break;
      }
    }
  }

  /* first and last points are the same */
  if( errNum == WLZ_ERR_NONE ){
    mpts[num_mpts] = mpts[0];
    mpts[num_mpts].index = new_polydmn->nvertices - 1;
    for(j = -searchParams->range; j <= searchParams->range; j++)
    {
      local_cost[num_mpts][j+searchParams->range] =
	local_cost[0][j+searchParams->range];

      if( costType == MATRACK_EDGE_COST_TYPE ){
	direction[num_mpts][j+searchParams->range] =
	  direction[0][j+searchParams->range];
      }
    }
  }

  /* determine offsets by minimising the global cost.
     This uses dynamic programming to include a local distance cost
     plus a non-local angle cost to mimic the snake behavior.
     Note this routine assumes that the match cost is normalised */
  if( errNum == WLZ_ERR_NONE ){
    switch( costType ){
    case MATRACK_CORRELATION_COST_TYPE:
      PMSnake(local_cost, num_mpts+1, mpts, searchParams,
	      costParams->LCParams, costParams->NLCParams,
	      1);
      WlzFreeObj(testObj1);
      break;

    case MATRACK_EDGE_COST_TYPE:
      PMEdgeSnake(local_cost, direction, num_mpts+1, mpts,
		  searchParams, edgeCostParams->LCParams,
		  (PMEdgeSnakeNLCParams *) (edgeCostParams->NLCParams),
		  1);
      AlcDouble2Free(direction);
      WlzFreeObj(testObj1);
      WlzFreeObj(testObj2);
      WlzGreyValueFreeWSp(gVWSp1);
      WlzGreyValueFreeWSp(gVWSp2);
      break;
    }
    AlcDouble2Free(local_cost);
  }
      
  /* calculate new polyline */
  if( errNum == WLZ_ERR_NONE ){
    for(i=0; i < num_mpts; i++)
    {
      dx1 = mpts[i].x_off;
      dy1 = mpts[i].y_off;
      dx2 = mpts[i+1].x_off;
      dy2 = mpts[i+1].y_off;
      kmax = mpts[i+1].index - mpts[i].index;
      for(j=mpts[i].index, k=0; j < mpts[i+1].index; j++, k++)
      {
	dx = WLZ_NINT( ((kmax - k) * dx1 + k * dx2) / kmax );
	dy = WLZ_NINT( ((kmax - k) * dy1 + k * dy2) / kmax );
	vtxs[j].vtX += dx;
	vtxs[j].vtY += dy;
      }
    }
    vtxs[j] = vtxs[0];
  

    /* strip repeated vertices */
    for(i=1, j=1; i < new_polydmn->nvertices; i++)
    {
      if( (vtxs[i].vtX == vtxs[i-1].vtX) && (vtxs[i].vtY == vtxs[i-1].vtY) )
      {
	continue;
      }
      vtxs[j] = vtxs[i];
      j++;
    }
    new_polydmn->nvertices = j;

    /* clean up and return */
    AlcFree( mpts );
  }

  if( wlzErr ){
    *wlzErr = errNum;
  }
  if( errNum != WLZ_ERR_NONE ){
    WlzFreePolyDmn(new_polydmn);
    new_polydmn = NULL;
  }
  return( new_polydmn );
}

static WlzBoundList *HGU_TrackBoundlist(
  WlzObject			*refObj,
  WlzObject			*dstObj,
  WlzBoundList			*refBnd,
  MATrackDomainSearchMethod	searchMethod,
  MATrackDomainSearchParams	*searchParams,
  MATrackDomainCostType		costType,
  MATrackDomainCostParams	*costParams,
  WlzErrorNum			*wlzErr)
{
  WlzBoundList	*new_bndlst;

  /* check boundlist for stopping condition */
  if( refBnd == NULL )
  {
    return NULL;
  }

  /* create a new boundlist structure */
  new_bndlst = (WlzBoundList *) AlcMalloc(sizeof(WlzBoundList));
  new_bndlst->type = refBnd->type;
  new_bndlst->linkcount = 0;
  new_bndlst->up = NULL;
  new_bndlst->next = HGU_TrackBoundlist(refObj, dstObj, refBnd->next,
					searchMethod, searchParams,
					costType, costParams, wlzErr);
  new_bndlst->down = HGU_TrackBoundlist(refObj, dstObj, refBnd->down,
					searchMethod, searchParams,
					costType, costParams, wlzErr);
  new_bndlst->wrap = 1;

  /* track this polyline */
  new_bndlst->poly = HGU_TrackPolyline(refObj, dstObj, refBnd->poly,
				       searchMethod, searchParams,
				       costType, costParams, wlzErr);
/*  new_bndlst->poly = WlzAssignPolygonDomain(new_bndlst->poly, NULL);*/

  return( new_bndlst );
}

WlzObject *HGU_TrackDomain(
  WlzObject			*refObj,
  WlzObject			*dstObj,
  WlzObject			*refDmn,
  MATrackDomainSearchMethod	searchMethod,
  MATrackDomainSearchParams	*searchParams,
  MATrackDomainCostType		costType,
  MATrackDomainCostParams	*costParams,
  WlzErrorNum			*wlzErr)
{	
  WlzObject	*new_domain, *ref_bound;
  WlzBoundList	*new_bndlst;

  /* check objects */
  if( (refObj == NULL) || (refDmn == NULL) || (dstObj == NULL) )
  {
    return NULL;
  }

  /* get reference domain boundary */
  ref_bound = WlzObjToBoundary( refDmn, 1, NULL );
  new_bndlst = HGU_TrackBoundlist(refObj, dstObj, ref_bound->domain.b,
				  searchMethod, searchParams,
				  costType, costParams, wlzErr);

  /* create the new domain */
  new_domain = WlzBoundToObj( new_bndlst, WLZ_SIMPLE_FILL, NULL );
  WlzFreeObj( ref_bound );
  WlzFreeBoundList( new_bndlst );

  return new_domain;
}



