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

    void        addCue(Cue *cue);
    void        removeCue(Cue *cue);
    int         getNumCues() const    {return m_cues.size();}
    void        fireLight(int x, int y, Firing *f);

    bool        hasPixel(int x, int y);
    Lightcolor  getPixel(int x, int y);
    QColor      getQColor(int x, int y);

    int         getXsize() const {return m_xsize;}
    int         getYsize() const {return m_ysize;}
    int         getFrame() const {return m_frame;}

    void        setView(Dancefloorwidget *dfwidget) { m_dfWidget = dfwidget;}
    void        printLayout();

public slots:
    void evaluate();
    void evaluateAllCues();
    void sendToDevice();
    void setHardwareStatus(bool status);

protected:
    int                 m_xsize, m_ysize;
    std::vector<Light>  m_lights;
    std::vector<Cue *>  m_cues;

    int                 m_frame;
    QTime               m_timeSinceLastUpdate;   // Used for monitoring responsiveness, not cueing.

    Dancefloorwidget    *m_dfWidget;
    Device              m_device;


#ifdef INLINE
    int getIndex(int x, int y) {return m_xsize*y + x;}
#else
    // This one has range checking
    int getIndex(int x, int y);
#endif
};

#endif // DANCEFLOORMODEL_H
