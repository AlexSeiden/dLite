#ifndef WHIP_H
#define WHIP_H

#include "Param.h"
#include <QToolButton>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

// TODO extend whip to other datatypes
class Whip : public QToolButton
{
    Q_OBJECT

public:
    explicit Whip(ParamBase *param, QWidget *parent = 0, bool inputConnection=true);

    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

signals:

public slots:

private:
    static QPixmap  *whipDisconnectedPixmap;
    static QIcon    *whipDisconnectedIcon;
    static QPixmap  *whipConnectedPixmap;
    static QIcon    *whipConnectedIcon;
    static void     initIcons();

    QPixmap         *_pixmap;
    ParamBase*      _param;
    const std::type_info & _paramType;
    bool            _dragTarget;
    bool            _inputConnection;
};

#endif // WHIP_H
