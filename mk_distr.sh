
#!/bin/bash

#------------------------------------------------------------
# creates distribution archive of snoopy
#------------------------------------------------------------
# script by alextov


PLATFORM=`uname -s`
DATE=`date +"%Y-%m-%d"`


#------------------------------------------------------------
# if def_ values are changed, then say_help() must be updated
VERBOSE=
DEF_VERBOSE="yes"

DEBUG=
DEF_DEBUG="no"
#------------------------------------------------------------
# variables must be set below
#
# output dir
OUTDIR=

# temporary distribution dir
DISTRTMP=/tmp/snoopy2

# subdirectories to be created
SUBDIRS=

# list of files to be copied, in form source==>dest
#FILES=
declare -a FILES
FILES_SIZE=0

# archive program command
ARCCMD=

# archive name
ARCHNAME=

# link to be created
LATESTLINK=
#------------------------------------------------------------
say_help()
{
    echo 'mk_distr.sh (make snoopy distribution archive)'
    echo "usage: mk.sh"
    echo "      [-h    --help]"
    echo ""
    echo "      [-v    --verbose (default)]"
    echo "      [-q    --quiet]"
    echo ""
    echo "      [-d    --debug]"
    echo "      [-n    --no-debug (default)]"
    exit 1
}

say_not_tested()
{
    echo "not tested yet, sorry"
    exit 1
}


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
	-v|--verbose)
	    if [ ! -z $VERBOSE ] ; then say_help ; fi
	    VERBOSE="yes" ;;
	-q|--quiet)
	    if [ ! -z $VERBOSE ] ; then say_help ; fi
	    VERBOSE="no" ;;
	*) say_help ;;
    esac
done

# seting default values if needed
if [ -z $DEBUG ] ; then DEBUG=$DEF_DEBUG ; fi
if [ -z $VERBOSE ] ; then VERBOSE=$DEF_VERBOSE ; fi

SNOOPYDIR="$HOME/workspace/snoopy"
BUILDDIR="Release-Linux-64"
MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
  # 64-bit stuff here
  MACHINE_TYPE="-64"
else
  # 32-bit stuff here
  MACHINE_TYPE=''
fi

if [ "$PLATFORM" = "Linux" ] ; then
    OUTDIR="$HOME"
    SUBDIRS="bin lib share"
    ARCHCMD="tar -czvf "

    WXLIBDIR="/usr/local/lib"

    if [ "$DEBUG" = "yes" ] ; then
      #BUILDDIR="Debug-Linux${MACHINE_TYPE}"
      ARCHNAME="snoopy-beta-linux${MACHINE_TYPE}-$DATE-debug.tgz"
	    LATESTLINK="snoopy-beta-linux${MACHINE_TYPE}-latest-debug.tgz"
	    FILES[$FILES_SIZE]="${SNOOPYDIR}/${BUILDDIR}/snoopy==>bin"
            ((FILES_SIZE++))
    else
      #BUILDDIR="Release-Linux${MACHINE_TYPE}"
	    ARCHNAME="snoopy-beta-linux${MACHINE_TYPE}-$DATE.tgz"
	    LATESTLINK="snoopy-beta-linux${MACHINE_TYPE}-latest.tgz"
	    FILES[$FILES_SIZE]="$SNOOPYDIR/$BUILDDIR/snoopy==>bin"
            ((FILES_SIZE++))
    fi
    FILES[$FILES_SIZE]="$SNOOPYDIR/distr/snoopy2-lnx.sh==>bin/snoopy2.sh"
    ((FILES_SIZE++))
    FILES[$FILES_SIZE]="$SNOOPYDIR/help.zip==>share"
    ((FILES_SIZE++))

    while read -d $'\n' f ; do
      if [[ "${f}" == *"/usr/local/"* ]] || [[ "${f}" == *"/home/"* ]] ; then
        fullfile="${f% (*}"
        fullfile="${fullfile##* => }"
        if [[ -e "${fullfile}" ]] ; then
          FILES[$FILES_SIZE]="${fullfile}==>lib"
          ((FILES_SIZE++))
        fi
      fi
    done < <(ldd "${SNOOPYDIR}"/"${BUILDDIR}"/snoopy)

echo "${FILES[*]}"

else
    echo "platform $PLATFORM is not supported yet"
    say_not_tested
fi

if [ "$VERBOSE" = "yes" ] ; then
    echo
    echo "VERBOSE=$VERBOSE"
    echo "PLATFORM=$PLATFORM"
    echo "DEBUG=$DEBUG"
    echo "OUTDIR=$OUTDIR"
    echo "DISTRTMP=$DISTRTMP"
    echo "ARCHNAME=$ARCHNAME"
    echo "LATESTLINK=$LATESTLINK"
fi

if [ ! -d "$OUTDIR" ] ; then
    echo "Output directory $OUTDIR is not available"
    exit 1
fi

if [ ! -d  "$DISTRTMP" ] ; then
    mkdir "$DISTRTMP"
    if [ "$VERBOSE" = "yes" ] ; then echo "created dir $DISTRTMP" ; fi
else
    if [ "$VERBOSE" = "yes" ] ; then echo "rm -rf $DISTRTMP/*" ; fi
    rm -rf $DISTRTMP/*
fi

for i in $SUBDIRS ; do
    if [ "$VERBOSE" = "yes" ] ; then echo mkdir "$DISTRTMP/$i" ; fi
    mkdir "$DISTRTMP/$i"
done

for (( i=0; i<${FILES_SIZE}; i++)) ; do
    f=${FILES[$i]}
    SRC="${f/==>*/}"
    DST="${f/*==>/}"
    if [ "$VERBOSE" = "yes" ] ; then echo cp -a "$SRC" "$DISTRTMP/$DST" ; fi
    if [ "$DST" = "lib" ] ; then
      cp -a "$SRC"* "$DISTRTMP/$DST"
    else
      cp -a "$SRC" "$DISTRTMP/$DST"
    fi
done

DIR_ABOVE_DISTR=${DISTRTMP%\/*}
DISTR=${DISTRTMP/*\//}

if [ "$VERBOSE" = "yes" ] ; then
    echo "cd $DIR_ABOVE_DISTR"
    echo "$ARCHCMD" "$OUTDIR/$ARCHNAME" "$DISTR"
    echo chmod 664 "$OUTDIR/$ARCHNAME"
    echo rm "$OUTDIR/$LATESTLINK"
fi

cd "$DIR_ABOVE_DISTR"
$ARCHCMD "$OUTDIR/$ARCHNAME" "$DISTR"
chmod 664 "$OUTDIR/$ARCHNAME"
rm "$OUTDIR/$LATESTLINK"

# on cygwin windows shortcut is created with ln -s
# instead of a soft link, so just copy the file
if [ "$PLATFORM" = "CYGWIN_NT-5.1" ] ; then
    if [ "$VERBOSE" = "yes" ] ; then
	echo "cp $ARCHNAME $OUTDIR/$LATESTLINK"
	echo "chmod 664 $OUTDIR/$LATESTLINK"
    fi
    cp "$OUTDIR/$ARCHNAME" "$OUTDIR/$LATESTLINK"
    chmod 664 "$OUTDIR/$LATESTLINK"
else
    if [ "$VERBOSE" = "yes" ] ; then
	echo ln -s "$ARCHNAME" "$OUTDIR/$LATESTLINK"
    fi
   ln -s "$ARCHNAME" "$OUTDIR/$LATESTLINK"
fi
