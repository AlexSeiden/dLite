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
    Q_PROPERTY(qreal     PIletterspacing    MEMBER m_PIletterspacing)
    Q_PROPERTY(int       paramTextOffset    MEMBER paramTextOffset)
    Q_PROPERTY(int       paramEditorOffset  MEMBER paramEditorOffset)
    Q_PROPERTY(int       paramBorderRadius  MEMBER paramBorderRadius)
    Q_PROPERTY(int       paramBorderWidth   MEMBER paramBorderWidth)
    Q_PROPERTY(QColor    paramBorderColor   MEMBER paramBorderColor)

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

    // Spectrograph widget
    Q_PROPERTY(QColor  sg_barColor          MEMBER sg_barColor)
    Q_PROPERTY(QColor  sg_lineColor         MEMBER sg_lineColor)
    Q_PROPERTY(QColor  sg_clipColor         MEMBER sg_clipColor)
    Q_PROPERTY(QColor  sg_gridColor         MEMBER sg_gridColor)
    Q_PROPERTY(QColor  sg_bg                MEMBER sg_bg)
    Q_PROPERTY(QColor  sg_textColor         MEMBER sg_textColor)
    Q_PROPERTY(QColor  sg_sublevelRegion    MEMBER sg_sublevelRegion)
    Q_PROPERTY(int     sg_sublevelPenwidth  MEMBER sg_sublevelPenwidth)
    Q_PROPERTY(int     sg_textHeight        MEMBER sg_textHeight)
    Q_PROPERTY(int     sg_textOffset        MEMBER sg_textOffset)
    Q_PROPERTY(QFont   sg_HzFont            MEMBER sg_HzFont)

    // Sublevel meter
    Q_PROPERTY(QColor  sl_bgColor               MEMBER sl_bgColor)
    Q_PROPERTY(QColor  sl_barColor              MEMBER sl_barColor)
    Q_PROPERTY(QColor  sl_squareColor           MEMBER sl_squareColor)
    Q_PROPERTY(QColor  sl_selectedColor         MEMBER sl_selectedColor)
    Q_PROPERTY(QColor  sl_dragTarget            MEMBER sl_dragTarget)
    Q_PROPERTY(int     sl_barHeight             MEMBER sl_barHeight)


    Q_PROPERTY(QColor  trans_bgColor            MEMBER trans_bgColor)
    Q_PROPERTY(QColor  trans_barColor           MEMBER trans_barColor)
    Q_PROPERTY(int     trans_barHeight          MEMBER trans_barHeight)
    Q_PROPERTY(int     trans_playheadWidth      MEMBER trans_playheadWidth)
    Q_PROPERTY(QColor  trans_playheadColor      MEMBER trans_playheadColor)
    Q_PROPERTY(QColor  trans_segmentColor       MEMBER trans_segmentColor)
    Q_PROPERTY(QColor  trans_segmentTextColor   MEMBER trans_segmentTextColor)
    Q_PROPERTY(QColor  trans_currentSegmentColor MEMBER trans_currentSegmentColor)
    Q_PROPERTY(QFont   trans_segmentFont        MEMBER trans_segmentFont)


public:
    explicit GuiSettings(QWidget *parent = 0);

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
    qreal     m_PIletterspacing ;
    QColor    m_PIfontcolor;
    int      paramTextOffset;
    int      paramEditorOffset;
    int      paramBorderRadius;
    int      paramBorderWidth;
    QColor   paramBorderColor;

    QColor   m_socketFillColor;
    QColor   socketOutlineColor ;
    QColor   outputParamFillColor ;
    int      m_socketWidth;
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

    // Spectrograph widget
    QColor  sg_barColor;
    QColor  sg_lineColor;
    QColor  sg_clipColor;
    QColor  sg_gridColor;
    QColor  sg_bg;
    QColor  sg_textColor;
    QColor  sg_sublevelRegion;
    int     sg_sublevelPenwidth;
    int     sg_textHeight;
    int     sg_textOffset;
    QFont   sg_HzFont;

    // Sublevel meter
    QColor  sl_bgColor;
    QColor  sl_barColor;
    QColor  sl_squareColor;
    QColor  sl_selectedColor;
    QColor  sl_dragTarget;
    int     sl_barHeight;

    // Transport
    QColor  trans_bgColor;
    QColor  trans_barColor;
    int     trans_barHeight;
    int     trans_playheadWidth;
    QColor  trans_playheadColor;
    QColor  trans_segmentColor;
    QColor  trans_segmentTextColor;
    QColor  trans_currentSegmentColor;
    QFont   trans_segmentFont;

    // QColor colorForType(ParamBase *param);

public slots:
    void loadStyleSheet();
};

extern GuiSettings *guisettings;

// Free function prototypes

// Utility function to set bg color on toolbar button
void setButtonColor(QToolButton *colorButton, const QColor &col);

#endif // GUISETTINGS_H
