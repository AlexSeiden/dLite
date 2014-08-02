#include "dancefloorcontroller.h"
#include "dancefloorwidget.h"
#include "dancefloormodel.h"

DancefloorController::DancefloorController(QObject *parent) :
    QObject(parent),
    dfModel(new Dancefloormodel()),
    dfw(new Dancefloorwidget())
{
}

DancefloorController::~DancefloorController()
{
    // TODO delete stuff
}

void
DancefloorController::setup(char *layout)
{

#if 0
    dfModel->ImportLayout(layout);
    //dfModel->display();

    dfw->setGrid(dfModel->getXsize(), dfModel->getYsize());
#else
    dfw->setGrid(28, 18);
#endif

#if 0
    for (int y=0; y<dfModel->getYsize(); ++y) {
        for (int x=0; x<dfModel->getXsize(); ++x) {
            dfw->setHasLights(x,y, dfModel->hasPixel(x,y));
        }
    }
#endif
    dfw->show();
}
