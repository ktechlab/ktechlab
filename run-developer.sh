#!/bin/sh
set -e
set -x

SCRIPTDIR=$(dirname $(readlink -f "$0"))

export PATH="$SCRIPTDIR/inst-developer/bin:$PATH"

"$SCRIPTDIR/inst-developer/bin/ktechlab" 2>&1 | tee "$SCRIPTDIR/developer-run.log"
