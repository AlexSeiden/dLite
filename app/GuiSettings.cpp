#include "GuiSettings.h"
#include <QToolButton>
#include <QPen>

// GraphWidget & related classes (CuesheetView, NodeItem, SocketItem, ParamItem, ConnectorItem)
QColor  GuiSettings::socketFillColor      = QColor(255,20,80);
QColor  GuiSettings::socketOutlineColor   = QColor(255,255,255);
/*
QColor  GuiSettings::nodeNameColor        = QColor(55,55,255);
QFont   GuiSettings::nodeNameFont         = QFont("Helvetica", 14);
QColor  GuiSettings::nodeBGColor          = QColor(120,100,60);
QColor  GuiSettings::nodeTextColor        = QColor(255,255,255);
*/
QColor  GuiSettings::paramTextColor       = QColor(255,255,255);
QFont   GuiSettings::paramTextFont        = QFont("Helvetica", 14);
QColor  GuiSettings::outputParamFillColor = QColor(100,100,100);
int     GuiSettings::socketWidth          = 16;                 // Width of the socket connector rectangle
int     GuiSettings::paramTextOffset      = 15;
int     GuiSettings::paramEditorOffset    = 80;
QPen    GuiSettings::socketOutlinePen     = QPen(GuiSettings::socketOutlineColor, 2, Qt::SolidLine);
QColor  GuiSettings::connectorCenterColor = QColor(80,80,80);
QColor  GuiSettings::connectorColor       = QColor(20,20,80);
QPen    GuiSettings::connectorPen         = QPen(GuiSettings::connectorColor, 2, Qt::SolidLine);
QPen    GuiSettings::connectorPenSelected = QPen(QColor(200,40,40), 4, Qt::SolidLine);
QBrush  GuiSettings::connectorBrush       = QBrush(GuiSettings::connectorCenterColor);
int     GuiSettings::connectorEndSize     = 5;
int     GuiSettings::zoomIconSize         = 16;
int     GuiSettings::colorChipSize        = 16;

// Dancefloorwidget
QColor  GuiSettings::df_bgColor           = QColor(30,30,80);
QColor  GuiSettings::df_panelSepColor     = QColor(255,255,200);
QColor  GuiSettings::df_cellSepColor      = QColor(100,100,100);
QColor  GuiSettings::df_noCellColor       = QColor(60,60,60);
QColor  GuiSettings::df_woodColor         = QColor(130,123,112);
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
QFont   GuiSettings::sg_HzFont            = QFont("Helvetica", 10);

QColor  GuiSettings::sl_bgColor           = QColor(0,0,0);
QColor  GuiSettings::sl_barColor          = GuiSettings::sg_barColor;
QColor  GuiSettings::sl_squareColor       = QColor(0,0,255);
QColor  GuiSettings::sl_selectedColor     = GuiSettings::sg_sublevelRegion;
QColor  GuiSettings::sl_dragTarget        = QColor(255,20,20);
int     GuiSettings::sl_barHeight         = 25;

QColor  GuiSettings::trans_bgColor        = QColor(24,24,22);
QColor  GuiSettings::trans_barColor       = QColor(38,50,95);
int     GuiSettings::trans_barHeight      = 25;
QColor  GuiSettings::trans_segmentColor   = QColor(226,222,170);
QColor  GuiSettings::trans_currentSegmentColor   = QColor(255,200,20);
//QFont   GuiSettings::trans_segmentFont    = QFont("Source Code Pro", 9, 99);
//QFont   GuiSettings::trans_segmentFont    = QFont("Avenir Heavy Oblique", 9, 50);
QFont   GuiSettings::trans_segmentFont    = QFont("Futura", 8, 70, true);
QColor  GuiSettings::trans_segmentTextColor   = QColor(0,0,0);
QColor  GuiSettings::trans_playheadColor   = QColor(255,255,255);
int     GuiSettings::trans_playheadWidth   = 2;


GuiSettings *guisettings;

GuiSettings::GuiSettings(QWidget *parent) :
    QWidget(parent)
{
    this->setObjectName(QString("guisettings"));
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

