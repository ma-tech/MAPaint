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
*   File       :   MAOpenGLUtils.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Wed Feb 25 06:58:08 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>

static HGUglTextFont *threeDFont = NULL;
static GLdouble	initial_rot[4][4];

void MAOpenGLInitCb(
  Widget 	w, 
  XtPointer	clientData,
  XtPointer	callData)
{
  static String xFontStr = "-*-courier-*-*-*-*-14-*-*-*-*-*-*-*";
  Font		xFont;
  WlzDVertex3	tVtx, orgVtx;
  Display	*dpy = XtDisplay( w );
 
  (void )HGUglwCreateCanvasGlxContext(w, NULL);
  HGUglwCanvasGlxMakeCurrent(w, NULL);
  xFont = XLoadFont(XtDisplay(w), xFontStr);
  if(xFont)
  {
    threeDFont = HGUglTextFontCreate(xFont);
  }
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  if( globals.toplDepth == 24 ){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE, GL_LINEAR);
  }
  else {
    glClearIndex( (GLfloat) BlackPixelOfScreen(XtScreen(w)) );
  }

  /* set the initial transform - should be from resources */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotated( (GLdouble) 90.0, (GLdouble) 1.0, (GLdouble) 0.0, (GLdouble) 0.0);
  glRotated((GLdouble) -40.0, (GLdouble) 0.0, (GLdouble) 0.0, (GLdouble) 1.0);
  glRotated( (GLdouble) 20.0, (GLdouble) 0.6, (GLdouble) 0.4, (GLdouble) 0.0);
  glGetDoublev(GL_MODELVIEW_MATRIX, &initial_rot[0][0]);

  /* create the default 3D display  DisplayList */
  globals.ref_display_list = glGenLists( (GLsizei) 1 );
  glNewList( globals.ref_display_list, GL_COMPILE );
  WLZ_VTX_3_SET(tVtx, 10.0, 10.0, 10.0);
  WLZ_VTX_3_SET(orgVtx, 5.0, 5.0, 5.0);
  if( globals.toplDepth == 24 ){
    glColor3d((GLdouble) 1.0, (GLdouble) 1.0, (GLdouble) 1.0);
  }
  else {
    glIndexi( HGU_XGetColorPixel(dpy, globals.cmap, 1.0, 1.0, 1.0) );
  }
  HGUglShapeWireCube(tVtx, orgVtx);

  glEndList();

  WLZ_VTX_3_SET(globals.bbox_vtx, -2.0, -2.0, -2.0);
  WLZ_VTX_3_SET(globals.bbox_size, 14.0, 14.0, 14.0);

  return;
}

void MAOpenGLDisplayBoundList(
  WlzBoundList *bndlist,
  float		z)
{
  WlzIVertex2	*vtxs;
  int		i, step;

  if( bndlist == NULL || bndlist->poly == NULL )
  {
    return;
  }
  vtxs = bndlist->poly->vtx;
  step = bndlist->poly->nvertices / 40;
  step = (step > 0) ? step : 1;

  glBegin(GL_LINE_LOOP);
  for(i=0; i < bndlist->poly->nvertices; i += step)
  {
    glVertex3f((GLfloat) vtxs[i].vtX, (GLfloat) vtxs[i].vtY, (GLfloat) z);
  }
  glEnd();

  MAOpenGLDisplayBoundList( bndlist->next, z );
  
  return;
}
void MAOpenGLDisplayXBoundList(
  WlzBoundList *bndlist,
  float		x)
{
  WlzIVertex2	*vtxs;
  int		i, step;

  if( bndlist == NULL || bndlist->poly == NULL )
  {
    return;
  }
  vtxs = bndlist->poly->vtx;
  step = bndlist->poly->nvertices / 40;
  step = (step > 0) ? step : 1;

  glBegin(GL_LINE_LOOP);
  for(i=0; i < bndlist->poly->nvertices; i += step)
  {
    glVertex3f((GLfloat) x, (GLfloat) vtxs[i].vtY, (GLfloat) vtxs[i].vtX);
  }
  glEnd();

  MAOpenGLDisplayXBoundList( bndlist->next, x );
  
  return;
}

void MAOpenGLDisplayYBoundList(
  WlzBoundList *bndlist,
  float		y)
{
  WlzIVertex2	*vtxs;
  int		i, step;

  if( bndlist == NULL || bndlist->poly == NULL )
  {
    return;
  }
  vtxs = bndlist->poly->vtx;
  step = bndlist->poly->nvertices / 40;
  step = (step > 0) ? step : 1;

  glBegin(GL_LINE_LOOP);
  for(i=0; i < bndlist->poly->nvertices; i += step)
  {
    glVertex3f((GLfloat) vtxs[i].vtX, (GLfloat) y, (GLfloat) vtxs[i].vtY);
  }
  glEnd();

  MAOpenGLDisplayYBoundList( bndlist->next, y );
  
  return;
}


void MAOpenGLDisplayDomainIndex(
WlzObject	*obj,
int		domain_index)
{
    WlzObject		*obj1, *boundobj;
    WlzPlaneDomain	*planedmn;
    WlzValues		values;
    int			z;
    float		red, green, blue;

    /* check the reference object */
    if( globals.obj == NULL )
	return;

    /* create a new display list */
    if( globals.domain_display_list[domain_index] != 0 ){
      glDeleteLists(globals.domain_display_list[domain_index], 1);
    }
    globals.domain_display_list[domain_index] =
      glGenLists( (GLsizei) 1 );
    glNewList( globals.domain_display_list[domain_index],
	       GL_COMPILE );

    /* set the colour */
    red = (float) globals.cmapstruct->ovly_red[domain_index] / 255.0;
    green = (float) globals.cmapstruct->ovly_green[domain_index] / 255.0;
    blue = (float) globals.cmapstruct->ovly_blue[domain_index] / 255.0;
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) red, (GLfloat) green, (GLfloat) blue);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  red, green, blue));
    }

    planedmn = obj->domain.p;
    for(z=planedmn->plane1; z <= planedmn->lastpl;
	z += globals.OpenGLDisplayDomainStep)
    {
      values.core = NULL;
      obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ,
			 planedmn->domains[z - planedmn->plane1],
			 values, NULL, NULL, NULL);
      if( obj1->domain.core != NULL )
      {
	boundobj = WlzObjToBoundary(obj1, 1, NULL);
	if( boundobj != NULL )
	{
	  MAOpenGLDisplayBoundList(boundobj->domain.b, (float) z );
	  WlzFreeObj( boundobj );
	}
      }
      WlzFreeObj( obj1 );
    }

    /* reset the colour */
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 1.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 1.0));
    }

    glEndList();

    MAOpenGLDrawScene( globals.canvas );

    return;
}

void MAOpenGLDrawScene(Widget canvasW)
{
  GLdouble	rot[4][4];
  WlzDVertex3	trans;
  Dimension	width, height;
  GLfloat	aspect, max_radius;
  GLdouble	theta;
  ViewListEntry	*vl = global_view_list;
  int		i;
  XEvent	event;

  /* check for queued timer events */
/*  if( XtAppPending(globals.app_con) & XtIMTimer )
  {
    fprintf(stderr, "ThreeDDrawScene: queued timer event\n");
    XtAppProcessEvent(globals.app_con, XtIMTimer);
    return;
  }*/

  max_radius = globals.bbox_size.vtX;
  max_radius = (globals.bbox_size.vtY > max_radius) ?
    globals.bbox_size.vtY : max_radius;
  max_radius = (globals.bbox_size.vtZ > max_radius) ?
    globals.bbox_size.vtZ : max_radius;

  XtVaGetValues(canvasW, XmNwidth, &width, XmNheight, &height, NULL);
  if((width > 0) && (height > 0))
  {
    HGUglwCanvasGlxMakeCurrent(canvasW, NULL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    aspect = (GLfloat) width / (GLfloat) height;
    theta = (GLdouble) height
      / 3.0 				/* pixels/mm */
      / 300.0; 				/* viewer dist. in mm */
    gluPerspective( (GLdouble) (theta * 180 / 3.1415962),
		    (GLdouble) aspect,
		    (GLdouble) max_radius / 10.0,
		    (GLdouble) 2 * max_radius / theta * 10.0);
    glTranslated(0.0,0.0,(GLdouble) -1.2 * max_radius / theta);
    
    /* this commented-out section enables orthographic projection */
    /*if(height > width)
    {
      aspect = (GLfloat )height/(GLfloat )width * max_radius;
      glOrtho(-max_radius, max_radius, -(aspect), aspect,
	      -max_radius, +max_radius);
    }
    else
    {
      aspect = (GLfloat )width/(GLfloat )height * max_radius;
      glOrtho(-(aspect), aspect, -max_radius, max_radius,
	      -max_radius, +max_radius);
    }*/
    /* set the fog parameters */
    glFogf(GL_FOG_START, 0.0);
    glFogf(GL_FOG_END, max_radius / theta / 2.0);

    HGUglwCanvasTbGetTranslate(canvasW, &trans);
    HGUglwCanvasTbGetRotateMatrixGL(canvasW, rot);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(trans.vtX*max_radius, trans.vtY*max_radius,
		 trans.vtZ*max_radius);
    glMultMatrixd(&rot[0][0]);
    glMultMatrixd(&initial_rot[0][0]);
    glTranslated((GLdouble) -(globals.bbox_vtx.vtX + globals.bbox_size.vtX/2),
		 (GLdouble) -(globals.bbox_vtx.vtY + globals.bbox_size.vtY/2),
		 (GLdouble) -(globals.bbox_vtx.vtZ + globals.bbox_size.vtZ/2));

    /* display the reference image sisplay list */
    glCallList( globals.ref_display_list );

    /* display the display list for each domain */
    for(i=0; i < 33; i++)
    {
      if( globals.domain_display_list[i] != 0 )
      {
	glCallList( globals.domain_display_list[i] );
      }
    }

    /* display the display list for each view window */
    while( vl != NULL )
    {
      if( vl->view_struct->wlzViewStr->initialised )
      {
	if( vl->view_struct->display_list ){
	  glCallList( vl->view_struct->display_list );
	}
      }
      vl = vl->next;
    }

    HGUglwCanvasSwapBuffers(canvasW);
  }
  glFinish();

  return;
}

void MAOpenGLDisplaySection(
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  WlzDVertex3		vtxs[12];
  int			num_vtxs, vtxCntr;

  /* get the list of intersection points between the plane and
     the bounding box in object coordinates */
  if( (num_vtxs = Wlz3DViewGetBoundingBoxIntersection(
    wlzViewStr, &(vtxs[0]), NULL)) <= 1 ){
    return;
  }

  if( view_struct->display_list == (GLuint) 0 )
  {
    view_struct->display_list = glGenLists( (GLsizei) 1 );
  }
  glNewList( view_struct->display_list, GL_COMPILE );
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  /* solid view feedback - fill polygon with yellow
     transparency would be good here */
  if( view_struct->controlFlag & MAPAINT_SOLID_VIEWFB_MODE )
  {
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 0.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  1.0, 1.0, 0.0));
    }
    glBegin(GL_TRIANGLE_FAN);
    vtxCntr = 0;
    while( vtxCntr < num_vtxs ){
      glVertex3d(vtxs[vtxCntr].vtX, vtxs[vtxCntr].vtY,
		 vtxs[vtxCntr].vtZ);
      vtxCntr++;
    }
    glEnd();
  }

  /* mask view feedback - simple fill of the region outside of the
     object but within the bounding box intersection polygon.
     if selected then use the domain of the object itself */
  if( view_struct->controlFlag & 
     (MAPAINT_MASK_VIEWFB_MODE|MAPAINT_TEMPLATE_VIEWFB_MODE|
      MAPAINT_SWITCH_VIEWFB_MODE) ){
    WlzObject	*templ, *mask, *poly, *tmpObj;
    WlzDomain	domain;
    WlzValues	values;
    WlzIntervalWSpace	iwsp;
    int		lineFlg;
    WlzErrorNum	errNum;

    /* get the mask and set the colour */
    if((view_struct->controlFlag&MAPAINT_MASK_VIEWFB_MODE) ||
       ((view_struct->controlFlag&MAPAINT_SWITCH_VIEWFB_MODE)&&
	(view_struct->controlFlag&MAPAINT_HIGHLIGHT_SECTION)) ){
      mask = WlzAssignObject(view_struct->masked_object, &errNum);
      (void) glLineWidth((GLfloat) 3.0 );
      if( globals.toplDepth == 24 ){
	glColor3f((GLfloat) 0.8, (GLfloat) 0.2, (GLfloat) 0.2);
      }
      else {
	glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				    0.8, 0.2, 0.2));
      }
    }
    else {
      /* transform the 3D vertex sequence back to section coordinates */
      WlzDVertex2	vtxs2[12];
      WlzDVertex3	vtxs3[12];

      /* create a polygon object and convert to a domain */
      vtxCntr = 0;
      while( vtxCntr < num_vtxs ){
	Wlz3DSectionTransformVtxR(wlzViewStr, vtxs[vtxCntr],
				  &vtxs3[vtxCntr]);
	vtxs2[vtxCntr].vtX = vtxs3[vtxCntr].vtX;
	vtxs2[vtxCntr].vtY = vtxs3[vtxCntr].vtY;
	vtxCntr++;
      }
      domain.poly = WlzMakePolygonDomain(WLZ_POLYGON_DOUBLE, num_vtxs,
				   (WlzIVertex2 *) (&vtxs2[0]),
				   num_vtxs, 1, &errNum);
      if( templ = WlzPolyToObj(domain.poly, WLZ_SIMPLE_FILL, &errNum) ){
	templ = WlzAssignObject(templ, NULL);
	if( mask = WlzDiffDomain(templ, view_struct->masked_object,
				 &errNum) ){
	  mask = WlzAssignObject(mask, NULL);
	}
	WlzFreeObj(templ);
      }
      WlzFreePolyDmn(domain.poly);

      /* set the colour */
      (void) glLineWidth((GLfloat) 1.0 );
      if( globals.toplDepth == 24 ){
	glColor3f((GLfloat) 0.8, (GLfloat) 0.8, (GLfloat) 0.2);
      }
      else {
	glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				    0.8, 0.8, 0.2));
      }
    }

    /* for now just display the mask */
    if( errNum == WLZ_ERR_NONE ){
      errNum = WlzInitRasterScan(mask, &iwsp, WLZ_RASTERDIR_ILIC);
      lineFlg = 0;
      while((errNum == WLZ_ERR_NONE) &&
	    ((errNum = WlzNextInterval(&iwsp)) == WLZ_ERR_NONE)){
	if( iwsp.nwlpos ){
	  lineFlg++;
	}
	if( (lineFlg % 2) == 0 ){
	  WlzDVertex3	vtx;
	  vtx.vtX = iwsp.lftpos;
	  vtx.vtY = iwsp.linpos;
	  vtx.vtZ = wlzViewStr->dist;
	  Wlz3DSectionTransformInvVtx(&vtx, wlzViewStr);
	  glBegin(GL_LINES);
	  glVertex3d((GLdouble) vtx.vtX,
		     (GLdouble) vtx.vtY,
		     (GLdouble) vtx.vtZ);
	  vtx.vtX = iwsp.rgtpos;
	  vtx.vtY = iwsp.linpos;
	  vtx.vtZ = wlzViewStr->dist;
	  Wlz3DSectionTransformInvVtx(&vtx, wlzViewStr);
	  glVertex3d((GLdouble) vtx.vtX,
		     (GLdouble) vtx.vtY,
		     (GLdouble) vtx.vtZ);
	  glEnd();
	}
      }
      if( errNum == WLZ_ERR_EOO ){
	errNum = WLZ_ERR_NONE;
      }
    }
    if( mask ){
      WlzFreeObj(mask);
    }
  }

  if( view_struct->controlFlag & MAPAINT_SHOW_FIXED_POINT )
  {
    /*glPointSize((GLfloat) 4.0); - until solaris 8 is standard */
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 1.0);
    }
    else {
      glPointSize((GLfloat) 4.0);
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  1.0, 1.0, 1.0));
    }
    glBegin(GL_POINTS);
    glVertex3d((GLdouble) wlzViewStr->fixed.vtX,
	       (GLdouble) wlzViewStr->fixed.vtY,
	       (GLdouble) wlzViewStr->fixed.vtZ);
    glEnd();

    glBegin(GL_LINES);
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  1.0, 0.0, 0.0));
    }
    glVertex3d((GLdouble) wlzViewStr->ref_obj->domain.p->kol1,
	       (GLdouble) wlzViewStr->fixed.vtY,
	       (GLdouble) wlzViewStr->fixed.vtZ);
    glVertex3d((GLdouble) wlzViewStr->ref_obj->domain.p->lastkl,
	       (GLdouble) wlzViewStr->fixed.vtY,
	       (GLdouble) wlzViewStr->fixed.vtZ);

    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 0.0, (GLfloat) 1.0, (GLfloat) 0.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  0.0, 1.0, 0.0));
    }
    glVertex3d((GLdouble) wlzViewStr->fixed.vtX,
	       (GLdouble) wlzViewStr->ref_obj->domain.p->line1,
	       (GLdouble) wlzViewStr->fixed.vtZ);
    glVertex3d((GLdouble) wlzViewStr->fixed.vtX,
	       (GLdouble) wlzViewStr->ref_obj->domain.p->lastln,
	       (GLdouble) wlzViewStr->fixed.vtZ);

    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 0.0, (GLfloat) 0.0, (GLfloat) 1.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  0.0, 0.0, 1.0));
    }
    glVertex3d((GLdouble) wlzViewStr->fixed.vtX,
	       (GLdouble) wlzViewStr->fixed.vtY,
	       (GLdouble) wlzViewStr->ref_obj->domain.p->plane1);
    glVertex3d((GLdouble) wlzViewStr->fixed.vtX,
	       (GLdouble) wlzViewStr->fixed.vtY,
	       (GLdouble) wlzViewStr->ref_obj->domain.p->lastpl);
    glEnd();
  }

  if((view_struct->controlFlag&MAPAINT_SHOW_FIXED_LINE) &&
     (view_struct->controlFlag&MAPAINT_FIXED_LINE_SET) )
  {
    /* glPointSize((GLfloat) 4.0); - until openGL bugs fixed */
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 1.0);
    }
    else {
      glPointSize((GLfloat) 4.0);
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  1.0, 1.0, 1.0));
    }
    glBegin(GL_POINTS);
    glVertex3d(wlzViewStr->fixed.vtX, wlzViewStr->fixed.vtY,
	       wlzViewStr->fixed.vtZ);
    glVertex3d(wlzViewStr->fixed_2.vtX, wlzViewStr->fixed_2.vtY,
	       wlzViewStr->fixed_2.vtZ);
    glEnd();

    glLineWidth( (GLfloat) 2.0 );
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 0.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  1.0, 1.0, 0.0));
    }
    glBegin(GL_LINES);
    glVertex3d((GLdouble) wlzViewStr->fixed.vtX,
	       (GLdouble) wlzViewStr->fixed.vtY,
	       (GLdouble) wlzViewStr->fixed.vtZ);
    glVertex3d((GLdouble) wlzViewStr->fixed_2.vtX,
	       (GLdouble) wlzViewStr->fixed_2.vtY,
	       (GLdouble) wlzViewStr->fixed_2.vtZ);
    glEnd();
  }

  if( view_struct->controlFlag & MAPAINT_SHOW_VIEW_DIRECTION )
  {
    double	r, rp;
    r = (wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX)/2.0;
    rp = (wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY)/2.0;
    r = (r > rp) ? r : rp;
    rp = (wlzViewStr->maxvals.vtZ - wlzViewStr->minvals.vtZ)/2.0;
    r = (r > rp) ? r : rp;

    glPushMatrix();
    (void) glLineWidth((GLfloat) 1.0 );
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 0.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  1.0, 1.0, 0.0));
    }
    glTranslated((GLdouble) wlzViewStr->fixed.vtX,
		 (GLdouble) wlzViewStr->fixed.vtY,
                 (GLdouble) wlzViewStr->fixed.vtZ);
    glRotated((GLdouble) (wlzViewStr->theta*180.0/WLZ_M_PI),
	      (GLdouble) 0.0, (GLdouble) 0.0, (GLdouble) 1.0);
    glRotated((GLdouble) (wlzViewStr->phi*180.0/WLZ_M_PI),
	      (GLdouble) 0.0, (GLdouble) 1.0, (GLdouble) 0.0);
    glBegin(GL_LINES);
    glVertex3d((GLdouble) 0.0, (GLdouble) 0.0, (GLdouble) 0.0);
    glVertex3d((GLdouble) 0.0, (GLdouble) 0.0, (GLdouble) r);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex3d((GLdouble) 0.0, (GLdouble) 0.0, (GLdouble) r);
    glVertex3d((GLdouble) 0.08*r, (GLdouble) 0.0, (GLdouble) 0.8*r);
    glVertex3d((GLdouble) 0.0, (GLdouble) 0.08*r, (GLdouble) 0.8*r);
    glVertex3d((GLdouble) -0.08*r, (GLdouble) 0.0, (GLdouble) 0.8*r);
    glVertex3d((GLdouble) 0.0, (GLdouble) -0.08*r, (GLdouble) 0.8*r);
    glVertex3d((GLdouble) 0.08*r, (GLdouble) 0.0, (GLdouble) 0.8*r);
    glEnd();
    glPopMatrix();
  }

  if( view_struct->controlFlag & MAPAINT_HIGHLIGHT_SECTION ){
    (void) glLineWidth((GLfloat) 3.0 );
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  1.0, 0.0, 0.0));
    }
  }
  else {
    (void) glLineWidth((GLfloat) 1.0 );
    if( globals.toplDepth == 24 ){
      glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 0.0);
    }
    else {
      glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				  1.0, 1.0, 0.0));
    }
  }
  glBegin(GL_LINE_LOOP);
  vtxCntr = 0;
  while( vtxCntr < num_vtxs ){
      glVertex3d((GLdouble) vtxs[vtxCntr].vtX,
		 (GLdouble) vtxs[vtxCntr].vtY,
		 (GLdouble) vtxs[vtxCntr].vtZ);
      vtxCntr++;
  }
  glEnd();

  if( globals.toplDepth == 24 ){
    glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 1.0);
  }	
  else {
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				1.0, 1.0, 1.0));
  }
  (void) glLineWidth((GLfloat) 1.0 );
  glPopMatrix();
  glEndList();

  return;
}


