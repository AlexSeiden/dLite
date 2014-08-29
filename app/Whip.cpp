#include "Whip.h"
#include <QDebug>

Whip::Whip(ParamBase *param, QWidget *parent) :
    QToolButton(parent),
    _param(param),
    _paramType(typeid(*param)),
    _dragTarget(false)
{
    setAcceptDrops(true);
    _pixmap = QPixmap(":/images/whip3.png");
    setIcon(QIcon(":/images/whip3.png"));
    setEnabled(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(16,16);
}

/*
 * Start drag event on mouse press.
 */
void
Whip::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    QDrag *drag = new QDrag(this);
    drag->setPixmap(_pixmap);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << _param;  // TODO register this with moc

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(_paramType.name(), itemData);

    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec();
    Q_UNUSED(dropAction);

    // TODO connect to receive signal from whoever gets drop--
    // or is it better to just pass the param handle through mime?
}

// Drag & drop from other connections, to this connection:

void Whip::dragEnterEvent(QDragEnterEvent *event)
{
    if (! event->mimeData()->hasFormat(_paramType.name()))
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
