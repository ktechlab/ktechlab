#!/bin/bash
# script to generate a list of all icons used in KTechLab; for testing purposes

set -e
set -x

SCRIPTDIR=$(dirname "$0")
( cd "$SCRIPTDIR/../../src"
    ICONLIST_LOADED=$(grep -R -e 'loadIcon *( *\"[^\"]*\"' --only-matching | cut -d\" -f 2)
    ICONLIST_CREATED=$(grep -R -e 'KIcon *( *\"[^\"]*\"' --only-matching | cut -d\" -f 2)
    ICONLIST=$( echo "$ICONLIST_LOADED $ICONLIST_CREATED" | sort | uniq )

    OUTFILE="../tests/loaded-icons/icon-list-generated.h"

    echo "/* AUTO-GENERATED FILE BY $0; DO NOT EDIT MANUALLY */" > "$OUTFILE"
    echo "static const char *iconNames[] = {" >> "$OUTFILE"
    for ICON in $ICONLIST ; do
        echo "    \"$ICON\"," >> "$OUTFILE"
    done
    echo "};" >> "$OUTFILE"
)
