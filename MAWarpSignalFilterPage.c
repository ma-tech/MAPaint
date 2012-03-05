#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAWarpSignalFilterPage_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAWarpSignalFilterPage.c
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

#include <MAPaint.h>
#include <MAWarp.h>

Widget createSignalFilterPage(
  Widget	notebook)
{
  Widget	page, button, toggle, slider, widget;
  float		fval;

  /* major page 2 - filtering */
  page = XtVaCreateManagedWidget("filter_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("filter_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMajorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_NONE);

  /* gauss smoothing toggle and width parameter */
  toggle = XtVaCreateManagedWidget("gauss_smooth",
				   xmToggleButtonGadgetClass, page,
				   XmNleftAttachment,	XmATTACH_FORM,
				   XmNtopAttachment,	XmATTACH_FORM,
				   NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, recalcWarpProcObjCb, NULL);
  widget = toggle;
  fval = 5.0;
  slider = HGU_XmCreateHorizontalSlider("gauss_width", page,
					fval, 0.5, 20.0, 1,
					recalcWarpProcObjCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

  return page;
}

