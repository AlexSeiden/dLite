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
    static QColor   connectorColor;

    void    editGuiColorsDialog(QObject *parent = 0);
    // QColor colorForType(ParamBase *param);

signals:

public slots:

};

#endif // GUICOLORS_H
