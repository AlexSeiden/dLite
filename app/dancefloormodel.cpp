#include "dancefloormodel.h"

#include <iostream>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>

Dancefloormodel::Dancefloormodel()
{
}

Dancefloormodel::~Dancefloormodel()
{
    if (values)
        delete values;
    if (lightIDs)
        delete lightIDs;
}

bool Dancefloormodel::ImportLayout(char *layoutCsvFile)
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
        vector <string> row;
        // Split each row into cells
        line = lines[i];
        istringstream ss(line);
        string token;

        while(getline(ss, token, ',')) {
            row.push_back(token);
        }
        cells.push_back(row);
    }

    // Check that all the rows are the same size
    size_t rowSize = cells[0].size();
    for (size_t i = 1; i< cells.size(); ++i) {
        if (cells[i].size() != rowSize) {
            printf("WE ARE FUCKED\n");
            return false;
        }
    }
    xsize = rowSize;

    // Now alloc arrays:
    lightIDs = new int[xsize*ysize];
    values = new Lightcolor[xsize*ysize];

    // And fill 'em up
    for (int y=0; y<ysize; ++y){
        for (int x=0; x<xsize; ++x){
            int index = _getIndex(x,y);
            vector <string> row = cells[y];
            string cell = row[x];
//printf("(%d %d): %s\n", x, y, cell.c_str());
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

