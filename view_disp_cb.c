#pragma ident "MRC HGU $Id$"
/************************************************************
* Copyright   :    Medical Research Council, UK. 
*                  Human Genetics Unit, 
*                  Western General Hospital. 
*                  Edinburgh. 
*************************************************************
* Project     :    Mouse Atlas Project
* File        :    view_disp_cb.c
*************************************************************
* Author      :    Richard Baldock  (richard@hgu.mrc.ac.uk)
* Date        :    Tue Jun 21 09:05:15 1994
* Synopsis    : 
*************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <MAPaint.h>

#ifdef SUNOS5
/* XIL stuff for test */
#include <xil/xil.h>

extern XilSystemState	paintXILState;
#else /* SUNOS5 */
extern void		*paintXILState;
#endif /* SUNOS5 */

static void display_scaled_image(
  Display		*dpy,
  Window		win,
  ThreeDViewStruct	*view_struct,
  int			widthp,
  int			heightp)
{
  XImage		*scaled_image;
  XImage		*ximage = view_struct->ximage;
  UBYTE			*scaled_data, *data, *linedata;
  int			scale = (int) view_struct->wlzViewStr->scale;
  int			i, xp, yp;
  XWindowAttributes	win_att;

#ifdef SUNOS5
  XilImage		xilSrcImage, xilDstImage;
  XilStorage 		storage;
#endif /* SUNOS5 */

  if( paintXILState == NULL ){
    if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
    {
      return;
    }

    scaled_data = (UBYTE *) AlcMalloc(sizeof(UBYTE) * widthp * heightp *
				      scale * scale);
    scaled_image = XCreateImage(dpy, win_att.visual, win_att.depth,
				ZPixmap, 0, (char *) scaled_data,
				widthp*scale,
				heightp*scale, 8, widthp*scale);

    data = (UBYTE *) view_struct->ximage->data;
    for(yp=0; yp < heightp; yp++)
    {
      linedata = data + (yp*view_struct->ximage->bytes_per_line);
      for(xp=0; xp < widthp; xp++, linedata++)
      {
	for(i=0; i < scale; i++, scaled_data++)
	{
	  *scaled_data = *linedata;
	}
      }
      for(i=1; i < scale; i++, scaled_data += widthp * scale)
      {
	memcpy((void *) scaled_data,
	       (const void *) (scaled_data - widthp * scale),
	       widthp * scale);
      }
    }

    XPutImage(dpy, win, globals.gc_greys, scaled_image, 0, 0,
	      0, 0, widthp*scale, heightp*scale);
    XFlush(dpy);
    AlcFree((void *) scaled_image->data);
    scaled_image->data = NULL;
    XDestroyImage(scaled_image);
  }
#ifdef SUNOS5
  else {
    /* do it with XIL! */
    /* create a source image */
    xilSrcImage = xil_create(paintXILState, widthp, heightp, 1, XIL_BYTE);
    xil_export(xilSrcImage);
    storage = xil_storage_create(paintXILState, xilSrcImage);
    xil_storage_set_pixel_stride(storage, 0, 1);
    xil_storage_set_scanline_stride(storage, 0,
				    view_struct->ximage->bytes_per_line);
    xil_storage_set_data(storage, 0, (void *) ximage->data);
    xil_set_storage_with_copy(xilSrcImage, storage);
    xil_import(xilSrcImage, TRUE);
    xil_storage_destroy(storage);

    /* create the display image */
    xilDstImage = xil_create_from_window(paintXILState, dpy, win);

    /* display the scaled image */
    xil_scale(xilSrcImage, xilDstImage, "nearest",
	      (float) scale, (float) scale);

    /* destroy the XIL images */
    xil_destroy(xilSrcImage);
    xil_destroy(xilDstImage);
  }
#endif /* SUNOS5 */

  return;
}
 

void redisplay_view_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
    WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
    Display		*dpy = XtDisplay(view_struct->canvas);
    Window		win = XtWindow(view_struct->canvas);
    unsigned int	widthp, heightp;
    WlzDVertex3		vtx;
    int			x, y;

    if( !wlzViewStr->initialised )
	if( init_view_struct( view_struct ) )
	    return;

    if( globals.gc_greys == NULL ){
	globals.gc_greys = HGU_XCreateGC( dpy, win );
    }

/*    widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
      heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;*/
    widthp = view_struct->ximage->width;
    heightp = view_struct->ximage->height;

    if( wlzViewStr->scale > 1.0 )
    {
      display_scaled_image(dpy, win, view_struct, widthp, heightp);
    }
    else
    {
      XPutImage(dpy, win, globals.gc_greys, view_struct->ximage, 0, 0,
		0, 0, widthp, heightp);
    }

    /* if dist is zero check for display fixed point or fixed line */
    if( WLZ_NINT(wlzViewStr->dist) == 0 ){
      WlzObject	*sectObj = view_struct->painted_object;

      if( view_struct->controlFlag&MAPAINT_SHOW_FIXED_POINT ){
	x = (0 - sectObj->domain.i->kol1) * wlzViewStr->scale;
	y = (0 - sectObj->domain.i->line1) * wlzViewStr->scale;
	HGU_XDrawCross(dpy, win, globals.gc_set, x, y);
      }
      if(view_struct->controlFlag&MAPAINT_SHOW_FIXED_LINE &&
	 view_struct->controlFlag&MAPAINT_FIXED_LINE_SET){
	x = (0 - sectObj->domain.i->kol1) * wlzViewStr->scale;
	y = (0 - sectObj->domain.i->line1) * wlzViewStr->scale;
	vtx = wlzViewStr->fixed_2;
	Wlz3DSectionTransformVtx(&vtx, wlzViewStr);
	vtx.vtX -= sectObj->domain.i->kol1;
	vtx.vtY -= sectObj->domain.i->line1;
	vtx.vtX *= wlzViewStr->scale;
	vtx.vtY *= wlzViewStr->scale;
	XDrawLine(dpy, win, globals.gc_set, x, y, (int) vtx.vtX, (int)vtx.vtY);
	HGU_XDrawCross(dpy, win, globals.gc_set, (int) vtx.vtX, (int)vtx.vtY);
      }
    }

    XFlush( dpy );
    return;
}

void redisplay_all_views_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
    ViewListEntry	*vl = global_view_list;

    while( vl != NULL ){
	if( vl->view_struct != paint_key )
	    redisplay_view_cb( w, (XtPointer) vl->view_struct, call_data );
	vl = vl->next;
    }

    return;
}

void display_view_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
    WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
    Display		*dpy = XtDisplay(view_struct->canvas);
    Window		win = XtWindow(view_struct->canvas);
    WlzObject		*sectObj;
    WlzErrorNum		errNum=WLZ_ERR_NONE;

    if( !wlzViewStr->initialised )
	if( init_view_struct( view_struct ) )
	    return;

    if( globals.gc_greys == NULL ){
	globals.gc_greys = HGU_XCreateGC( dpy, win );
    }

    /* get the section and set the painted object pointer */
    sectObj = WlzGetSectionFromObject(globals.obj, wlzViewStr, NULL);
    if( view_struct->painted_object ){
      WlzFreeObj(view_struct->painted_object);
    }
    view_struct->painted_object = WlzAssignObject(sectObj, NULL);

    /* set the section object to the ximage */
    view_struct->ximage->data = (char *)
      sectObj->values.r->values.ubp;
    view_struct->ximage->bytes_per_line = sectObj->values.r->width;

    /* display it */
    redisplay_view_cb(w, client_data, call_data);
    return;
}

void display_all_views_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
    ViewListEntry	*vl = global_view_list;

    while( vl != NULL ){
	if( vl->view_struct != paint_key )
	    display_view_cb( w, (XtPointer) vl->view_struct, call_data );
	vl = vl->next;
    }

    return;
}

void view_feedback_cb(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  ViewListEntry		*vl = global_view_list;
  Position		x0, y0, x1, y1;
  double		x, y, angle, t;
  int			xmax, ymax;
  Display		*dpy;
  Window		win;
  XRectangle		xrectangle;
  WlzDVertex2		vtx;

  /* check initialisation */
  if( !view_struct->wlzViewStr->initialised )
    if( init_view_struct( view_struct ) )
      return;

  /* switch this off for now */
  while( vl != NULL ){

    if((vl->view_struct == view_struct) || (vl->view_struct == paint_key)){
      vl = vl->next;
      continue;
    }

    /* check initialisation */
    if( !vl->view_struct->wlzViewStr->initialised )
      if( init_view_struct( vl->view_struct ) )
	continue;

    vtx = Wlz3DViewGetIntersectionPoint(wlzViewStr,
					vl->view_struct->wlzViewStr,
					NULL);
    x = vtx.vtX;
    y = vtx.vtY;
    angle = Wlz3DViewGetIntersectionAngle(wlzViewStr,
					  vl->view_struct->wlzViewStr,
					  NULL);
    dpy = XtDisplay(vl->view_struct->canvas);
    win = XtWindow(vl->view_struct->canvas);

    t = tan( angle );
    xmax = vl->view_struct->wlzViewStr->maxvals.vtX -
      vl->view_struct->wlzViewStr->minvals.vtX;
    ymax = vl->view_struct->wlzViewStr->maxvals.vtY -
      vl->view_struct->wlzViewStr->minvals.vtY;
    if( fabs( t ) > 1.0 ){
      y0 = 0;
      y1 = ymax;
      x0 = x + (y0 - y) / t;
      x1 = x + (y1 - y) / t;
    } else {
      x0 = 0;
      x1 = xmax;
      y0 = y + (x0 - x) * t;
      y1 = y + (x1 - x) * t;
    }

    /* reset for scale setting */
    x0 *= vl->view_struct->wlzViewStr->scale;
    y0 *= vl->view_struct->wlzViewStr->scale;
    x1 *= vl->view_struct->wlzViewStr->scale;
    y1 *= vl->view_struct->wlzViewStr->scale;

    xrectangle.x = xrectangle.y = 0;
    xrectangle.width = (xmax+1) * vl->view_struct->wlzViewStr->scale;
    xrectangle.height = (ymax+1) * vl->view_struct->wlzViewStr->scale;
    XSetClipRectangles(dpy, globals.gc_set, 0, 0, &xrectangle, 1,
		       Unsorted);
    XDrawLine(dpy, win, globals.gc_set, x0, y0, x1, y1);
    XFlush( dpy );
    vl = vl->next;
  }

  /* modify the 3D feedback display list */
  MAOpenGLDisplaySection(view_struct);
  MAOpenGLDrawScene( globals.canvas );

  return;
}

void destroy_view_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    ThreeDViewStruct	*view_struct = (ThreeDViewStruct *) client_data;
    ViewListEntry	*vl1 = global_view_list, *vl2;

    if( vl1->view_struct == view_struct ){
	global_view_list = vl1->next;
	AlcFree( (void *) vl1 );
    } else {
	vl2 = vl1;
	vl1 = vl1->next;
	while( vl1 != NULL && vl1->view_struct != view_struct ){
	    vl2 = vl1;
	    vl1 = vl1->next;
	}
	if( vl1 != NULL ){
	    vl2->next = vl1->next;
	    AlcFree( (void *) vl1 );
	}
    }

    free_view_struct( view_struct );
    AlcFree( (void *) view_struct );
    return;
}
