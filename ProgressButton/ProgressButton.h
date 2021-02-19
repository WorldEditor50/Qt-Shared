#ifndef PROGRESSBUTTON_H
#define PROGRESSBUTTON_H
#include <QtUiPlugin/QDesignerExportWidget>
#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QColor>
#include <QBrush>
#include <QPainter>

class ProgressButtonPrivate;

class QDESIGNER_WIDGET_EXPORT ProgressButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double minimum READ getMinimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ getMaximum WRITE setMaximum)
    Q_PROPERTY(double value READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int padding READ getPadding WRITE setPadding)
    Q_PROPERTY(int radius READ getRadius WRITE setRadius)
    Q_PROPERTY(QString text READ getText WRITE setText)
    Q_PROPERTY(QString finishText READ getFinishText WRITE setFinishText)
    Q_PROPERTY(QFont textFont READ getFont WRITE setFont)
    Q_PROPERTY(QColor valueColor READ getValueColor WRITE setValueColor)
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
    Q_PROPERTY(QColor progressTextColor READ getProgressTextColor WRITE setProgressTextColor)
    Q_PROPERTY(QColor buttonTextColor READ getButtonTextColor WRITE setButtonTextColor)
    Q_PROPERTY(QBrush valueBrush READ getValueBrush WRITE setValueBrush)
    Q_PROPERTY(QPixmap buttonPixmap READ getPixmap WRITE setPixmap)
public:
    explicit ProgressButton(QWidget *parent = nullptr);
    ~ProgressButton();
    double getMinimum() const;
    double getMaximum() const;
    double getValue() const;
    int getPadding() const;
    int getRadius() const;
    QString getText() const;
    QString getFinishText() const;
    QColor getValueColor() const;
    QColor getBackgroundColor() const;
    QColor getTextColor() const;
    QColor getProgressTextColor() const;
    QColor getButtonTextColor() const;
    QBrush getValueBrush() const;
    QFont getFont() const;
    QPixmap getPixmap() const;
protected:
    void paintEvent(QPaintEvent *event) override;
    void drawBackgroud(QPainter* painter);
    void drawText(QPainter* painter);
    void drawValue(QPainter* painter);
    void mousePressEvent(QMouseEvent *event) override;
public Q_SLOTS:
    void setValue(double value);
    void setMaximum(double value);
    void setMinimum(double value);
    void setPadding(int value);
    void setRadius(int value);
    void setText(const QString &text);
    void setFinishText(const QString &text);
    void setFont(const QFont &font);
    void setValueColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setProgressTextColor(const QColor &color);
    void setButtonTextColor(const QColor &color);
    void setValueBrush(const QBrush &brush);
    void setPixmap(const QPixmap &pixmap);
Q_SIGNALS:
    void valueChanged(int value);
    void clickedProgressBar();
    void clickedButton();
private:
    double minimum;
    double maximum;
    double value;
    int padding;
    int radius;
    QString text;
    QString finishText;
    QFont textFont;
    QColor valueColor;
    QColor backgroundColor;
    QColor textColor;
    QColor progressTextColor;
    QColor buttonTextColor;
    QBrush valueBrush;
    QPixmap buttonPixmap;
    Q_DECLARE_PRIVATE(ProgressButton)
    Q_DISABLE_COPY(ProgressButton)
};


#endif // PROGRESSBUTTON_H
