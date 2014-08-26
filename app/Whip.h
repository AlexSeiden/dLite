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

    enum datatype_t {FLOAT, INT, COLOR };

public:
    explicit Whip(Param<float> *param, QWidget *parent = 0);  // TODO extend to other datatypes

    void mousePressEvent(QMouseEvent *event);

signals:

public slots:

private:
    QPixmap          m_pixmap;
    Param<float>*    m_destination;
    datatype_t       m_datatype;

};

#endif // WHIP_H
