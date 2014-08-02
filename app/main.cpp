#include "mainwidget.h"
#include "dancefloormodel.h"
#include "dancefloorwidget.h"
#include "controlpanel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Qt Multimedia spectrum analyzer");

    MainWidget w;
    w.show();

    Dancefloormodel *dfModel = new Dancefloormodel();
    dfModel->ImportLayout("/Users/alex/src/floorit/layout.csv");

    Dancefloorwidget *dfw = new Dancefloorwidget();
    dfw->setModel(dfModel);
    dfw->show();

    Controlpanel *cp = new Controlpanel();
    cp->show();

    return app.exec();
}
