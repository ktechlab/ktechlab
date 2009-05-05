#ifndef COMPONENTCONTAINER_H
#define COMPONENTCONTAINER_H

#include "../ktlinterfacesexport.h"
#include <Plasma/DataContainer>

class KTLINTERFACES_EXPORT ComponentContainer : public Plasma::DataContainer
{
public:
    ComponentContainer( const QString &component = QString() );

private:
    QString m_component;
};

#endif // COMPONENTCONTAINER_H
