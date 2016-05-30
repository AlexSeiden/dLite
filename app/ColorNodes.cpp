// ColorNodes.h
//
//  Nodes that alter or return color values.

#include "ColorNodes.h"
#include "NodeFactory.h"
#include "utils.h"

// ------------------------------------------------------------------------------
//  Color ramp
//      Mix between two colors
ColorRamp::ColorRamp() 
{
    setName("ColorRamp");
    m_type = COLOR;

    addParam<Lightcolor>("out", 0.0, true);
    addParam<Lightcolor>("Color0", 0.0);
    addParam<Lightcolor>("Color1", 1.0);
    addParam<float>("mix", .5);
}

void ColorRamp::operator()()
{
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    Lightcolor color0, color1;
    float mix;
    getValue("Color0", color0);
    getValue("Color1", color1);
    getValue("mix", mix);

    Lightcolor part0, part1;
    Lightcolor out;
    part0 = (1.0-mix)*color0;
    part1 = color1*mix;
    out = part0 + part1;

    setValue("out", out);
}

ColorRamp* ColorRamp::clone()
{
    ColorRamp* lhs = new ColorRamp;
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
//  BriteColor
//      Creates a new hi-brightness, hi-sat color every trigger
BriteColor::BriteColor()
{
    setName("BriteColor");
    m_type = COLOR;

    addParam<Lightcolor>("out", 0.0, true);
    addParam<float>("hue min", 0.0, false, false);
    addParam<float>("hue max", 1.0, false, false);
    addParam<float>("sat min", 0.7, false, false);
    addParam<float>("sat max", 1.0, false, false);
    addParam<float>("value min", 0.8, false, false);
    addParam<float>("value max", 1.0, false, false);
    addParam<bool>("trigger", true);

    setRandomEngine();
}

void BriteColor::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());

    // Initialize the distrubution on the range [_min, _max)
    // NOTE:  this is the HALF-OPEN interval--inclusive of min, but exclusive of max
    float hmin, hmax, smin, smax, vmin, vmax;
    getValue("hue min", hmin);
    getValue("hue max", hmax);
    getValue("sat min", smin);
    getValue("sat max", smax);
    getValue("value min", vmin);
    getValue("value max", vmax);
    m_distHue = new std::uniform_real_distribution<float>(hmin, hmax);
    m_distSat = new std::uniform_real_distribution<float>(smin, smax);
    m_distVal = new std::uniform_real_distribution<float>(vmin, vmax);
}

// This is called whenever a parameter gets edited, so
// that the random number engine can be reset.
// XXX If these are connections, this is going to break, since
// this doesn't get called when a connection value updates,
// just when a manual edit happens to the widget.
void BriteColor::paramHasBeenEdited()
{
    setRandomEngine();
}

void BriteColor::operator()()
{
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();
    bool trigger;
    getValue("trigger", trigger);

    if (trigger) {
        QColor out;
        double sat = (*m_distSat)(*_randGenerator);
        double hue = (*m_distHue)(*_randGenerator);
        double val = (*m_distVal)(*_randGenerator);
        out.setHsvF(hue,sat,val);

        // Since the param color type is "lightcolor", we need
        // to call the setter with an appropriate type.
        Lightcolor lc(out);
        setValue("out", lc);
    }
}

BriteColor* BriteColor::clone()
{
    BriteColor* lhs = new BriteColor;
    cloneHelper(*lhs);
    setRandomEngine();

    return lhs;
}

// ------------------------------------------------------------------------------
//  Palette
//      Choose between a bunch of colors
Palette::Palette()
{
    setName("Palette");
    m_type = COLOR;

    addParam<Lightcolor>("out", 0.0, true);
    addParam<int>("selector");

    // TODO have a "add color" button on editor
    for (int i=0; i<8;++i) {
        addParam<Lightcolor>(QString("c%1").arg(i), 0);
    }
}

void Palette::operator()()
{
    // Boilerplate start-of-operator():
    if (evaluatedThisFrame())
        return;
    evalAllInputs();
    int selector;
    getValue("selector", selector);

    QString paramname = QString("c%1").arg(selector);
    Lightcolor out;
    getValue(paramname, out);

    setValue("out", out);
}

Palette* Palette::clone()
{
    Palette* lhs = new Palette;
    cloneHelper(*lhs);

    return lhs;
}

#if 0
// TODO palatte serialization
void Palette::writeToJSONObj(QJsonObject &json) const
{
    Node::writeToJSONObj(json);

    // Override for serialization
    QJsonArray pathJsonArray;
    foreach (const Position pos, _positions) {
         QJsonObject posJ;
         posJ["x"] = pos.x;
         posJ["y"] = pos.y;
         pathJsonArray.append(posJ);
     }
    json["positions"] = pathJsonArray;
}

void Palette::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
    QJsonArray positionsArray = json["positions"].toArray();
    for (int i=0; i<positionsArray.size(); ++i) {
        QJsonObject posJsonObject = positionsArray[i].toObject();
        Position pos;
        pos.x = posJsonObject["x"].toInt();
        pos.y = posJsonObject["y"].toInt();
        _positions.push_back(pos);      // (Hopefully) this uses copy to push onto QList of positons.
    }
}
#endif


static Registrar<BriteColor>     registrar0("BriteColor", Node::COLOR);
static Registrar<ColorRamp>      registrar1("ColorRamp", Node::COLOR);
static Registrar<Palette>        registrar2("Palette", Node::COLOR);
