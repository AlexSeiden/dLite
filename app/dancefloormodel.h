#ifndef DANCEFLOORMODEL_H
#define DANCEFLOORMODEL_H

#include <string>
#include "lightcolor.h"
using namespace std;

class Dancefloormodel
{
public:
    Dancefloormodel();
    ~Dancefloormodel();
    bool ImportLayout(char *layoutCsvFile);

    bool hasPixel(int x, int y);
    Lightcolor getPixel(int x, int y);
    bool setPixel(int x, int y, Lightcolor rgb);
    void display();

    int getXsize() {return xsize;}
    int getYsize() {return ysize;}

private:
    int xsize, ysize;
    Lightcolor *values;
    int        *lightIDs;

    int _getIndex(int x, int y) {return xsize*y + x;}
};

#endif // DANCEFLOORMODEL_H
