#include <cmath>
#include <iostream>

#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStringList>

using namespace std;

const int minPrefixExp = -24;
const int maxPrefixExp = 24;
const int numPrefix = int((maxPrefixExp - minPrefixExp) / 3) + 1;
const QString SIprefix[] = {"y", "z", "a", "f", "p", "n", QChar(0xB5), "m", "", "k", "M", "G", "T", "P", "E", "Z", "Y"};

/**
 * Converts a number string (including an optional SI suffix) to a real number.
 */
double toReal(QString text);

const QString inputFile = "spicemodels_bipolar_transistors.txt";
const QString outputFile = "output.lib";

#define setProperty(property, value) outputStream << QString("%1=%2\n").arg(property).arg(value)

int main()
{
    QFile input(inputFile);
    if (!input.open(QIODevice::ReadOnly)) {
        cerr << "Could not open input file \"" << inputFile << "\" for reading.\n";
        return 1;
    }

    QFile output(outputFile);
    if (!output.open(QIODevice::WriteOnly)) {
        cerr << "Could not open output file \"" << outputFile << "\" for writing.\n";
        return 1;
    }

    QTextStream inputStream(&input);
    QTextStream outputStream(&output);

    QStringList IDs;

    // Set true inside the while loop so when we come across a new model
    // in the input file, we can close the previous one in the output file.
    bool modelOpen = false;

    while (!inputStream.atEnd()) {
        QString line = inputStream.readLine();

        // Make life easier
        line.replace(QRegularExpression("\\.MODEL[^(]*\\("), "+ ");
        line.remove(")");

        if (line.isEmpty())
            continue;

        if (line.startsWith("*SRC=")) {
            if (modelOpen)
                outputStream << "[/]\n\n";
            modelOpen = true;

            line.remove(0, 5); // remove "*SRC="

            QStringList segments = line.split(';'); // QStringList::split( ';', line ); // 2018.12.01
            if (segments.size() != 5) {
                cerr << "Wrong number of \";\" for line \"" << line.latin1() << "\".\n";
                continue;
            }

            QStringList::iterator segmentsEnd = segments.end();
            for (QStringList::iterator it = segments.begin(); it != segmentsEnd; ++it)
                *it = (*it).trimmed();

            QString ID = segments[0];
            outputStream << QString("[%1]\n").arg(ID);
            if (IDs.contains(ID)) {
                cerr << "Already have ID \"" << ID << "\".\n";
                continue;
            } else
                IDs.append(ID);

            if (segments[2] == "BJTs PNP")
                setProperty("Type", "PNP");
            else if (segments[2] == "BJTs NPN")
                setProperty("Type", "NPN");
            else
                cerr << "Unknown type \"" << segments[2] << "\".\n";

            setProperty("Description", segments[4]);
        }

        else if (line.startsWith("+ ")) {
            line.remove(0, 2); // remove "+ ":
            QRegularExpression rx("([^\\s=]*)=([^\\s]*)");

            int pos = 0;
            while ((pos = rx.search(line, pos)) >= 0) {
                QString property = rx.cap(1);
                QString value = rx.cap(2);

                setProperty(property, toReal(value));

                pos += 4; // avoid the string we just found
            }
        }

        else
            cerr << "Unknown line for line \"" << line.latin1() << "\".\n";
    }

    // And the final closing brace
    if (modelOpen)
        outputStream << "[/]\n\n";

    input.close();
    output.close();

    return 0;
}

double getMultiplier(QString mag)
{
    if (mag.isEmpty())
        return 1.0;

    // Allow the user to enter in "u" instead of mu
    if (mag == "u")
        mag = QChar(0xB5);

    for (int i = 0; i < numPrefix; ++i) {
        if (mag == SIprefix[i])
            return pow(10., (i * 3) + minPrefixExp);
    }

    // default
    return 1.0;
}

double toReal(QString text)
{
    if (text.isEmpty()) {
        cerr << "Attempted to convert empty text to number.\n";
        return 0.0;
    }

    // Extract the possible SI suffix
    QChar suffix = text[text.length() - 1];
    double multiplier = 1.0;
    if (suffix.isLetter()) {
        multiplier = getMultiplier(QString(suffix));
        text.remove(text.length() - 1, 1);
    }

    bool ok;
    double val = text.toDouble(&ok);

    if (!ok) {
        cerr << "Could not convert number string \"" << text << "\" to real.\n";
        return 0.0;
    }

    return val * multiplier;
}
