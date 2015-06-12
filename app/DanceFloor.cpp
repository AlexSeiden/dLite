#include "DanceFloor.h"

#include <QDebug>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>
#include <QString>
#include <QStringList>
#include "dancefloorwidget.h"
#include "Cupid.h"          // GROSS
#include "GraphWidget.h"    // GROSS


Dancefloor::Dancefloor(QObject *parent) :
    QObject(parent),
    _frame(0)
{
    // _timeSinceLastUpdate is tracked just to see how well the interface is really
    // responding.
    _timeSinceLastUpdate.start();
}

Dancefloor::~Dancefloor() { }

// ------------------------------------------------------------------------------
// Import and setup
bool Dancefloor::ImportLayout(std::string &layoutCsvFile)
{
    // LATER add Foot-squares as well as lights

    std::string line;
    std::vector< std::string> lines;
    std::vector< std::vector <std::string> > cells;

    // Read in all lines
    std::ifstream myfile(layoutCsvFile);
    if (! myfile.is_open())
        // ErrorHandling
        return false;

    while ( getline (myfile,line, '\r') ) {
        lines.push_back(line);
    }
    myfile.close();

    // OK, now we know the number of rows
    _ysize = lines.size();

    // Break into individual cells
    for (size_t i = 0; i<lines.size(); ++i) {
        // Split each row into cells
        line = lines[i];
        std::istringstream ss(line);
        std::string token;

        std::vector<std::string> row;
        while(getline(ss, token, ',')) {
            row.push_back(token);
        }
        cells.push_back(row);
    }

    // Find longest row, use that for x size.
   size_t rowSize = cells[0].size();
   for (size_t i = 1; i< cells.size(); ++i)
        rowSize = std::max(cells[i].size(),  rowSize);

    _xsize = rowSize;

    // Alloc the Lights.  These store the pixel values, as well as the hardware
    // address of the particular LED.
    size_t arraySize = _xsize*_ysize;
    _lights.resize(arraySize);

    // And fill 'em up
    for (int y=0; y<_ysize; ++y){
        for (int x=0; x<_xsize; ++x){
            int index = _getIndex(x,y);
            std::string cell = cells[y][x];
            if (cell.size() == 0 || cell.compare("X")==0)
                _lights[index]._lightID = 0;
            else {
                // GROSS
                int lightID = std::atoi(cell.c_str());
                _lights[index]._lightID = lightID;
                int stringNum = lightID / 100 - 1;
                int lightIndex = lightID % 100;

                _lights[index]._controllerIndex = stringNum/2;
                _lights[index]._lightIndex = lightIndex + stringNum%2*50;

//                qDebug() <<"\t lightID"         <<  _lights[index]._lightID
//                         <<"\t stringnum"       << stringNum
//                         <<"\t controllerindex" << _lights[index]._controllerIndex
//                         <<"\t initLightIndex"  << lightIndex
//                         <<"\t lightIndex"      << _lights[index]._lightIndex ;
            }
            _lights[index]._value = Lightcolor();
        }
    }

    return true;
}

void Dancefloor::printLayout()
{
    // Test routine to print layout on console.
    for (int y=0; y<_ysize; ++y){
        for (int x=0; x<_xsize; ++x)
            printf("%4d", _lights[_getIndex(x,y)]._lightID);
        printf("\n");
    }
}

// ------------------------------------------------------------------------------
// Access
bool Dancefloor::hasPixel(int x, int y)
{
    if (x < 0 || y < 0 || x >= _xsize || y >= _ysize)
        return false;

    int index = _getIndex(x,y);
    if (_lights[index]._lightID == 0)
        return false;
    else
        return true;
}

#ifndef INLINE
int Dancefloor::_getIndex(int x, int y)
{
    if (!(x >= 0 && x < _xsize && y >= 0 && y < _ysize)) {
        qDebug() << "BOUNDS ERROR: " <<x <<y;
        qDebug() << "MAX SIZE    : " <<_xsize <<_ysize;
        return 0;
    }
    Q_ASSERT(x >= 0 && x < _xsize && y >= 0 && y < _ysize);
    return _xsize*y + x;
}
#endif

Lightcolor Dancefloor::getPixel(int x, int y) {
    return _lights[_getIndex(x,y)]._value;
}

QColor Dancefloor::getQColor(int x, int y) {
    // Convinience method to return light value as a QColor rather than a Lightcolor
    return _lights[_getIndex(x,y)]._value.toQColor();
}

// ------------------------------------------------------------------------------
// fireLight
//      Called by the cues to set up firings.
void Dancefloor::fireLight(int x, int y, Firing *firing)
{
    // TODO Need to insert the firings correctly into the buffer,
    // so that repeat firings by the same cue do the correct thing.
    // Should check firing->cue, and remove old firings from the same
    // cue, if that is desired behavior.
    Light &light = _lights[_getIndex(x,y)];
    light._firings.push_back(firing);
}

// ------------------------------------------------------------------------------
// evaluate
void Dancefloor::evaluate()
{
//    qDebug("Time elapsed: %d ms", _timeSinceLastUpdate.elapsed());
    _timeSinceLastUpdate.restart();

    // Calls all active cues, which will set new firings.
    evaluateAllCues();

    // For every light, get the firing vector:
    for (auto light = _lights.begin(); light != _lights.end(); ++light) {

        // For every firing, apply the decay & composite.
#if 0
        Lightcolor lightColor(light->_value);  // start with what was in buffer.
#else
        Lightcolor lightColor(0,0,0);  // start with black
#endif
        // This starts with the oldest firing;
        // Right now, firings are added to the back of the vector
        auto firing = light->_firings.begin();
        while (firing != light->_firings.end())  {
            // Calculate the value of this firing
            bool keep = (*firing)->evaluate();

            // Comp it over the others
            // TODO other comp modes
            switch ((*firing)->_compMode) {
            case OVER:
                lightColor = (*firing)->compOver(lightColor);
                break;
            default:
            case ADD:
                lightColor = (*firing)->compAdd(lightColor);
                break;
            }

            // Remove firing from list if the event is over.
            if (! keep) {
                delete (*firing);
                firing = light->_firings.erase(firing);
            }
            else
                firing++;
        }
        light->_value = lightColor;
    }

    // STYLE should this be a signal/slot?
    // Update the UI
    _dfWidget->update();

    // Send to the hardware
    sendToDevice();


    // This keeps track of the current "frame" that's being evaluated.
    // It's used to make sure certain things are evaluated once--and only
    // once--per frame, as well as allowing triggers to reset correctly.
    // Note that each frame will *not* take the exact same amount of time,
    // although they should in general be pretty close.
    _frame++;
}

void Dancefloor::evaluateAllCues() {
    if (Cupid::Singleton()->getGraphWidget()->useAllCues())
        for (Cue *cue : _cues)
            cue->evaluate();
    else {
        // Only use cues on the active cuesheet
        foreach (Cue *cue,
                 Cupid::Singleton()->getGraphWidget()->getCurrentCues())
            cue->evaluate();
    }

}

void Dancefloor::addCue(Cue *cue) {
    _cues.push_back(cue);
}

void Dancefloor::removeCue(Cue *cue) {
    auto cueIter = _cues.begin();
    while (cueIter != _cues.end())  {
        // Remove firing from list if the event is over.
        if (*cueIter == cue)
            cueIter = _cues.erase(cueIter);
        else
            cueIter++;
    }
}

void Dancefloor::sendToDevice()
{
    // GROSS should move this to inside device.
    for (auto light = _lights.begin(); light != _lights.end(); ++light) {
        if (light->_lightID == 0)
            continue;

        unsigned char rgb[3];
        light->_value.setRGB(rgb);
        _device.setlight(light->_controllerIndex, light->_lightIndex, rgb);
    }
    // Output to network.
    _device.send();
}

void Dancefloor::setHardwareStatus(bool status)
{
    _device.setActive(status);
}
