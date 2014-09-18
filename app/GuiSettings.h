#ifndef GUISETTINGS_H
#define GUISETTINGS_H

#include <QObject>
#include <QColor>

QT_BEGIN_NAMESPACE
class QToolButton;
class QPen;
QT_END_NAMESPACE


// Singleton class to maintain color values for the UI.
// Derived from QObject so we can used signals & slots
class GuiSettings : public QObject
{
    Q_OBJECT
public:
    explicit GuiSettings(QObject *parent = 0);
    // Graph widget
    static QColor   nodeNameColor;
    static QColor   nodeBGColor;
    static QColor   nodeTextColor;
    static int      nodeWidth;
    static int      nodeSpacing;
    static QColor   selectedNodePenColor;
    static int      selectedNodePenWidth;
    static QColor   socketFillColor;
    static QColor   socketOutlineColor;
    static QColor   paramFillColor;
    static QColor   outputParamFillColor;
    static QColor   paramTextColor;
    static int      socketWidth;
    static int      paramHeight;
    static QPen     socketOutlinePen;
    static QColor   connectorColor;
    static QColor   connectorCenterColor;
    static QPen     connectorPen;
    static QBrush   connectorBrush;
    static int      connectorEndSize;
    static int      iconSize;

    // Dance floor widget
    static QColor  df_cellSepColor;
    static QColor  df_bgColor;
    static QColor  df_panelSepColor;
    static QColor  df_noCellColor;
    static QColor  df_woodColor;
    static int     df_cellsize;
    static int     df_cellspace;

    // Spectrograph widget
    static QColor  sg_barColor;
    static QColor  sg_lineColor;
    static QColor  sg_clipColor;
    static QColor  sg_gridColor;
    static QColor  sg_bg;
    static QColor  sg_textColor;
    static QColor  sg_sublevelRegion;
    static int     sg_sublevelPenwidth;
    static int     sg_textHeight;
    static int     sg_textOffset;

    // Sublevel meter
    static QColor  sl_bgColor;
    static QColor  sl_barColor;
    static QColor  sl_squareColor;
    static QColor  sl_selectedColor;
    static QColor  sl_dragTarget;
    static int     sl_barHeight;

    // Transport
    static QColor  trans_bgColor;
    static QColor  trans_barColor;
    static int     trans_barHeight;




    void    editGuiColorsDialog(QObject *parent = 0);
    // QColor colorForType(ParamBase *param);
};


// Free function prototypes

// Utility function to set bg color on toolbar button
void setButtonColor(QToolButton *colorButton, const QColor &col);

#endif // GUISETTINGS_H
