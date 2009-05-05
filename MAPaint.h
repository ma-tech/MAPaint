#ifndef MAPAINT_H
#define MAPAINT_H

#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _MAPaint.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         MAPaint.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri May  1 13:18:22 2009
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

#include <X11/keysym.h>
#include <Xm/XmAll.h>

#include <Wlz.h>
#include <WlzExtFF.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <HGUgl.h>

#include <HGU_DrawingA.h>

#include <HGU_XmUtils.h>
#include <HGU_XInteract.h>

#include <MAPaintTypes.h>
#include <MAPaintProto.h>
#include <MAPaintHelp.h>
#include <MAPaintEvent.h>

/* define the release version - currently put in by hand */
#define MAPAINT_RELEASE		2
#define MAPAINT_REVISION	0
#define MAPAINT_UPDATE_LEVEL	x
#define MAPAINT_RELEASE_STR	"MAPaint Release 2.0.x"

/* do not add anything after this line */
#endif /* MAPAINT_H */
