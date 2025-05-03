#!/bin/sh
set -e
set -x

# script for building ktechlab for development/testing
# currently it (1) enables address sanitizer

#SCRIPTDIR=$(dirname $(readlink -f "$0"))

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

LOGFILE="$SCRIPTDIR/build-developer.log"

log_cmd() {
    "$@" 2>&1  | tee -a "$LOGFILE"
}

log_cmd echo "== starting build at $( date ) == "

(
    mkdir -p "$SCRIPTDIR/build-developer/"
    cd "$SCRIPTDIR/build-developer/"

    if [ -f "$SCRIPTDIR/build-developer/CMakeCache.txt" ] ; then
        echo "buildsystem generated, using it"
    else
        echo "buildsystem being generated"

        log_cmd cmake   -DCMAKE_INSTALL_PREFIX="$SCRIPTDIR/inst-developer/" \
                        -DKTECHLAB_DEVELOPER_BUILD=true \
            "$SCRIPTDIR"

    fi

    log_cmd make install -j"$( nproc )"
)
