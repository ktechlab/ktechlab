#!/bin/sh
set -e
set -x

SCRIPTDIR=$(dirname $(readlink -f "$0"))

export PATH="$SCRIPTDIR/inst-simple/bin:$PATH"

"$SCRIPTDIR/inst-simple/bin/ktechlab"
