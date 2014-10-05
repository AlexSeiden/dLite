#include "Imager.h"
#include "DanceFloor.h"
#include "Cupid.h"
#include <QPainter>

// ------------------------------------------------------------------------------
// Imager base class
Imager::Imager()
{
    _type = CUE;
    addParam<float>("alpha", 1.0);
    addParam<Lightcolor>("color", Lightcolor(255,255,255));

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
            float alpha;
            getValue("alpha", alpha);
            color *= alpha;

            firing->_color = color;
            firing->_alpha = alpha;
            firing->setDecayMode(_decayMode);           // TODO
            firing->setCompMode(_compMode);             // TODO
            _dfModel->fireLight(x, y, firing);
        }
}

// ------------------------------------------------------------------------------
// Circle
Circle::Circle() :
    Imager()
{
    setName(QString("CircleCue"));

    // Declare params.
    addParam<float>("x", 10.);
    addParam<float>("y", 8.);
    addParam<float>("scale", 1.0);
    // alpha & color???

}

void Circle::draw()
{
    Lightcolor color;
    int x,y;
    float scale;
    getValue("color", color);
    getValue("x", x);
    getValue("y", y);
    getValue("scale", scale);

    QPainter painter(_image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(_image->rect(), Qt::black);
    painter.setBrush(QBrush(color.toQColor()));
    QPointF center(x, y);
    painter.drawEllipse(center, scale, scale);

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
    Imager()
{
    setName(QString("Box"));

    // Declare params.
    addParam<float>("x", 10.);
    addParam<float>("y", 8.);
    addParam<float>("width", 1.0);
    addParam<float>("height", 1.0);
    addParam<float>("rotation", 0.);
    addParam<bool>("antialiased", false);
    // TODO _rotation.setRange(true, -10000., 10000., 1.0);
}

void Box::draw()
{
    Lightcolor color;
    float x,y;
    float width, height;
    float rotation;
    bool antialiased;
    getValue("color", color);
    getValue("x", x);
    getValue("y", y);
    getValue("width", width);
    getValue("height", height);
    getValue("rotation", rotation);
    getValue("antialiased", antialiased);

    QPainter painter(_image);
    painter.setRenderHint(QPainter::Antialiasing, antialiased);

    painter.fillRect(_image->rect(), Qt::black);
    painter.setBrush(QBrush(color.toQColor()));

    QPointF topLeft(x, y);
    QRectF rr(0.0, 0.0, width, height);
    painter.save();
    painter.translate(topLeft);
    painter.rotate(rotation);
    painter.fillRect(rr, color.toQColor());
    painter.restore();

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
