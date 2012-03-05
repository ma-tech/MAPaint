#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAWarpSignalAutoPage_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAWarpSignalAutoPage.c
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

Widget createSignalAutoPage(
  Widget	notebook)
{
  Widget	page, button, label;
  XmString	xmstr;

  /* major page for automatic segmentation, apply fixed algorithms,
     classifier training, parameter settings, macros */
  page = XtVaCreateManagedWidget("auto_process_page",
				 xmFormWidgetClass, 	notebook,
				 XmNnotebookChildType, XmPAGE,
				 NULL);
  button = XtVaCreateManagedWidget("auto_process_tab",
				   xmPushButtonWidgetClass, notebook,
				   XmNnotebookChildType, XmMAJOR_TAB,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, thresholdMajorPageSelectCb,
		(XtPointer) WLZ_RGBA_THRESH_NONE);

  /* label to provide instructions for background removalfeedback for now */
  label = XtVaCreateManagedWidget("auto_label",
				  xmLabelWidgetClass, page,
				  XmNtopAttachment,	XmATTACH_FORM,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNborderWidth,	0,
				  XmNalignment,		XmALIGNMENT_BEGINNING,
				  NULL);
  xmstr = XmStringCreateLtoR(
    "This page will provide controls for\nautomatic signal extraction.\n"
    "These will be implmented in the\nnext version. ",
    XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(label, XmNlabelString, xmstr, NULL);
  XmStringFree(xmstr);

  return page;
}
