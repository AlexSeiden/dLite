#include "GuiColors.h"
#include <QToolButton>

QColor  GuiColors::connectorColor       = QColor(20,20,80);
QColor  GuiColors::nodeBGColor          = QColor(120,100,60);
QColor  GuiColors::nodeTextColor        = QColor(255,255,255);
QColor  GuiColors::selectedNodePenColor = QColor(255,100,100);
int     GuiColors::selectedNodePenWidth = 2;

QColor  GuiColors::df_bgColor           = QColor(30,30,80);
QColor  GuiColors::df_panelSepColor     = QColor(255,255,200);
QColor  GuiColors::df_cellSepColor      = QColor(100,100,100);
QColor  GuiColors::df_noCellColor       = QColor(60,60,60);

QColor  GuiColors::sg_barColor          = QColor(51, 204, 102);
QColor  GuiColors::sg_lineColor         = GuiColors::sg_barColor.lighter();
QColor  GuiColors::sg_clipColor         = QColor(255, 255, 0);
QColor  GuiColors::sg_gridColor         = GuiColors::sg_barColor.darker();
QColor  GuiColors::sg_bg                = QColor(0,0,0);
QColor  GuiColors::sg_textColor         = QColor(230,230,255);
QColor  GuiColors::sg_sublevelRegion    = QColor(20,20,255);

QColor  GuiColors::sl_bgColor           = QColor(0,0,0);
QColor  GuiColors::sl_barColor          = GuiColors::sg_barColor;
QColor  GuiColors::sl_squareColor       = QColor(0,0,255);
QColor  GuiColors::sl_selectedColor     = GuiColors::sg_sublevelRegion;
QColor  GuiColors::sl_dragTarget        = QColor(255,20,20);

GuiColors::GuiColors(QObject *parent) :
    QObject(parent)
{ }

void    GuiColors::editGuiColorsDialog(QObject *parent)
{
    //TODO void    GuiColors::editGuiColorsDialog()
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
