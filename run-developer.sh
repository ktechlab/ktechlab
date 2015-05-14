#!/bin/sh
set -e
set -x

SCRIPTDIR=$(dirname $(readlink -f "$0"))

"$SCRIPTDIR/inst-developer/bin/ktechlab"
