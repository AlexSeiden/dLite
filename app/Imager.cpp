#include "Imager.h"
#include "DanceFloor.h"
#include "Cupid.h"
#include <QPainter>

Imager::Imager() :
    _x(10),
    _y(8),
    _scale(1.0),
    _alpha(1.0),
    _color(Lightcolor(255,255,255))
{
    setName(QString("ImageCue"));
    _type = CUE;
    int xsize = Cupid::Singleton()->getDancefloor()->getXsize();
    int ysize = Cupid::Singleton()->getDancefloor()->getYsize();
    _image = new QImage(xsize, ysize, QImage::Format_ARGB32_Premultiplied);

    // Declare params.
    _x.setName("x");
    _x.setOutput(false);
    _x.setConnectable(true);

    _y.setName("y");
    _y.setOutput(false);
    _y.setConnectable(true);

    _scale.setName("scale");
    _scale.setOutput(false);
    _scale.setConnectable(true);

    _alpha.setName("alpha");
    _alpha.setOutput(false);
    _alpha.setConnectable(true);

    _color.setName("color");
    _color.setOutput(false);
    _color.setConnectable(true);

    _paramList << &_x <<&_y <<&_scale << &_alpha << &_color;
    setParamParent();

}

void Imager::draw()
{
    QPainter painter(_image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(_image->rect(), Qt::black);
    painter.setBrush(QBrush(_color._value.toQColor()));
    QPointF center(_x._value, _y._value);
    painter.drawEllipse(center, _scale._value, _scale._value);

    painter.end();
}

void Imager::operator()() {
    evaluate();
}

void Imager::evaluate()
{
    if (!_active) return;

    evalAllInputs();
    draw();
    fire();
}

void Imager::fire()
{
    for (int y=0; y<_image->height(); ++y)
        for (int x=0; x<_image->width(); ++x) {
            // TODO XXX $$$ this is super-inefficient
            Firing *firing = new Firing;
            Lightcolor color(_image->pixel(x,y));
            color *= _alpha._value;

            firing->_color = color;
            firing->_alpha = _alpha._value;
            firing->setDecayMode(_decayMode);           // TODO
            firing->setCompMode(_compMode);
            _dfModel->fireLight(x, y, firing);
        }
}

static Registrar<Imager>     registrar("Imager", Node::CUE);
