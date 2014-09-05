#include "Node.h"
#include "utils.h"

// ------------------------------------------------------------------------------
//  Node
//  base class

Node::Node() :
    _name(QString()),
    _active(true),
    _type(UNDEFINED)
{ }

Node::node_t Node::getType()
{
    return _type;
}

Node::~Node() {}


// ------------------------------------------------------------------------------
//  TriggerEvery
//  Impulse class

void TriggerEvery::operator ()(bool &value)
{
    if (getCurrentTime() > _nextRefresh) {
       _value = true;
       _lastRefresh = _nextRefresh;
       _nextRefresh += _interval;
    }
    value = _value;

    // Clear a "true" value after it's been read.
    // XXX This assumes that this node is only connected to one output;
    _value = false;
}


// ------------------------------------------------------------------------------
//  Color ramp
//      Mix between two colors
ColorRamp::ColorRamp() :
    _output(0),
    _c0(0),
    _c1(1),
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

    _paramList << &_output << &_c0 << &_c1 << &_mix;
}

void ColorRamp::operator ()(Lightcolor &value)
{
    Lightcolor c0, c1;
    _c0.getValue(c0);
    _c1.getValue(c1);

    float t;
    _mix.getValue(t);

    Lightcolor part0, part1;
    part0 = c0*(1.0-t);     // XXX Doesn't seem to compile if float is first.
    part1 = c1*t;

//    _value = (1.0-t)*c0 + t*c1;
    _value = part0 + part1;

    value = _value;
}

// ------------------------------------------------------------------------------
//  RandomFloat

RandomFloat::RandomFloat() :
    _value(0.0),
    _output(0.0),
    _min(0.0),
    _max(1.0)
{
    setRandomEngine();
    setName("RandomFloat");
    _type = FLOAT;

    // Declare params.
    // TODO this could be factored out into static members.
    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _min.setName("min");
    _min.setOutput(false);
    _min.setConnectable(true);

    _max.setName("max");
    _max.setOutput(false);
    _max.setConnectable(true);

    _paramList << &_output << &_min << &_max;
}



void RandomFloat::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());
    // Initialize the distrubution on the range [_min, _max)
    // NOTE:  this is the HALF-OPEN interval--inclusive of min, but exclusive of max
    float min, max;
    _min.getValue(min);
    _max.getValue(max);
    // TODO re-run this when distributions change.
    // XXX if these are connections, this is going to break.
    // Do we want a "non-animating" parameter class, or flag?
    _distribution = new std::uniform_real_distribution<float>(min, max);
}


void RandomFloat::operator ()(float &value)
{
    // First, check the trigger to see if it's time for a new number
    bool trigVal = true;
    //XXX _trigger(trigVal);
    if (trigVal) {
        _value =  (*_distribution)(*_randGenerator);
    }

    value = _value;
}

// ------------------------------------------------------------------------------
//  RandomInt

RandomInt::RandomInt() :
    _value(0),
    _output(0),
    _min(0),
    _max(20)
{
    setRandomEngine();
    setName("RandomInt");
    _type = INT;

    // Declare params.
    // TODO this could be factored out into static members.
    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _min.setName("min");
    _min.setOutput(false);
    _min.setConnectable(true);

    _max.setName("max");
    _max.setOutput(false);
    _max.setConnectable(true);

    _paramList << &_output << &_min << &_max;
}



void RandomInt::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());
    // Initialize the distrubution on the range [_min, _max]
    // NOTE:  this is the CLOSED interval--inclusive of both min and max.
    int min, max;
    _min.getValue(min);
    _max.getValue(max);
    // TODO re-run this when distributions change.
    // XXX if these are connections, this is going to break.
    // Do we want a "non-animating" parameter class, or flag?
    _distribution = new std::uniform_int_distribution<int>(min, max);
}


void RandomInt::operator ()(float &value)
{
    // First, check the trigger to see if it's time for a new number
    bool trigVal = true;
    //XXX _trigger(trigVal);
    if (trigVal) {
        _value =  (*_distribution)(*_randGenerator);
    }

    value = _value;
}


// ------------------------------------------------------------------------------
//  NodePrototypes

NodeFactory::NodeFactory() {}


void NodeFactory::registerNodetype(QString name, Node::node_t typeInfo,
        NodeInstatiator_t instantiatorFunction)
{
    // register the class factory function
    _registry[name.toStdString()] = instantiatorFunction;
    _registryByType[typeInfo] << name;

}

#if 0
std::shared_ptr<Node> NodeFactory::instatiateNode(QString name)
#else
Node * NodeFactory::instatiateNode(QString name)
#endif
{
    Node *instance = nullptr;

    // find name in the registry and call factory method.
    NodeInstatiator_t instancer = _registry[name.toStdString()];
    if (instancer)
        instance = instancer();
        // TODO error

#if 0
    // wrap instance in a shared ptr and return
    // XXX not sure if I'm handling the shared_ptr stuff correctly
    if (instance != nullptr)
        return std::shared_ptr<Node>(instance);
    else
        return nullptr;
#else
    return instance;
#endif
}

NodeFactory * NodeFactory::Singleton() {
    // We need this--rather than just using a single instance of NodeFactory--
    // so that we get the right order of initialization, and the Registrar
    // won't be trying to access an uninitialized factory.
    static NodeFactory factory;
    return &factory;
}

const QStringList & NodeFactory::getNodesOfType(Node::node_t typeInfo) {
    return _registryByType[typeInfo];
}


static Registrar<RandomFloat>   registrar("RandomFloat", Node::FLOAT);
static Registrar<RandomInt>     registrar2("RandomInt", Node::INT);
static Registrar<ColorRamp>     registrar3("ColorRamp", Node::COLOR);
