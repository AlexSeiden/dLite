#include "GuiSettings.h"
#include <QToolButton>
#include <QPen>
#include <QFile>
#include <QApplication>

GuiSettings *guisettings;

GuiSettings::GuiSettings(QWidget *parent) :
    QWidget(parent)
{
    // XXX dunno why this crashes
//    this->setObjectName(QString("guisettings"));
//    loadStyleSheet();
}

// Utility function
void setButtonColor(QToolButton *colorButton, const QColor &col){
    if(col.isValid()) {
        // I grabbed this bit of black magic code from StackOverflow...
        // seems like the only way to set the color of the button.
        QString qss = QString("background-color: %1").arg(col.name());
        colorButton->setStyleSheet(qss);
    }
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
    guisettings->socketOutlinePen     = QPen(guisettings->socketOutlineColor, 2, Qt::SolidLine);
    guisettings->connectorPen         = QPen(QBrush(guisettings->connectorColor), 2.0, Qt::SolidLine);
    guisettings->connectorPenSelected = QPen(QColor(200,40,40), 4, Qt::SolidLine);
    guisettings->connectorBrush       = QBrush(guisettings->connectorCenterColor);
    guisettings->m_PIfont.setLetterSpacing(QFont::PercentageSpacing, guisettings->m_PIletterspacing);

    this->repaint();
    qApp->processEvents();
}
