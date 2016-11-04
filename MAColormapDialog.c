#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAColormapDialog_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAColormapDialog.c
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
#include <sys/types.h>
#include <sys/stat.h>

#include <MAPaint.h>

Widget	colormap_dialog=NULL;

void colormap_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
     /* put up the colormap dialog */
    PopupCallback(w, (XtPointer) XtParent(colormap_dialog), NULL);

    return;
}

static void gamma_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget	slider, dialog = (Widget) client_data;

    if( (slider = XtNameToWidget(dialog, "*.gamma")) == NULL )
	return;

    globals.cmapstruct->gamma = HGU_XmGetSliderValue( slider );
    set_paint_colormap( dialog, globals.cmap, globals.cmapstruct );
    return;
}

static void low_thresh_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget	slider1, slider2, dialog = (Widget) client_data;
    int		low, high;
    float	val;

    if( (slider1 = XtNameToWidget(dialog, "*.low_thresh")) == NULL )
	return;
    low = HGU_XmGetSliderValue( slider1 );

    if( (slider2 = XtNameToWidget(dialog, "*.high_thresh")) == NULL )
	return;
    high = HGU_XmGetSliderValue( slider2 );

    if( low >= high )
    {
      globals.cmapstruct->low_thresh = high - 1;
      val = (float) high - 1;
      HGU_XmSetSliderValue( slider1, val );
    }
    else
    {
      globals.cmapstruct->low_thresh = low;
    }

    set_paint_colormap( dialog, globals.cmap, globals.cmapstruct );
    return;
}

static void high_thresh_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget	slider1, slider2, dialog = (Widget) client_data;
    int		low, high;
    float	val;

    if( (slider1 = XtNameToWidget(dialog, "*.low_thresh")) == NULL )
	return;
    low = HGU_XmGetSliderValue( slider1 );

    if( (slider2 = XtNameToWidget(dialog, "*.high_thresh")) == NULL )
	return;
    high = HGU_XmGetSliderValue( slider2 );

    if( low >= high )
    {
      globals.cmapstruct->high_thresh = low + 1;
      val = (float) low + 1;
      HGU_XmSetSliderValue( slider2, val );
    }
    else
    {
      globals.cmapstruct->high_thresh = high;
    }

    set_paint_colormap( dialog, globals.cmap, globals.cmapstruct );
    return;
}

static void invert_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget				dialog = (Widget) client_data;
    XmToggleButtonCallbackStruct 	*cbs =
	(XmToggleButtonCallbackStruct *) call_data;

    if( cbs->set == True )
	globals.cmapstruct->invert = 1;
    else
	globals.cmapstruct->invert = 0;
    set_paint_colormap( dialog, globals.cmap, globals.cmapstruct );
    return;
}

static String	ovly_strs[] = {"0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18",
"19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31", "32"};
static int	current_overlay=1;

static void overlay_select_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget	slider, dialog = (Widget) client_data;
    float	val;

    sscanf(XtName(w), "%d", &current_overlay);

    if( (slider = XtNameToWidget(dialog, "*.red")) == NULL )
	return;
    val = globals.cmapstruct->ovly_red[current_overlay];
    HGU_XmSetSliderValue(slider, val);
    
    if( (slider = XtNameToWidget(dialog, "*.green")) == NULL )
	return;
    val = globals.cmapstruct->ovly_green[current_overlay];
    HGU_XmSetSliderValue(slider, val);
    
    if( (slider = XtNameToWidget(dialog, "*.blue")) == NULL )
	return;
    val = globals.cmapstruct->ovly_blue[current_overlay];
    HGU_XmSetSliderValue(slider, val);
    
    if( (slider = XtNameToWidget(dialog, "*.contrast")) == NULL )
	return;
    val = globals.cmapstruct->ovly_contrast[current_overlay];
    HGU_XmSetSliderValue(slider, val);
    if( current_overlay > globals.cmapstruct->num_overlays )
    {
      XtSetSensitive(slider, False);
    }
    else
    {
      XtSetSensitive(slider, True);
    }
    
    return;
}

static void overlay_red_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget		slider, dialog = (Widget) client_data;
    unsigned char	val;
    
    if( (slider = XtNameToWidget(dialog, "*.red")) == NULL )
	return;
    val = HGU_XmGetSliderValue( slider );

    globals.cmapstruct->ovly_red[current_overlay] = val;
    set_paint_colormap( dialog, globals.cmap, globals.cmapstruct );
    return;
}

static void overlay_green_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget		slider, dialog = (Widget) client_data;
    unsigned char	val;
    
    if( (slider = XtNameToWidget(dialog, "*.green")) == NULL )
	return;
    val = HGU_XmGetSliderValue( slider );

    globals.cmapstruct->ovly_green[current_overlay] = val;
    set_paint_colormap( dialog, globals.cmap, globals.cmapstruct );
    return;
}

static void overlay_blue_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget		slider, dialog = (Widget) client_data;
    unsigned char	val;
    
    if( (slider = XtNameToWidget(dialog, "*.blue")) == NULL )
	return;
    val = HGU_XmGetSliderValue( slider );

    globals.cmapstruct->ovly_blue[current_overlay] = val;
    set_paint_colormap( dialog, globals.cmap, globals.cmapstruct );
    return;
}

static void overlay_contrast_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
    Widget		slider, dialog = (Widget) client_data;
    float		val;
    
    if( (slider = XtNameToWidget(dialog, "*.contrast")) == NULL )
	return;
    val = HGU_XmGetSliderValue( slider );

    globals.cmapstruct->ovly_contrast[current_overlay] = val;
    set_paint_colormap( dialog, globals.cmap, globals.cmapstruct );
    return;
}

static Widget create_overlay_option_menu(
String		name,
Widget		parent,
Widget		dialog)
{
    Widget	option_menu, widget;
    MenuItem	*items;
    int		i, col;
    String	str;
    int		num_overlays;

    num_overlays = globals.cmapstruct->num_overlays +
      globals.cmapstruct->num_solid_overlays;

    items = (MenuItem *) calloc(33, sizeof(MenuItem));

    for(i=0; i < 32; i++){
	items[i].name               = ovly_strs[i+1];
	items[i].wclass             = &xmPushButtonWidgetClass;
	items[i].mnemonic           = 0;
	items[i].accelerator        = NULL;
	items[i].accel_text         = NULL;
	items[i].item_set           = False;
	items[i].callback           = overlay_select_cb;
	items[i].callback_data      = (XtPointer) dialog;
	items[i].help_callback      = NULL;
	items[i].help_callback_data = NULL;
	items[i].tear_off_model     = XmTEAR_OFF_DISABLED;
	items[i].radio_behavior     = False;
	items[i].always_one         = False;
	items[i].subitems           = NULL;
    }

    option_menu = HGU_XmBuildMenu( parent, XmMENU_OPTION, name, 0,
				  XmTEAR_OFF_DISABLED, items);
    free( items );

    str = (String) malloc(sizeof(char) * (8));
    for(i=1; i <= 32; i++){
      if( i <= globals.cmapstruct->num_overlays )
      {
	col = globals.cmapstruct->ovly_cols[i] + globals.cmapstruct->gmax -
	  globals.cmapstruct->gmin;
      }
      else
      {
	col = globals.cmapstruct->ovly_cols[i];
      }
      col = (int) HGU_XGetColorPixel(XtDisplay(parent), globals.cmap,
			       (float) globals.colormap[0][col]/255.0,
			       (float) globals.colormap[1][col]/255.0,
			       (float) globals.colormap[2][col]/255.0);
      (void) sprintf(str, "*%s", ovly_strs[i]);
      if((widget = XtNameToWidget(option_menu, str))){
	XtVaSetValues(widget, XmNbackground, col, NULL);
      }
      if( i > num_overlays ){
	XtUnmanageChild(widget);
      }
    }
    free( str );

    return( option_menu );
}

static void WriteCmapVtk(
  FILE		*fp,
  String	name)
{
  XColor	colorcell;
  int		i;
  float		r, g, b, a;
   
  /* write the initial text */
  fprintf(fp, "LOOKUP_TABLE %s 256\n", name);

  /* write all colours in the colormap asign alpha = 1.0 */
  colorcell.flags = DoRed|DoGreen|DoBlue;
  for(i=0; i < 256; i++)
  {
    colorcell.pixel = i;
    XQueryColor(globals.dpy, globals.cmap, &colorcell);
    r = ((float) colorcell.red) / 65535;
    g = ((float) colorcell.green) / 65535;
    b = ((float) colorcell.blue) / 65535;
    a = 1.0;

    if( i < globals.cmapstruct->gmin )
    {
      r = g = b = a = 0.0;
    }

    fprintf(fp, "%.3f %.3f %.3f %.3f\n", r, g, b, a);
  }

  return;
}

static void WriteCmapVff(
  FILE		*fp,
  String	name)
{
  XColor	colorcell;
  int		i, count;
  int		r, g, b;
   
  /* write the initial text */
  count = 0;
  count += fprintf(fp, "ncaa\ncolormapsize=256;\ncolormap=");

  /* write all colours in the colormap in hex format */
  colorcell.flags = DoRed|DoGreen|DoBlue;
  for(i=0; i < 256; i++)
  {
    colorcell.pixel = i;
    XQueryColor(globals.dpy, globals.cmap, &colorcell);
    r = colorcell.red / 256;
    g = colorcell.green / 256;
    b = colorcell.blue / 256;

    if( i < globals.cmapstruct->gmin )
    {
      r = g = b = 0;
    }

    if( i == 255 )
    {
      r = 255;
      g = b = 0;
    }

    count += fprintf(fp, "%2.2x%2.2x%2.2x ", r, g, b);
  }
  count += fprintf(fp, ";\n");

  while( (count % 16) < 15 )
  {
    count += fprintf(fp, "\n");
  }
  fprintf(fp, "\014");

  return;
}


static void save_lut_cb(
Widget			w,
XtPointer		client_data,
XtPointer		call_data)
{
  FILE			*fp;
  static String		old_file=NULL;
  String		new_file;
  struct stat		stat_buf;

  /* get the new file name */
  if( old_file == NULL )
  {
    old_file = (String) malloc(sizeof(char) * 16);
    strcpy(old_file, "cmap.vtk");
  }
  if( (new_file = HGU_XmUserGetstr(globals.topl, "Please input a filename:",
				   "write", "cancel", old_file)) == NULL )
    return;

  /* check status and get the file pointer */
  if( !stat( new_file, &stat_buf ) ){
    if( !HGU_XmUserConfirm(globals.topl, "Overwrite existing file?",
			   "yes", "no", 0) ){
      free( new_file );
      return;
    }
  }
  if( (fp = fopen(new_file, "w")) == NULL ){
    free( new_file );
    return;
  }

  /* write the data */
  if( strcmp(XtName(w), "Save vff") )
  {
    WriteCmapVtk( fp, new_file );
  }
  else
  {
    WriteCmapVff( fp, new_file );
  }
  if( fclose( fp ) == EOF ){
    HGU_XmUserError(globals.topl,
		    "Save LUT:\n"
		    "    Error in closing the lutfile\n"
		    "    Please check disk space or quotas\n"
		    "    Look-Up-Table not saved",
		    XmDIALOG_FULL_APPLICATION_MODAL);
  }    

  /* keep the file name */
  if( old_file != NULL )
    free( old_file );
  old_file = new_file;

  /* if necessary create the file selection dialog */

  /* manage/popup the file selection dialog */

  return;
}

static ActionAreaItem   colormap_dialog_actions[] = {
{"Save vtk",	NULL,		NULL},
{"Save vff",	NULL,		NULL},
{"Dismiss",     NULL,           NULL},
{"Help",        NULL,           NULL},
};

Widget create_colormap_dialog(
  Widget	topl)
{
    Widget	dialog, control, form, widget, widg, slider, scale;
    float	val, minval, maxval;

    dialog = HGU_XmCreateStdDialog(topl, "colormap_dialog", xmFormWidgetClass,
				   colormap_dialog_actions, 4);

    if( (widget = XtNameToWidget(dialog, "*.Save vtk")) != NULL ){
	XtAddCallback(widget, XmNactivateCallback, save_lut_cb,
		      XtParent(dialog));
    }

    if( (widget = XtNameToWidget(dialog, "*.Save vff")) != NULL ){
	XtAddCallback(widget, XmNactivateCallback, save_lut_cb,
		      XtParent(dialog));
    }

    if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
	XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		      XtParent(dialog));
    }

    control = XtNameToWidget( dialog, "*.control" );

    val = globals.cmapstruct->gamma;
    minval = 0.1;
    maxval = 5.0;
    slider = HGU_XmCreateHorizontalSlider("gamma", control,
					  val, minval, maxval, 1,
					  gamma_cb,
					  (XtPointer) dialog);
    scale = XtNameToWidget(slider, "*scale");
    XtAddCallback(scale, XmNdragCallback, gamma_cb, (XtPointer) dialog);
    XtVaSetValues(slider,
                  XmNtopAttachment,     XmATTACH_FORM,
                  XmNleftAttachment,    XmATTACH_FORM,
                  XmNrightAttachment,  	XmATTACH_FORM,
                  NULL);
    widget = slider;

    val = globals.cmapstruct->low_thresh;
    minval = 0.0;
    maxval = 255.0;
    slider = HGU_XmCreateHorizontalSlider("low_thresh", control,
					  val, minval, maxval, 0,
					  low_thresh_cb,
					  (XtPointer) dialog);
    scale = XtNameToWidget(slider, "*scale");
    XtAddCallback(scale, XmNdragCallback, low_thresh_cb, (XtPointer) dialog);
    XtVaSetValues(slider,
                  XmNtopAttachment,     XmATTACH_WIDGET,
		  XmNtopWidget,		widget,
                  XmNleftAttachment,    XmATTACH_FORM,
                  XmNrightAttachment,  	XmATTACH_FORM,
                  NULL);
    widget = slider;

    val = globals.cmapstruct->high_thresh;
    minval = 0.0;
    maxval = 255.0;
    slider = HGU_XmCreateHorizontalSlider("high_thresh", control,
					  val, minval, maxval, 0,
					  high_thresh_cb,
					  (XtPointer) dialog);
    scale = XtNameToWidget(slider, "*scale");
    XtAddCallback(scale, XmNdragCallback, high_thresh_cb, (XtPointer) dialog);
    XtVaSetValues(slider,
                  XmNtopAttachment,     XmATTACH_WIDGET,
		  XmNtopWidget,		widget,
                  XmNleftAttachment,    XmATTACH_FORM,
                  XmNrightAttachment,  	XmATTACH_FORM,
                  NULL);
    widget = slider;

    form = XtVaCreateWidget("form", xmFormWidgetClass, control,
			    XmNtopAttachment,     XmATTACH_WIDGET,
			    XmNtopWidget,         slider,
			    XmNleftAttachment,    XmATTACH_FORM,
			    XmNrightAttachment,    XmATTACH_FORM,
			    NULL);

    widget = XtVaCreateManagedWidget("invert", xmToggleButtonGadgetClass,
				     form,
				     XmNtopAttachment,	XmATTACH_FORM,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNbottomAttachment, XmATTACH_FORM,
				     XmNvisibleWhenOff,	True,
				     XmNset,	globals.cmapstruct->invert,
				     NULL);
    XtAddCallback(widget, XmNvalueChangedCallback, invert_cb, dialog);
    widg = widget;

    widget = create_overlay_option_menu("overlay", form, dialog);

    XtVaSetValues(widget,
                  XmNtopAttachment,	XmATTACH_FORM,
                  XmNrightAttachment,	XmATTACH_FORM,
                  XmNbottomAttachment,	XmATTACH_FORM,
                  XmNleftAttachment,    XmATTACH_WIDGET,
		  XmNleftWidget,	widg,
                  NULL);
    XtManageChild( widget );
    XtManageChild( form );
    widget = form;

    widget = XtVaCreateManagedWidget("separator_1", xmSeparatorGadgetClass,
				     control,
				     XmNtopAttachment,	XmATTACH_WIDGET,
				     XmNtopWidget,	widget,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNrightAttachment,XmATTACH_FORM,
				     NULL);

    val = globals.cmapstruct->ovly_red[1];
    minval = 0.0;
    maxval = 255.0;
    slider = HGU_XmCreateHorizontalSlider("red", control,
					  val, minval, maxval, 0,
					  overlay_red_cb,
					  (XtPointer) dialog);
    scale = XtNameToWidget(slider, "*scale");
    XtAddCallback(scale, XmNdragCallback, overlay_red_cb,
		  (XtPointer) dialog);
    XtVaSetValues(slider,
                  XmNtopAttachment,     XmATTACH_WIDGET,
		  XmNtopWidget,		widget,
                  XmNleftAttachment,    XmATTACH_FORM,
                  XmNrightAttachment,  	XmATTACH_FORM,
                  NULL);
    widget = slider;

    val = globals.cmapstruct->ovly_green[1];
    minval = 0.0;
    maxval = 255.0;
    slider = HGU_XmCreateHorizontalSlider("green", control,
					  val, minval, maxval, 0,
					  overlay_green_cb,
					  (XtPointer) dialog);
    scale = XtNameToWidget(slider, "*scale");
    XtAddCallback(scale, XmNdragCallback, overlay_green_cb,
		  (XtPointer) dialog);
    XtVaSetValues(slider,
                  XmNtopAttachment,     XmATTACH_WIDGET,
		  XmNtopWidget,		widget,
                  XmNleftAttachment,    XmATTACH_FORM,
                  XmNrightAttachment,  	XmATTACH_FORM,
                  NULL);
    widget = slider;

    val = globals.cmapstruct->ovly_blue[1];
    minval = 0.0;
    maxval = 255.0;
    slider = HGU_XmCreateHorizontalSlider("blue", control,
					  val, minval, maxval, 0,
					  overlay_blue_cb,
					  (XtPointer) dialog);
    scale = XtNameToWidget(slider, "*scale");
    XtAddCallback(scale, XmNdragCallback, overlay_blue_cb,
		  (XtPointer) dialog);
    XtVaSetValues(slider,
                  XmNtopAttachment,     XmATTACH_WIDGET,
		  XmNtopWidget,		widget,
                  XmNleftAttachment,    XmATTACH_FORM,
                  XmNrightAttachment,  	XmATTACH_FORM,
                  NULL);
    widget = slider;

    val = globals.cmapstruct->ovly_contrast[1];
    minval = 0.0;
    maxval = 1.0;
    slider = HGU_XmCreateHorizontalSlider("contrast", control,
					  val, minval, maxval, 2,
					  overlay_contrast_cb,
					  (XtPointer) dialog);
    scale = XtNameToWidget(slider, "*scale");
    XtAddCallback(scale, XmNdragCallback, overlay_contrast_cb,
		  (XtPointer) dialog);
    XtVaSetValues(slider,
                  XmNtopAttachment,     XmATTACH_WIDGET,
		  XmNtopWidget,		widget,
                  XmNleftAttachment,    XmATTACH_FORM,
                  XmNrightAttachment,  	XmATTACH_FORM,
                  NULL);
    widget = slider;

    return( dialog );
}
