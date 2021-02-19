#include "ProgressButton.h"

ProgressButton::ProgressButton(QWidget *parent) :
    QWidget(parent),
    minimum(0),
    maximum(100),
    value(0),
    padding(2),
    radius(16),
    text(""),
    finishText("finish"),
    valueColor(QColor(0, 150, 255)),
    backgroundColor(Qt::white),
    textColor(Qt::white),
    progressTextColor(Qt::black),
    buttonTextColor(Qt::black)
{
    resize(100, 50);
    maximum = width();
    textFont.setPointSize(11);
    textFont.setFamily("Microsoft YaHei");
    setWindowFlag(Qt::FramelessWindowHint);
}

ProgressButton::~ProgressButton()
{

}

void ProgressButton::paintEvent(QPaintEvent *event)
{
   Q_UNUSED(event)
   QPainter painter(this);
   painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
   /* draw background  */
   drawBackgroud(&painter);
   /* draw value */
   drawValue(&painter);
   /* draw text */
   drawText(&painter);
   return;
}

void ProgressButton::drawBackgroud(QPainter *painter)
{
    painter->save();
    QRect rect(padding, 0, width() - padding * 2, height());
    if (value >= 0) {
        painter->setPen(Qt::gray);
    } else {
        painter->setPen(Qt::NoPen);
    }

    if (value == minimum) {
        painter->setBrush(valueColor);
    } else {
        painter->setBrush(backgroundColor);
    }
    painter->drawRoundedRect(rect, radius, radius);
    painter->restore();
    return;
}

void ProgressButton::drawText(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setFont(textFont);
    QString strValue;
    if (value < 0) {
        painter->setPen(buttonTextColor);
        strValue = text;
    } else if (value == minimum) {
        painter->setPen(textColor);
        strValue = text;
    } else if (value >= maximum) {
        painter->setPen(textColor);
        strValue = finishText;
    } else {
        double temp = value / (maximum - minimum) * 100;
        painter->setPen(progressTextColor);
        strValue = QString("%1%").arg(temp, 0, 'f', 0);
    }
    QRect rect(0, 0, width(), height());
    painter->drawText(rect, Qt::AlignCenter, strValue);
    painter->restore();
    return;
}

void ProgressButton::drawValue(QPainter *painter)
{
    if (value < 0) {
        return;
    }
    if (value > maximum) {
        value = maximum;
    }
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(valueColor);
    double step = value / (maximum - minimum);
    int progress = (width() - padding * 2) * step;
    QRect rect(padding, 0, progress, height());
    painter->drawRoundedRect(rect, radius, radius);
    painter->restore();
    return;
}

void ProgressButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (value < maximum) {
        emit clickedProgressBar();
    } else {
        emit clickedButton();
    }
    return;
}

double ProgressButton::getMinimum() const
{
    return minimum;
}

double ProgressButton::getMaximum() const
{
    return maximum;
}

QString ProgressButton::getText() const
{
    return text;
}

QString ProgressButton::getFinishText() const
{
    return finishText;
}

QColor ProgressButton::getValueColor() const
{
    return valueColor;
}

QColor ProgressButton::getBackgroundColor() const
{
    return backgroundColor;
}

QColor ProgressButton::getTextColor() const
{
    return textColor;
}

QColor ProgressButton::getProgressTextColor() const
{
    return progressTextColor;
}

QColor ProgressButton::getButtonTextColor() const
{
    return buttonTextColor;
}

QBrush ProgressButton::getValueBrush() const
{
    return valueBrush;
}

QFont ProgressButton::getFont() const
{
    return textFont;
}

QPixmap ProgressButton::getPixmap() const
{
    return buttonPixmap;
}

double ProgressButton::getValue() const
{
    return value;
}

int ProgressButton::getPadding() const
{
    return padding;
}

int ProgressButton::getRadius() const
{
    return radius;
}

void ProgressButton::setValue(double value)
{
    this->value = value;
    this->update();
    return;
}

void ProgressButton::setMaximum(double value)
{
    this->maximum = value;
    return;
}

void ProgressButton::setMinimum(double value)
{
    this->minimum = value;
    return;
}

void ProgressButton::setPadding(int value)
{
    this->padding = value;
    return;
}

void ProgressButton::setRadius(int value)
{
    this->radius = value;
    return;
}

void ProgressButton::setFinishText(const QString &text)
{
    this->finishText = text;
    this->update();
    return;
}

void ProgressButton::setText(const QString &text)
{
    this->text = text;
    this->update();
    return;
}

void ProgressButton::setFont(const QFont &font)
{
    this->textFont = font;
    this->update();
    return;
}

void ProgressButton::setValueColor(const QColor &color)
{
    this->valueColor = color;
    this->update();
    return;
}

void ProgressButton::setBackgroundColor(const QColor &color)
{
    this->backgroundColor = color;
    return;
}

void ProgressButton::setTextColor(const QColor &color)
{
    this->textColor = color;
    return;
}

void ProgressButton::setProgressTextColor(const QColor &color)
{
    this->progressTextColor = color;
    return;
}

void ProgressButton::setButtonTextColor(const QColor &color)
{
    this->value = -1;
    this->buttonTextColor = color;
    return;
}

void ProgressButton::setValueBrush(const QBrush &brush)
{
    this->valueBrush = brush;
    return;
}

void ProgressButton::setPixmap(const QPixmap &pixmap)
{
    Q_UNUSED(pixmap)
    return;
}
