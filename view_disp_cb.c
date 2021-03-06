#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _view_disp_cb_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         view_disp_cb.c
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
#include <float.h>

#include <MAPaint.h>

void HGU_XPutImage8To24(
  Display	*dpy,
  Window	win,
  GC		gc,
  XImage	*ximage,
  int		srcX,
  int		srcY,
  int		dstX,
  int		dstY,
  unsigned int	width,
  unsigned int	height,
  unsigned char	colormap[3][256])
{
  XImage	*new;
  unsigned char	*newdata, *data;
  int		i, j, srcOff, dstOff;
  XWindowAttributes	win_att;
  int		r_off, g_off, b_off, a_off;
  unsigned int	widthb;

  /* get window attribute to get the visual */
  if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
  {
    return;
  }

  /* assume 24 bit required and so don't check the window attributes */
  data = (unsigned char *) ximage->data;

  /* check padding */
  widthb = width*4;
  i = BitmapPad(dpy)>>3;
  if( widthb%i ){
    widthb += i  - widthb%i;
  }
  newdata = (unsigned char *) AlcMalloc(widthb*height*sizeof(char));
  new = XCreateImage(dpy, win_att.visual, win_att.depth,
		     ZPixmap, 0, (char *) newdata,
		     width, height, BitmapPad(dpy), widthb);

  /* transfer data  via colormap to 24 bit  - use visual for colour masks */
  srcOff = 0;
  /* assume only two options for the rgb masks - no doubt wrong */
  /* try harder - find the rgba byte offsets from the masks
     Note - must also take account of byte-ordering - yuk */
  r_off = HGU_XGetColorIndexFromMask24(win_att.visual->red_mask,
				       new->byte_order);
  g_off = HGU_XGetColorIndexFromMask24(win_att.visual->green_mask,
				       new->byte_order);
  b_off = HGU_XGetColorIndexFromMask24(win_att.visual->blue_mask,
				       new->byte_order);
  a_off = 6 - r_off - g_off - b_off;
  for(j=0; j < height; j++){
    dstOff = (srcY+j) * ximage->bytes_per_line + srcX;
    srcOff = j * widthb;
    for(i=0; i < width; i++, dstOff++){
      newdata[srcOff + r_off] = colormap[0][data[dstOff]];
      newdata[srcOff + g_off] = colormap[1][data[dstOff]];
      newdata[srcOff + b_off] = colormap[2][data[dstOff]];
      newdata[srcOff + a_off] = 0;
      srcOff += 4;
    }
  }

  /* display the image and clear allocated memory */
  XPutImage(dpy, win, gc, new, 0, 0, dstX, dstY, width, height);
  new->data = NULL;
  XDestroyImage(new);
  AlcFree(newdata);

  return;
}

static void display_scaled_image(
  Display		*dpy,
  Window		win,
  ThreeDViewStruct	*view_struct,
  int			widthp,
  int			heightp,
  XExposeEvent		*event)
{
  unsigned int		widthb;
  XImage		*scaled_image;
  WlzUByte		*scaled_data, *data, *line1data, *linedata;
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

  widthb = (width_exp*scale);
  i = BitmapPad(dpy)>>3;
  if( widthb%i ){
    widthb += i -  widthb%i;
  }

  if((scaled_data = (WlzUByte *) AlcMalloc(sizeof(WlzUByte) * widthb *
					   height_exp * scale))){

    scaled_image = XCreateImage(dpy, win_att.visual, 8,
				ZPixmap, 0, (char *) scaled_data,
				width_exp*scale,
				height_exp*scale, BitmapPad(dpy), widthb);

    data = (WlzUByte *) view_struct->ximage->data;
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
      scaled_data += widthb;
      for(i=1; i < scale; i++, scaled_data += widthb)
      {
	memcpy((void *) scaled_data,
	       (const void *) (scaled_data - widthb),
	       width_exp * scale);
      }
    }

    if( globals.toplDepth == 8 ){
      XPutImage(dpy, win, globals.gc_greys, scaled_image, 0, 0,
		x_exp*scale, y_exp*scale, width_exp*scale,
		height_exp*scale);
    }
    else {
      HGU_XPutImage8To24(dpy, win, globals.gc_greys, scaled_image, 0, 0,
			 x_exp*scale, y_exp*scale, width_exp*scale,
			 height_exp*scale, globals.colormap);
    }
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
  unsigned int		widthb;
  XImage		*scaled_image;
  WlzUByte		*scaled_data, *data, *linedata;
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

  /* round up to an integer multiple of BitmapPad/8 */
  i = BitmapPad(dpy)>>3;
  widthb = width_exp;
  if( width_exp % i ){
    widthb += i - width_exp % i;
  }
  if((scaled_data = (WlzUByte *) AlcMalloc(sizeof(WlzUByte) *
					   widthb * height_exp))){

    scaled_image = XCreateImage(dpy, win_att.visual, 8,
				ZPixmap, 0, (char *) scaled_data,
				width_exp, height_exp, BitmapPad(dpy), widthb);

    data = (WlzUByte *) view_struct->ximage->data;
    for(yp=y_exp; yp < heightp; yp++)
    {
      linedata = data + (yp * scale * view_struct->ximage->bytes_per_line) +
	x_exp * scale;
      scaled_data = (WlzUByte *) scaled_image->data + (yp-y_exp) * widthb;
      for(xp=x_exp; xp < widthp; xp++, scaled_data++, linedata += scale)
      {
	*scaled_data = *linedata;
      }
    }

    if( globals.toplDepth == 8 ){
      XPutImage(dpy, win, globals.gc_greys, scaled_image, 0, 0,
		x_exp, y_exp, width_exp, height_exp);
    }
    else {
      HGU_XPutImage8To24(dpy, win, globals.gc_greys, scaled_image, 0, 0,
			 x_exp, y_exp, width_exp, height_exp,
			 globals.colormap);
    }

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
  XExposeEvent		event;
  XmDrawingAreaCallbackStruct
    *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  Widget		x_bar, y_bar, clip=NULL, scrolled_window;
  int 			minimum, maximum, value, width, height, size;
  int			code;

  if( !wlzViewStr->initialised ){
    if( init_view_struct( view_struct ) ){
      return;
    }
  }

  if( globals.gc_greys == NULL ){
    globals.gc_greys = HGU_XCreateGC( dpy, win );
  }

  if((view_struct->ximage == NULL) ||
     (view_struct->ximage->data == NULL)){
    /* something wrong here */
    return;
  }

/*    widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
      heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;*/
  widthp = view_struct->ximage->width * wlzViewStr->scale;
  heightp = view_struct->ximage->height * wlzViewStr->scale;

  /* check for expose event */
  if( cbs && (cbs->event) && (cbs->event->type == Expose) ){
    event = cbs->event->xexpose;
    code = 1;
  }
  else {
    event.type = Expose;
    event.x = 0;
    event.y = 0;
    event.width = widthp;
    event.height = heightp;
    code = 2;
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
    if( globals.toplDepth == 8 ){
      XPutImage(dpy, win, globals.gc_greys, view_struct->ximage,
		event.x, event.y, event.x, event.y,
		event.width, event.height);
    }
    else {
      GC	gc;
      XGCValues  gcvalues;
      gc = XCreateGC(dpy, win, 0, &gcvalues);
      HGU_XPutImage8To24(dpy, win, gc, view_struct->ximage,
			 event.x, event.y, event.x, event.y,
			 event.width, event.height,
			 globals.colormap);
    }
  }

  /* check logging */
  if( globals.logfileFp ){
    char strBuf[128];
    sprintf(strBuf, "(%d,%d,%d,%d)", event.x, event.y,
	    event.width, event.height);
    MAPaintLogData("Displayed", strBuf, code, view_struct->dialog);
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
  if((sectObj = WlzGetMaskedSectionFromObject(globals.obj, wlzViewStr,
					      WLZ_INTERPOLATION_NEAREST,
					      &errNum))){
    WlzObject	*rectObj;
    WlzDomain	domain;
    domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_RECT,
				     WLZ_NINT(wlzViewStr->minvals.vtY),
				     WLZ_NINT(wlzViewStr->maxvals.vtY),
				     WLZ_NINT(wlzViewStr->minvals.vtX),
				     WLZ_NINT(wlzViewStr->maxvals.vtX),
				     &errNum);

    /* this is not safe and assumes GetSection returns a rectangular
       value table of this size at least */
    rectObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain, sectObj->values,
			  NULL, NULL, &errNum);
    if( view_struct->masked_object ){
      WlzFreeObj(view_struct->masked_object);
    }
    view_struct->masked_object = WlzAssignObject(sectObj, NULL);

    if( view_struct->painted_object ){
      WlzFreeObj(view_struct->painted_object);
    }
    view_struct->painted_object = WlzAssignObject(rectObj, NULL);

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
    redisplay_view_cb( w, (XtPointer) vl->view_struct, call_data );

    /* check initialisation */
    if( !vl->view_struct->wlzViewStr->initialised ){
      if( init_view_struct( vl->view_struct ) ){
	continue;
      }
    }

    vtx = Wlz3DViewGetIntersectionPoint(wlzViewStr,
					vl->view_struct->wlzViewStr,
					&errNum);
    if( errNum == WLZ_ERR_NONE ){
      x = vtx.vtX - vl->view_struct->wlzViewStr->minvals.vtX;
      y = vtx.vtY - vl->view_struct->wlzViewStr->minvals.vtY;

      angle = Wlz3DViewGetIntersectionAngle(wlzViewStr,
					    vl->view_struct->wlzViewStr,
					    &errNum);
      if( errNum == WLZ_ERR_NONE ){
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
      }
    }

    /* reset the error in case it is WLZ_ERR_ALG */
    if( errNum != WLZ_ERR_NONE ){
      if( errNum == WLZ_ERR_ALG ){
	errNum = WLZ_ERR_NONE;
      }
      else {
	MAPaintReportWlzError(globals.topl, "view_feedback_cb", errNum);
	return;
      }
    }
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

    /* release paint key */
    if( paint_key == view_struct ){
      removeCurrentPaintTool(view_struct->canvas, view_struct);
      paint_key = NULL;

      /* clear the undo domains */
      clearUndoDomains();

      /* clear the section view image */
      if( view_struct->view_object ){
	WlzFreeObj(view_struct->view_object);
	view_struct->view_object = NULL;
      }

      /* install new domains, update all views */
      installViewDomains(view_struct);

      /* restart the 3D feedback display */
      HGUglwCanvasTbAnimate(globals.canvas);
      XtSetSensitive(globals.canvas, True);
    }

    /* remove the view from the view list */
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
