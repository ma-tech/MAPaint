#ifndef MAPAINT_H
#define MAPAINT_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAPaint_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAPaint.h
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

/* Platfrom defines. */
#ifdef __linux__
#define LINUX2
#else
#ifdef __MACH__
#define DARWIN
#else
#if defined(sun) || defined(__sun)
#define SUNOS5
#endif
#endif
#endif
/* do not add anything after this line */
#endif /* MAPAINT_H */
