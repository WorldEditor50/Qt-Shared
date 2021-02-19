#include "custombutton.h"

CustomButton::CustomButton(QWidget *parent) :
    QWidget(parent),
    text(""),
    padding(2),
    radius(8),
    isFlat(false)
{
    textPosition = QRect(0, 0, width(), height());
    imagePosition = QRect(0, 0, width(), height());
    textColor = Qt::black;
    backgroundColor = Qt::gray;
}

CustomButton::~CustomButton()
{

}

void CustomButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    /* draw background */
    if (isFlat == false) {
        drawBackground(painter);
    }
    /* draw image */
    drawImage(painter);
    /* draw text */
    drawText(painter);
    return;
}

void CustomButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
    return;
}

void CustomButton::drawBackground(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(backgroundColor);
    QRect rect(padding, 0, width() - 2 * padding, height());
    painter.drawRoundedRect(rect, radius, radius);
    painter.restore();
    return;
}

void CustomButton::drawText(QPainter &painter)
{
    painter.save();
    painter.setPen(textColor);
    painter.setFont(textFont);
    painter.drawText(textPosition, Qt::AlignCenter, text);
    painter.restore();
    return;
}

void CustomButton::drawImage(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::NoPen);
    QRectF srcRect(0, 0, pixmap.width(), pixmap.height());
    painter.drawPixmap(imagePosition, pixmap, srcRect);
    painter.restore();
    return;
}

QString CustomButton::getText() const
{
    return text;
}

QPixmap CustomButton::getPixmap() const
{
    return pixmap;
}

QRectF CustomButton::getImagePosition() const
{
    return imagePosition;
}

QRect CustomButton::getTextPosition() const
{
    return textPosition;
}

QColor CustomButton::getTextColor() const
{
    return textColor;
}

QFont CustomButton::getTextFont() const
{
    return textFont;
}

QColor CustomButton::getBackgroundColor() const
{
    return backgroundColor;
}

int CustomButton::getPadding() const
{
    return padding;
}

int CustomButton::getRadius() const
{
    return radius;
}

bool CustomButton::getIsFlat() const
{
    return isFlat;
}

void CustomButton::setText(const QString &text)
{
    this->text = text;
    this->update();
    return;
}

void CustomButton::setPixmap(const QPixmap &pixmap)
{
    this->pixmap = pixmap;
    this->update();
    return;
}

void CustomButton::setImagePosition(const QRectF &pos)
{
    this->imagePosition = pos;
    this->update();
    return;
}

void CustomButton::setTextPosition(const QRect &pos)
{
    this->textPosition = pos;
    this->update();
    return;
}

void CustomButton::setTextColor(const QColor &color)
{
    this->textColor = color;
    this->update();
    return;
}

void CustomButton::setTextFont(const QFont &font)
{
    this->textFont = font;
    this->update();
    return;
}

void CustomButton::setBackgroundColor(const QColor &color)
{
    this->backgroundColor = color;
    this->update();
    return;
}

void CustomButton::setFlat(bool on)
{
    this->isFlat = on;
    this->update();
    return;
}

void CustomButton::setPadding(int pad)
{
    this->padding = pad;
    this->update();
    return;
}

void CustomButton::setRadius(int rad)
{
    this->radius = rad;
    this->update();
    return;
}
