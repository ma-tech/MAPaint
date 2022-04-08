##
# \file         configure.in
# \author       Bill Hill
# \date         September 2007
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
# \brief        Process this file with autoconf to produce a configure script.
##

m4_define([MAPAINT_VERSION], [1.1.1])

AC_INIT(HGUX, HGUX_VERSION, WOOLZ-DEV@hgu.mrc.ac.uk)
AC_CANONICAL_TARGET

AC_GNU_SOURCE

AM_INIT_AUTOMAKE

AC_PROG_CC

AC_CONFIG_SRCDIR([MAPaint.c])

# Set defaults
AC_PREFIX_DEFAULT(/opt/MouseAtlas)
AC_DISABLE_SHARED
SIMD=''
CDEF_ARCH=""
USE_CTAGS=1
USE_ETAGS=0
HAVE_HGX="no"
HGUX_DIR="no"
INCL_HGX=""
LIBS_HGX=""
HAVE_WLZ="no"
WOOLZ_DIR="no"
INCL_WLZ=""
LIBS_WLZ=""
HAVE_XBA="no"
XBAE_DIR="no"
INCL_XBA=""
LIBS_XBA=""
HAVE_MOT="no"
MOTIF_DIR="no"
INCL_MOT=""
LIBS_MOT=""
HAVE_GL="no"
OPENGL_DIR="no"
INCL_GL=""
LIBS_GL=""
HAVE_NIF="no"
NIFTI_DIR="no"
CJSON_DIR="no"
INCL_NIF=""
LIBS_NIF=""
HAVE_JPG="no"
JPEG_DIR="no"
INCL_JPG=""
LIBS_JPG=""
HAVE_TIF="no"
TIFF_DIR="no"
INCL_TIF=""
LIBS_TIF=""
HAVE_BZLIB="no"
HAVE_ZLIB="no"

# Check CPU type
echo "Checking target cpu/system type"
case ${target_cpu} in
  i386|i486|i586|i686)
    CDEF_ARCH="-D__x86"
    ;;
  x86_64|amd64)
    CDEF_ARCH="-D__x86"
    ;;
  ia64)
    CDEF_ARCH="-D__x86"
    ;;
  alpha)
    CDEF_ARCH="-D__alpha"
    ;;
  sparc|sparc64)
    CDEF_ARCH="-D__sparc"
    ;;
  ppc|powerpc)
    CDEF_ARCH="-D__ppc"
    ;;
  mips)
    CDEF_ARCH="-D__mpis"
    ;;
  *)
    echo "Warning: CPU type ${target_cpu} unknown."
    ;;
esac

# Check for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
LT_INIT
AC_PROG_MKDIR_P

# Check for system functions
AC_FUNC_MMAP

# Check for libraries.
AC_CHECK_LIB(m, pow)

# Check for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([	assert.h \
			ctype.h \
			errno.h \
			float.h \
			limits.h \
			stddef.h \
			stdlib.h \
			string.h \
			strings.h \
                  	sys/time.h \
                  	sys/types.h \
			time.h \
			unistd.h])

# Check for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Check for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([	floor \
			getcwd \
			gethostname \
			gettimeofday \
			memset \
			mkdir \
			modf \
			pow \
			regcmp \
			regcomp \
			sqrt \
			strcasecmp \
			strchr \
			strerror \
			strrchr \
			strstr])

# Check for doxygen
AC_PATH_PROG(DOXYGEN, doxygen)
AC_CHECK_PROG(HAVE_DOXYGEN, doxygen, "yes", "no")
AM_CONDITIONAL(HAVE_DOXYGEN, test x$HAVE_DOXYGEN = xyes)

# Compilation and linking flags
CFLAGS=""

AC_PROG_CC

AC_ARG_ENABLE(icc,
  [  --enable-icc            build using the Intel C compiler])
AS_IF([test x"$enable_icc" = "xyes"], [
  CFLAGS="${CFLAGS} -w2"
  CC="icc"
  CXX="icpc"
  LD="xild"
  AR="xiar"
], [
  CFLAGS="${CFLAGS} -pipe -Wall -Wno-deprecated"
  CC="gcc"
  CXX="g++"
])

CFLAGS="${CFLAGS} ${CDEF_ARCH}"

# Set target architecture flags
if test "$CC" = "gcc"
then
  case ${target_cpu} in
    i386|i486|i586)
      CFLAGS="${CFLAGS} -m32 -march=i586"
      ;;
    i686)
      CFLAGS="${CFLAGS} -m32 -march=i686"
      ;;
    x86_64|amd64)
      CFLAGS="${CFLAGS} -m64"
      ;;
    *)
      ;;
  esac
else
  #Intel CC
  if test "$CC" = "icc"
  then
    case ${target_cpu} in
      i386|i486|i586)
        CFLAGS="${CFLAGS} -m32 -xK"
        ;;
      i686)
        CFLAGS="${CFLAGS} -xN"
        ;;
      x86_64|amd64)
        CFLAGS="${CFLAGS} -m64"
        ;;
      *)
        ;;
    esac
  fi
fi

CPP="cpp"

AC_ARG_ENABLE(debug,
  [  --enable-debug	build with debugging flags set],
    CFLAGS="${CFLAGS} -g -DDEBUG"
    LDFLAGS="${LDFLAGS} -g -DDEBUG")

AC_ARG_ENABLE(optimise,
  [  --enable-optimise	   build with optimisation flags set],
  [
    # GNU CC
    if test "$CC" = "gcc"
    then
      case ${target_cpu} in
	i386|i486|i586|i686)
	  CFLAGS="${CFLAGS} -fPIC -O2 -fomit-frame-pointer"
	  ;;
	x86_64|amd64)
	  if test "x$SIMD" = "x"
	  then
	    SIMD="-msse3"
	  fi
          CFLAGS="${CFLAGS} -fPIC -O2"
	  ;;
        *)
          CFLAGS="${CFLAGS} -fPIC -O2"
	  ;;
      esac
    else
      #Intel CC
      if test "$CC" = "icc"
      then
	CFLAGS="${CFLAGS} -O3 -ip"
      else
	# AIX CC
	if test "`uname`" = "AIX"
	then
	  CFLAGS="${CFLAGS} -O3"
	fi
	# SGI CC
	if test "`uname`" = "IRIX" || test "`uname`" = "IRIX64"
	then
	  CFLAGS="${CFLAGS} -Ofast"
	  LDFLAGS="${LDFLAGS} -IPA"
	fi
	# HP-UX CC
	if test "`uname`" = "HP-UX"
	then
	  CFLAGS="${CFLAGS} -fast"
	fi
	# Sun CC
	if test "`uname`" = "SunOS"
	then
	  CFLAGS="${CFLAGS} -fast"
	fi
      fi
    fi
    ])

AC_ARG_ENABLE(avx2,
  [AC_HELP_STRING([--enable-avx2],[enable AVX2 SIMD optimizations])],
  SIMD='-mavx2')

AC_ARG_ENABLE(openmp,
  [  --enable-openmp    build with openmp flags set],
    # GNU CC
    if test "$CC" = "gcc"
    then
      CFLAGS="${CFLAGS} -fopenmp"
    fi
    # Intel CC
    if test "$CC" = "icc"
    then
      CFLAGS="${CFLAGS} -openmp"
    fi)

AC_ARG_ENABLE(lto,
  [  --enable-lto	          build with link time optimisation flags set])
AS_IF([test "x$enable_lto" = "xyes"], [
    # GNU CC
    if test "$CC" = "gcc"
    then
      CFLAGS="${CFLAGS} -flto"
    fi
    # Intel CC
    if test "$CC" = "icc"
    then
      CFLAGS="${CFLAGS} -ipo"
    fi
])

AC_ARG_ENABLE(profile,
  [  --enable-profile	  build with profiling set],
    CFLAGS="${CFLAGS} -pg"
    LDFLAGS="${LDFLAGS} -pg")

if test ${USE_CTAGS} = 1
then
  AC_CHECK_PROG(GNC_CTAGS_FILE, ctags, tags)
fi

dnl Check for zlib and bzlib
AS_IF([test x"$enable_bzip2" != x"no"], [
  AC_SEARCH_LIBS([BZ2_bzCompress], [bz2], [HAVE_BZLIB=yes])
])
AS_IF([test x"$enable_zlib" != x"no"], [
  AC_SEARCH_LIBS([zlibVersion], [z], [HAVE_ZLIB=yes])
])
if test ${HAVE_BZLIB} = "no"
then
  CFLAGS="${CFLAGS} -DHAVE_BZLIB=0"
else
  CFLAGS="${CFLAGS} -DHAVE_BZLIB=1"
fi
if test ${HAVE_ZLIB} = "no"
then
  CFLAGS="${CFLAGS} -DHAVE_ZLIB=0"
else
  CFLAGS="${CFLAGS} -DHAVE_ZLIB=1"
fi

dnl Check for HGUX
AC_ARG_WITH(static-hgux,
            [ --with-static-hgux=HGUX_DIR     Directory containing the HGU X
                          include and lib directories.],
            HGUX_DIR=$withval, HGUX_DIR=no)
if test ${HGUX_DIR} = "no"
then
AC_ARG_WITH(hgux, [ --with-hgux=HGUX_DIR     Directory containing the HGU X
                          include and lib directories.],
            HGUX_DIR=$withval, HGUX_DIR=no)
  if test ${HGUX_DIR} = "no"
  then
    AC_CHECK_HEADER(HGUgl.h,
		    AC_CHECK_LIB(hguGL, HGUglTextFontChar,
				 [
				   HAVE_HGX="yes"
				   LIBS_HGX="-lHguXm -lHguX -lhguGL"
				 ]))
  else
    HAVE_HGX="yes"
    INCL_HGX="-I${HGUX_DIR}/include"
    LIBS_HGX="-L${HGUX_DIR}/lib -lHguXm -lHguX -lhguGL"
  fi
else
  HAVE_HGX="yes"
  INCL_HGX="-I${HGUX_DIR}/include"
  LIBS_HGX="${HGUX_DIR}/lib/libHguXm.a ${HGUX_DIR}/lib/libHguX.a"
  LIBS_HGX="${LIBS_HGX} ${HGUX_DIR}/lib/libhguGL.a"
fi
if test ${HAVE_HGX} = "yes"
then
  CFLAGS="$CFLAGS -DHAVE_HGX=1 $INCL_HGX"
  LDFLAGS="$LDFLAGS $LIBS_HGX"
else
  CFLAGS="$CFLAGS -DHAVE_HGX=0"
fi

dnl Check for Woolz
AC_ARG_WITH(static-woolz,
            [ --with-static-woolz=WOOLZ_DIR   Directory containing the Woolz
                          include and lib directories.],
            WOOLZ_DIR=$withval, WOOLZ_DIR=no)
if test ${WOOLZ_DIR} = "no"
then
  AC_ARG_WITH(woolz,
              [ --with-woolz=WOOLZ_DIR   Directory containing the Woolz
			    include and lib directories.],
	      WOOLZ_DIR=$withval, WOOLZ_DIR=no)
  if test ${WOOLZ_DIR} = "no"
  then
    AC_CHECK_HEADER(WlzExtFF.h,
		    AC_CHECK_LIB(WlzExtFF, WlzEffNumberOfFormats,
				 [
		 HAVE_WLZ="yes"
		 LIBS_WLZ="-lWlzExtFF -lbibfile -lhguDlpList -lWlz -lAlg -lAlc"
	       ]))
  else
    HAVE_WLZ="yes"
    INCL_WLZ="-I${WOOLZ_DIR}/include"
    LIBS_WLZ="-L${WOOLZ_DIR}/lib -lWlzExtFF -lbibfile -lhguDlpList"
    LIBS_WLZ="${LIBS_WLZ} -lWlz -lAlg -lAlc"
  fi
else
  HAVE_WLZ="yes"
  INCL_WLZ="-I${WOOLZ_DIR}/include"
  LIBS_WLZ="${WOOLZ_DIR}/lib/libWlzExtFF.a ${WOOLZ_DIR}/lib/libbibfile.a"
  LIBS_WLZ="${LIBS_WLZ} ${WOOLZ_DIR}/lib/libhguDlpList.a"
  LIBS_WLZ="${LIBS_WLZ} ${WOOLZ_DIR}/lib/libWlz.a ${WOOLZ_DIR}/lib/libAlg.a"
  LIBS_WLZ="${LIBS_WLZ} ${WOOLZ_DIR}/lib/libAlc.a"
fi
if test ${HAVE_WLZ} = "yes"
then
  CFLAGS="$CFLAGS -DHAVE_WLZ=1 $INCL_WLZ"
  LDFLAGS="$LDFLAGS $LIBS_WLZ"
else
  CFLAGS="$CFLAGS -DHAVE_WLZ=0"
  AC_MSG_ERROR(Could not find Woolz.);
fi

dnl Check for Motif
AC_ARG_WITH(static-motif,
            [ --with-static-motif=MOTIF_DIR   Directory containing the Motif
                          include and lib directories.],
            MOTIF_DIR=$withval, MOTIF_DIR=no)
if test ${MOTIF_DIR} = "no"
then
  AC_ARG_WITH(motif, [ --with-motif=MOTIF_DIR   Directory containing the Motif
			    include and lib directories.],
	      MOTIF_DIR=$withval, MOTIF_DIR=no)
  if test ${MOTIF_DIR} = "no"
  then
    AC_CHECK_HEADER(Xm/Xm.h,
		    AC_CHECK_LIB(Xm,XmStringGenerate,
				 [
				   HAVE_MOT=yes
				   LIBS_MOT="-lXm"
				 ]))
  else
    HAVE_MOT="yes"
    INCL_MOT="-I${MOT_DIR}/include"
    LIBS_MOT="-L${MOT_DIR}/lib -lXm -lXt -lX11"
  fi
else
  HAVE_MOT="yes"
  INCL_MOT="-I${MOT_DIR}/include"
  LIBS_MOT="${MOT_DIR}/lib/libXm.a"
fi

dnl Check for Motif Xbae widget
AC_ARG_WITH(static-xbae,
            [ --with-static-xbae=XBAE_DIR     Directory containing the Motif
                          Xbae widget include and lib directories.],
            XBAE_DIR=$withval, XBAE_DIR=no)
if test ${XBAE_DIR} = "no"
then
  AC_ARG_WITH(xbae,
              [ --with-xbae=XBAE_DIR     Directory containing the Motif
			    Xbae widget include and lib directories.],
	      XBAE_DIR=$withval, XBAE_DIR=no)
  if test ${XBAE_DIR} = "no"
  then
    AC_CHECK_HEADER(Xbae/Matrix.h,
		    AC_CHECK_LIB(Xbae, XbaeCreateMatrix,
				 [
				   HAVE_MOT=yes
				   LIBS_MOT="-lXbae"
				 ]))
  else
    HAVE_XBA="yes"
    INCL_XBA="-I${XBAE_DIR}/include"
    LIBS_XBA="-L${XBAE_DIR}/lib -lXbae"
  fi
else
  HAVE_XBA="yes"
  INCL_XBA="-I${XBAE_DIR}/include"
  LIBS_XBA="${XBAE_DIR}/lib/libXbae.a"
fi

if test ${HAVE_MOT} = "yes" -a ${HAVE_XBA} = "yes"
then
  CFLAGS="$CFLAGS -DHAVE_XBA=1 $INCL_XBA -DHAVE_MOT=1 $INCL_MOT"
  LDFLAGS="$LDFLAGS $LIBS_XBA $LIBS_MOT"
else
  CFLAGS="$CFLAGS -DHAVE_XBA=0 -DHAVE_MOT=0"
  AC_MSG_ERROR(Could not find both Motif and the Motif Xbae widget.);
fi

dnl Check for OpenGL including the possibility of static Mesa
AC_ARG_WITH(static-opengl,
            [ --with-static-opengl=OPENGL_DIR   Directory containing the Mesa
                          OpenGL include and lib directories.],
            OPENGL_DIR=$withval, OPENGL_DIR=no)
if test ${OPENGL_DIR} = "no"
then
  AC_ARG_WITH(opengl,
              [ --with-opengl=OPENGL_DIR     Directory containing the OpenGL
			    include and lib directories.],
	      OPENGL_DIR=$withval, OPENGL_DIR=no)
  if test ${OPENGL_DIR} = "no"
  then
    AC_CHECK_HEADER(GL/gl.h, [
			       HAVE_GL=yes
			       LIBS_GL="-dynamic -lGLU -lGL -static"
			     ])
  else
    HAVE_GL="yes"
    INCL_GL="-I${OPENGL_DIR}/include"
    LIBS_GL="-L${OPENGL_DIR}/lib -dynamic -lGLU -lGL -static"
  fi
else
  HAVE_GL="yes"
  INCL_GL="-I${OPENGL_DIR}/include"
  LIBS_GL="${OPENGL_DIR}/lib/libGLU.a ${OPENGL_DIR}/lib/libGL.a"
fi

if test ${HAVE_GL} = "yes"
then
  CFLAGS="$CFLAGS -DHAVE_GL=1 $INCL_GL"
  LDFLAGS="$LDFLAGS $LIBS_GL"
else
  CFLAGS="$CFLAGS -DHAVE_GL=0"
  AC_MSG_ERROR(Could not find OpenGL.);
fi

dnl Check for NIFTI
AC_ARG_WITH(static-nifti,
            [ --with-static-nifti=NIFTI_DIR   Directory containing the NIFTI
                          include and lib directories.],
            NIFTI_DIR=$withval, NIFTI_DIR=no)
if test ${NIFTI_DIR} = "no"
then
  AC_ARG_WITH(nifti,
              [ --with-nifti=NIFTI_DIR   Directory containing the NIFTI
			    include and lib directories.],
	      NIFTI_DIR=$withval, NIFTI_DIR=no)
  if test ${NIFTI_DIR} = "no"
  then
    HAVE_NIF=no
    CFLAGS="${CFLAGS} -DHAVE_NIFTI=0"
  else
    HAVE_NIF="yes"
    INCL_NIF="-I${NIFTI_DIR}/include"
    LIBS_NIF="-L${NIFTI_DIR}/lib -lniftiio -lznz"
  fi
else
  HAVE_NIF="yes"
  INCL_NIF="-I${NIFTI_DIR}/include"
  LIBS_NIF="${NIFTI_DIR}/lib/libniftiio.a ${NIFTI_DIR}/lib/libznz.a"
fi
if test ${HAVE_NIF} = "yes"
then
  CFLAGS="$CFLAGS -DHAVE_NIFTI=1 $INCL_NIF"
  LDFLAGS="$LDFLAGS $LIBS_NIF"
else
  CFLAGS="$CFLAGS -DHAVE_NIFTI=0"
  AC_MSG_ERROR(Could not find NIfTI.);
fi

dnl Check for TIFF
AC_ARG_WITH(static-tiff,
            [ --with-static-tiff=TIFF_DIR     Directory containing the Tiff
                          include and lib directories.],
            TIFF_DIR=$withval, TIFF_DIR=no)
if test ${TIFF_DIR} = "no"
then
  AC_ARG_WITH(tiff,
              [ --with-tiff=TIFF_DIR     Directory containing the Tiff
			    include and lib directories.],
	      TIFF_DIR=$withval, TIFF_DIR=no)
  if test ${TIFF_DIR} = "no"
  then
    AC_CHECK_HEADER(tiff.h, [
			      HAVE_TIF=yes
			      LIBS_TIF="-ltiff -llzma"
			    ])
  else
    HAVE_TIF="yes"
    INCL_TIF="-I${TIFF_DIR}/include"
    LIBS_TIF="-L${TIFF_DIR}/lib -ltiff -llzma"
  fi
else
  HAVE_TIF="yes"
  INCL_TIF="-I${TIFF_DIR}/include"
  LIBS_TIF="${TIFF_DIR}/lib/libtiff.a -llzma"
fi
if test ${HAVE_TIF} = "yes"
then
  CFLAGS="$CFLAGS -DHAVE_TIFF=1 $INCL_TIF"
  LDFLAGS="$LDFLAGS $LIBS_TIF"
else
  CFLAGS="$CFLAGS -DHAVE_TIFF=0"
  AC_MSG_ERROR(Could not find Tiff);
fi

dnl Check for Jpeg
AC_ARG_WITH(static-jpeg,
            [ --with-static-jpeg=JPEG_DIR     Directory containing the JPEG
                          include and lib directories.],
            JPEG_DIR=$withval, JPEG_DIR=no)
if test ${JPEG_DIR} = "no"
then
  AC_ARG_WITH(jpeg,
              [ --with-jpeg=JPEG_DIR     Directory containing the JPEG
			    include and lib directories.],
	      JPEG_DIR=$withval, JPEG_DIR=no)
  if test ${JPEG_DIR} = "no"
  then
    AC_CHECK_HEADER(jpeglib.h, [
				 HAVE_JPG=yes
				 LIBS_JPG="-ljpeg"
			       ])
  else
    HAVE_JPG=yes
    INCL_JPG="-I${JPEG_DIR}/include"
    LIBS_JPG="-L${JPEG_DIR}/lib -ljpeg"
  fi
else
  HAVE_JPG=yes
  INCL_JPG="-I${JPEG_DIR}/include"
  LIBS_JPG="${JPEG_DIR}/lib/libjpeg.a"
fi
if test ${HAVE_JPG} = "yes"
then
  CFLAGS="$CFLAGS -DHAVE_JPEG=1 $INCL_JPG"
  LDFLAGS="$LDFLAGS $LIBS_JPG"
else
  CFLAGS="$CFLAGS -DHAVE_JPEG=0"
  AC_MSG_ERROR(Could not find Jpeg);
fi

dnl Check for cJSON
AC_ARG_WITH(static-cjson,
            [ --with-static-cjson=CJSON_DIR     Directory containing the CJSON
                          include and lib directories.],
            CJSON_DIR=$withval, CJSON_DIR=no)
if test ${CJSON_DIR} = "no"
then
  AC_ARG_WITH(cjson,
              [ --with-cjson=CJSON_DIR     Directory containing the CJSON
			    include and lib directories.],
	      CJSON_DIR=$withval, CJSON_DIR=no)
  if test ${CJSON_DIR} = "no"
  then
    AC_CHECK_HEADER(cjson.h, [
				 HAVE_CJSON=yes
				 LIBS_CJSON="-lcjson"
			       ])
  else
    HAVE_CJSON=yes
    INCL_CJSON="-I${CJSON_DIR}/include"
    LIBS_CJSON="-L${CJSON_DIR}/lib -lcjson"
  fi
else
  HAVE_CJSON=yes
  INCL_CJSON="-I${CJSON_DIR}/include"
  LIBS_CJSON="${CJSON_DIR}/lib/libcjson.a"
fi
if test ${HAVE_CJSON} = "yes"
then
  CFLAGS="$CFLAGS -DHAVE_CJSON=1 $INCL_CJSON"
  LDFLAGS="$LDFLAGS $LIBS_CJSON"
else
  CFLAGS="$CFLAGS -DHAVE_CJSON=0"
  AC_MSG_ERROR(Could not find cJSON);
fi

AS_IF([ test "$CC" = "gcc" -a "x$enable_optimise" = "xyes" ],
      CFLAGS="$CFLAGS $SIMD")

AC_CONFIG_FILES([	Makefile ])


AC_OUTPUT

AC_MSG_RESULT([
MAPaint has been configured on ${host} with

  Source directory:                 ${srcdir}
  Installation directory:           ${prefix}
  Target CPU:                       ${target_cpu}
  C compiler:                       ${CC}
  Linker:                           ${LD}
  Archiver:                         ${AR}
  CFLAGS:                         ${CFLAGS}
  LDFLAGS:                        ${LDFLAGS}

To build HGUX:                     make
To install HGUX:                   make install
To build HGUX documentation:       make doc
To install HGUX documentation:     make install-doc
])
