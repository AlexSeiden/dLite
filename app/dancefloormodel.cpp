#include "dancefloormodel.h"

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

using namespace std;

Dancefloormodel::Dancefloormodel(QObject *parent) :
    QObject(parent)
{
    _numCues = 0;

    _t.start(); // XXX debugging
}

Dancefloormodel::~Dancefloormodel() { }

// TODO add Foot-squares as well as lights
bool Dancefloormodel::ImportLayout(string &layoutCsvFile)
{
    string line;
    vector< string> lines;
    vector< vector <string> > cells;

    // Read in all lines
    std::ifstream myfile(layoutCsvFile);
    if (! myfile.is_open())
        // TODO error
        return false;

    while ( getline (myfile,line, '\r') ) {
        lines.push_back(line);
    }
    myfile.close();

    // OK, now we know the number of rows
    ysize = lines.size();

    // Break into individual cells
    for (size_t i = 0; i<lines.size(); ++i) {
        // Split each row into cells
        line = lines[i];
        istringstream ss(line);
        string token;

        vector<string> row;
        while(getline(ss, token, ',')) {
            row.push_back(token);
        }
        cells.push_back(row);
    }

    // Find longest row, use that for x size.
   size_t rowSize = cells[0].size();
   for (size_t i = 1; i< cells.size(); ++i)
        rowSize = max(cells[i].size(),  rowSize);

    xsize = rowSize;

    // Now alloc arrays:
    size_t arraySize = xsize*ysize;
    lightIDs.resize(arraySize);
    values.resize(arraySize);
    firings.resize(arraySize);
    _lights.resize(arraySize);

    // And fill 'em up
    for (int y=0; y<ysize; ++y){
        for (int x=0; x<xsize; ++x){
            int index = _getIndex(x,y);
            string cell = cells[y][x];
            if (cell.size() == 0 || cell.compare("X")==0) {
                _lights[index]._lightID = 0;
            } else
                _lights[index]._lightID = std::atoi(cell.c_str());
            _lights[index]._value = Lightcolor();
        }
    }

    return true;
}

bool Dancefloormodel::hasPixel(int x, int y)
{
    int index = _getIndex(x,y);
    if (_lights[index]._lightID == 0)
        return false;
    else
        return true;
}

/*
 * Test routine to print layout.
 */
void
Dancefloormodel::printLayout()
{
    for (int y=0; y<ysize; ++y){
        for (int x=0; x<xsize; ++x){
            int index = _getIndex(x,y);
            printf("%4d", lightIDs[index]);
            //cout << lightIDs[index];
        }
        printf("\n");
    }
}

#ifndef INLINE
int Dancefloormodel::_getIndex(int x, int y)
{
    if (!(x >= 0 && x < xsize && y >= 0 && y < ysize)) {
        qDebug() << "BOUNDS ERROR: " <<x <<y;
        qDebug() << "MAX SIZE    : " <<xsize <<ysize;
    }
    Q_ASSERT(x >= 0 && x < xsize && y >= 0 && y < ysize);
    return xsize*y + x;
}
#endif

Lightcolor Dancefloormodel::getPixel(int x, int y) {
    return _lights[_getIndex(x,y)]._value;
}

QColor Dancefloormodel::getQColor(int x, int y) {
    return _lights[_getIndex(x,y)]._value.toQColor();
}

void Dancefloormodel::fireLight(int x, int y, Firing *firing)
{
    // TODO Need to insert the firings correctly into the buffer,
    // so that repeat firings by the same cue do the correct thing.
    // Should check firing->cue, and remove old firings from the same
    // cue, if that is desired behavior.
    Light &light = _lights[_getIndex(x,y)];
    light._firings.push_back(firing);
}

void Dancefloormodel::evaluate()
{
    //qDebug("Time elapsed: %d ms", _t.elapsed());
    _t.restart();

    evaluateAllCues();

    // For every light, get the firing vector:
    for (auto light = _lights.begin(); light != _lights.end(); ++light) {

        // For every firing, apply the decay & composite. starting with the "backmost"
#if 0
        Lightcolor lightColor(light->_value);  // start with what was in buffer.
#else
        Lightcolor lightColor;  // start with black
#endif
        auto firing = light->_firings.begin();
        while (firing != light->_firings.end())  {
            // Calculate the value of this firing
            bool keep = (*firing)->evaluate();

            // Comp it over the others
            // TODO other comp modes
            lightColor = (*firing)->compOver(lightColor);

            // Remove firing from list if the event is over.
            if (! keep)
                firing = light->_firings.erase(firing);
            else
                firing++;
        }
        light->_value = lightColor;
    }

    _dfWidget->update();
}

void Dancefloormodel::evaluateAllCues() {
    for (Cue *cue : _cues) {
        cue->evaluate();
    }
}

void Dancefloormodel::addCue(Cue *cue) {
    _cues.push_back(cue);
    _numCues++;
}

