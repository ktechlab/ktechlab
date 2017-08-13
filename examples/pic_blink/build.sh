#!/bin/sh
set -e
set -x

do_build() {
    mkdir -p out
    local RES=0
    sdcc -mpic14 -p16f84 --use-non-free pic_blink.c -o out/pic_blink || RES=$?
    echo "Result code: $RES"
    return "$RES"
}

main() {
    local SCRIPTDIR="$( dirname "$0" )"
    local LOGFILE="$( basename "$0" ).log"
    ( cd "$SCRIPTDIR"
      echo "start $0 at $( date )"
      do_build $@
      echo "done $0 at $( date )"
    ) 2>&1 | tee -a "$LOGFILE"
}

main $@
