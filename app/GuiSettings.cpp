#include "GuiSettings.h"
#include <QToolButton>
#include <QPen>
#if 0
QColor  GuiSettings::sl_bgColor           = QColor(0,0,0);
QColor  GuiSettings::sl_barColor          = QColor(51, 204, 102);
QColor  GuiSettings::sl_squareColor       = QColor(0,0,255);
QColor  GuiSettings::sl_selectedColor     =  QColor(20,20,255);
QColor  GuiSettings::sl_dragTarget        = QColor(255,20,20);
int     GuiSettings::sl_barHeight         = 25;

QColor  GuiSettings::trans_bgColor        = QColor(24,24,22);
QColor  GuiSettings::trans_barColor       = QColor(38,50,95);
int     GuiSettings::trans_barHeight      = 25;
QColor  GuiSettings::trans_segmentColor   = QColor(226,222,170);
QColor  GuiSettings::trans_currentSegmentColor   = QColor(255,200,20);
QFont   GuiSettings::trans_segmentFont    = QFont("Futura", 8, 70, true);
QColor  GuiSettings::trans_segmentTextColor   = QColor(0,0,0);
QColor  GuiSettings::trans_playheadColor   = QColor(255,255,255);
int     GuiSettings::trans_playheadWidth   = 2;
#endif


GuiSettings *guisettings;

GuiSettings::GuiSettings(QWidget *parent) :
    QWidget(parent)
{
    // XXX dunno why this crashes
//    this->setObjectName(QString("guisettings"));
}

void    GuiSettings::editGuiColorsDialog(QObject *parent)
{
    Q_UNUSED(parent);
    //LATER void    GuiColors::editGuiColorsDialog()
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

