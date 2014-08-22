#ifndef DANCEFLOORMODEL_H
#define DANCEFLOORMODEL_H

#include <QObject>
#include <string>
#include "lightcolor.h"
#include "Param.h"

using namespace std;

class Dancefloormodel : public QObject
{
    Q_OBJECT

public:
    explicit Dancefloormodel(QObject *parent = 0);
    ~Dancefloormodel();
    bool ImportLayout(std::string & layoutCsvFile);

    bool hasPixel(int x, int y);
    Lightcolor getPixel(int x, int y);
    void setPixel(int x, int y, Lightcolor rgb);
    void display();

    int getXsize() {return xsize;}
    int getYsize() {return ysize;}

public slots:
    void lightChanged(int x, int y, Lightcolor rgb);

private:
    int xsize, ysize;
    vector<Lightcolor>   values;
    vector<Firing*>      firings;
    vector<int>          lightIDs;

    vector<Light>       _lights;

#ifdef INLINE
    int _getIndex(int x, int y) {return xsize*y + x;}
#else
    // This one has range checking
    int _getIndex(int x, int y);
#endif
};

#endif // DANCEFLOORMODEL_H
