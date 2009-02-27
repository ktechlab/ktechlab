/***************************************************************************
 *   Copyright 2006 Adam Treat <treat@kde.org>                         *
 *   Copyright 2007 Alexander Dymo <adymo@kdevelop.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include "languagecontroller.h"

#include <QHash>
#include <QMutexLocker>

#include <kmimetype.h>

#include <interfaces/idocument.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/iplugin.h>
#include <interfaces/iplugincontroller.h>
#include <language/interfaces/ilanguagesupport.h>
#include <language/backgroundparser/backgroundparser.h>

#include "core.h"
#include "language.h"
//#include "settings/ccpreferences.h"
#include "completionsettings.h"

namespace KTechLab {


typedef QHash<QString, KDevelop::ILanguage*> LanguageHash;
typedef QHash<QString, QList<KDevelop::ILanguage*> > LanguageCache;

struct LanguageControllerPrivate {
    LanguageControllerPrivate(LanguageController *controller)
        : dataMutex(QMutex::Recursive), backgroundParser(new KDevelop::BackgroundParser(controller)), m_controller(controller) {}

    void documentActivated(KDevelop::IDocument *document)
    {
        KUrl url = document->url();
        if (!url.isValid()) {
            return;
        }

        foreach (KDevelop::ILanguage *lang, activeLanguages) {
            lang->deactivate();
        }

        activeLanguages.clear();

        QList<KDevelop::ILanguage*> languages = m_controller->languagesForUrl(url);
        foreach (KDevelop::ILanguage *lang, languages) {
            lang->activate();
            activeLanguages << lang;
        }
    }

    QList<KDevelop::ILanguage*> activeLanguages;

    mutable QMutex dataMutex;
    
    LanguageHash languages; //Maps language-names to languages
    LanguageCache languageCache; //Maps mimetype-names to languages
    typedef QMultiMap<KMimeType::Ptr, KDevelop::ILanguage*> MimeTypeCache;
    MimeTypeCache mimeTypeCache; //Maps mimetypes to languages

    KDevelop::BackgroundParser *backgroundParser;
    
    KDevelop::ILanguage* addLanguageForSupport(KDevelop::ILanguageSupport* support);

private:
    LanguageController *m_controller;
};

KDevelop::ILanguage* LanguageControllerPrivate::addLanguageForSupport(KDevelop::ILanguageSupport* languageSupport) {

    if(languages.contains(languageSupport->name()))
        return languages[languageSupport->name()];

    Q_ASSERT(dynamic_cast<KDevelop::IPlugin*>(languageSupport));

    KDevelop::ILanguage* ret = new Language(languageSupport, m_controller);
    languages.insert(languageSupport->name(), ret);

    QVariant mimetypes = Core::self()->pluginController()->pluginInfo(dynamic_cast<KDevelop::IPlugin*>(languageSupport)).property("X-KDevelop-SupportedMimeTypes");

    foreach(QString mimeTypeName, mimetypes.toStringList()) {
        kDebug() << "adding supported mimetype:" << mimeTypeName << "language:" << languageSupport->name();
        languageCache[mimeTypeName] << ret;
        KMimeType::Ptr mime = KMimeType::mimeType(mimeTypeName);
        if(mime) {
            mimeTypeCache.insert(mime, ret);
        } else {
            kWarning() << "could not create mime-type" << mimeTypeName;
        }
    }

    return ret;
}

LanguageController::LanguageController(QObject *parent)
    : ILanguageController(parent)
{
    setObjectName("LanguageController");
    d = new LanguageControllerPrivate(this);
}

LanguageController::~LanguageController()
{
    delete d;
}

void LanguageController::initialize()
{
    connect(Core::self()->documentController(), SIGNAL(documentActivated(KDevelop::IDocument*)),
            SLOT(documentActivated(KDevelop::IDocument*)));
}

QList<KDevelop::ILanguage*> LanguageController::activeLanguages()
{
    QMutexLocker lock(&d->dataMutex);
    
    return d->activeLanguages;
}

KDevelop::ICompletionSettings *LanguageController::completionSettings() const {
    return &CompletionSettings::self();
}

QList<KDevelop::ILanguage*> LanguageController::loadedLanguages() const
{
    QMutexLocker lock(&d->dataMutex);
    QList<KDevelop::ILanguage*> ret;
    foreach(KDevelop::ILanguage* lang, d->languages)
        ret << lang;
    return ret;
}

KDevelop::ILanguage *LanguageController::language(const QString &name) const
{
    QMutexLocker lock(&d->dataMutex);
    
    if(d->languages.contains(name))
        return d->languages[name];
    else{
        KDevelop::ILanguage* ret = 0;
        QStringList constraints;
        ///@todo Lookup by language-name still does not work
        constraints << QString("'%1' in [X-KDevelop-Language]").arg(name);
        QList<KDevelop::IPlugin*> supports = Core::self()->pluginController()->
            allPluginsForExtension("ILanguageSupport", constraints);
        if(!supports.isEmpty()) {
            KDevelop::ILanguageSupport *languageSupport = supports[0]->extension<KDevelop::ILanguageSupport>();
            if(supports[0])
                ret = d->addLanguageForSupport(languageSupport);
        }
        return ret;
    }
}

QList<KDevelop::ILanguage*> LanguageController::languagesForUrl(const KUrl &url)
{
    QMutexLocker lock(&d->dataMutex);
    
    QList<KDevelop::ILanguage*> languages;
    
    QString fileName = url.fileName();

    for(int a = 0; a < 2; ++a) {

        ///non-crashy part: Use the mime-types of known languages
        ///Very inefficient right now
        for(LanguageControllerPrivate::MimeTypeCache::const_iterator it = d->mimeTypeCache.begin(); it != d->mimeTypeCache.end(); ++it) {
            foreach(QString pattern, it.key()->patterns()) {
                if(pattern.startsWith('*'))
                pattern = pattern.mid(1);

                QRegExp exp(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
                if(int position = exp.indexIn(fileName)) {
                    
                    if(position != -1 && exp.matchedLength() + position == fileName.length())
                        languages << *it;
                }
            }
    }
        
        if(!languages.isEmpty() || a)
            return languages;
        
        ///Crashy and unsafe part: Load missing language-supports
        KMimeType::Ptr mimeType = KMimeType::findByUrl(url);

        LanguageCache::iterator it = d->languageCache.find(mimeType->name());
        if (it != d->languageCache.constEnd()) {
            languages = it.value();
        } else {
            QStringList constraints;
            constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg(mimeType->name());
            QList<KDevelop::IPlugin*> supports = Core::self()->pluginController()->
                allPluginsForExtension("ILanguageSupport", constraints);

            foreach (KDevelop::IPlugin *support, supports) {
                KDevelop::ILanguageSupport* languageSupport = support->extension<KDevelop::ILanguageSupport>();
                kDebug() << "language-support:" << languageSupport;
                if(languageSupport)
                    return languages << d->addLanguageForSupport(languageSupport);
            }
        }
    }
    return languages;
}

KDevelop::BackgroundParser *LanguageController::backgroundParser() const
{
    return d->backgroundParser;
}

}

#include "languagecontroller.moc"
