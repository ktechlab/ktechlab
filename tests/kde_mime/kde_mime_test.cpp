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
#include <qdir.h>
#include <qtemporaryfile.h>

/**
 * @return file name
 */
static
QString createNewEmptyCircuitFile(QTemporaryFile &tmpFile){

	tmpFile.setFileTemplate(QDir::tempPath().append(QDir::separator())
		.append("ktl-XXXXXX.circuit"));
	tmpFile.setAutoRemove(false);
	tmpFile.open();
	qDebug() << "creating temporary file: " << tmpFile.fileName()
		<< "pattern: " << tmpFile.fileTemplate();
	// write a minial circuit document into the temporary file
	tmpFile.write("<!DOCTYPE KTechlab>\n"
				"<document type=\"circuit\" >"
				"</document>"
				);
	tmpFile.close();
	return tmpFile.fileName();
}

static
QStringList readGlobFile(QString globFileName) {
	QStringList ret;
	QFile globFile(globFileName);
	if(!globFile.open(QIODevice::ReadOnly)) {
		return ret;
	}
	QTextStream globStream(&globFile);
	while(!globStream.atEnd()) {
		QString line = globStream.readLine();
		if(line.startsWith("#")) {
			continue;
		}
		if(line.contains("application/x-circuit")) {
			ret.append(globFileName.append(": '").append(line).append("'"));
		}
	}
	globFile.close();
	return ret;
}

int main(void){
	QTemporaryFile tmpFile;
	createNewEmptyCircuitFile(tmpFile);
	QString path(tmpFile.fileName());
	KUrl url(tmpFile.fileName());
	kDebug() << "url: " << url;

	// as used by kdevplatform
	KMimeType::Ptr mimeType0 = KMimeType::findByUrl( url );
	kDebug() << "mime type 0: " << mimeType0->name();

	if(!mimeType0->is("application/x-circuit")){
		kDebug() << "EXPECT PROBLEMS";
	}
	// as used by kdevplatform
	int accuracy4 = -1;
	KMimeType::Ptr mimeType4 = KMimeType::findByUrl( url, 0, false, false, &accuracy4 );
	kDebug() << "mime type 4: " << mimeType4->name();
	kDebug() << "mime type accuracy 4: " << accuracy4;

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
			kDebug() << "found circuit mime at " << (int)mimePtr;
			kDebug() << "patterns: " << mimePtr->patterns();
			circuitDefinitions++;
		}
	}
	kDebug() << "total circuit mime definitions: " << circuitDefinitions;
	//
	const QStringList globFiles = KGlobal::dirs()->findAllResources(
		"xdgdata-mime", QString::fromLatin1("globs"));
	kDebug() << "Mime glob files are located at: " << globFiles;

	QStringList allRelevantLines;
	Q_FOREACH(QString globFileName, globFiles){
		QStringList relevantLines = readGlobFile(globFileName);
		allRelevantLines.append(relevantLines);
	}
	kDebug() << "Relevant Mime file lines: " << allRelevantLines;
	if(allRelevantLines.size() > 1){
		kDebug() << "Multiple circuit definitions?";
	}
	if(allRelevantLines.size() == 0){
		kDebug() << "No file definitions?";
	}

}
