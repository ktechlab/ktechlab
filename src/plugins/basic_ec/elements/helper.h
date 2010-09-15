
#ifndef HELPER_H
#define HELPER_H

#include <KDebug>

inline QStringList pinListFromParent(const QVariantMap& p)
{
    if (!p.contains("pinList") || !p.value("pinList").canConvert(QVariant::StringList)){
        kError() << "cannot find pinList in QVariantMap: " << p;
        return QStringList();
    }

    return p.value("pinList").toStringList();
}

#endif