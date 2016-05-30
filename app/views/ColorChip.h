#ifndef COLORCHIP_H
#define COLORCHIP_H

#include <QToolButton>
#include <QPushButton>
#include "engine/lightcolor.h"

class ColorChip : public QToolButton
{
    Q_OBJECT
public:
    explicit ColorChip(QWidget *parent = 0);

signals:
    void valueChanged(Lightcolor);

public slots:
    void launchColorDialog();
    void setButtonColor(const QColor color);

private:
    QColor  m_color;
};

#endif // COLORCHIP_H
