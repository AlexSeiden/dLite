#include "Shape.h"
#include "DanceFloor.h"
#include "Cupid.h"
#include <QPainter>
#include "NodeFactory.h"

// ------------------------------------------------------------------------------
Shape::Shape()
{
    m_type = CUE;
    addParam<float>("alpha", 1.0);
    addParam<Lightcolor>("color", Lightcolor(255,255,255));

    int xsize = Cupid::Singleton()->getDancefloor()->getXsize();
    int ysize = Cupid::Singleton()->getDancefloor()->getYsize();
    m_image = new QImage(xsize, ysize, QImage::Format_ARGB32_Premultiplied);
}

void Shape::draw() { }

void Shape::operator()() {
    evaluate();
}

Shape* Shape::clone()
{
    Shape* lhs = new Shape;
    cloneHelper(*lhs);
    return lhs;
}

void Shape::evaluate()
{
    if (!isActive()) return;

    evalAllInputs();
    draw();
    fire();
}

void Shape::fire()
{
    for (int y=0; y<m_image->height(); ++y)
        for (int x=0; x<m_image->width(); ++x) {
            // This can be optimized...
            Firing *firing = new Firing;
            Lightcolor color(m_image->pixel(x,y));
            float alpha;
            getValue("alpha", alpha);
            color *= alpha;

            firing->m_color = color;
            firing->m_alpha = alpha;
            firing->setDecayMode(m_decayMode);
            firing->setCompMode(m_compMode);
            m_dfModel->fireLight(x, y, firing);
        }
}

// ------------------------------------------------------------------------------
Circle::Circle() : Shape()
{
    setName(QString("CircleCue"));

    // Declare params.
    addParam<float>("x", 10.);
    addParam<float>("y", 8.);
    addParam<float>("width", 1.0);
    addParam<float>("height", 1.0);
}

void Circle::draw()
{
    Lightcolor color;
    float x,y;
    float width;
    float height;
    getValue("color", color);
    getValue("x", x);
    getValue("y", y);
    getValue("width", width);
    getValue("height", height);

    QPainter painter(m_image);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(m_image->rect(), Qt::black);
    painter.setBrush(QBrush(color.toQColor()));
    QPointF center(x, y);
    painter.drawEllipse(center, width, height);

    painter.restore();
    painter.end();
}

Circle* Circle::clone()
{
    Circle* lhs = new Circle;
    cloneHelper(*lhs);
    return lhs;
}


// ------------------------------------------------------------------------------
// Box
Box::Box() : Shape()
{
    setName(QString("Box"));

    // Declare params.
    addParam<float>("x", 10.);
    addParam<float>("y", 8.);
    addParam<float>("width", 1.0);
    addParam<float>("height", 1.0);
    ParamBase *rot = addParam<float>("rotation", 0.);
    addParam<bool>("antialiased", false);
    rot->setRange(true, -10000., 10000., 1.0);
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

    QPainter painter(m_image);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, antialiased);

    painter.fillRect(m_image->rect(), Qt::black);
    painter.setBrush(QBrush(color.toQColor()));

    QPointF topLeft(x, y);
    QRectF rr(0.0, 0.0, width, height);
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
    return lhs;
}

static Registrar<Circle>     registrar1("Circle", Node::CUE);
static Registrar<Box>        registrar2("Box", Node::CUE);
