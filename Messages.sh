#! /bin/sh
$EXTRACTRC `find . -name "*.rc" -o -name "*.ui" -o -name "*.kcfg" | grep -v '/tests/'` >> rc.cpp
$XGETTEXT `find . -name \*.h -o -name \*.cpp | grep -v '/tests/'` -o $podir/ktechlab.pot
rm -f rc.cpp
