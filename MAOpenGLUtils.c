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
  glClearIndex( (GLfloat) BlackPixelOfScreen(XtScreen(w)) );

  /* set the initial transform - should be from resources */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotated( 90.0, 1.0, 0.0, 0.0);
  glRotated(-40.0, 0.0, 0.0, 1.0);
  glRotated( 20.0, 0.6, 0.4, 0.0);
  glGetDoublev(GL_MODELVIEW_MATRIX, &initial_rot[0][0]);

  /* create the default 3D display  DisplayList */
  globals.ref_display_list = glGenLists( (GLsizei) 1 );
  glNewList( globals.ref_display_list, GL_COMPILE );
  WLZ_VTX_3_SET(tVtx, 10.0, 10.0, 10.0);
  WLZ_VTX_3_SET(orgVtx, 5.0, 5.0, 5.0);
  glIndexi( HGU_XGetColorPixel(dpy, globals.cmap, 1.0, 1.0, 1.0) );
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
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap,
				red, green, blue)); 

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
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 1.0));

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
    theta = (GLdouble) width
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

    HGUglwCanvasTbGetTranslate(canvasW, &trans);
    HGUglwCanvasTbGetRotateMatrixGL(canvasW, rot);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(trans.vtX*max_radius, trans.vtY*max_radius,
		 trans.vtZ*max_radius);
    glMultMatrixd(&rot[0][0]);
    glMultMatrixd(&initial_rot[0][0]);
    glTranslated(-(globals.bbox_vtx.vtX + globals.bbox_size.vtX/2),
		 -(globals.bbox_vtx.vtY + globals.bbox_size.vtY/2),
		 -(globals.bbox_vtx.vtZ + globals.bbox_size.vtZ/2));

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

  if( view_struct->controlFlag & MAPAINT_SHOW_SOLID_SECTION )
  {
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 0.0));
    glBegin(GL_TRIANGLE_FAN);
    vtxCntr = 0;
    while( vtxCntr < num_vtxs ){
      glVertex3d(vtxs[vtxCntr].vtX, vtxs[vtxCntr].vtY,
		 vtxs[vtxCntr].vtZ);
      vtxCntr++;
    }
    glEnd();
  }

  if( view_struct->controlFlag & MAPAINT_SHOW_FIXED_POINT )
  {
    glPointSize(4.0);
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 1.0));
    glBegin(GL_POINTS);
    glVertex3d(wlzViewStr->fixed.vtX, wlzViewStr->fixed.vtY,
	       wlzViewStr->fixed.vtZ);
    glEnd();

    glBegin(GL_LINES);
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 0.0, 0.0));
    glVertex3d((double) wlzViewStr->ref_obj->domain.p->kol1,
	       wlzViewStr->fixed.vtY, wlzViewStr->fixed.vtZ);
    glVertex3d((double) wlzViewStr->ref_obj->domain.p->lastkl,
	       wlzViewStr->fixed.vtY, wlzViewStr->fixed.vtZ);

    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 0.0, 1.0, 0.0));
    glVertex3d(wlzViewStr->fixed.vtX,
	       (double) wlzViewStr->ref_obj->domain.p->line1,
	       wlzViewStr->fixed.vtZ);
    glVertex3d(wlzViewStr->fixed.vtX,
	       (double) wlzViewStr->ref_obj->domain.p->lastln,
	       wlzViewStr->fixed.vtZ);

    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 0.0, 0.0, 1.0));
    glVertex3d(wlzViewStr->fixed.vtX, wlzViewStr->fixed.vtY,
	       (double) wlzViewStr->ref_obj->domain.p->plane1);
    glVertex3d(wlzViewStr->fixed.vtX, wlzViewStr->fixed.vtY,
	       (double) wlzViewStr->ref_obj->domain.p->lastpl);
    glEnd();
  }

  if((view_struct->controlFlag&MAPAINT_SHOW_FIXED_LINE) &&
     (view_struct->controlFlag&MAPAINT_FIXED_LINE_SET) )
  {
    glPointSize(4.0);
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 1.0));
    glBegin(GL_POINTS);
    glVertex3d(wlzViewStr->fixed.vtX, wlzViewStr->fixed.vtY,
	       wlzViewStr->fixed.vtZ);
    glVertex3d(wlzViewStr->fixed_2.vtX, wlzViewStr->fixed_2.vtY,
	       wlzViewStr->fixed_2.vtZ);
    glEnd();

    glLineWidth( (GLfloat) 2.0 );
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 0.0));
    glBegin(GL_LINES);
    glVertex3d(wlzViewStr->fixed.vtX, wlzViewStr->fixed.vtY,
	       wlzViewStr->fixed.vtZ);
    glVertex3d(wlzViewStr->fixed_2.vtX, wlzViewStr->fixed_2.vtY,
	       wlzViewStr->fixed_2.vtZ);
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
    (void) glLineWidth( (GLfloat) 1.0 );
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 0.0));
    glTranslated(wlzViewStr->fixed.vtX, wlzViewStr->fixed.vtY,
                 wlzViewStr->fixed.vtZ);
    glRotated( (wlzViewStr->theta*180.0/WLZ_M_PI), 0.0, 0.0, 1.0);
    glRotated( (wlzViewStr->phi*180.0/WLZ_M_PI), 0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, r);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex3d(0.0, 0.0, r);
    glVertex3d(0.08*r, 0.0, 0.8*r);
    glVertex3d(0.0, 0.08*r, 0.8*r);
    glVertex3d(-0.08*r, 0.0, 0.8*r);
    glVertex3d(0.0, -0.08*r, 0.8*r);
    glVertex3d(0.08*r, 0.0, 0.8*r);
    glEnd();
    glPopMatrix();
  }

  if( view_struct->controlFlag & MAPAINT_HIGHLIGHT_SECTION ){
    (void) glLineWidth( (GLfloat) 3.0 );
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 0.0, 0.0));
  }
  else {
    (void) glLineWidth( (GLfloat) 1.0 );
    glIndexi(HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 0.0));
  }
  glBegin(GL_LINE_LOOP);
  vtxCntr = 0;
  while( vtxCntr < num_vtxs ){
      glVertex3d(vtxs[vtxCntr].vtX, vtxs[vtxCntr].vtY,
		 vtxs[vtxCntr].vtZ);
      vtxCntr++;
  }
  glEnd();

  glIndexi( HGU_XGetColorPixel(globals.dpy, globals.cmap, 1.0, 1.0, 1.0) );
  (void) glLineWidth( (GLfloat) 1.0 );
  glPopMatrix();
  glEndList();

  return;
}


