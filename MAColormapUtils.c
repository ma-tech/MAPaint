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
*   File       :   MAColormapUtils.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Jan 19 09:47:09 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MAPaint.h>


static unsigned char ovly_cols[33][3] = 
{
/* greys */ 
  {0,0,0},
/* wash overlays */ 
  {255,0,0}, {0,0,255}, {0,255,0}, {255,255,0}, {0,255,255},
/* solid overlays */
  {255,0,0}, {255,129,0}, {255,184,0}, {255,255,0}, {158,255,0},
  {76,122,0}, {0,77,0},
  {0,103,68}, {0,147,110}, {0,192,128}, {0,255,112},
  {0,214,255}, {0,128,255},
  {0,0,181}, {0,0,108}, {29,0,49}, {81,0,74}, {128,0,137},
  {184,0,223}, {192,128,255}, {255,187,250}, {242,124,181},
  {192,0,128}, {174,0,47},
  {255,192,128}, {250,128,64}, {192,128,0}
};

/* resource structures and type converters */
#define set_att_offset(field)   XtOffsetOf(PaintCmapStruct, field)
 
static XtResource set_att_res[] = {
{"invert", "Invert", XtRInt, sizeof(int),
     set_att_offset(invert), XtRImmediate, (caddr_t) 0},
{"gamma", "Gamma", XtRFloat, sizeof(float),
     set_att_offset(gamma), XtRString, "1.0"},
{"numSolidOverlays", "NumOverlays", XtRInt, sizeof(int),
     set_att_offset(num_solid_overlays), XtRImmediate, (caddr_t) 5},
{"numWashOverlays", "NumOverlays", XtRInt, sizeof(int),
     set_att_offset(num_overlays), XtRImmediate, (caddr_t) 5},
{"colormapLowThreshold", "Threshold", XtRInt, sizeof(int),
     set_att_offset(low_thresh), XtRImmediate, (caddr_t) 0},
{"colormapHighThreshold", "Threshold", XtRInt, sizeof(int),
     set_att_offset(high_thresh), XtRImmediate, (caddr_t) 255},
};
 
/* function to allocate colormap cells for setting values,
   no values are set but the values in the colormap structure are */
Colormap init_paint_colormap(
  Widget		w,
  PaintCmapStruct	*cmpstr)
{
  Display		*dpy = XtDisplay( w );
  Window		win = XtWindow( w );
  XWindowAttributes	win_att;
  Colormap		cmap;
  unsigned long	plane_masks[1], free_pixels[256];
  unsigned int	i, n_free_pixels;
  unsigned long	min_required_val, max_required_val;

  cmap = globals.cmap;

  /* allocate the required colorcells */
  n_free_pixels = 0;
  cmpstr->npixels = 0;
  min_required_val = cmpstr->gmin;
  max_required_val = cmpstr->gmax +
    cmpstr->ovly_incr[cmpstr->num_overlays] + cmpstr->num_solid_overlays;
  for(i=0; i < 256; i++){

    if( globals.toplDepth == 8 ){
      if( XAllocColorCells(dpy, cmap, False, plane_masks,
			   0, &(free_pixels[n_free_pixels]), 1) == 0 ){
	continue;
      }
    }
    else {
      free_pixels[n_free_pixels] = i;
    }

    /* check if required */
    if(free_pixels[n_free_pixels] >= min_required_val &&
       free_pixels[n_free_pixels] <= max_required_val){
      cmpstr->pixels[cmpstr->npixels] = free_pixels[n_free_pixels];
      cmpstr->npixels++;
    } else {
      n_free_pixels++;
    }
  }

  /* free unwanted colorcells */
  if( globals.toplDepth == 8 ){
    XFreeColors(dpy, cmap, free_pixels, n_free_pixels, 0);
  }

  return( cmap );
}

int set_paint_colormap(
  Widget		w,
  Colormap	cmap,
  PaintCmapStruct	*cmpstr)
{
  unsigned char	colormap[3][256], gval;
  int			i, j;
  double		val;
  XColor		colorcells[256];
  unsigned int	solid_ovly;

  /* setup the colormap  - grey-level */
  for(i=0; i < cmpstr->gmin; i++){
      colormap[0][i] = 0;
      colormap[1][i] = 0;
      colormap[2][i] = 0;
  }  
  for(; i <= cmpstr->gmax; i++){

    /* grey-levels: use invert and gamma parameters */
    val = ((i - cmpstr->gmin) * 255 )/ (cmpstr->gmax - cmpstr->gmin);

    if( val <= cmpstr->low_thresh )
    {
      val = 0.0;
    }
    else if( val >= cmpstr->high_thresh )
    {
      val = 255.0;
    }
    else
    {
      val = ((val - cmpstr->low_thresh) * 255) /
	(cmpstr->high_thresh - cmpstr->low_thresh);
    }

    gval = pow( (val / 255), (double) cmpstr->gamma ) * 255;
    if( cmpstr->invert )
      gval = 255 - gval;

    colormap[0][i] = gval;
    colormap[1][i] = gval;
    colormap[2][i] = gval;
  }
  for(; i < 256; i++){
      colormap[0][i] = 0;
      colormap[1][i] = 0;
      colormap[2][i] = 0;
  }  

  /* set the wash overlays */
  for(j=1; j <= cmpstr->num_overlays; j++){
    int	incr = cmpstr->ovly_incr[j];
    float	factor = (255.0 * cmpstr->ovly_contrast[j] /
			  ((float) cmpstr->ovly_red[j] +
			   (float) cmpstr->ovly_green[j] +
			   (float) cmpstr->ovly_blue[j]));

    for(i=cmpstr->gmin; i <= cmpstr->gmax; i++){
      colormap[0][i+incr] = 
	(float) colormap[0][i]*(1.0 - cmpstr->ovly_contrast[j])
	+ (float) cmpstr->ovly_red[j] * factor; 
      colormap[1][i+incr] =
	(float) colormap[1][i]*(1.0 - cmpstr->ovly_contrast[j])
	+ (float) cmpstr->ovly_green[j] * factor; 
      colormap[2][i+incr] =
	(float) colormap[2][i]*(1.0 - cmpstr->ovly_contrast[j])
	+ (float) cmpstr->ovly_blue[j] * factor; 
    }

  }

  /* set the solid overlays */
  for(i=0; i < cmpstr->npixels; i++){
    j = cmpstr->pixels[i];
    if( j > (cmpstr->gmax + cmpstr->ovly_incr[cmpstr->num_overlays]) ){
      solid_ovly = j -
	(cmpstr->gmax + cmpstr->ovly_incr[cmpstr->num_overlays]) +
	cmpstr->num_overlays;
      cmpstr->ovly_cols[solid_ovly] = j;
      colormap[0][j] = cmpstr->ovly_red[solid_ovly];
      colormap[1][j] = cmpstr->ovly_green[solid_ovly];
      colormap[2][j] = cmpstr->ovly_blue[solid_ovly];
    }
  }

  switch( globals.visualMode ){
  case MAPAINT_8BIT_ONLY_MODE:
  case MAPAINT_8_24BIT_MODE:
    /* set the colormap colours */
    for(i=0; i < cmpstr->npixels; i++){
      j = cmpstr->pixels[i];
      colorcells[i].red   = colormap[0][j] * 256;
      colorcells[i].green = colormap[1][j] * 256;
      colorcells[i].blue  = colormap[2][j] * 256;
      colorcells[i].pixel = j;
      colorcells[i].flags = DoRed | DoGreen | DoBlue;

    }
    XStoreColors(XtDisplay(w), cmap, colorcells, cmpstr->npixels);
    for(j=0; j < 256; j++){
      globals.colormap[0][j] = colormap[0][j];
      globals.colormap[1][j] = colormap[1][j];
      globals.colormap[2][j] = colormap[2][j];
    }

    /* set the supplementary colours */
    for(i=1; i <= cmpstr->num_overlays; i++)
    {
      (void) HGU_XGetColorPixel(XtDisplay(w), cmap,
				(float) cmpstr->ovly_red[i]/255.0,
				(float) cmpstr->ovly_green[i]/255.0,
				(float) cmpstr->ovly_blue[i]/255.0);
    }
    (void) HGU_XGetColorPixel(XtDisplay(w), cmap, 1.0, 0.0, 0.0);
    (void) HGU_XGetColorPixel(XtDisplay(w), cmap, 0.0, 1.0, 0.0);
    (void) HGU_XGetColorPixel(XtDisplay(w), cmap, 0.0, 0.0, 1.0);

    /* shouldn't do this but necessary on the 24bit displays */
    /*XInstallColormap(XtDisplay(w), cmap);*/
    break;

  case MAPAINT_24BIT_ONLY_MODE:
    for(i=0; i < cmpstr->npixels; i++){
      j = cmpstr->pixels[i];
      globals.colormap[0][j] = colormap[0][j];
      globals.colormap[1][j] = colormap[1][j];
      globals.colormap[2][j] = colormap[2][j];
    }
    break;
  }

  return( 0 );
}

void init_paint_cmapstruct(
Widget	canvas)
{
    PaintCmapStruct	*cmpstr;
    int			i;

    /* setup the colormap structure */
    globals.cmapstruct = (PaintCmapStruct *)
      AlcMalloc(sizeof(PaintCmapStruct));
    cmpstr = globals.cmapstruct;

    cmpstr->gmin = 32;		    /* should get this lot from resources */
    cmpstr->gmax = 63;

    XtGetApplicationResources(canvas, cmpstr, set_att_res,
                              XtNumber(set_att_res), NULL, 0);
    if( cmpstr->num_overlays > 5 )
    {
      cmpstr->num_overlays = 5;
    }

    if( cmpstr->num_solid_overlays > 27 )
    {
      cmpstr->num_solid_overlays = 27;
    }

    if( globals.sectViewFlg ){
      cmpstr->num_overlays = 5;
      cmpstr->num_solid_overlays = 0;
    }

    cmpstr->ovly_planes  = 224;
    cmpstr->ovly_cols[0] = 32;
    cmpstr->ovly_cols[1] = 64;
    cmpstr->ovly_cols[2] = 96;
    cmpstr->ovly_cols[3] = 128;
    cmpstr->ovly_cols[4] = 160;
    cmpstr->ovly_cols[5] = 192;
    cmpstr->ovly_incr[0] = 0;
    cmpstr->ovly_incr[1] = 32;
    cmpstr->ovly_incr[2] = 64;
    cmpstr->ovly_incr[3] = 96;
    cmpstr->ovly_incr[4] = 128;
    cmpstr->ovly_incr[5] = 160;

    for(i=0; i < 33; i++)
    {
      cmpstr->ovly_red[i] = ovly_cols[i][0];
      cmpstr->ovly_green[i] = ovly_cols[i][1];
      cmpstr->ovly_blue[i] = ovly_cols[i][2];
    }
      
    cmpstr->ovly_contrast[0] = 0.0;
    cmpstr->ovly_contrast[1] = 0.25;
    cmpstr->ovly_contrast[2] = 0.25;
    cmpstr->ovly_contrast[3] = 0.25;
    cmpstr->ovly_contrast[4] = 0.25;
    cmpstr->ovly_contrast[5] = 0.25;

    for(i=6; i < 33; i++)
    {
      cmpstr->ovly_contrast[i] = 1.0;
    }

    globals.cmap = init_paint_colormap( canvas, cmpstr );
    set_paint_colormap( canvas, globals.cmap, cmpstr );

    return;
}

void HGU_XmCreatePrivateColormap(Display *dpy)
{
    Window	win;
    Visual	*visual;
    Colormap	cmap, def_cmap;
    XColor	colorcell;
    unsigned long	plane_masks[1], pixels[256];
    unsigned int	n_pixels;
    int			i;

    /* create a private colormap */
    win = RootWindow( dpy, DefaultScreen( dpy ) );
    visual = globals.toplVisual;
    if( visual == NULL ){
      visual = DefaultVisual( dpy, DefaultScreen( dpy ) );
    }
    cmap = XCreateColormap( dpy, win, visual, AllocNone );
    globals.cmap = cmap;
    def_cmap = DefaultColormap( dpy, DefaultScreen( dpy ) );
    
    /* copy all existing colours */
    n_pixels = 0;
    colorcell.flags = DoRed|DoGreen|DoBlue;
    for(i=0; i < 256; i++){

	if( XAllocColorCells(dpy, cmap, False, plane_masks,
			     0, &(pixels[n_pixels]), 1) == 0 )
	    continue;
	if( i >= 24 )	/* only free colours over pixel value 24 */
	    n_pixels++;

	colorcell.pixel = i;
	XQueryColor(dpy, def_cmap, &colorcell);
	XStoreColor(dpy, cmap, &colorcell);
    }

    XFreeColors(dpy, cmap, pixels, n_pixels, 0);
    
    return;
}

void install_cmap_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    Widget			canvas = (Widget) client_data;
    XSetWindowAttributes	set_win_att;

    set_win_att.colormap = globals.cmap;
    XChangeWindowAttributes(XtDisplay(canvas), XtWindow(canvas),
			    CWColormap, &set_win_att);
    XInstallColormap(XtDisplay(canvas), globals.cmap);
    return;
}

unsigned long HGU_XGetColorPixel(
  Display	*dpy,
  Colormap	cmap,
  float		red,
  float		green,
  float		blue)
{
  XColor	colorcell_def;

  colorcell_def.red = (short) (red * 255.0) * 256;
  colorcell_def.green = (short) (green * 255.0) * 256;
  colorcell_def.blue = (short) (blue * 255.0) * 256;
  colorcell_def.flags = DoRed | DoGreen | DoBlue;
  colorcell_def.pixel = 0;
  colorcell_def.pad = '\0';

  if( XAllocColor(dpy, cmap, &colorcell_def) ){
    return( colorcell_def.pixel );
  }
  else {
    XColor	cells[256];
    int		i, imin, dist, mindist;
    for(i=0; i < 256; i++){
      cells[i].pixel = i;
    }
    XQueryColors(dpy, cmap, cells, 256);
    dist = (abs(colorcell_def.red - cells[0].red) +
	    abs(colorcell_def.green - cells[0].green) +
	    abs(colorcell_def.blue - cells[0].blue));
    mindist = dist;
    imin = 0;
    for(i=1; i < 256; i++){
      dist = (abs(colorcell_def.red - cells[i].red) +
	      abs(colorcell_def.green - cells[i].green) +
	      abs(colorcell_def.blue - cells[i].blue));
      if( dist < mindist ){
	mindist = dist;
	imin = i;
      }
    }
    return cells[imin].pixel;
  }
}

