#include "Whip.h"
#include <QDebug>

QPixmap *Whip::whipDisconnectedPixmap = nullptr;
QIcon   *Whip::whipDisconnectedIcon = nullptr;
QPixmap *Whip::whipConnectedPixmap = nullptr;
QIcon   *Whip::whipConnectedIcon = nullptr;

void Whip::initIcons() {
    whipDisconnectedPixmap = new QPixmap(":/images/whip_disconnected.png");
    whipDisconnectedIcon = new QIcon(*whipDisconnectedPixmap);
    whipConnectedPixmap = new QPixmap(":/images/whip_connected.png");
    whipConnectedIcon = new QIcon(*whipConnectedPixmap);
}


/*
 * The "Whip" is the little icon you can use to connect parameters to drivers,
 * like the whip used in Adobe After Effects.
 */

Whip::Whip(ParamBase *param, QWidget *parent, bool inputConnection) :
    QToolButton(parent),
    _param(param),
    _paramType(typeid(*param)),
    _dragTarget(false),
    _inputConnection(inputConnection)
{
    if (! whipDisconnectedPixmap)
        initIcons();

    setAcceptDrops(true);
    _pixmap = whipDisconnectedPixmap;
    setIcon(*whipDisconnectedIcon);
    setEnabled(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(16,16);
}

// Initiate drag event on mouse press.
// TODO: on click & release, bring up connected object.
// on click & drag, initiate drag&drop.
void Whip::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    QDrag *drag = new QDrag(this);
    drag->setPixmap(*_pixmap);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(_paramType.name(), itemData);

    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec();
    if (dropAction == Qt::IgnoreAction)
        return;

    // This will be the widget that accepted the drag:
    QObject *target = drag->target();

    this->setIcon(*whipConnectedIcon);
    this->_pixmap = whipConnectedPixmap;

    // Right now, whoever receives the drop sets up the connection.
    // Alternatives:
    //  a) use signals & slots?
    //  b) pass the param handle through mime?
    //  c) pass the current drag/drop initiator up to the controller,
    //     since by definition THERE CAN BE ONLY ONE (drag/drop going on at once,
    //     that is).
}


// Process Drag/drops intiated by other widgets, to this connection:
void Whip::dragEnterEvent(QDragEnterEvent *event)
{
    // Check for correct format
    if (! event->mimeData()->hasFormat(_paramType.name()))
        return;

    // Don't accept drag from yourself!
    if (event->source() == this)
        return;

    event->acceptProposedAction();

    // Set drag target to "true" so that can distinguish droppable
    // state by icon look.
    _dragTarget = true;
    update();

}

void Whip::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    _dragTarget = false;
    update();
}

void Whip::dropEvent(QDropEvent *event)
{
    // TODO distinguish between data source & sink
    if (! event->mimeData()->hasFormat(_paramType.name()))
        return;

    event->acceptProposedAction();
    // TODO emit signal that drop is accepted
    // create closure for value, and return it.
    // or is it better to just pass the param handle through mime?

#if 0
    Param<float> *param = getFromStream();
    param->setProvider(createProviderClosure());
#endif

    // Once drop has happened, special draw mode can end.
    _dragTarget = false;
    update();
}
