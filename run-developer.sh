#!/bin/sh
set -e
set -x

SCRIPTDIR=$(dirname $(readlink -f "$0"))

export PATH="$SCRIPTDIR/inst-developer/bin:$PATH"

export ASAN_OPTIONS=new_delete_type_mismatch=0

"$SCRIPTDIR/inst-developer/bin/ktechlab" 2>&1 | tee "$SCRIPTDIR/developer-run.log"
