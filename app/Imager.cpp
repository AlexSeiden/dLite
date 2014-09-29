#include "Imager.h"
#include "DanceFloor.h"
#include "Cupid.h"
#include <QPainter>

// ------------------------------------------------------------------------------
// Imager base class
Imager::Imager() :
    _alpha(1.0),
    _color(Lightcolor(255,255,255))
{
    _type = CUE;
    int xsize = Cupid::Singleton()->getDancefloor()->getXsize();
    int ysize = Cupid::Singleton()->getDancefloor()->getYsize();
    _image = new QImage(xsize, ysize, QImage::Format_ARGB32_Premultiplied);
}

void Imager::draw()
{
}

void Imager::operator()() {
    evaluate();
}

Imager* Imager::clone()
{
    Imager* lhs = new Imager;
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}

void Imager::evaluate()
{
    if (!isActive()) return;

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

// ------------------------------------------------------------------------------
// Circle
Circle::Circle() :
    Imager(),
    _x(10),
    _y(8),
    _scale(1.0)
{
    setName(QString("CircleCue"));

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

void Circle::draw()
{
    QPainter painter(_image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(_image->rect(), Qt::black);
    painter.setBrush(QBrush(_color._value.toQColor()));
    QPointF center(_x._value, _y._value);
    painter.drawEllipse(center, _scale._value, _scale._value);

    painter.end();
}

Circle* Circle::clone()
{
    Circle* lhs = new Circle;
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}


// ------------------------------------------------------------------------------
// Box
Box::Box() :
    Imager(),
    _x(10),
    _y(8),
    _width(1.0),
    _height(1.0),
    _rotation(0.0),
    _antialiased(false)
{
    setName(QString("Box"));

    // Declare params.
    _x.setName("x");
    _x.setOutput(false);
    _x.setConnectable(true);

    _y.setName("y");
    _y.setOutput(false);
    _y.setConnectable(true);

    _width.setName("width");
    _width.setOutput(false);
    _width.setConnectable(true);

    _height.setName("height");
    _height.setOutput(false);
    _height.setConnectable(true);

    _rotation.setName("rotation");
    _rotation.setOutput(false);
    _rotation.setConnectable(true);
    _rotation.setRange(true, -10000., 10000., 1.0);

    _antialiased.setName("antialiased");
    _antialiased.setOutput(false);
    _antialiased.setConnectable(true);

    _alpha.setName("alpha");
    _alpha.setOutput(false);
    _alpha.setConnectable(true);

    _color.setName("color");
    _color.setOutput(false);
    _color.setConnectable(true);

    _paramList << &_x << &_y << &_width << &_height << &_rotation <<&_antialiased << &_alpha << &_color;
    setParamParent();
}

void Box::draw()
{
    QPainter painter(_image);
    painter.setRenderHint(QPainter::Antialiasing, _antialiased._value);

    painter.fillRect(_image->rect(), Qt::black);
    painter.setBrush(QBrush(_color._value.toQColor()));

    QPointF topLeft(_x._value, _y._value);
    QSizeF sizef(_width._value+1., _height._value+1.);
#if 0
    QRectF rr(topLeft, sizef);
    painter.drawRect(rr);
#else
//    QRectF rr(topLeft, sizef);
//    QRectF rr(0.0,0.0,_width._value+1.,_height._value+1.);
    QRectF rr(0.0,0.0,_width._value,_height._value);
    painter.save();
    painter.translate(topLeft);
    painter.rotate(_rotation._value);
//    painter.drawRect(rr);
    painter.fillRect(rr, _color._value.toQColor());
    painter.restore();
#endif

    painter.end();
}

Box* Box::clone()
{
    Box* lhs = new Box;
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}
//static Registrar<Imager>     registrar0("Imager", Node::CUE);
static Registrar<Circle>     registrar1("Circle", Node::CUE);
static Registrar<Box>        registrar2("Box", Node::CUE);
