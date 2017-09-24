#!/bin/sh
set -e
set -x

# script for building ktechlab for development/testing
# currently it (1) builds ktechlab with gcc-5 and (2) enables address sanitizer

SCRIPTDIR=$(dirname $(readlink -f "$0"))

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

        # -DCMAKE_C_COMPILER=gcc-5 -DCMAKE_CXX_COMPILER=g++-5 \ # 2017.09.18 - deprecated

    fi

    # work around the bug: core directory has to be built first
    log_cmd make -C "$SCRIPTDIR/build-developer/src/core"

    log_cmd make install -j"$( nproc )"
)
