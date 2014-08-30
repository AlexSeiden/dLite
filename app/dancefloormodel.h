#ifndef DANCEFLOORMODEL_H
#define DANCEFLOORMODEL_H

#include <QObject>
#include <QColor>
#include <string>
#include <QTime>
#include "lightcolor.h"
#include "Param.h"
#include "Cue.h"


using namespace std;

class Cue;
class Dancefloorwidget;

class Dancefloormodel : public QObject
{
    Q_OBJECT

public:
    explicit Dancefloormodel(QObject *parent = 0);
    ~Dancefloormodel();
    bool ImportLayout(std::string & layoutCsvFile);
    void printLayout();

    bool hasPixel(int x, int y);
    Lightcolor getPixel(int x, int y);
    QColor getQColor(int x, int y);
    void setPixel(int x, int y, Lightcolor rgb);

    int getXsize() {return xsize;}
    int getYsize() {return ysize;}

    void addCue(Cue *cue);
    int getNumCues() {return _numCues;}

    void setView(Dancefloorwidget *dfwidget) { _dfWidget = dfwidget;}

public slots:
    void evaluate();
    void evaluateAllCues();
    void fireLight(int x, int y, Firing *f);

private:
    int xsize, ysize;
    vector<Lightcolor>   values;
    vector<Firing*>      firings;
    vector<int>          lightIDs;

    vector<Light>       _lights;

    std::vector<Cue *>  _cues;
    QTime               _t;
    Dancefloorwidget    *_dfWidget;
    int                 _numCues;


#ifdef INLINE
    int _getIndex(int x, int y) {return xsize*y + x;}
#else
    // This one has range checking
    int _getIndex(int x, int y);
#endif
};

#endif // DANCEFLOORMODEL_H
