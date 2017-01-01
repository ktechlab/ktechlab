#!/bin/bash
# script to generate a list of all icons used in KTechLab; for testing purposes

set -e
set -x

SCRIPTDIR=$(dirname "$0")
( cd "$SCRIPTDIR/../../src"
    ICONLIST_LOADED=$(grep -R -e 'loadIcon *( *\"[^\"]*\"' --only-matching | cut -d\" -f 2)
    ICONLIST_CREATED=$(grep -R -e 'KIcon *( *\"[^\"]*\"' --only-matching | cut -d\" -f 2)
    ICONLIST_PIXMAPS=$(grep -R -e 'SmallIcon *( *\"[^\"]*\"' --only-matching | cut -d\" -f 2)
    ICONLIST_GUIITEM=$(grep -R -e 'setIconName *( *\"[^\"]*\"' --only-matching | cut -d\" -f 2)
    # TODO addPage( m_generalOptionsWidget, i18n("General"), "misc", i18n("General Options") );
    #                                                         ^~~~
    ICONLIST=$( echo $ICONLIST_LOADED $ICONLIST_CREATED $ICONLIST_PIXMAPS $ICONLIST_GUIITEM | xargs -n1 | sort -u )

    OUTFILE="../tests/loaded-icons/icon-list-generated.h"

    echo "/* AUTO-GENERATED FILE BY $0; DO NOT EDIT MANUALLY */" > "$OUTFILE"
    echo "/* date of generation: $(date) */" >> "$OUTFILE"
    echo "static const char *iconNames[] = {" >> "$OUTFILE"
    for ICON in $ICONLIST ; do
        echo "    \"$ICON\"," >> "$OUTFILE"
    done
    echo "};" >> "$OUTFILE"
)
