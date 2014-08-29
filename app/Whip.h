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
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

signals:

public slots:

private:
    QPixmap         _pixmap;
    ParamBase*      _param;
    const std::type_info & _paramType;
    bool            _dragTarget;
};

#endif // WHIP_H
