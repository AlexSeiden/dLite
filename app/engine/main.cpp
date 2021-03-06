// Copyright (C) 2014-2016 Alex Seiden 
//
#include "views/main_widget.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("dLite");
    app.setStyle(QStyleFactory::create("Fusion"));

    MainWidget mainwidget;
    mainwidget.show();


    return app.exec();
}
