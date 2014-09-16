#include "mainwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("dLite");

    MainWidget mainwidget;
    mainwidget.show();


    return app.exec();
}
