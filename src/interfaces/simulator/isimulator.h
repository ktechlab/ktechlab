/***************************************************************************
 *    ISimulator interface.                                                *
 *       A controller for simulations in KTechLab                          *
 *     Copyright (c) 2010 Zoltan Padrah                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ISIMULATOR_H
#define ISIMULATOR_H

#include "../ktlinterfacesexport.h"

#include <QObject>
#include <QVariantMap>

namespace KTechLab {

class IDocumentModel;

    class IElement;
    class IComponentDocument;

/**
 * \brief A general interface for a simulator.
 * One simulator can simulate exactly one document.
 */
class KTLINTERFACES_EXPORT ISimulator : public QObject {
    Q_OBJECT

public:
    /**
     * create a simulator associated with a document
     * @param doc the document associated with the simulator
     */
    ISimulator(IDocumentModel *doc);

    /**
     * destroy the simulator
     */
    virtual ~ISimulator();

    // control methods
    // TODO check if these should be slots?
    /**
     * start the simulation. don't do anything if it's already running
     */
    virtual void start() = 0;

    /**
     * pause the simulation. if it's paused, do nothing
     */
    virtual void pause() = 0;

    /**
     * change the state of paused/running of the simulator
     */
    virtual void tooglePause() = 0;

    /**
     * @return the IElement associated with a component
     */
    // virtual IElement *getModelForComponent(QVariantMap *component) = 0;
//
public slots:
    /**
     * Slot activate in case of the document structure has changed.
     * The simulator should rebuild its data structures.
     */
    virtual void documentStructureChanged() = 0;

    /**
     * Slot to be activated my the document in case of the parameters
     * of one model changes. (for example, a resistance).
     *
     * The optional parameter indicates the model for which the change
     * occured. It is 0 (NULL) in case of the component is not specified.
     */
    virtual void componentParameterChanged(QVariantMap * component = NULL) = 0;

};

}
#endif // ISIMULATOR_H
