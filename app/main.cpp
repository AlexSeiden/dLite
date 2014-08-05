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


    return app.exec();
}
