#include "dancefloormodel.h"

#include <iostream>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>
#include <QString>
#include <QStringList>

Dancefloormodel::Dancefloormodel() :
    values(NULL),
    lightIDs(NULL)
{
}

Dancefloormodel::~Dancefloormodel()
{
#if 0
    if (values)
        delete values;
    if (lightIDs)
        delete lightIDs;
#endif
}

bool Dancefloormodel::ImportLayout(char *layoutCsvFile)
{
#if 1
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

    // Check that all the rows are the same size
    size_t rowSize = cells[0].size();
#if 0
    for (size_t i = 1; i< cells.size(); ++i) {
        if (cells[i].size() != rowSize) {
            printf("WE ARE FUCKED\n");
            // TODO probably can just use max of all rows...
            return false;
        }
    }
#endif

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
#else
    xsize = 24;
    ysize = 18;
    // Now alloc arrays:
    lightIDs = (int*)malloc(sizeof(int)*10*xsize*ysize);
    //values = new Lightcolor[xsize*ysize];

    for (int y=0; y<ysize; ++y){
        for (int x=0; x<xsize; ++x){
            int index = _getIndex(x,y);
            if (x < 3) {
                lightIDs[index] = 0;
            } else
                lightIDs[index] = x + y*100;
     //       values[index] = Lightcolor();
        }
    }
    return true;
#endif
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

