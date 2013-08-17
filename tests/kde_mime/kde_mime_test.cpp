/**
 * Simple program to test KDE's mime type registration
 *
 * Opening files with ktechlab apparently is problematic
 */
#include <kdebug.h>
#include <kglobal.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include <qt4/QtCore/qstring.h>

int main(void){
	QString path("/tmp/some-circuit.circuit");
	KUrl url(path);
	kDebug() << "url: " << url;

	// as used by kdevplatform
	KMimeType::Ptr mimeType0 = KMimeType::findByUrl( url );
	kDebug() << "mime type 0: " << mimeType0->name();

	// fast mode
	int accuracy1 = -1;
	KMimeType::Ptr mimeType1 = KMimeType::findByUrl( url, 0, false, true, &accuracy1 );
	kDebug() << "mime type 1: " << mimeType1->name();
	kDebug() << "mime type accuracy 1: " << accuracy1;

	// normal mode
	int accuracy2 = -1;
	KMimeType::Ptr mimeType2 = KMimeType::findByUrl( url, 0, false, false, &accuracy2 );
	kDebug() << "mime type 2: " << mimeType2->name();
	kDebug() << "mime type accuracy 2: " << accuracy2;

	//
	int circuitDefinitions = 0;
	KMimeType::List allMime = KMimeType::allMimeTypes();
	Q_FOREACH(KMimeType::Ptr mimePtr, allMime){
		if(mimePtr->is("application/x-circuit")){
			kDebug() << "found circuit mime at " << mimePtr;
			circuitDefinitions++;
		}
	}
	kDebug() << "total circuit mime definitions: " << circuitDefinitions;
	//
	const QStringList globFiles = KGlobal::dirs()->findAllResources(
		"xdgdata-mime", QString::fromLatin1("globs"));
	kDebug() << "Mime glob files are located at: " << globFiles;


}
