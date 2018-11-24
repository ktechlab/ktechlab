#!/bin/sh
set -e
set -x

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

export PATH="$SCRIPTDIR/inst-developer/bin:$PATH"

export ASAN_OPTIONS=new_delete_type_mismatch=0

"$SCRIPTDIR/inst-developer/bin/ktechlab" 2>&1 | tee "$SCRIPTDIR/developer-run.log"
