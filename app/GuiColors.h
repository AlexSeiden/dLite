#ifndef GUICOLORS_H
#define GUICOLORS_H

#include <QObject>
#include <QColor>

// Singleton class to maintain color values for the UI.
// Derived from QObject so we can used signals & slots
class GuiColors : public QObject
{
    Q_OBJECT
public:
    explicit GuiColors(QObject *parent = 0);
    // Graph widget
    static QColor   connectorColor;
    static QColor   nodeBGColor;
    static QColor   nodeTextColor;
    static QColor   selectedNodePenColor;
    static int      selectedNodePenWidth;

    // Dance floor widget
    static QColor  df_cellSepColor;
    static QColor  df_bgColor;
    static QColor  df_panelSepColor;
    static QColor  df_noCellColor;

    // Spectrograph widget
    static QColor  sg_barColor;
    static QColor  sg_lineColor;
    static QColor  sg_clipColor;
    static QColor  sg_gridColor;
    static QColor  sg_bg;
    static QColor  sg_textColor;
    static QColor  sg_sublevelRegion;

    // Sublevel meter
    static QColor  sl_bgColor;
    static QColor  sl_barColor;
    static QColor  sl_squareColor;
    static QColor  sl_selectedColor;
    static QColor  sl_dragTarget;


    void    editGuiColorsDialog(QObject *parent = 0);
    // QColor colorForType(ParamBase *param);
};


// Free function prototypes

// Utility function to set bg color on toolbar button
void setButtonColor(QToolButton *colorButton, const QColor &col);

#endif // GUICOLORS_H
