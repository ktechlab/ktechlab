#! /bin/sh
$EXTRACTRC `find . -name "*.rc" -o -name "*.ui" -o -name "*.kcfg"` >> rc.cpp
$XGETTEXT `find . -name \*.h -o -name \*.cpp` -o $podir/ktechlab.pot
rm -f rc.cpp
