/**
 * Simple program to test KDE's mime type registration
 *
 * Opening files with ktechlab apparently is problematic
 */
#include <kurl.h>
#include <kmimetype.h>

#include <qt4/QtCore/qstring.h>

int main(void){
	QString path("/tmp/some-circuit.circuit");
	KUrl url(path);
	KMimeType::Ptr mimeType;
	mimeType = KMimeType::findByUrl( url );
}
