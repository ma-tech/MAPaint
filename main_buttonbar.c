#pragma ident "MRC HGU $Id$"
/************************************************************************
* Project:	MRC HGU General IP and Display Utilities		*
* Title:	main_buttonbar.c					*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council, UK.				*
* Date:		
* Synopsis:	
************************************************************************/
#include <stdio.h>
#include <memory.h>

#include <Xm/XmAll.h>

#include <MAPaint.h>

/* private globals */

/* action and callback procedures */


Widget create_main_buttonbar(
Widget	main_w)
{
    Widget	buttonbar, frame, checkbar, widget, widg1, preview;

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

