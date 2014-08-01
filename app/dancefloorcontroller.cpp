#include "dancefloorcontroller.h"

DancefloorController::DancefloorController() :
    dfModel(new Dancefloormodel),
    dfw(new Dancefloorwidget)
{
    char *layout =  "/Users/alex/src/floorit/layout.csv";
    dfModel->ImportLayout(layout);
    //dfModel->display();

    dfw->setGrid(dfModel->getXsize(), dfModel->getYsize());
    for (int y=0; y<dfModel->getYsize(); ++y) {
        for (int x=0; x<dfModel->getXsize(); ++x) {
            dfw->setHasLights(x,y, dfModel->hasPixel(x,y));
        }
    }
    dfw->show();
}
