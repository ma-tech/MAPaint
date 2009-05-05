#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _main_buttonbar_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         main_buttonbar.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:49:24 2009
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par Copyright:
* Copyright (C) 2005 Medical research Council, UK.
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
*               
*
* Maintenance log with most recent changes at top of list.
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

