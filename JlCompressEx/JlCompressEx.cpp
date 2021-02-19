#include "JlCompressEx.h"

static bool copyData(QIODevice &inFile, QIODevice &outFile)
{
    while (!inFile.atEnd()) {
        char buf[4096];
        qint64 readLen = inFile.read(buf, 4096);
        if (readLen <= 0)
            return false;
        if (outFile.write(buf, readLen) != readLen)
            return false;
    }
    return true;
}

bool JlCompressEx::compressFile(QuaZip* zip, QString fileName, QString fileDest)
{
    // zip: oggetto dove aggiungere il file
    // fileName: nome del file reale
    // fileDest: nome del file all'interno del file compresso

    // Controllo l'apertura dello zip
    if (!zip) return false;
    if (zip->getMode()!=QuaZip::mdCreate &&
        zip->getMode()!=QuaZip::mdAppend &&
        zip->getMode()!=QuaZip::mdAdd) return false;

    // Apro il file originale
    QFile inFile;
    inFile.setFileName(fileName);
    if(!inFile.open(QIODevice::ReadOnly)) return false;

    // Apro il file risulato
    QuaZipFile outFile(zip);
    if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileDest, inFile.fileName()))) return false;

    // Copio i dati
    if (!copyData(inFile, outFile) || outFile.getZipError()!=UNZ_OK) {
        return false;
    }

    // Chiudo i file
    outFile.close();
    if (outFile.getZipError()!=UNZ_OK) return false;
    inFile.close();

    return true;
}

bool JlCompressEx::compressToMemory(QString file, QIODevice& zipIoDevice)
{
    QuaZip zip(&zipIoDevice);
    if (!zip.open(QuaZip::mdCreate)) {
        return false;
    }
    if (!compressFile(&zip, file, QFileInfo(file).fileName())) {
        return false;
    }

    zip.close();
    if (zip.getZipError() != UNZ_OK) {
        return false;
    }

    return true;
}

bool JlCompressEx::compressToMemory(QByteArray& sourceData, QString fileName, QIODevice& zipIoDevice)
{
    QuaZip zip(&zipIoDevice);
    if (!zip.open(QuaZip::mdCreate)) {
        return false;
    }

    if (!compressBuffer(zip, sourceData, fileName)) {
        return false;
    }
    zip.close();
    if (zip.getZipError() != UNZ_OK) {
        return false;
    }

    return true;
}

bool JlCompressEx::compressToMemory(QStringList files, QIODevice& zipIoDevice)
{
    QuaZip zip(&zipIoDevice);
    if (!zip.open(QuaZip::mdCreate)) {
        return false;
    }
    QFileInfo info;
    for (int index = 0; index < files.size(); ++index) {
        const QString & file(files.at(index));
        info.setFile(file);
        if (!info.exists() || !compressFile(&zip, file, info.fileName())) {
            return false;
        }
    }

    zip.close();
    if (zip.getZipError() != 0) {
        return false;
    }

    return true;
}

bool JlCompressEx::extractToMemory(QString fileName, QVector<JlCompressData>& dataVec)
{
    QuaZip zip(fileName);
    extractToMemory(zip, dataVec);
    return true;
}

bool JlCompressEx::extractToMemory(QIODevice &ioDevice, QVector<JlCompressData> &dataVec)
{
    QuaZip zip(&ioDevice);
    extractToMemory(zip, dataVec);
    return true;
}

bool JlCompressEx::extractToMemory(QuaZip &zip, QVector<JlCompressData> &dataVec)
{
    if (!zip.open(QuaZip::mdUnzip)) {
        return false;
    }
    QStringList fileList = zip.getFileNameList();
    if (fileList.size() == 0) {
        return false;
    }
    for (auto x: fileList) {
        zip.setCurrentFile(x);
        QuaZipFile file(&zip);
        if (file.open(QIODevice::ReadOnly) ) {
            JlCompressData compressData;
            compressData.data = file.readAll();
            compressData.dataName = x;
            dataVec.push_back(compressData);
        }
        file.close();
    }
    zip.close();
    return true;
}

bool JlCompressEx::compressBuffer(QuaZip& zip, QByteArray& sourceData, QString fileName)
{
    if (zip.getMode() != QuaZip::mdCreate &&
        zip.getMode() != QuaZip::mdAppend &&
        zip.getMode() != QuaZip::mdAdd) {
        return false;
    }
    QuaZipFile outFile(&zip);
    if (!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName))) {
        return false;
    }
    if (outFile.write(sourceData) != sourceData.size()) {
        return false;
    }
    if (outFile.getZipError() != UNZ_OK) {
        return false;
    }
    outFile.close();
    if (outFile.getZipError() != UNZ_OK) {
        return false;
    }
    return true;
}
