#ifndef COMPONENTCONTAINER_H
#define COMPONENTCONTAINER_H

#include "../ktlinterfacesexport.h"
#include <QObject>
#include <datacontainer.h>

namespace KTechLab
{

class KTLINTERFACES_EXPORT ComponentContainer : public DataContainer
{
  Q_OBJECT
public:
    ComponentContainer( const QString &component = QString() );

private:
    QString m_component;
};

}
#endif // COMPONENTCONTAINER_H
