#ifndef WHIP_H
#define WHIP_H

#include "Param.h"
#include <QToolButton>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

class Whip : public QToolButton
{
    Q_OBJECT

public:
    explicit Whip(ParamBase *param, QWidget *parent = 0);  // TODO extend to other datatypes

    void mousePressEvent(QMouseEvent *event);

signals:

public slots:

private:
    QPixmap          m_pixmap;
    ParamBase*    m_destination;
};

#endif // WHIP_H
