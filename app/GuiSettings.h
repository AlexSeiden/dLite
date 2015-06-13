#ifndef GUISETTINGS_H
#define GUISETTINGS_H

#include <QWidget>
#include <QColor>

QT_BEGIN_NAMESPACE
class QToolButton;
class QPen;
QT_END_NAMESPACE


// Singleton class to maintain color values for the UI.
// Derived from QObject so we can used signals & slots
class GuiSettings : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor PIbgcolor MEMBER m_PIbgcolor)

public:
    explicit GuiSettings(QWidget *parent = 0);
    // Graph widget
    static QColor   nodeNameColor;
    static QFont    nodeNameFont;
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
    static QFont    paramTextFont;
    static int      socketWidth;
    static int      paramHeight;
    static int      paramTextOffset;
    static int      paramEditorOffset;
    static QPen     socketOutlinePen;
    static QColor   connectorColor;
    static QColor   connectorCenterColor;
    static QPen     connectorPen;
    static QPen     connectorPenSelected;
    static QBrush   connectorBrush;
    static int      connectorEndSize;
    static int      zoomIconSize;
    static int      colorChipSize;

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
    static QFont   sg_HzFont;

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
    static int     trans_playheadWidth;
    static QColor  trans_playheadColor;
    static QColor  trans_segmentColor;
    static QColor  trans_segmentTextColor;
    static QColor  trans_currentSegmentColor;
    static QFont   trans_segmentFont;

    // Stylesheet
    QColor m_PIbgcolor;



    void    editGuiColorsDialog(QObject *parent = 0);
    // QColor colorForType(ParamBase *param);
};

extern GuiSettings *guisettings;

// Free function prototypes

// Utility function to set bg color on toolbar button
void setButtonColor(QToolButton *colorButton, const QColor &col);

#endif // GUISETTINGS_H
