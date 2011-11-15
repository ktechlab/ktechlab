/***************************************************************************
*   Copyright (C) 2011 Zoltan Padrah <zoltan_padrah@users.sourceforge.net>*
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KTL_GUI_PLUGIN_H
#define KTL_GUI_PLUGIN_H

#include <kdevplatform/interfaces/iplugin.h>
#include <QVariantList>

namespace KTechLab
{

    class KTLGuiPlugin : public KDevelop::IPlugin
    {
        Q_OBJECT
        public:
            explicit KTLGuiPlugin( QObject *parent = 0,
                                        const QVariantList &args = QVariantList() );
            virtual ~KTLGuiPlugin();

        private:

    };

}

#endif

