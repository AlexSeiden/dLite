#ifndef GUISETTINGS_H
#define GUISETTINGS_H

#include <QWidget>
#include <QColor>
#include <QPen>

QT_BEGIN_NAMESPACE
class QToolButton;
QT_END_NAMESPACE


// Singleton class to maintain color values for the UI.
// Derived from QObject so we can used signals & slots
class GuiSettings : public QWidget
{
    Q_OBJECT
    // Node properties
    Q_PROPERTY(QColor    NNNameColor        MEMBER m_NNNameColor)
    Q_PROPERTY(QFont     NNNameFont         MEMBER m_NNNameFont)
    Q_PROPERTY(QColor    NNBGColor          MEMBER m_NNBGColor)
    Q_PROPERTY(QColor    NNTextColor        MEMBER m_NNTextColor)
    Q_PROPERTY(QColor    NNselectedColor    MEMBER m_NNselectedColor)
    Q_PROPERTY(int       NNselectedWidth    MEMBER m_NNselectedWidth)
    Q_PROPERTY(int       NNWidth            MEMBER m_NNWidth)
    Q_PROPERTY(int       NNSpacing          MEMBER m_NNSpacing)
    Q_PROPERTY(int       NNborderRadius     MEMBER m_NNborderRadius)
    Q_PROPERTY(int       NNselectedBorderRadius     MEMBER m_NNselectedBorderRadius)

    // ParamItem properties
    Q_PROPERTY(QColor    PIbgcolor          MEMBER m_PIbgcolor)
    Q_PROPERTY(QColor    PIoutputbgcolor    MEMBER m_PIoutputbgcolor)
    Q_PROPERTY(int       PIheight           MEMBER m_PIheight)
    Q_PROPERTY(QFont     PIfont             MEMBER m_PIfont)
    Q_PROPERTY(QColor    PIfontcolor        MEMBER m_PIfontcolor)
    Q_PROPERTY(int      paramTextOffset     MEMBER paramTextOffset)
    Q_PROPERTY(int      paramEditorOffset   MEMBER paramEditorOffset)

    Q_PROPERTY(QColor   socketFillColor     MEMBER m_socketFillColor)
    Q_PROPERTY(QColor   socketOutlineColor  MEMBER socketOutlineColor )
    Q_PROPERTY(int      socketWidth         MEMBER m_socketWidth)
    Q_PROPERTY(QColor   connectorColor      MEMBER connectorColor)
    Q_PROPERTY(QColor   connectorCenterColor    MEMBER connectorCenterColor)
    Q_PROPERTY(int      connectorEndSize    MEMBER connectorEndSize)
    Q_PROPERTY(int      zoomIconSize        MEMBER zoomIconSize)

    // Dance floor widget
    Q_PROPERTY(QColor  df_cellSepColor      MEMBER df_cellSepColor)
    Q_PROPERTY(QColor  df_bgColor           MEMBER df_bgColor)
    Q_PROPERTY(QColor  df_panelSepColor     MEMBER df_panelSepColor)
    Q_PROPERTY(QColor  df_noCellColor       MEMBER df_noCellColor)
    Q_PROPERTY(QColor  df_woodColor         MEMBER df_woodColor)
    Q_PROPERTY(int     df_cellsize          MEMBER df_cellsize)
    Q_PROPERTY(int     df_cellspace         MEMBER df_cellspace)

public:
    // Stylesheet
    // NodeItem
    QColor    m_NNNameColor;
    QFont     m_NNNameFont ;
    QColor    m_NNBGColor  ;
    int       m_NNWidth    ;
    int       m_NNSpacing  ;
    int       m_NNselectedWidth;
    QColor    m_NNselectedColor;
    QColor    m_NNTextColor;
    int       m_NNborderRadius;
    int       m_NNselectedBorderRadius;

    // ParamItem
    QColor    m_PIbgcolor;
    QColor    m_PIoutputbgcolor;
    int       m_PIheight;
    QFont     m_PIfont ;
    QColor    m_PIfontcolor;

    QColor   m_socketFillColor;
    QColor   socketOutlineColor ;
    QColor   outputParamFillColor ;
    int      m_socketWidth;
    int      paramTextOffset;
    int      paramEditorOffset;
    QColor   connectorColor;
    QColor   connectorCenterColor;
    int      connectorEndSize;
    int      zoomIconSize;
    int      colorChipSize;
    // Derived
    QPen     socketOutlinePen;
    QPen     connectorPen;
    QPen     connectorPenSelected;
    QBrush   connectorBrush;

    // Dance floor widget
    QColor  df_cellSepColor;
    QColor  df_bgColor;
    QColor  df_panelSepColor;
    QColor  df_noCellColor;
    QColor  df_woodColor;
    int     df_cellsize;
    int     df_cellspace;
    

    explicit GuiSettings(QWidget *parent = 0);

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


    void    editGuiColorsDialog(QObject *parent = 0);
    // QColor colorForType(ParamBase *param);
};

extern GuiSettings *guisettings;

// Free function prototypes

// Utility function to set bg color on toolbar button
void setButtonColor(QToolButton *colorButton, const QColor &col);

#endif // GUISETTINGS_H
