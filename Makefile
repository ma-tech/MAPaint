##########################################################################
# Project:      Mouse Atlas						#
# Title:        Makefile		                               	#
#$Id$
# Date:         September 1994	                                    	#
# Author:       Bill Hill 				    		#
#$Revision$
# Purpose:      Template makefile for Mouse Atlas Project sources.	#
# Maintenance:	Log changes below, with most recent at top of list.	#
# Christophe : 15 Sep 94 : added doc and www templates                  #
#########################################################################

# Include the configuration makefile (modify as required).
include			../../Makefile.conf

# local mods to Makefile.conf defaults

# Names of executables to be built (modify as required).
EXECUTABLES		= \
			MAPaint
ifeq ($(UNIXTYPE), DARWIN)
EXECUTABLES		= \
			MAPaint
endif

SCRIPTS			= \
			MAPaint.log

# A version number for a versioned release
RELEASE			= 1.0

# List of all 'C' source files (modify as required).
CSOURCES		= \
			HGU_Tablet.c \
			MAAutosaveDialog.c \
			MAColormapDialog.c \
			MAColormapUtils.c \
			MAConformalPoly.c \
			MADomainReviewDialog.c \
			MADomainSurgeryDialog.c \
			MAEmageReview.c \
			MAOpenGLUtils.c \
			MAPaint.c \
			MAPaintEvent.c \
			MAPaintUndoDomain.c \
			MARealignmentDialog.c \
			MASaveSequenceDialog.c \
			MAPaintSocket.c \
			MAPMSnake.c \
			MATrackDomain.c \
			MAWarpInput2DDialog.c \
			MAWarpInputUtils.c \
			MAWarpInputInteractUtils.c \
			MAWarpInputPages.c \
			MAWarpInputPaintingCb.c \
			MAWarpInputXUtils.c \
			MAWarpSignalAutoPage.c \
			MAWarpSignalColorUtils.c \
			MAWarpSignalDomain.c \
			MAWarpSignalFilterPage.c \
			MAWarpSignalInteract.c \
			MAWarpSignalPreProcPage.c \
			MAWarpSignalPreprocUtils.c \
			MAWarpSignalProc.c \
			MAWarpSignalPostProcPage.c \
			MAWarpSignalThresholdPage.c \
			anatomy_menu.c \
			domainControls.c \
			domain_menu.c \
			domain_utils.c \
			file_menu.c \
			main_buttonbar.c \
			main_menubar.c \
			main_work_area.c \
			objPropsDialog.c \
			options_menu.c \
			paint_utils.c \
			theilerStage.c \
			tools_affine.c \
			tools_fill_domain.c \
			tools_interactive_geom.c \
			tools_interactive_paint.c \
			tools_morphological.c \
			tools_tablet.c \
			tools_edge_tracking.c \
			tools_tracking.c \
			view_disp_cb.c \
			view_geom_cb.c \
			view_menu.c \
			view_utils.c \
			view2DPaintingCb.c \
			viewFixedPointUtils.c

ifeq ($(UNIXTYPE), LINUX2)
CSOURCES		= \
			MAAutosaveDialog.c \
			MAColormapDialog.c \
			MAColormapUtils.c \
			MAConformalPoly.c \
			MADomainReviewDialog.c \
			MADomainSurgeryDialog.c \
			MAEmageReview.c \
			MAOpenGLUtils.c \
			MAPaint.c \
			MAPaintEvent.c \
			MAPaintUndoDomain.c \
			MARealignmentDialog.c \
			MASaveSequenceDialog.c \
			MAPaintSocket.c \
			MAPMSnake.c \
			MATrackDomain.c \
			MAWarpInput2DDialog.c \
			MAWarpInputUtils.c \
			MAWarpInputInteractUtils.c \
			MAWarpInputPages.c \
			MAWarpInputPaintingCb.c \
			MAWarpInputXUtils.c \
			MAWarpSignalAutoPage.c \
			MAWarpSignalColorUtils.c \
			MAWarpSignalDomain.c \
			MAWarpSignalFilterPage.c \
			MAWarpSignalInteract.c \
			MAWarpSignalPreProcPage.c \
			MAWarpSignalPreprocUtils.c \
			MAWarpSignalProc.c \
			MAWarpSignalPostProcPage.c \
			MAWarpSignalThresholdPage.c \
			anatomy_menu.c \
			domainControls.c \
			domain_menu.c \
			domain_utils.c \
			file_menu.c \
			main_buttonbar.c \
			main_menubar.c \
			main_work_area.c \
			objPropsDialog.c \
			options_menu.c \
			paint_utils.c \
			theilerStage.c \
			tools_affine.c \
			tools_fill_domain.c \
			tools_interactive_geom.c \
			tools_interactive_paint.c \
			tools_morphological.c \
			tools_edge_tracking.c \
			tools_tracking.c \
			view_disp_cb.c \
			view_geom_cb.c \
			view_menu.c \
			view_utils.c \
			view2DPaintingCb.c \
			viewFixedPointUtils.c
endif
ifeq ($(UNIXTYPE), DARWIN)
CSOURCES		= \
			MAAutosaveDialog.c \
			MAColormapDialog.c \
			MAColormapUtils.c \
			MAConformalPoly.c \
			MADomainReviewDialog.c \
			MADomainSurgeryDialog.c \
			MAEmageReview.c \
			MAOpenGLUtils.c \
			MAPaint.c \
			MAPaintEvent.c \
			MAPaintUndoDomain.c \
			MARealignmentDialog.c \
			MASaveSequenceDialog.c \
			MAPaintSocket.c \
			MAPMSnake.c \
			MATrackDomain.c \
			MAWarpInput2DDialog.c \
			MAWarpInputUtils.c \
			MAWarpInputInteractUtils.c \
			MAWarpInputPages.c \
			MAWarpInputPaintingCb.c \
			MAWarpInputXUtils.c \
			MAWarpSignalAutoPage.c \
			MAWarpSignalColorUtils.c \
			MAWarpSignalDomain.c \
			MAWarpSignalFilterPage.c \
			MAWarpSignalInteract.c \
			MAWarpSignalPreProcPage.c \
			MAWarpSignalPreprocUtils.c \
			MAWarpSignalProc.c \
			MAWarpSignalPostProcPage.c \
			MAWarpSignalThresholdPage.c \
			anatomy_menu.c \
			domainControls.c \
			domain_menu.c \
			domain_utils.c \
			file_menu.c \
			main_buttonbar.c \
			main_menubar.c \
			main_work_area.c \
			objPropsDialog.c \
			options_menu.c \
			paint_utils.c \
			theilerStage.c \
			tools_affine.c \
			tools_fill_domain.c \
			tools_interactive_geom.c \
			tools_interactive_paint.c \
			tools_morphological.c \
			tools_edge_tracking.c \
			tools_tracking.c \
			view_disp_cb.c \
			view_geom_cb.c \
			view_menu.c \
			view_utils.c \
			view2DPaintingCb.c \
			viewFixedPointUtils.c
endif

# List of all header files that are available outside of either this archive
# or these executables (modify as required).
INCLUDES_PUB		= 

# List of all header files that are required by this archive or these
# executables (modify as required).
INCLUDES_PRV		= \
			MAPaint.h \
			MAPaintTypes.h \
			MAPaintProto.h \
			MAPaintHelp.h \
			HGU_Tablet.h \
			MAWarp.h

# List of all troff sources for manual pages, these should have the appropriate
# extension (eg cat.1) (modify as required).
MANPAGES		=

# List of all files of the documentation (Usually .tex .ps )
# (modify as required).
DOC_FILES		=

# List of all html directories for interactive help
# (modify as required).
WWW_DIRS		= ../www/paint ../www/paint/paint_ref_html

# List of all header file directories to be searched (modify as required).
# add TEMPLATE_DIRS to it if C++
INCDIRS			= .  \
			../../Core/libAlc \
			../../Core/libbibfile \
			../../Core/libAlg \
			../../Core/libWlz \
			../../Core/libWlzExtFF \
			../../Core/libhguDlpList \
			../../HGUX/libHguX \
			../../HGUX/libHguXm \
			../../HGUX/libhguGL \
			../../External/Tiff/tiff-v3.5.7/libtiff \
			../../External/Jpeg/jpeg-6b \
			  $(HGU_INC_DIR) \
			  $(OPENGL_INC_DIR) \
			  $(X11_INC_DIR) \
			  $(MOTIF_INC_DIR)

# List of library search paths (modify as required).
ifeq ($(UNIXTYPE), IRIX5)
LIBDIRS			= . \
			../../Core/libAlc \
			../../Core/libbibfile \
			../../Core/libAlg \
			../../Core/libWlz \
			../../Core/libWlzExtFF \
			../../Core/libReconstruct \
			../../HGUX/libHguX \
			../../HGUX/libHguXm \
			../../HGUX/libhguGL \
			  $(OPENGL_LIB_DIR) \
			  $(SYSLIB) \
			  $(HGU_LIB_DIR)
else
LIBDIRS			= . \
			  $(SYSLIB) \
			../../Core/libAlc \
			../../Core/libbibfile \
			../../Core/libAlg \
			../../Core/libWlz \
			../../Core/libWlzExtFF \
			../../Core/libhguDlpList \
			../../HGUX/libHguX \
			../../HGUX/libHguXm \
			../../HGUX/libhguGL \
			../../External/Tiff/tiff-v3.5.7/libtiff \
			../../External/Jpeg/jpeg-6b \
			  $(HGU_LIB_DIR) \
			  $(OPENGL_LIB_DIR) \
			  $(X11_LIB_DIR) \
			  $(MOTIF_LIB_DIR)
endif
ifeq ($(UNIXTYPE), DARWIN)
LIBDIRS			= . \
			../../Core/libAlc \
			../../Core/libbibfile \
			../../Core/libAlg \
			../../Core/libWlz \
			../../Core/libWlzExtFF \
			../../Core/libhguDlpList \
			../../HGUX/libHguX \
			../../HGUX/libHguXm \
			../../HGUX/libhguGL \
			../../External/Tiff/tiff-v3.5.7/libtiff \
			../../External/Jpeg/jpeg-6b \
			  $(HGU_LIB_DIR) \
			  $(MOTIF_LIB_DIR) \
			  $(OPENGL_LIB_DIR) \
			  $(X11_LIB_DIR)
endif
ifeq ($(UNIXTYPE), IRIX646)
LIBDIRS			=  \
			../../Core/libAlc \
			../../Core/libbibfile \
			../../Core/libAlg \
			../../Core/libWlz \
			../../Core/libWlzExtFF \
			../../Core/libReconstruct \
			../../HGUX/libHguX \
			../../HGUX/libHguXm \
			../../HGUX/libhguGL \
			  $(OPENGL_LIB_DIR) \
			  $(SYSLIB) \
			  $(HGU_LIB_DIR)
endif
ifeq ($(UNIXTYPE), IRIX6)
LIBDIRS			=  \
			../../Core/libAlc \
			../../Core/libbibfile \
			../../Core/libAlg \
			../../Core/libWlz \
			../../Core/libWlzExtFF \
			../../Core/libReconstruct \
			../../HGUX/libHguX \
			../../HGUX/libHguXm \
			../../HGUX/libhguGL \
			  $(OPENGL_LIB_DIR) \
			  $(SYSLIB) \
			  $(HGU_LIB_DIR)
endif

# List of all sub directories that contain makefiles. The subsystems will be
# built in the order in which they are given here (modify as required).
SUBSYSTEMS		= 

# Name of distribution tar file (modify as required).

# List of files (should be top level directories) to be put used in creating
# a compressed tar file for distribution (modify as required).
TARSOURCES		= 

# List of all header files, ie the union of the private and public header
# files (should not need modifying).
INCLUDES_ALL		= $(sort $(INCLUDES_PUB) $(INCLUDES_PRV))

# List of all object files required for archive (should not need modifying).
# C++ edit the next line
OBJECTS			= $(CSOURCES:%.c=%.o)

# List of libraries to link (modify as required).
EXTRA_LIBS		= tiff jpeg m gen
X11LIBS         	= Xt Xmu X11 Xext
ifeq 		($(UNIXTYPE), SUNOS5)
EXTRA_LIBS		= tiff jpeg m gen socket nsl
X11LIBS         	= Xt Xmu X11  Xi Xext
endif
ifeq	 	($(UNIXTYPE), LINUX2)
EXTRA_LIBS		= tiff jpeg m SM ICE dl pthread
X11LIBS         	= Xt Xmu Xp X11 Xext
endif
ifeq	 	($(UNIXTYPE), DARWIN)
EXTRA_LIBS		= tiff jpeg m
X11LIBS         	= Xt X11 Xmu Xp
endif

OPENGLLIBS		= GLU GL
MOTIFLIBS       	= Xm
LOCALLIBS		= hguGL HguXm HguX WlzExtFF Wlz bibfile Alg Alc
LIBRARIES		= hguGL HguXm HguX WlzExtFF Wlz bibfile Alg Alc \
			$(OPENGLLIBS) $(MOTIFLIBS) $(X11LIBS) $(EXTRA_LIBS)

# Basic flags for controlling compilation (modify as required).
#CDEBUG			= -g
#COPTIMISE		=

DEFINES			= -D$(UNIXTYPE) $(UNIXFLAGS) -D__EXTENSIONS__
CPPFLAGS		= $(INCDIRS:%=-I%) $(DEFINES)
CFLAGS			= $(CDEBUG) $(COPTIMISE) $(ANSI_CONFORMANCE)

ifeq 		($(UNIXTYPE), SUNOS5)
#CDEBUG			= -g
#COPTIMISE		= -xcg92
endif


ifeq 		($(UNIXTYPE), SUNOS5)
LDFLAGS			= $(LIBDIRS:%=-L%) -R$(SYSLIB):$(HGU_LIB_DIR):$(OPENGL_LIB_DIR):$(X11_LIB_DIR):$(MOTIF_LIB_DIR)
endif

ifeq	 	($(UNIXTYPE), LINUX2)
LDFLAGS			= $(LIBDIRS:%=-L%)
endif

LDLIBS			= $(LOCALLIBS:%=-l%) $(OPENGLLIBS:%=-l%) \
			$(MOTIFLIBS:%=-l%) \
			$(X11LIBS:%=-l%) $(EXTRA_LIBS:%=-l%)
LDLIBS_PRE		= $(LOCALLIBS:%=-l%)
LDLIBS_MESA		= -static $(OPENGLLIBS:%=-l%)
LDLIBS_POST		= $(MOTIFLIBS:%=-l%) $(X11LIBS:%=-l%) $(EXTRA_LIBS:%=-l%)

ifeq 		($(UNIXTYPE), SUNOS5)
LDLIBS_MESA		= -Bstatic $(OPENGLLIBS:%=-l%) -Bdynamic
endif
ifeq 		($(UNIXTYPE), IRIX5)
LDLIBS_MESA		= /opt/Mesa/lib/libGLU.a /opt/Mesa/lib/libGL.a
endif

# Default traget which builds everything (should not need modifying).
all:			archive executables includes manpages 
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif

# this fakes a link using the library. CommonTypes.cc defines the templates
# to be instanciated. Then the instanciations found in ptrepository are
# collected and added to the library.
sun-instanciate-templates : 
		@echo "Now instanciating the templates for Sun CC v3"
		$(CXX) -c -o CommonTypes.o $(CPPFLAGS) $(CXXFLAGS) CommonTypes.cc
		-$(CXX) -pta -o dummy $(LIBDIRS:%=-L%) $(CPPFLAGS) \
$(CXXFLAGS) CommonTypes.o ./$(ARCHIVE) $(REQUIRED_LIBS)
		$(RM) dummy CommonTypes.o
		$(AR) rv  ./$(ARCHIVE) ptrepository/*.o
		@echo "Instanciation finished"
		true

# Target which installs everything after it has been built (should not need
# modifying).
install:		install_includes install_archive \
			install_executables install_release

# Target which installs an archive (should not need modifying).
install_archive:	$(ARCHIVE)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_archive)

# Target which installs executables (should not need modifying).
install_executables:	$(EXECUTABLES) $(SCRIPTS)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_executables)

# Target which installs release executables (should not need modifying).
install_release:	$(EXECUTABLES) $(SCRIPTS)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_release)


# Target which installs the public 'C' header files (should not need 
# modifying).
install_includes:	$(INCLUDES_PUB)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_includes)

# Target which installs the www files (should not need modifying)
install_www:		$(WWW_DIRS)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
ifneq ($(strip $(WWW_DIRS)),)
			$(install_www)
endif

# Target which installs the doc files (should not need modifying)
# modifying).
install_doc:	$(DOC_FILES)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_doc)

# Target which installs the manual pages in the appropriate directories,
# watch out for the tabs in the sed regexp's (should not need modifying).
install_manpages:	$(MANPAGES)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_manpages)

# Target for compressed tar files suitable for distribution (should not need
# modifying).
tar:			$(TARSOURCES) clobber allsources
ifneq ($(strip $(TARSOURCES)),)
			$(TAR) $(TARFLAGS) $(TARFILE) $(TARSOURCES)
			$(COMPRESS) $(TARFILE)
endif

# Target for the archive (library), just a dependency (should not need
# modifying).
archive:		$(ARCHIVE)

# Target for executables, just a dependency (should not need modifying).
executables:		$(EXECUTABLES)

# Target for public header files, just a dependency (should not need modifying).
includes:		$(INCLUDES_PUB)

# Target for manual pages, just a dependency (should not need modifying).
manpages:		$(MANPAGES)

# Target for lint (modify as required).
lint:			$(CSOURCES) $(INCLUDES_PRV)
			$(LINT) $(LINTFLAGS) $(CPPFLAGS) $(CSOURCES)

# Target to clean up after making (should not need modifying).
clean:
			-$(RM) core $(OBJECTS) $(EXECUTABLES) $(ARCHIVE)

# Target to clean up everything that might not be wanted (should not need
# modifying).
clobber:		clean
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			-$(RM) *.o *.a .pure *.pure *.bak *~

# making the tag file
tags:
	$(ETAGS) -f $(ETAGSFILE) $(ETAGSFLAGS) $(CSOURCES) $(CCSOURCES) $(INCLUDES_ALL)

# Target to create editable sources from SCCS (should not need modifying).
edit:
			$(SCCS) edit $(CSOURCES) $(INCLUDES_ALL) Makefile

# Target to create read only sources from SCCS (should not need modifying).
sources:
			-$(SCCS) get $(CSOURCES) $(INCLUDES_ALL) Makefile

allsources:		sources
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif

# Targets to create archives, dependencies and rules (modify as required).
# C++ uncommment the (INSTANCIATE_TEMPLATES) macro

ifneq ($(strip $(ARCHIVE)),)
$(ARCHIVE):		$(OBJECTS)
			$(AR) r $(ARCHIVE) $?
#			-$(INSTANCIATE_TEMPLATES)
			$(RANLIB) $(ARCHIVE)
endif


# Rules for linking (modify as required).
# C++ version for OSCC programs (modify as required)
#compiling the schema (should not need modifying)
#schema_standin: 	schema.cc
#			OSCC $(CPPFLAGS) $(CXXFLAGS) \
#        	           -batch_schema $(OS_COMPILATION_SCHEMA_DB_PATH) schema.cc
#			touch schema_standin

#compilation rule for Object Store programs
#progname:		$(OBJECTS) schema_standin
#			$(OS_PRELINK) .os_schema.cc\
#		        $(OS_COMPILATION_SCHEMA_DB_PATH) \
#        		$(OS_APPLICATION_SCHEMA_DB_PATH) \
#			$(OBJECTS) $(CXXLIBS)
#			OSCC -c .os_schema.cc
#			$(CXX) -o $@ $(OBJECTS) .os_schema.o $(CXXFLAGS) $(LDFLAGS) $(CXXLIBS)
#			$(OS_POSTLINK) $@

$(OBJECTS):		%.o: %.c $(INCLUDES_PRV)

# Targets to create executables, just a dependencies (modify as required).
MAPaint.o:		MAPaint.c MAPaintResources.h MAPaintHelpResource.h $(INCLUDES_ALL)
MAPaint:		$(OBJECTS)
MAPaint.mesa:		$(OBJECTS)

# Target for Purified executables (modify as required).
purify:			MAPaint.pure
MAPaint.pure:		$(OBJECTS)
