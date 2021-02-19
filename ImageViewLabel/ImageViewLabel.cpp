#include "ImageViewLabel.h"
const int btn_expand_width = 24;
const int btn_shrink_width = 6;
QList<QPixmap> ImageViewLabel::pixmapList;
ImageViewLabel::ImageViewLabel(QWidget *parent)
    : QLabel(parent)
    , m_offset(0)
    , m_curIndex(0)
    , m_preIndex(0)
    , m_delay(9000)
    , m_bLeftToRight(true)
{

    resize(850, 134);
    m_pBtnGroup = new QButtonGroup(this);
    connect(m_pBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &ImageViewLabel::switchImage);

    m_pParalAnimationGroup = new QParallelAnimationGroup(this);
    m_pImageAnimation = new QPropertyAnimation(m_pParalAnimationGroup, "");
    m_pImageAnimation->setEasingCurve(QEasingCurve::OutCirc);
    m_pImageAnimation->setDuration(800);
    connect(m_pImageAnimation, &QPropertyAnimation::valueChanged, this, &ImageViewLabel::imageValueChanged);

    QSequentialAnimationGroup* sequentialGroup = new QSequentialAnimationGroup(m_pParalAnimationGroup);
    m_pExpandAnimation = new QPropertyAnimation(sequentialGroup, "");
    m_pExpandAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_pExpandAnimation, &QPropertyAnimation::valueChanged, this, &ImageViewLabel::expandValueChanged);
    m_pExpandAnimation->setDuration(400);

    m_pShrinkAnimation = new QPropertyAnimation(sequentialGroup, "");
    m_pShrinkAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_pShrinkAnimation, &QPropertyAnimation::valueChanged, this, &ImageViewLabel::shrinkValueChanged);
    m_pShrinkAnimation->setDuration(400);
    /* switching button sequencely */
    sequentialGroup->addAnimation(m_pExpandAnimation);
    sequentialGroup->addAnimation(m_pShrinkAnimation);
    /* concurrent switching image and button  */
    m_pParalAnimationGroup->addAnimation(m_pImageAnimation);
    m_pParalAnimationGroup->addAnimation(sequentialGroup);
    /* open official host */
    connect(this, &ImageViewLabel::clicked, this, [=]() {
        QUrl url = QUrl::fromUserInput(QString("www.baidu.com"));
        QDesktopServices::openUrl(url);
    });
    /* check pixmapList timer */
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ImageViewLabel::relyFinished);
    checkPixmapListTimerID = startTimer(20000);
    initControl();
}

void ImageViewLabel::initControl()
{
    m_pHorizontalLayoutWidget = new QWidget(this);
    m_pHorizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
    m_pSwitchBtnLayout = new QHBoxLayout(m_pHorizontalLayoutWidget);
    m_pSwitchBtnLayout->setSpacing(12);
    m_pSwitchBtnLayout->setContentsMargins(0, 0, 0, 0);

    m_pImageTimer = new QTimer(this);
    m_pImageTimer->setInterval(9000);
    connect(m_pImageTimer, &QTimer::timeout, this, &ImageViewLabel::imageShowTimeOut);
    m_pImageTimer->start();
    /* control btn */
    m_pNextBtn = new QPushButton(this);
    m_pPreBtn = new QPushButton(this);
    m_pNextBtn->setGeometry(822, 40, 18, 39);
    m_pPreBtn->setGeometry(10, 40, 18, 39);
    m_pNextBtn->setIcon(QIcon(":image/NEXT"));
    m_pNextBtn->setIconSize(QSize(18, 39));
    m_pNextBtn->setStyleSheet("border:none");
    m_pNextBtn->setFlat(true);
    m_pPreBtn->setIcon(QIcon(":image/PREVIOUS"));
    m_pPreBtn->setIconSize(QSize(18, 39));
    m_pPreBtn->setStyleSheet("border:none");
    m_pPreBtn->setFlat(true);
    connect(m_pNextBtn, &QPushButton::clicked, this, [=](){
        switchImage(m_curIndex + 1);
    });
    connect(m_pPreBtn, &QPushButton::clicked, this, [=](){
        switchImage(m_curIndex - 1);
    });
    /* add image */
    if (pixmapList.size() == 0) {
        pixmapList.append(QPixmap(":/image/0"));
        pixmapList.append(QPixmap(":/image/1"));
        pixmapList.append(QPixmap(":/image/2"));
        pixmapList.append(QPixmap(":/image/3"));
    }
    addButton(pixmapList.size());
    return;
}

void ImageViewLabel::imageValueChanged(const QVariant & variant)
{
    m_offset = variant.toInt();
    update();
    return;
}

void ImageViewLabel::expandValueChanged(const QVariant & variant)
{
    m_pBtnGroup->button(m_curIndex)->setFixedWidth(variant.toInt());
    return;
}

void ImageViewLabel::shrinkValueChanged(const QVariant & variant)
{
    for (int index = 0; index < pixmapList.size(); index++) {
        if (m_curIndex != index && m_pBtnGroup->button(index)->width() > btn_shrink_width) {
            m_pBtnGroup->button(index)->setFixedWidth(variant.toInt());
        }
    }
    return;
}

void ImageViewLabel::switchImage(int index)
{
    if (m_curIndex == index) {
        return;
    }
    if (index < 0) {
        index = pixmapList.size() - 1;
    }
    if (index >= pixmapList.size()){
        index = 0;
    }

    m_preIndex = m_curIndex;
    m_curIndex = index;
    /* switch image */
    if (m_preIndex < m_curIndex) {
        /* slide to left */
        m_pImageAnimation->setStartValue(0);
        m_pImageAnimation->setEndValue(0 - width());
        m_bLeftToRight = false;
    }
    else{
        /* slide to right */
        m_pImageAnimation->setStartValue(0);
        m_pImageAnimation->setEndValue(width());
        m_bLeftToRight = true;
    }
    /* switch button */
    m_pShrinkAnimation->setStartValue(btn_expand_width);
    m_pShrinkAnimation->setEndValue(btn_shrink_width);

    m_pExpandAnimation->setStartValue(btn_shrink_width);
    m_pExpandAnimation->setEndValue(btn_expand_width);

    m_pImageTimer->start(9000);
    m_pParalAnimationGroup->start();
    return;
}

void ImageViewLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    painter.setPen(QColor(210, 210, 210, 200));
    QFont font = painter.font();
    font.setFamily(QString::fromLocal8Bit("Microsoft YaHei"));
    font.setBold(true);
    font.setPixelSize(18);
    painter.setFont(font);

    if (m_bLeftToRight == true) {
        painter.drawPixmap(m_offset, 0, pixmapList.at(m_preIndex));
        painter.drawPixmap(m_offset - width(), 0, pixmapList.at((m_curIndex)));
        //painter.drawText(QRect(m_offset + 12, height() - 52, width(), 32), m_imgList.at(m_preIndex).second, option);
        //painter.drawText(QRect(m_offset - width() + 12, height() - 52, width(), 32), m_imgList.at(m_curIndex).second, option);
    } else{
        painter.drawPixmap(m_offset, 0, pixmapList.at(m_preIndex));
        painter.drawPixmap(m_offset + width(), 0, pixmapList.at(m_curIndex));
        //painter.drawText(QRect(m_offset + 12, height() - 52, width(), 32), m_imgList.at(m_preIndex).second, option);
        //painter.drawText(QRect(m_offset + width() + 12, height() - 52, width(), 32), m_imgList.at(m_curIndex).second, option);
    }
    return;
}

void ImageViewLabel::resizeEvent(QResizeEvent *event)
{
    m_pHorizontalLayoutWidget->setGeometry(QRect(12, height() - 18, width() -  24, 6));
    QLabel::resizeEvent(event);
    return;
}

void ImageViewLabel::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == checkPixmapListTimerID) {
        if (pixmapList.size() == 0) {
            //todo: download image
        }
    }
    return;
}

void ImageViewLabel::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
    return;
}

void ImageViewLabel::addButton(int buttonNum)
{
    for (int index = 0; index < buttonNum; index++) {
        QPushButton* btn = new QPushButton(m_pHorizontalLayoutWidget);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedSize(btn_shrink_width, btn_shrink_width);
        btn->setStyleSheet("width:6px; height:6px;"
                           "border: none;"
                           "border-radius: 3px;"
                           "background-color: #ffffff;"
                           "color: #ffffff;"
                           "min-width: 6px;");
        m_pBtnGroup->addButton(btn, index);
        m_pSwitchBtnLayout->addWidget(btn);
    }
    m_pSwitchBtnLayout->addStretch();
    m_pBtnGroup->button(m_curIndex)->setFixedWidth(btn_expand_width);
    update();
    return;
}

void ImageViewLabel::imageShowTimeOut()
{
    switchImage(m_curIndex + 1);
    return;
}

void ImageViewLabel::relyFinished(QNetworkReply *reply)
{
    QByteArray imgArray = reply->readAll();
}

void ImageViewLabel::updateView(QVector<QByteArray> pics)
{
    pixmapList.clear();
    for (auto x : pics) {
        QPixmap pixmap;
        pixmap.loadFromData(x);
        pixmapList.append(pixmap);
    }
    return;
}
