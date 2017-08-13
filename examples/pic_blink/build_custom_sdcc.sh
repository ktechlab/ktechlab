#!/bin/sh
set -e
set -x

do_build() {
    mkdir -p out
    local RES=0
    local SDCC_INSTDIR="$HOME/.opt/sdcc-3.3.0"
    local SDCC_EXE="$SDCC_INSTDIR/bin/sdcc"
    local SDCC_INC="$SDCC_INSTDIR/share/sdcc/include"
    "$SDCC_EXE" \
        --nostdinc -I"$SDCC_INC" -I"$SDCC_INC/pic14" -I"$SDCC_INC/../non-free/include/pic14"\
        -mpic14 -p16f84 --use-non-free pic_blink.c -o out/pic_blink \
        || RES=$?
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
