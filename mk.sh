#!/bin/bash

#------------------------------------------------------------
#PLEASE USE THIS SCRIPT INSTEAD OF SIMPLY CALLING MAKE!
#------------------------------------------------------------
# script by alextov


PLATFORM=`uname -s`

# build information (date, user@host) is written in this file
HNAME="sp_build.h"

BDATE=`date +"%Y-%m-%d"`
BHOST=`uname -n`
BUSER=`whoami`

#------------------------------------------------------------
# if def_ values are changed, then say_help() must be updated
VERBOSE=
DEF_VERBOSE="yes"

DEBUG=
DEF_DEBUG="yes"

GTK=
DEF_GTK="1"

# wxWindows version, 2.4.2 ("2-4" should be set) or 2.6 ("2-6")
WXVER=
DEF_WXVER="2-4"


#------------------------------------------------------------
# output directory, flags, etc, must be set below
OUT=
WXHOME=
WXLIBS=
LIBS=
LIBPATH=
C_FLAGS=
I_PATH=

# make parameters
MKARG=

#------------------------------------------------------------
say_help()
{
    echo 'mk.sh (make snoopy)'
    echo "usage: mk.sh"
    echo "      [-h    --help]"
    echo ""
    echo "      [-m=<args>  --make_arg=<args>]"
    echo ""
    echo "      [-v    --verbose (default)]"
    echo "      [-q    --quiet]"
    echo ""
    echo "      [-d    --debug (default)]"
    echo "      [-n    --no-debug]"
    echo ""
    echo "      [-1    --gtk1 (default)]"
    echo "      [-2    --gtk2]"
    echo ""
    echo "      [-4    --wx2-4 (default)]"
    echo "      [-6    --wx2-6]"
    exit 1
}

say_not_tested()
{
    echo "not tested yet, sorry"
    exit 1
}


write_build_info()
{

    BDEF=`cat <<-EOF
#ifndef __SP_BUILD_H__
#define __SP_BUILD_H__

#define SP_APP_BUILD "${BUSER}@$BHOST on $BDATE"

#endif // __SP_BUILD_H__
EOF`
    if [ -f "$HNAME" ] ; then
	READ=`cat $HNAME`
	if [ "$BDEF" = "$READ" ] ; then
	    if [ "$VERBOSE" = "yes" ] ; then
		echo "leaving $HNAME unchanged"
	    fi
	    return
	fi
    fi
    if [ "$VERBOSE" = "yes" ] ; then
	echo "writing build info in $HNAME"
    fi
    echo "$BDEF" > $HNAME
}
#------------------------------------------------------------


# main processing starts here with parsing arguments
for i in "$@"; do
    case "$i" in
	-h|--help)
	    say_help ;;
	-d|--debug)
	    if [ ! -z $DEBUG ] ; then say_help ; fi
	    DEBUG="yes" ;;
	-n|--no-debug) 
	    if [ ! -z $DEBUG ] ; then say_help ; fi
	    DEBUG="no" ;;
	-1|--gtk1) 
	    if [ ! -z $GTK ] ; then say_help ; fi
	    GTK=1 ;;
	-2|--gtk2)
	    if [ ! -z $GTK ] ; then say_help ; fi
	    GTK=2 ;;
	-4|--wx2-4) 
	    if [ ! -z $WXVER ] ; then say_help ; fi
	    WXVER="2-4" ;;
	-6|--wx2-6)
	    if [ ! -z $WXVER ] ; then say_help ; fi
	    WXVER="2-6" ;;
	-v|--verbose)
	    if [ ! -z $VERBOSE ] ; then say_help ; fi
	    VERBOSE="yes" ;;
	-q|--quiet)
	    if [ ! -z $VERBOSE ] ; then say_help ; fi
	    VERBOSE="no" ;;
	-m=*|--makearg=*)
	    MKARG=`echo "$i" | sed -e 's/^[^=]*=//'` ;;
	*) say_help ;;
    esac
done

# seting default values if needed
if [ -z $DEBUG ] ; then DEBUG=$DEF_DEBUG ; fi
if [ -z $GTK ] ; then GTK=$DEF_GTK ; fi
if [ -z $WXVER ] ; then WXVER=$DEF_WXVER ; fi
if [ -z $VERBOSE ] ; then VERBOSE=$DEF_VERBOSE ; fi


if [ "$PLATFORM" = "Linux" ] ; then
    LIBS="-lxerces-c -pthread -lgvc"
    GRAPHVIZINC="/usr/local/include/graphviz"
    LIBPATH="-L/usr/local/lib/graphviz"
    if [ "$WXVER" = "2-4" ] ; then
	WXHOME="/home/alextov/job/wxGTK-2.4.2"
	if [ "$GTK" = "1" ] ; then	    	    
	    if [ "$DEBUG" = "yes" ] ; then
		OUT="Debug"
		OFLAGS="-O0 -Wall  -g -fno-inline"

		WXLIBS="-L$WXHOME/build-d/lib -lwx_gtkd-2.4  \
                -lwx_gtkd_ogl-2.4 -lwx_gtkd_fl-2.4"
		
		WXFLAGS="-I$WXHOME/include \
		-I$WXHOME/build-d/lib/wx/include/gtkd-2.4 \
                -I$WXHOME/contrib/include \
                -I$GRAPHVIZINC \
  	        -D__WXDEBUG__ -D__WXGTK__ \
		-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES \
		-fpermissive"
	    else
		OUT="Release"
		OFLAGS="-O2"

		WXLIBS="-L$WXHOME/build/lib -lwx_gtk-2.4  \
                -lwx_gtk_ogl-2.4 -lwx_gtk_fl-2.4"

		WXFLAGS="-I$WXHOME/build/lib/wx/include/gtk-2.4 \
		-I$WXHOME/include \
                -I$WXHOME/contrib/include \
                -I$GRAPHVIZINC \
		-DGTK_NO_CHECK_CASTS -D__WXGTK__ \
		-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES"
	    fi
	else  ## GTK2
	    if [ "$DEBUG" = "yes" ] ; then
		OUT="Debug2"
		OFLAGS="-O0 -Wall  -g -fno-inline"

		WXLIBS="-L$WXHOME/build2-d/lib -lwx_gtk2d-2.4  \
                -lwx_gtk2d_ogl-2.4 -lwx_gtk2d_fl-2.4"
		
		WXFLAGS="-I$WXHOME/include \
		-I$WXHOME/build2-d/lib/wx/include/gtk2d-2.4 \
                -I$WXHOME/contrib/include \
                -I$GRAPHVIZINC \
  	        -D__WXDEBUG__ -D__WXGTK__ \
		-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES"
	    else
		OUT="Release2"
		OFLAGS="-O2"
		
		WXLIBS="-L$WXHOME/build2/lib -lwx_gtk2-2.4  \
                -lwx_gtk2_ogl-2.4 -lwx_gtk2_fl-2.4"

		WXFLAGS="-I$WXHOME/build2/lib/wx/include/gtk2-2.4 \
		-I$WXHOME/include \
                -I$WXHOME/contrib/include \
                -I$GRAPHVIZINC \
		-DGTK_NO_CHECK_CASTS -D__WXGTK__ \
		-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES"
	    fi
	fi
    else # wx 2.6
	WXHOME="/home/alextov/job/wxGTK-2.6.1"
	if [ "$GTK" = "1" ] ; then
    	    if [ "$DEBUG" = "yes" ] ; then
		OUT="Debug-2-6-1"
		OFLAGS="-O0 -Wall -g -fno-inline"

		WXLIBS="-L$WXHOME/build-d1/lib \
		 -lwx_gtkd_core-2.6  -lwx_based-2.6 \
		 -lwx_gtkd_ogl-2.6 -lwx_gtkd_fl-2.6"

		WXFLAGS="-I$WXHOME/build-d1/lib/wx/include/gtk-ansi-debug-2.6 \
		 -I$WXHOME/include -I$WXHOME/contrib/include \
                 -I$GRAPHVIZINC \
		 -D__WXDEBUG__ -D__WXGTK__ \
		 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D_LARGEFILE_SOURCE=1 \
		  -fpermissive"
	    else
		say_not_tested
	    fi
	else  ## GTK2	    
	    if [ "$DEBUG" = "yes" ] ; then
		OUT="Debug-2-6"
		OFLAGS="-O0 -Wall  -g -fno-inline"

		WXLIBS="-L$WXHOME/build-d/lib \
		 -lwx_gtk2d_core-2.6  -lwx_based-2.6 \
		 -lwx_gtk2d_ogl-2.6 -lwx_gtk2d_fl-2.6"

		WXFLAGS="-I$WXHOME/build-d/lib/wx/include/gtk2-ansi-debug-2.6 \
		 -I$WXHOME/include -I$WXHOME/contrib/include \
                 -I$GRAPHVIZINC \
		 -D__WXDEBUG__ -D__WXGTK__ \
		 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D_LARGEFILE_SOURCE=1 \
		  -fpermissive"
	    else
		say_not_tested
	    fi
	fi
    fi
elif [ "$PLATFORM" = "SunOS" ] ; then
    LIBS="-lxerces-c"
    LIBPATH="-L/home/dssztool/xerces-c-2.6.0/lib"
    I_PATH="-I/home/dssztool/xerces-c-2.6.0/include"

    if [ "$WXVER" = "2-4" ] ; then
	if [ "$GTK" = "1" ] ; then  
	    
	    #at the moment there is no debug version of wx on sun

	    WXHOME="/home/dssztool/wxGTK-2.4.2"
	    WXFLAGS="-I$WXHOME/include -I$WXHOME/lib/wx/include/gtk-2.4 \
		    -DGTK_NO_CHECK_CASTS -D__WXGTK__ \
		    -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES"
	    WXLIBS="-L$WXHOME/lib -lwx_gtk-2.4 -l_ogl- -l_fl-"

	    if [ "$DEBUG" = "yes" ] ; then
		OUT="Debug"
		OFLAGS="-O0 -Wall  -g -fno-inline"
	    else
		OUT="Release"
		OFLAGS="-O2"
	    fi
	else  ## GTK2
	    say_not_tested
	fi
    else # wx 2.6
	say_not_tested
    fi
elif [ "$PLATFORM" = "CYGWIN_NT-5.1" ] ; then
    ## cygwin, but we are actually using visual studio makefile and nmake


    # microsoft tools should be prefered, otherwise problems come.
    # for example, link.exe from cygwin is called instead of LINK.EXE from
    # microsoft
    PATH=/cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio/VC98/Bin:$PATH
    export PATH
    if [ "$WXVER" = "2-4" ] ; then
	WXHOME="C:\wx\wxWindows-2.4.2"
	if [ "$DEBUG" = "yes" ] ; then
	    OUT="Debug"
	    CFG="SnoopyVC - Win32 Debug"
	else
	    OUT="Release"
	    CFG="SnoopyVC - Win32 Release"
	fi
    else # wx 2.6
	say_not_tested
    fi


else
    echo "platform $PLATFORM is not supported yet"
    say_not_tested
fi

if [ "$VERBOSE" = "yes" ] ; then
    echo
    echo "VERBOSE=$VERBOSE"
    echo "PLATFORM=$PLATFORM"
    echo "DEBUG=$DEBUG"
    echo "GTK=$GTK"
    echo "WXVER=$WXVER"
    echo "OUT=$OUT"
fi


if [ -z  $OUT ] ; then
     echo "sorry, error in mk.sh"
     exit 1
fi

if [ ! -d  $OUT ] ; then
    mkdir $OUT
    if [ "$VERBOSE" = "yes" ] ; then echo "created dir $OUT" ; fi
fi

write_build_info

if [ "$PLATFORM" = "CYGWIN_NT-5.1" ] ; then
    if [ "$VERBOSE" = "yes" ] ; then
	echo 
	echo "calling nmake:"
	echo nmake -f SnoopyVC.mak $MKARG CFG="$CFG" 
	echo 
    fi
    nmake -f SnoopyVC.mak $MKARG CFG="$CFG" 
else
    C_FLAGS="$WXFLAGS $I_PATH $CXXFLAGS"
    LIBS="$LIBS $WXLIBS"

    if [ "$VERBOSE" = "yes" ] ; then
	echo 
	echo "calling make:"
	echo make LIBPATH="$LIBPATH" LIBS="$LIBS" \
	    CXXFLAGS="$C_FLAGS" OFLAGS="$OFLAGS" \
	    --directory=$OUT --makefile=../makefile.unx $MKARG
	echo 
    fi

    make LIBPATH="$LIBPATH" LIBS="$LIBS" CXXFLAGS="$C_FLAGS" \
    OFLAGS="$OFLAGS" --directory=$OUT --makefile=../makefile.unx $MKARG
fi
