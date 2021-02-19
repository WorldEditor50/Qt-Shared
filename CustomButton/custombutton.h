#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QIcon>
#include <QPixmap>

class CustomButtonPrivate;
class CustomButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ getText WRITE setText)
    Q_PROPERTY(QRect textPosition READ getTextPosition WRITE setTextPosition)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
    Q_PROPERTY(QFont textFont READ getTextFont WRITE setTextFont)
    Q_PROPERTY(QPixmap pixmap READ getPixmap WRITE setPixmap)
    Q_PROPERTY(QRectF imagePosition READ getImagePosition WRITE setImagePosition)
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(bool isFlat READ getIsFlat WRITE setFlat)
    Q_PROPERTY(int pading READ getPadding WRITE setPadding)
    Q_PROPERTY(int radius READ getRadius WRITE setRadius)
public:
    explicit CustomButton(QWidget *parent = nullptr);
    ~CustomButton();
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void drawBackground(QPainter &painter);
    void drawText(QPainter &painter);
    void drawImage(QPainter &painter);
    QString getText() const;
    QRect getTextPosition() const;
    QColor getTextColor() const;
    QFont getTextFont() const;
    QPixmap getPixmap() const;
    QRectF getImagePosition() const;
    QColor getBackgroundColor() const;
    int getPadding() const;
    int getRadius() const;
    bool getIsFlat() const;
Q_SIGNALS:
    void clicked();
public Q_SLOTS:
    void setText(const QString &text);
    void setTextPosition(const QRect & pos);
    void setTextColor(const QColor &color);
    void setTextFont(const QFont &font);
    void setPixmap(const QPixmap &pixmap);
    void setImagePosition(const QRectF &pos);
    void setBackgroundColor(const QColor &color);
    void setFlat(bool on);
    void setPadding(int pad);
    void setRadius(int rad);
private:
    QString text;
    QColor textColor;
    QRect textPosition;
    QFont textFont;
    QPixmap pixmap;
    QRectF imagePosition;
    QColor backgroundColor;
    int padding;
    int radius;
    bool isFlat;
    Q_DECLARE_PRIVATE(CustomButton)
    Q_DISABLE_COPY(CustomButton)
};


#endif // CUSTOMBUTTON_H
