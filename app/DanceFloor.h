#ifndef DANCEFLOORMODEL_H
#define DANCEFLOORMODEL_H

#include <QObject>
#include <QColor>
#include <string>
#include <QTime>
#include "lightcolor.h"
#include "Param.h"
#include "Cue.h"
#include "Device.h"

class Cue;
class Dancefloorwidget;

class Dancefloor : public QObject
{
    Q_OBJECT

public:
    explicit Dancefloor(QObject *parent = 0);
    ~Dancefloor();

    bool        ImportLayout(std::string & layoutCsvFile);
    void        printLayout();

    bool        hasPixel(int x, int y);
    Lightcolor  getPixel(int x, int y);
    QColor      getQColor(int x, int y);

    int         getXsize() const {return _xsize;}
    int         getYsize() const {return _ysize;}

    void        addCue(Cue *cue);
    int         getNumCues() const    {return _cues.size();}

    int         getFrame() const      {return _frame;}

    void        setView(Dancefloorwidget *dfwidget) { _dfWidget = dfwidget;}

public slots:
    void evaluate();
    void evaluateAllCues();
    void sendToDevice();
    void fireLight(int x, int y, Firing *f);

protected:
    int                 _xsize, _ysize;
    std::vector<Light>  _lights;
    std::vector<Cue *>  _cues;

    int                 _frame;
    QTime               _timeSinceLastUpdate;   // Used for monitoring responsiveness, not cueing.

    Dancefloorwidget    *_dfWidget;
    Device              _device;


#ifdef INLINE
    int _getIndex(int x, int y) {return xsize*y + x;}
#else
    // This one has range checking
    int _getIndex(int x, int y);
#endif
};

#endif // DANCEFLOORMODEL_H
