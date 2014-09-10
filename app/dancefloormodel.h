#ifndef DANCEFLOORMODEL_H
#define DANCEFLOORMODEL_H

#include <QObject>
#include <QColor>
#include <string>
#include <QTime>
#include "lightcolor.h"
#include "Param.h"
#include "Cue.h"

class Cue;
class Dancefloorwidget;

class Dancefloormodel : public QObject
{
    Q_OBJECT

    friend class Cupid;

public:
    explicit Dancefloormodel(QObject *parent = 0);
    ~Dancefloormodel();
    bool ImportLayout(std::string & layoutCsvFile);
    void printLayout();

    bool        hasPixel(int x, int y);
    Lightcolor  getPixel(int x, int y);
    QColor      getQColor(int x, int y);

    int getXsize() {return _xsize;}
    int getYsize() {return _ysize;}

    void addCue(Cue *cue);
    int getNumCues()    {return _numCues;}

    int getFrame()      {return _frame;}    // GROSS

    void setView(Dancefloorwidget *dfwidget) { _dfWidget = dfwidget;}

public slots:
    void evaluate();
    void evaluateAllCues();
    void fireLight(int x, int y, Firing *f);

protected:
    int                 _xsize, _ysize;
    std::vector<Light>  _lights;
    std::vector<Cue *>  _cues;
    int                 _numCues;

    int                 _frame;
    QTime               _timeSinceLastUpdate;

    Dancefloorwidget    *_dfWidget;


#ifdef INLINE
    int _getIndex(int x, int y) {return xsize*y + x;}
#else
    // This one has range checking
    int _getIndex(int x, int y);
#endif
};

#endif // DANCEFLOORMODEL_H
