#include "PdfGenerator.h"

PdfGenerator::PdfGenerator(QObject *parent) :
    QObject(parent),
    rows(0),
    charset("utf-8")

{
    out.setString(&htmlData);
}

void PdfGenerator::pageBegin()
{
    out << QString("<html>")
        << QString("<head>")
        << QString("<meta content='text/html; charset=%1'>").arg(charset)
        << QString("<meta name=Generator content='Microsoft Word 12 (filtered)'>")
        << QString("</head>")
        << QString("<body>");
}

void PdfGenerator::insertTitle(const QString &title, const QString &fontStyle)
{
    QString dateTime = QString(tr("DateTime:%1")).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    out << QString("<p style='text-align:right'><span style='font-size:12px;font-family:Microsoft YaHei'>%1</span></p>\n")
        .arg(dateTime)
        << QString("<p style='text-align:center'><span style='%1'>%2</span></p>\n").arg(fontStyle).arg(title);
}

void PdfGenerator::insertSecondTitle(const QString &title2)
{
    out << QString("<p style='text-align:left'><span style='font-size:16px;font-family:Microsoft YaHei'><b>%1</b></span></p>\n")
        .arg(title2);
}

void PdfGenerator::tableBegin(const QStringList &head)
{
    out << QString("<div align='left' style='border-width:800px; margin-left:0px; margin-top:0px;page-break-after:always;'>")
        << QString("<table class=print width='100%' border=1 cellspacing=0 cellpadding=4 style='border-collapse:collapse'>\n")
        << QString("<thead><tr>\n");
    for (auto x : head) {
        out << QString("<td nowrap='nowarp' style='background-color:#E6E6E6' style='border:solid windowtext 2px'><p align='left'><span align='center' style='font-size:10px;font-family:Microsoft YaHei'><b>%1</b></span></p></td>\n")
            .arg(x);
    }
    out << "</tr></thead>\n";
    return;
}

void PdfGenerator::tableBeginNoBorder()
{
    out << QString("<div align='left' style='border-width:1000px; margin-left:0px; margin-top:0px'>")
        << QString("<table class=print width='100%' border=0 cellspacing=0 cellpadding=4 style='border-collapse:collapse'>\n");
    return;
}

void PdfGenerator::tableBeginNoHead()
{
    out << QString("<div align='center' style='border-width:1000px; margin-left:0px; margin-top:0px'>")
        << QString("<table class=print width='100%' border=1 cellspacing=0 cellpadding=4 style='border-collapse:collapse'>\n");
    return;
}

void PdfGenerator::insert2TableRow(const QStringList &rowData)
{
    out << QString("<tr>\n");
    for (auto x : rowData) {
        if (rows % 2 == 1) {
            out << QString("<td><p align='left' style='background-color:#E6E6E6' ><span align='left' style='font-size:8px;font-family:Microsoft YaHei'>%1</span></p></td>\n")
            .arg(x);
        } else {
            out << QString("<td><p align='left' style='background-color:#ffffff' ><span align='left' style='font-size:8px;font-family:Microsoft YaHei'>%1</span></p></td>\n")
            .arg(x);
        }
    }
    out << QString("</tr>\n");
    rows++;
    return;
}

void PdfGenerator::rowBegin()
{
    out << QString("<tr>\n");
    return;
}

void PdfGenerator::rowEnd()
{
    out << QString("</tr>\n");
    return;
}

void PdfGenerator::insert2Row(const QString &x, const QString &cellAlign, const QString &cellStyle, const QString &fontAlign, const QString &fontStyle)
{
    out << QString("<td><p align='%1' style='%2' ><span align='%3' style='%4'>%5</span></p></td>\n")
            .arg(cellAlign).arg(cellStyle).arg(fontAlign).arg(fontStyle).arg(x);
    return;
}

void PdfGenerator::insertToTable(const QStringList &rowData, const QString &backgroundColor, const QString &fontStyle)
{
    out << QString("<tr>\n");
    for (auto x : rowData) {
        out << QString("<td><p align='left' style='background-color:%1' ><span align='left' style='%2'>%3</span></p></td>\n")
        .arg(backgroundColor).arg(fontStyle).arg(x);
    }
    out << QString("</tr>\n");
    return;
}

void PdfGenerator::printWithPreview()
{
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPageSize(QPrinter::A4);
    QPrintPreviewDialog preview(&printer);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &PdfGenerator::getPreviewData);
    preview.setWindowState(Qt::WindowMaximized);
    preview.exec();
    resetState();
    return;
}

void PdfGenerator::printDirect()
{
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPageSize(QPrinter::A4);
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {
        getPreviewData(&printer);
    }
    resetState();
    return;
}

void PdfGenerator::resetState()
{
    htmlData.clear();
    charset = "utf-8";
    return;
}

void PdfGenerator::getPreviewData(QPrinter *printer)
{
    QTextDocument document;
    document.setHtml(htmlData.toUtf8());
    document.print(printer);
#if 0
    QFile tmp("tmp.html");
    if (tmp.open(QIODevice::WriteOnly)) {
        tmp.write(htmlData.toUtf8());
        tmp.close();
    }
#endif
    return;
}
