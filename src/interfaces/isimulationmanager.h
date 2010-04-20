/***************************************************************************
 *    ISimulationManager interface.                                        *
 *       A controller for simulations in KTechLab                          *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ISIMULATIONMANAGER_H
#define ISIMULATIONMANAGER_H

#include <QObject>

namespace KTechLab {

    class ISimulator;
    class IComponentDocument;

    /**
     * A simulation manager interface. 
     * This interface provides access to all the simulation-related
     * services.
     */
    class ISimulationManager : public QObject {

        Q_OBJECT

        public:
            /** @return the static ISimulationManager instance */
            static ISimulationManager *self();

            virtual ~ISimulationManager();

            /** @return an existing simulator for a document or NULL if there is 
             * no simulator associated with the respective document */
            virtual ISimulator *simulatorForDocument(IComponentDocument *document) = 0;

            /** @return a new simulator suitable for a given component document */
            virtual ISimulator *createSimulatorForDocument
                                    (IComponentDocument *document) = 0;
        protected:
            ISimulationManager();

            static ISimulationManager *m_self;
    };

}
#endif // ISIMULATIONMANAGER_H
