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

using namespace std;

Dancefloormodel::Dancefloormodel(QObject *parent) :
    QObject(parent),
    values(NULL),
    lightIDs(NULL)
{
}

Dancefloormodel::~Dancefloormodel()
{
    if (values)
        delete values;
    if (lightIDs)
        delete lightIDs;
}

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
    lightIDs = new int[xsize*ysize];
    values = new Lightcolor[xsize*ysize];

    // And fill 'em up
    for (int y=0; y<ysize; ++y){
        for (int x=0; x<xsize; ++x){
            int index = _getIndex(x,y);
            string cell = cells[y][x];
            if (cell.size() == 0 || cell.compare("X")==0) {
                lightIDs[index] = 0;
            } else
                lightIDs[index] = std::atoi(cell.c_str());
            values[index] = Lightcolor();
        }
    }

    return true;
}

bool
Dancefloormodel::hasPixel(int x, int y)
{
    int index = _getIndex(x,y);
    if (lightIDs[index]==0)
        return false;
    else
        return true;
}

void
Dancefloormodel::display()
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
    Q_ASSERT(x >= 0 && x < xsize && y >= 0 && y < ysize);
    return xsize*y + x;
}
#endif

void Dancefloormodel::setPixel(int x, int y, Lightcolor rgb)
{
    values[_getIndex(x,y)] = rgb;
}


Lightcolor Dancefloormodel::getPixel(int x, int y)
{
    return values[_getIndex(x,y)];
}


void Dancefloormodel::lightChanged(int x, int y, Lightcolor rgb)
{
    //qDebug() << "light Changed" << x << y << rgb.getRed() << rgb.getGreen() << rgb.getBlue()  ;
    setPixel(x,y,rgb);
}
