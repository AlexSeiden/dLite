#ifndef DANCEFLOORCONTROLLER_H
#define DANCEFLOORCONTROLLER_H

#include <QObject>
#include "dancefloormodel.h"
#include "dancefloorwidget.h"

class DancefloorController : public QObject
{
   Q_OBJECT

public:
    explicit DancefloorController(QObject *parent = 0);
    ~DancefloorController();

    void setup(char *layout);

private:
    Dancefloormodel *dfModel;
    Dancefloorwidget *dfw;
};

#endif // DANCEFLOORCONTROLLER_H
