#include "GuiColors.h"
#include <QToolButton>
#include <QPen>

// GraphWidget aka CuesheetView
QColor  GuiSettings::socketFillColor      = QColor(255,20,80);
QColor  GuiSettings::socketOutlineColor   = QColor(255,255,255);
QColor  GuiSettings::connectorColor       = QColor(20,20,80);
QColor  GuiSettings::nodeNameColor        = QColor(55,55,255);
QColor  GuiSettings::nodeBGColor          = QColor(120,100,60);
QColor  GuiSettings::nodeTextColor        = QColor(255,255,255);
QColor  GuiSettings::paramTextColor       = QColor(255,255,255);
QColor  GuiSettings::paramFillColor       = QColor(120,100,60);
QColor  GuiSettings::outputParamFillColor = QColor(100,100,100);
int     GuiSettings::nodeWidth            = 230;                // Width of the entire node
int     GuiSettings::nodeSpacing          = 20;                 // Space between nodes
int     GuiSettings::socketWidth          = 16;                 // Width of the socket connector rectangle
int     GuiSettings::paramHeight          = 25;                 // Height of each param
QColor  GuiSettings::selectedNodePenColor = QColor(255,100,100);
int     GuiSettings::selectedNodePenWidth = 2;
QPen    GuiSettings::socketOutlinePen   = QPen(GuiSettings::socketOutlineColor, 2, Qt::SolidLine);
QPen    GuiSettings::connectorPen       = QPen(GuiSettings::connectorColor, 2, Qt::SolidLine);
int     GuiSettings::connectorEndSize     = 5;
int     GuiSettings::iconSize             = 16;

// Dancefloorwidget
QColor  GuiSettings::df_bgColor           = QColor(30,30,80);
QColor  GuiSettings::df_panelSepColor     = QColor(255,255,200);
QColor  GuiSettings::df_cellSepColor      = QColor(100,100,100);
QColor  GuiSettings::df_noCellColor       = QColor(60,60,60);
int     GuiSettings::df_cellsize          = 20;
int     GuiSettings::df_cellspace         = 4;

// Spectrograph
QColor  GuiSettings::sg_barColor          = QColor(51, 204, 102);
QColor  GuiSettings::sg_lineColor         = GuiSettings::sg_barColor.lighter();
QColor  GuiSettings::sg_clipColor         = QColor(255, 255, 0);
QColor  GuiSettings::sg_gridColor         = GuiSettings::sg_barColor.darker();
QColor  GuiSettings::sg_bg                = QColor(0,0,0);
QColor  GuiSettings::sg_textColor         = QColor(230,230,255);
QColor  GuiSettings::sg_sublevelRegion    = QColor(20,20,255);
int     GuiSettings::sg_sublevelPenwidth  = 2;
int     GuiSettings::sg_textHeight        = 20;
int     GuiSettings::sg_textOffset        = GuiSettings::sg_textHeight/4;

QColor  GuiSettings::sl_bgColor           = QColor(0,0,0);
QColor  GuiSettings::sl_barColor          = GuiSettings::sg_barColor;
QColor  GuiSettings::sl_squareColor       = QColor(0,0,255);
QColor  GuiSettings::sl_selectedColor     = GuiSettings::sg_sublevelRegion;
QColor  GuiSettings::sl_dragTarget        = QColor(255,20,20);
int     GuiSettings::sl_barHeight         = 25;

GuiSettings::GuiSettings(QObject *parent) :
    QObject(parent)
{ }

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
