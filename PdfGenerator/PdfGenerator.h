#ifndef PDF_GENERATOR_H
#define PDF_GENERATOR_H

#include <QObject>
#include <QAbstractItemModel>
#include <QTextStream>
#include <QPrinter>
#include <QTextEdit>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QDateTime>
#include <QFile>

class PdfGenerator : public QObject
{
    Q_OBJECT
public:
    explicit PdfGenerator(QObject *parent = 0);
    ~PdfGenerator(){};
    void setCharSet(const QString &set = QString("utf-8")){ charset = set; }
    /* page */
    void pageBegin();
    void pageEnd(){ out << QString("</body>")<< QString("</html>"); }
    /* title */
    void insertTitle(const QString &title, const QString &fontStyle);
    void insertSecondTitle(const QString &title2);
    void insertSeperator(){ out << QString("<br>\n"); }
    /* insert to table */
    void tableBegin(const QStringList &head);
    void tableBeginNoBorder();
    void tableBeginNoHead();
    void tableEnd(){ out << QString("</table></div>\n"); }
    void insert2TableRow(const QStringList &rowData);
    /* insert to row */
    void rowBegin();
    void insert2Row(const QString &x,  const QString &cellAlign, const QString &cellStyle, const QString &fontAlign, const QString &fontStyle);
    void rowEnd();
    void insertToTable(const QStringList &rowData, const QString &backgroundColor, const QString &fontStyle);
    /* print */
    void printWithPreview();
    void printDirect();
public:
    int rows;
private:
    void resetState();
private slots:
    void getPreviewData(QPrinter *printer);
private:
    QString charset;
    QString htmlData;
    QTextStream out;

};

#endif // PDF_GENERATOR_H
