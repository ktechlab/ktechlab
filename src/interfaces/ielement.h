/***************************************************************************
 *    IElement interface.                                                  *
 *       A common interface for all compoent models in KTechLab            *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef IELEMENT_H
#define IELEMENT_H

#include <QObject>

namespace KTechLab {
    /**
     * common interface for all the models used by a simulator
     */
    class IElement : public QObject {
      Q_OBJECT

      public:
            virtual ~IElement();

            // TODO add methods
    };
}

#endif // IELEMENT_H
