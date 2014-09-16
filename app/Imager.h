#ifndef IMAGER_H
#define IMAGER_H
#include <QImage>
#include "Cue.h"

class Imager : public Cue
{
public:
    Imager();

    void evaluate();
    void operator() (void);

protected:
    Param<float>        _x;
    Param<float>        _y;
    Param<float>        _scale;
    Param<float>        _alpha;
    Param<Lightcolor>   _color;

    // friend class ImagerView;

private:
    void    draw();
    void    fire();
    QImage  *_image;
};

#endif // IMAGER_H
