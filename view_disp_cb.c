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

static void display_scaled_image(
  Display		*dpy,
  Window		win,
  ThreeDViewStruct	*view_struct,
  int			widthp,
  int			heightp,
  XExposeEvent		*event)
{
  XImage		*scaled_image;
  XImage		*ximage = view_struct->ximage;
  UBYTE			*scaled_data, *data, *line1data, *linedata;
  int			scale = (int) view_struct->wlzViewStr->scale;
  int			i, xp, yp;
  XWindowAttributes	win_att;
  int			x_exp, y_exp, width_exp, height_exp;

  if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
  {
    return;
  }

  /* get exposed region */
  if( event ){
    x_exp = event->x / scale;
    y_exp = event->y / scale;
    width_exp = event->width / scale + 2;
    height_exp = event->height / scale + 2;
  }
  else {
    /* should get the region actually visible here  - done elsewhere*/
    x_exp = 0;
    y_exp = 0;
    width_exp = widthp;
    height_exp = heightp;
  }
  widthp = WLZ_MIN(widthp, x_exp + width_exp);
  heightp = WLZ_MIN(heightp, y_exp + height_exp);
  /* make all measures of width and exposed width consistent */
  width_exp = widthp - x_exp;
  height_exp = heightp - y_exp;
  /* quit if nothing to display */
  if( (width_exp * height_exp * scale) <= 0 ){
    return;
  }

  if( scaled_data = (UBYTE *) AlcMalloc(sizeof(UBYTE) * width_exp *
					height_exp * scale * scale) ){
    scaled_image = XCreateImage(dpy, win_att.visual, win_att.depth,
				ZPixmap, 0, (char *) scaled_data,
				width_exp*scale,
				height_exp*scale, 8, width_exp*scale);

    data = (UBYTE *) view_struct->ximage->data;
    for(yp=y_exp; yp < heightp; yp++)
    {
      linedata = data + (yp*view_struct->ximage->bytes_per_line) + x_exp;
      line1data = scaled_data;
      for(xp=x_exp; xp < widthp; xp++, linedata++)
      {
	for(i=0; i < scale; i++, line1data++)
	{
	  *line1data = *linedata;
	}
      }
      scaled_data += width_exp * scale;
      for(i=1; i < scale; i++, scaled_data += width_exp * scale)
      {
	memcpy((void *) scaled_data,
	       (const void *) (scaled_data - width_exp * scale),
	       width_exp * scale);
      }
    }

    XPutImage(dpy, win, globals.gc_greys, scaled_image, 0, 0,
	      x_exp*scale, y_exp*scale, width_exp*scale, height_exp*scale);
    XFlush(dpy);
    AlcFree((void *) scaled_image->data);
    scaled_image->data = NULL;
    XDestroyImage(scaled_image);
  }
  else {
    MAPaintReportWlzError(globals.topl, "display_scaled_image",
			  WLZ_ERR_MEM_ALLOC);
  }

  return;
}

static void display_scaled_down_image(
  Display		*dpy,
  Window		win,
  ThreeDViewStruct	*view_struct,
  int			widthp,
  int			heightp,
  XExposeEvent		*event)
{
  XImage		*scaled_image;
  XImage		*ximage = view_struct->ximage;
  UBYTE			*scaled_data, *data, *linedata;
  int			scale = (int) (1.0 / view_struct->wlzViewStr->scale);
  int			i, xp, yp;
  XWindowAttributes	win_att;
  int			x_exp, y_exp, width_exp, height_exp;

  if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
  {
    return;
  }

  /* get exposed region */
  if( event ){
    x_exp = event->x;
    y_exp = event->y;
    width_exp = event->width;
    height_exp = event->height;
  }
  else {
    /* should get the region actually visible here  - done elsewhere */
    x_exp = 0;
    y_exp = 0;
    width_exp = widthp / scale;
    height_exp = heightp / scale;
  }
  widthp = WLZ_MIN(widthp / scale, x_exp + width_exp);
  heightp = WLZ_MIN(heightp / scale, y_exp + height_exp);

  if( scaled_data = (UBYTE *) AlcMalloc(sizeof(UBYTE) *
					width_exp * height_exp) ){
    scaled_image = XCreateImage(dpy, win_att.visual, win_att.depth,
				ZPixmap, 0, (char *) scaled_data,
				width_exp, height_exp, 8, width_exp);

    data = (UBYTE *) view_struct->ximage->data;
    for(yp=y_exp; yp < heightp; yp++)
    {
      linedata = data + (yp * scale * view_struct->ximage->bytes_per_line) +
	x_exp * scale;
      scaled_data = (UBYTE *) scaled_image->data + (yp-y_exp) * width_exp;
      for(xp=x_exp; xp < widthp; xp++, scaled_data++, linedata += scale)
      {
	*scaled_data = *linedata;
      }
    }

    XPutImage(dpy, win, globals.gc_greys, scaled_image, 0, 0,
	      x_exp, y_exp, width_exp, height_exp);

    XFlush(dpy);
    AlcFree((void *) scaled_image->data);
    scaled_image->data = NULL;
    XDestroyImage(scaled_image);
  }
  else {
    MAPaintReportWlzError(globals.topl, "display_scaled_down_image",
			  WLZ_ERR_MEM_ALLOC);
  }

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
  XExposeEvent	event;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  Widget		x_bar, y_bar, clip=NULL, scrolled_window;
  int 		minimum, maximum, value, width, height, size;

  if( !wlzViewStr->initialised ){
    if( init_view_struct( view_struct ) ){
      return;
    }
  }

  if( globals.gc_greys == NULL ){
    globals.gc_greys = HGU_XCreateGC( dpy, win );
  }

/*    widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
      heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;*/
  widthp = view_struct->ximage->width * wlzViewStr->scale;
  heightp = view_struct->ximage->height * wlzViewStr->scale;

  /* check for expose event */
  if( cbs && (cbs->event) && (cbs->event->type == Expose) ){
    event = cbs->event->xexpose;
  }
  else {
    event.type = Expose;
    event.x = 0;
    event.y = 0;
    event.width = widthp;
    event.height = heightp;
  }

  /* check if exposed region can be reduced */
  /* get the scrolled window parent */
  scrolled_window = XtParent(view_struct->canvas);
  while( !XtIsSubclass(scrolled_window, xmScrolledWindowWidgetClass) ){
    scrolled_window = XtParent(scrolled_window);
  }

  /* get the scrollbars */
  XtVaGetValues(scrolled_window,
		XmNhorizontalScrollBar, &x_bar,
		XmNverticalScrollBar, &y_bar,
		XmNclipWindow, &clip,
		NULL);

  /* set x expose region */
  XtVaGetValues(x_bar,
		XmNminimum, &minimum,
		XmNmaximum, &maximum,
		XmNvalue,   &value,
		XmNsliderSize, &size, NULL);
  x = value * widthp / (maximum - minimum);
  width = size * widthp / (maximum - minimum);
      
  /* set y expose region */
  XtVaGetValues(y_bar,
		XmNminimum, &minimum,
		XmNmaximum, &maximum,
		XmNvalue,   &value,
		XmNsliderSize, &size, NULL);
  y = value * heightp / (maximum - minimum);
  height = size * heightp / (maximum - minimum);

  event.x = WLZ_MAX(x, event.x);
  event.y = WLZ_MAX(y, event.y);
  event.width = WLZ_MIN(width, widthp - event.x);
  event.height = WLZ_MIN(height, heightp - event.y);
    
  if( wlzViewStr->scale > 1.0 )
  {
    display_scaled_image(dpy, win, view_struct,
			 view_struct->ximage->width,
			 view_struct->ximage->height, &event);
  }
  else if( wlzViewStr->scale < 1.0 )
  {
    display_scaled_down_image(dpy, win, view_struct,
			      view_struct->ximage->width,
			      view_struct->ximage->height, &event);
  }
  else
  {
    XPutImage(dpy, win, globals.gc_greys, view_struct->ximage,
	      event.x, event.y, event.x, event.y,
	      event.width, event.height);
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

  if( !wlzViewStr->initialised ){
    if( init_view_struct( view_struct ) ){
      return;
    }
  }

  if( globals.gc_greys == NULL ){
    globals.gc_greys = HGU_XCreateGC( dpy, win );
  }

  /* get the section and set the painted object pointer */
  if( sectObj = WlzGetSectionFromObject(globals.obj, wlzViewStr,
					&errNum) ){
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
  }
  else {
    MAPaintReportWlzError(globals.topl, "display_view_cb", errNum);
  }

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
  WlzErrorNum		errNum=WLZ_ERR_NONE;

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
    if( !vl->view_struct->wlzViewStr->initialised ){
      if( init_view_struct( vl->view_struct ) ){
	continue;
      }
    }

    vtx = Wlz3DViewGetIntersectionPoint(wlzViewStr,
					vl->view_struct->wlzViewStr,
					&errNum);
    if( errNum != WLZ_ERR_NONE ){
      MAPaintReportWlzError(globals.topl, "view_feedback_cb", errNum);
      return;
    }
    x = vtx.vtX;
    y = vtx.vtY;

    angle = Wlz3DViewGetIntersectionAngle(wlzViewStr,
					  vl->view_struct->wlzViewStr,
					  &errNum);
    if( errNum != WLZ_ERR_NONE ){
      MAPaintReportWlzError(globals.topl, "view_feedback_cb", errNum);
      return;
    }
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
    XSetClipMask(dpy, globals.gc_set, None);
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
