#include "mainwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("dLite");

    MainWidget w;
    w.show();


    return app.exec();
}
