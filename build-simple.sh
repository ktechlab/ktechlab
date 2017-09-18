#!/bin/sh
set -e
set -x

SCRIPTDIR=$(dirname $(readlink -f "$0"))

LOGFILE="$SCRIPTDIR/build-simple.log"

log_cmd() {
    "$@" 2>&1  | tee -a "$LOGFILE"
}

log_cmd echo "== starting build at $( date ) == "

(
    mkdir -p "$SCRIPTDIR/build-simple/"
    cd "$SCRIPTDIR/build-simple/"

    if [ -f "$SCRIPTDIR/build-simple/CMakeCache.txt" ] ; then
        echo "buildsystem generated, using it"
    else
        echo "buildsystem being generated"

        log_cmd cmake -DCMAKE_INSTALL_PREFIX="$SCRIPTDIR/inst-simple/" "$SCRIPTDIR"
    fi

    # work around the bug: core directory has to be built first # 2017.09.18 - not needed anymore
    #log_cmd make -C "$SCRIPTDIR/build-simple/src/core"

    log_cmd make install -j2
)
