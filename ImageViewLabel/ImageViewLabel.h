#ifndef IMAGEVIEWLABEL_H
#define IMAGEVIEWLABEL_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QPainter>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ImageViewLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageViewLabel(QWidget *parent = nullptr);
    void initControl();
    void addButton(int buttonNum);
signals:
    void clicked();
public slots:
    void imageValueChanged(const QVariant & variant);
    void expandValueChanged(const QVariant & variant);
    void shrinkValueChanged(const QVariant & variant);
    void switchImage(int index);
    void imageShowTimeOut();
    void relyFinished(QNetworkReply* reply);
    void updateView(QVector<QByteArray> pics);
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    int m_offset;
    int m_curIndex;
    int m_preIndex;
    int m_delay;
    bool m_bLeftToRight;
    QPushButton *m_pNextBtn;
    QPushButton *m_pPreBtn;
    QButtonGroup *m_pBtnGroup;
    QParallelAnimationGroup *m_pParalAnimationGroup;
    QPropertyAnimation *m_pImageAnimation;
    QPropertyAnimation *m_pExpandAnimation;
    QPropertyAnimation *m_pShrinkAnimation;
    QSequentialAnimationGroup* m_pSequentialGroup;
    QWidget *m_pHorizontalLayoutWidget;
    QHBoxLayout *m_pSwitchBtnLayout;
    QTimer *m_pImageTimer;
    QNetworkAccessManager *manager;
    int checkPixmapListTimerID;
    static QList<QPixmap> pixmapList;
};
#endif // IMAGEVIEWLABEL_H
