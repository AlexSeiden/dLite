#include "Param.h"
#include "lightcolor.h"
#include <QDebug>
#include "Node.h"
#include <QJsonObject>
#include <QJsonArray>

// These constants defined for convinience & speed when doing type checks.
// ??? Are these still needed?  Would it be better just to have use an enum?
const std::type_info & paramTypeFloat       = typeid(Param<float>);
const std::type_info & paramTypeInt         = typeid(Param<int>);
const std::type_info & paramTypeLightcolor  = typeid(Param<Lightcolor>);
const std::type_info & paramTypeBool        = typeid(Param<bool>);

std::function<void()> ParamBase::getProvider()
{
    // The Node implements operator() to create closure/functor.
    // All output values will be updated.
    return [this]() {(*this->_parentNode)();};
}


// ------------------------------------------------------------------------------
// isConnectableTo
//      returns true if this parameter can be connected to "otherParam"
bool ParamBase::isConnectableTo(ParamBase *otherParam)
{
    if (!this->isConnectable())
        return false;

    if (!otherParam->isConnectable())
        return false;

    // Check that the types match
    if (typeid(*this) != typeid(*otherParam))
        return false;

    // LATER allow for automatic type promotion/conversion--
    // e.g. convert "int" types to "floats",
    //      "floats" to "colors"
    // perhaps even "floats" to "ints" via trucation.

    // Make this is an input param and the other is an output param
    if (this->isOutput() == otherParam->isOutput())
        return false;

    // XXX check for cycles :-P

    // Everything looks OK!
    return true;
}

// ------------------------------------------------------------------------------
// connectTo
//      connects this parameter (the "client") to an output parameter on another
//      node (the "server").
void ParamBase::connectTo(ParamBase *server)
{
    // This assumes that server and client have already
    // been verified as being compatable:  i.e., server is an output,
    // client is an input; they have the same type; etc.

    this->_connectedParam = server;

    // GROSS -- There's got to be a better way to do this.
    {
    Param<int> *s = dynamic_cast<Param<int> *>(server);
    Param<int> *c = dynamic_cast<Param<int> *>(this);
    if (s && c) {
        c->setProvider(s->getProvider());
        return;
    }
    }

    {
    Param<float> *s = dynamic_cast<Param<float> *>(server);
    Param<float> *c = dynamic_cast<Param<float> *>(this);
    if (s && c) {
        c->setProvider(s->getProvider());
        return;
    }
    }

    {
    Param<Lightcolor> *s = dynamic_cast<Param<Lightcolor> *>(server);
    Param<Lightcolor> *c = dynamic_cast<Param<Lightcolor> *>(this);
    if (s && c) {
        c->setProvider(s->getProvider());
        return;
    }
    }

    {
    Param<bool> *s = dynamic_cast<Param<bool> *>(server);
    Param<bool> *c = dynamic_cast<Param<bool> *>(this);
    if (s && c) {
        c->setProvider(s->getProvider());
        return;
    }
    }

    qDebug() << "ERROR" << Q_FUNC_INFO;

    qDebug() << "as per typeid(*this/server):";
    qDebug() << "thisType    " << typeid(*this).name();
    qDebug() << "serverType  " << typeid(*server).name() << endl;

    qDebug() << "as per _type:";
    qDebug() << "thisType    " << this->_type.name();
    qDebug() << "serverType  " << server->_type.name() << endl;

    qDebug() << "as per getType:";
    qDebug() << "thisType    " << this->getType().name();
    qDebug() << "serverType  " << server->getType().name() << endl;
}


// ------------------------------------------------------------------------------
// Serialization
//      (or "file i/o," as we called in the old days.)

// Writing (base class)
void ParamBase::writeToJSONObj(QJsonObject &json) const
{
//    json["type"] = _type; // TODO translate
    json["name"] = _name;
    json["uuid"] = _uuid.toString();
    if (_connectedParam)
        json["connectedTo"] = _connectedParam->_uuid.toString();
#if 0
    // These shouldn't change for a given parameter, and so for now
    // we don't output them.
    json["isOutput"] = _isOutput;
    json["isConnectable"] = _isConnectable;
#endif
    // Values of params are written by the type-specialized versions below.
}

// Writing (template specializations)
template <> void Param<Lightcolor>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;

    json["red"] = _value.getRed();
    json["green"] = _value.getGreen();
    json["blue"] = _value.getBlue();
}

template <> void Param<float>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

template <> void Param<bool>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

template <> void Param<int>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

// Reading (base class)
void ParamBase::readFromJSONObj(const QJsonObject &json)
{
    // ErrorHandling
    _uuid = QUuid(json["uuid"].toString());
}

// Reading (template specializations)
template <> void Param<int>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    if (isOutput())
        return;
    setValue(json["value"].toInt());
}

template <> void Param<float>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    if (isOutput())
        return;
    setValue(json["value"].toDouble());
}

template <> void Param<bool>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    if (isOutput())
        return;
    setValue(json["value"].toBool());
}

template <> void Param<Lightcolor>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    if (isOutput())
        return;
    _value = Lightcolor(json["red"].toInt(), json["green"].toInt(), json["blue"].toInt());
}
