#include "Whip.h"
#include <QDebug>

Whip::Whip(Param<float> *param, QWidget *parent) : // TODO extend to other datatypes
    QToolButton(parent),
    m_destination(param)
{
    setAcceptDrops(true);
    m_pixmap = QPixmap(":/images/record.png");
    setIcon(QIcon(":/images/record.png"));
    setEnabled(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(30,30);
}

void
Whip::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    QDrag *drag = new QDrag(this);
    drag->setPixmap(m_pixmap);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << m_destination;  // TODO register this with moc

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("whipConnection/float", itemData);
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec();
    Q_UNUSED(dropAction);

    // TODO connect to receive signal from whoever gets drop--
    // or is it better to just pass the param handle through mime?
}
