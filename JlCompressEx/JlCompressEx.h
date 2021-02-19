#ifndef JLCOMPRESSEX_H
#define JLCOMPRESSEX_H

#include <QBuffer>
#include <QByteArray>
#include <QVector>
#include "JlCompress.h"
#include "quazip.h"

class JlCompressData
{
public:
    JlCompressData(){}
    QString dataName;
    QByteArray data;
};
class JlCompressEx
{
public:
    /* compress data to memory */
    static bool compressToMemory(QString file, QIODevice& zipIoDevice);
    static bool compressToMemory(QByteArray& sourceData, QString fileName, QIODevice& zipIoDevice);
    static bool compressToMemory(QStringList files, QIODevice& zipIoDevice);
    static bool compressBuffer(QuaZip& zip, QByteArray& sourceData, QString fileName);
    static bool compressFile(QuaZip* zip, QString fileName, QString fileDest);
    /* extract data to memory */
    static bool extractToMemory(QString fileName, QVector<JlCompressData>& dataVec);
    static bool extractToMemory(QIODevice &ioDevice, QVector<JlCompressData>& dataVec);
    static bool extractToMemory(QuaZip& zip, QVector<JlCompressData>& dataVec);
    JlCompressEx(){}
    ~JlCompressEx(){}
};

#endif // JLCOMPRESSEX_H
