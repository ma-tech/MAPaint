#!/bin/sh
# This script will configure and build the MAPaint application. Uncomment
# the appropriate configure command lines for the build you want. The
# easiest way to use this script is probably to copy it to mybuild.sh and
# the edit that script.

set -x
# In most cases a simple autoreconf should be sufficient
autoreconf
# If you hit problems with missing files or libtool use the following
# autoreconf
# autoreconf -i --force

#export MA=$HOME
#export MA=$HOME/MouseAtlas/Build/
export MA=/opt/MouseAtlas

# Configure
#./configure --with-pic --enable-optimise --enable-openmp \
#            --with-woolz=$MA --with-nifti=$MA --with-tiff=$MA --with-jpeg=$MA \
#            --with-xbae=$MA --with-hgux=$MA --with-cjson=$MA

./configure --with-pic --enable-optimise --enable-openmp \
	    --enable-avx2 --enable-lto \
            --with-static-woolz=$MA --with-static-nifti=$MA \
	    --with-static-tiff=$MA --with-static-jpeg=$MA \
	    --with-static-xbae=$MA --with-static-hgux=$MA \
	    --with-static-cjson=$MA

# Make and install
make
make install
