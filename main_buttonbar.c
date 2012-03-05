#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _main_buttonbar_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         main_buttonbar.c
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
#include <memory.h>

#include <Xm/XmAll.h>

#include <MAPaint.h>

/* private globals */

/* action and callback procedures */


Widget create_main_buttonbar(
Widget	main_w)
{
    Widget	buttonbar, widget;

    buttonbar = XtVaCreateWidget("buttonbar", xmFormWidgetClass, main_w,
				 XmNfractionBase,	19,
				 XmNborderWidth,	0,
				 XmNverticalSpacing,	2,
				 NULL);

    widget = XtVaCreateManagedWidget("reset", xmPushButtonGadgetClass,
				     buttonbar,
				     XmNborderWidth,	0,
				     XmNshadowThickness,	3,
				     XmNmarginWidth,	5,
				     XmNmarginHeight,	3,
				     XmNleftAttachment,	XmATTACH_FORM,
				     NULL);

    XtAddCallback(widget, XmNactivateCallback, ThreeDResetCb, NULL);
 
    XtManageChild( buttonbar );
    return( buttonbar );
}

void init_main_buttonbar(
Widget	topl)
{
    return;
}

