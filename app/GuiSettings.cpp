#include "GuiSettings.h"
#include <QToolButton>
#include <QPen>
#include <QFile>
#include <QApplication>

GuiSettings *guisettings;

GuiSettings::GuiSettings(QWidget *parent) :
    QWidget(parent)
{
    this->setObjectName(QString("guisettings"));
}

void GuiSettings::loadStyleSheet()
{
    QFile file("/Users/alex/src/dLite/app/dLite.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);

    // Need to run process events here so that custom properties will get updated.
    qApp->processEvents(QEventLoop::AllEvents, 1000);

    // Derived
    guisettings->m_socketOutlinePen =
            QPen(guisettings->m_socketOutlineColor,
                 guisettings->m_socketOutlineWidth,
                 Qt::SolidLine);
    guisettings->m_connectorPen         =
            QPen(QBrush(guisettings->m_connectorColor),
                 guisettings->m_connectorWidth,
                 Qt::SolidLine);
    guisettings->m_connectorPenSelected =
            QPen(guisettings->m_connectorSelectedColor,
                 guisettings->m_connectorSelectedWidth,
                 Qt::SolidLine);
    guisettings->m_connectorBrush       =
            QBrush(guisettings->m_connectorCenterColor);

    // XXX not sure why this is getting applied inconsistantly/incorrectly:
    //    guisettings->m_PIfont.setLetterSpacing(QFont::PercentageSpacing,
    //                                           guisettings->m_PIletterspacing);

    qApp->processEvents();
}

// Utility function
void setButtonColor(QToolButton *colorButton, const QColor &col){
    if(col.isValid()) {
        QString qss = QString("background-color: %1").arg(col.name());
        colorButton->setStyleSheet(qss);
    }
}
