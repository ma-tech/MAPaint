#ifndef WLZXM_H
#define WLZXM_H

#pragma ident "MRC HGU $Id$"
/*!
* \file         WlzXm.h
* \author       richard <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 30 10:22:52 2003
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Copyright:
*               1994-2002 Medical Research Council, UK.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \ingroup      WlzXm
* \brief
*/

/* hold all type and prototypes for now */
extern Widget WlzXmCreateExtFFObjectFSB(
  Widget	parent,
  String	name,
  XtCallbackProc	proc,
  XtPointer		client_data);

WlzObject *WlzXmReadExtFFObject(
  Widget				dialog,
  XmFileSelectionBoxCallbackStruct	*cbs,
  WlzEffFormat				*dstFmt,
  WlzErrorNum				*dstErr);


/* do not add anything after this line */
#endif /* WLZXM_H */
