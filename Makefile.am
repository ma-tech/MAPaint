##
# \file         Makefile.am
# \author       Bill Hill
# \date         November 2007
# \version      $Id$
# \par
# Address:
#               MRC Human Genetics Unit,
#               MRC Institute of Genetics and Molecular Medicine,
#               University of Edinburgh,
#               Western General Hospital,
#               Edinburgh, EH4 2XU, UK.
# \par
# Copyright (C), [2012],
# The University Court of the University of Edinburgh,
# Old College, Edinburgh, UK.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be
# useful but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public
# License along with this program; if not, write to the Free
# Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
# \brief        Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS		= \
		  	-I. \
			$(all_includes)

LDADD			= \
			-lXt -lXmu -lXp -lX11 -lXext \
			-lm -lSM -lICE -ldl -lpthread

bin_PROGRAMS		= \
			MAPaint

MAPaint_SOURCES       	= \
                        anatomy_menu.c \
                        domainControls.c \
                        domain_menu.c \
                        domain_utils.c \
                        file_menu.c \
                        MAAutosaveDialog.c \
                        MAColormapDialog.c \
                        MAColormapUtils.c \
                        MAConformalPoly.c \
                        MADomainReviewDialog.c \
                        MADomainSurgeryDialog.c \
                        MAEmageReview.c \
                        main_buttonbar.c \
                        main_menubar.c \
                        main_work_area.c \
                        MAOpenGLUtils.c \
                        MAPaint.c \
                        MAPaintEvent.c \
                        MAPaintSocket.c \
                        MAPaintUndoDomain.c \
                        MAPMSnake.c \
                        MARealignmentDialog.c \
                        MASaveSequenceDialog.c \
                        MATrackDomain.c \
                        MAWarpInput2DDialog.c \
                        MAWarpInputInteractUtils.c \
                        MAWarpInputPages.c \
                        MAWarpInputPaintingCb.c \
                        MAWarpInputUtils.c \
                        MAWarpInputXUtils.c \
                        MAWarpSignalAutoPage.c \
                        MAWarpSignalColorUtils.c \
                        MAWarpSignalDomain.c \
                        MAWarpSignalFilterPage.c \
                        MAWarpSignalInteract.c \
                        MAWarpSignalPostProcPage.c \
                        MAWarpSignalPreProcPage.c \
                        MAWarpSignalPreprocUtils.c \
                        MAWarpSignalProc.c \
                        MAWarpSignalThresholdPage.c \
                        objPropsDialog.c \
                        options_menu.c \
                        paint_utils.c \
                        theilerStage.c \
                        tools_affine.c \
                        tools_edge_tracking.c \
                        tools_fill_domain.c \
                        tools_interactive_geom.c \
                        tools_interactive_paint.c \
                        tools_morphological.c \
                        tools_tracking.c \
                        view2DPaintingCb.c \
                        view_disp_cb.c \
                        viewFixedPointUtils.c \
                        view_geom_cb.c \
                        view_menu.c \
                        view_utils.c

include_HEADERS         = \
			HGU_Tablet.h \
			MAPaintEvent.h \
			MAPaint.h \
			MAPaintHelp.h \
			MAPaintHelpResource.h \
			MAPaintProto.h \
			MAPaintResources.h \
			MAPaintTypes.h \
			MAWarp.h \
			WlzXm.h

MAPaint_LDADD           = $(LDADD)
MAPaint_LDFLAGS         = $(AM_LFLAGS)

