#include "ColorNodes.h"
#include "utils.h"

// ------------------------------------------------------------------------------
//  Color ramp
//      Mix between two colors
ColorRamp::ColorRamp() :
    _output(0),
    _c0(0.0),
    _c1(1.0),
    _mix(.5)
{
    setName("ColorRamp");
    _type = COLOR;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _c0.setName("Color0");
    _c0.setOutput(false);
    _c0.setConnectable(true);

    _c1.setName("Color1");
    _c1.setOutput(false);
    _c1.setConnectable(true);

    _mix.setName("mix");
    _mix.setOutput(false);
    _mix.setConnectable(true);

    _paramList << &_output << &_c1 << &_c0 << &_mix;
    setParamParent();
}

void ColorRamp::operator()()
{
    // Boilerplate start of operator:
    // TODO:  add start & end boilerplate to the generic
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    Lightcolor part0, part1;
    part0 = (1.0-_mix._value)*_c0._value;
    part1 = _c1._value*_mix._value;
    _output._value = part0 + part1;

    // Boilerplate end of operator:
    _output._qvOutput.setValue(_output._value);
}

// ------------------------------------------------------------------------------
//  BriteColor
//      Creates a new hi-brightness, hi-sat color every trigger
BriteColor::BriteColor() :
    _output(0),
    _trigger(true)
{
    setName("BriteColor");
    _type = COLOR;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _trigger.setName("trigger");
    _trigger.setOutput(false);
    _trigger.setConnectable(true);

    _paramList << &_output << &_trigger;
    setParamParent();
    setRandomEngine();
}

void BriteColor::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());

    // Initialize the distrubution on the range [_min, _max)
    // NOTE:  this is the HALF-OPEN interval--inclusive of min, but exclusive of max
    // XXX if these are connections, this is going to break.
#if 0
    // LATER  -- range params for BriteColor
    float minSat, maxSat;
    _min.getValue(minSat);
    _max.getValue(maxSat);
    _distSat = new std::uniform_real_distribution<float>(minSat, maxSat);
#else
    _distSat = new std::uniform_real_distribution<float>(.7, 1.0);
#endif
    _distHue = new std::uniform_real_distribution<float>(0.0, 1.0);
    _distVal = new std::uniform_real_distribution<float>(0.8, 1.0);
}

// This is called whenever a parameter gets edited, so
// that the random number engine can be reset.
void BriteColor::paramHasBeenEdited()
{
    // Need to reset random engine.
    setRandomEngine();
}

void BriteColor::operator()()
{
    // Boilerplate start of operator:
    // TODO:  add start & end boilerplate to the generic
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    if (_trigger._value) {
        QColor out;
        double sat = (*_distSat)(*_randGenerator);
        double hue = (*_distHue)(*_randGenerator);
        double val = (*_distVal)(*_randGenerator);
#if 0
        // shouldn't be needed since the rand generators shouldn't be returning
        // values outside this range
        hue = clamp(0.0,1.0,hue);
        sat = clamp(0.0,1.0,sat);
        val = clamp(0.0,1.0,val);
#endif
        out.setHsvF(hue,sat,val);
        _output._value = out;
    }

    // Boilerplate end of operator:
    _output._qvOutput.setValue(_output._value);
}

// ------------------------------------------------------------------------------
//  Palette
//      Choose between a bunch of colors
Palette::Palette() :
    _output(0),
    _selector(0)
{
    setName("Palette");
    _type = COLOR;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _selector.setName("selector");
    _selector.setOutput(false);
    _selector.setConnectable(true);

    _paramList << &_output << &_selector;
    for (int i=0; i<8;++i) {
        Param<Lightcolor>* paletteEntry = new(Param<Lightcolor>);
        paletteEntry->setConnectable(true);
        paletteEntry->setOutput(false);
        paletteEntry->setName(QString("c%1").arg(i));
        _paramList << paletteEntry;
        _allcolors << paletteEntry;
    }

    setParamParent();
}

void Palette::operator()()
{
    // Boilerplate start-of-operator():
    // TODO:  add start & end boilerplate to the generic
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    int index = _selector._value % _allcolors.length();
    _output._value = _allcolors[index]->_value;

    // Boilerplate end-of-operator():
    _output._qvOutput.setValue(_output._value);
}

#if 0

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
