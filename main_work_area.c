#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _main_work_area_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         main_work_area.c
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

#include <MAPaint.h>

/* action and callback procedures */

static void ThreeDDrawSceneCb(
  Widget	w,
  XtPointer	clientData,
  XtPointer	callData)
{
  MAOpenGLDrawScene(w);

  return;
}

static void ThreeDResizeCb(
  Widget	w,
  XtPointer	clientData,
  XtPointer	callData)
{
  HGUglwCanvasCallbackStruct *glwCbs;

  glwCbs = (HGUglwCanvasCallbackStruct *)callData;
  glViewport(0, 0, (GLint)(glwCbs->width), (GLint)(glwCbs->height));

  return;
}

static void ThreeDInputCb(
  Widget	w,
  XtPointer	clientData,
  XtPointer	callData)
{
  char		buffer[1];
  KeySym	keysym;
  HGUglwCanvasCallbackStruct *glwCbs;

  glwCbs = (HGUglwCanvasCallbackStruct *)callData;
  switch(glwCbs->event->type)
  {
    case KeyRelease:
      if(XLookupString((XKeyEvent *)glwCbs->event, buffer, 1,
      		       &keysym, NULL) == 1)
      {
	switch(keysym)
	{
	  case XK_Escape:
	  case XK_Q:
	  case XK_q:
	    break;
	}
      }
      break;
  }

  return;
}

void ThreeDResetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGUglwCanvasTbReset( globals.canvas );
  MAOpenGLDrawScene( globals.canvas );

  return;
}

Widget create_main_work_area(
  Widget	main_w)
{
  Widget	work_area;
  Pixel	bg;
  XVisualInfo	*visualInfo, visualTemplate;
  int		nitems;
  Boolean	rgbModeFlag=False;

  bg = BlackPixelOfScreen(XtScreen(main_w));

  visualTemplate.visualid = globals.toplVisual->visualid;
  visualInfo = XGetVisualInfo(XtDisplay(main_w), VisualIDMask,
			      &visualTemplate, &nitems);
  if( globals.toplDepth == 24 ){
    rgbModeFlag = True;
  }

  work_area = XtVaCreateManagedWidget("work_area",
				      hguGLwCanvasTbWidgetClass,
				      main_w,
				      XmNbackground,		bg,
				      HGUglwNtrackballSize,	512,
				      HGUglwNvisualInfo,	visualInfo,
				      XtNvisual,	globals.toplVisual,
				      XtNcolormap,	globals.cmap,
				      XtNdepth,		globals.toplDepth,
				      HGUglwNrgba,	rgbModeFlag,
				      NULL);

  XtAddCallback(work_area, HGUglwNexposeCallback,   ThreeDDrawSceneCb,
		NULL);
  XtAddCallback(work_area, HGUglwNresizeCallback,   ThreeDResizeCb, NULL);
  XtAddCallback(work_area, HGUglwNinitCallback,     MAOpenGLInitCb, NULL);
  XtAddCallback(work_area, HGUglwNinputCallback,    ThreeDInputCb, NULL);
  XtAddCallback(work_area, HGUglwNtrackballCallback, ThreeDDrawSceneCb,
		NULL);

  globals.canvas = work_area;
  XtVaSetValues(work_area, XmNcolormap, globals.cmap, NULL);

  XtManageChild( work_area );
  return( work_area );
}

Widget create_main_work_area_X(
Widget	main_w)
{
    Widget	work_area;
    Pixel	bg;

    bg = BlackPixelOfScreen(XtScreen(main_w));

    work_area = XtVaCreateManagedWidget("work_area",
					xmDrawingAreaWidgetClass,
					main_w,
					XmNbackground,		bg,
					NULL);

    globals.canvas = work_area;

    XtManageChild( work_area );
    return( work_area );
}

void init_main_work_area(
  Widget	topl)
{
  /* sort out the display window */
  globals.dpy = XtDisplay( globals.canvas );
  globals.win = XtWindow ( globals.canvas );

  return;
}
