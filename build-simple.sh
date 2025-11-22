#!/bin/sh
set -e
set -x



#SCRIPTDIR=$(dirname $(readlink -f "$0"))

# a safe default
CPU_COUNT_TO_USE_MAKE_PARAM=-j2

case "$(uname -s)" in

   Darwin)
     echo 'Mac OS X'
     readlinkf(){ perl -MCwd -e 'print Cwd::abs_path shift' "$1";}
     SCRIPTDIR=$(dirname $(readlinkf "$0"))
     ;;

   # taken from: https://en.wikipedia.org/wiki/Uname
   Linux|*BSD)
     echo 'Linux'
     SCRIPTDIR=$(dirname $(readlink -f "$0"))
     CPU_COUNT_TO_USE_MAKE_PARAM="-j$( nproc --ignore=2 )"
     ;;

   CYGWIN*|MINGW32*|MSYS*)
     echo 'MS Windows'
     SCRIPTDIR=$(dirname $(readlink -f "$0")) # untested
     ;;

   # Add here more strings to compare
   #

   *)
     echo 'other OS, please notify the developers'
     SCRIPTDIR=$(dirname $(readlink -f "$0")) # untested
     ;;
esac

LOGFILE="$SCRIPTDIR/build-simple.log"

log_cmd() {
    "$@" 2>&1  | tee -a "$LOGFILE"
}

log_cmd echo "== starting build at $( date ) == "

(
    mkdir -p "$SCRIPTDIR/build-simple/"
    cd "$SCRIPTDIR/build-simple/"

    if [ -f "$SCRIPTDIR/build-simple/Makefile" ] ; then
        echo "buildsystem generated, using it"
    else
        echo "buildsystem being generated"

        log_cmd cmake -DCMAKE_INSTALL_PREFIX="$SCRIPTDIR/inst-simple/" "$SCRIPTDIR"
    fi

    log_cmd make install "$CPU_COUNT_TO_USE_MAKE_PARAM"
)
