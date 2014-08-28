#include "Whip.h"
#include <QDebug>

Whip::Whip(ParamBase *param, QWidget *parent) : // TODO extend to other datatypes
    QToolButton(parent),
    m_destination(param)
{
    setAcceptDrops(true);
    m_pixmap = QPixmap(":/images/whip3.png");
    setIcon(QIcon(":/images/whip3.png"));
    setEnabled(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(16,16);
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
