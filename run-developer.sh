#!/bin/sh
set -e
set -x

SCRIPTDIR=$(dirname $(readlink -f "$0"))

export PATH="$SCRIPTDIR/inst-developer/bin:$PATH"

"$SCRIPTDIR/inst-developer/bin/ktechlab"
