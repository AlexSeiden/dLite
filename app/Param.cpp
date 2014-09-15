#include "Param.h"
#include "lightcolor.h"
#include <QDebug>
#include "Node.h"
#include <QJsonObject>
#include <QJsonArray>

// These constants defined for convinience & speed when doing type checks.
const std::type_info & paramTypeFloat = typeid(Param<float>);
const std::type_info & paramTypeInt = typeid(Param<int>);
const std::type_info & paramTypeLcolor = typeid(Param<Lightcolor>);
const std::type_info & paramTypeBool = typeid(Param<bool>);

std::function<void()> ParamBase::getProvider()
{
    // The Node implements operator() to create closure/functor.
    // All output values will be updated.
    return [this]() {(*this->_parentNode)();};
}

bool ParamBase::isConnectableTo(ParamBase *otherParam)
{
    if (!this->isConnectable())
        return false;

    if (!otherParam->isConnectable())
        return false;

    // Check that the types match
    if (typeid(*this) != typeid(*otherParam))
        return false;

    // Make sure one is an input param and one is an output param
    if (this->isOutput() == otherParam->isOutput())
        return false;

    // LATER allow for auto type promotion/conversion

    // LATER check for cycles :-P

    // Everything looks OK!
    return true;
}

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

// -------------------------
// Param i/o

// Writing
void ParamBase::write(QJsonObject &json) const
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


// Template specializations
template <> void Param<Lightcolor>::write(QJsonObject &json) const
{
    ParamBase::write(json);
    if (isOutput())
        return;

    json["red"] = _value.getRed();
    json["green"] = _value.getGreen();
    json["blue"] = _value.getBlue();
}

template <> void Param<float>::write(QJsonObject &json) const
{
    ParamBase::write(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

template <> void Param<bool>::write(QJsonObject &json) const
{
    ParamBase::write(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

template <> void Param<int>::write(QJsonObject &json) const
{
    ParamBase::write(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

// Reading

void ParamBase::read(const QJsonObject &json)
{
    // ErrorHandling
    _uuid = QUuid(json["uuid"].toString());
}

template <> void Param<int>::read(const QJsonObject &json)
{
    ParamBase::read(json);
    if (isOutput())
        return;
    setValue(json["value"].toInt());
}

template <> void Param<float>::read(const QJsonObject &json)
{
    ParamBase::read(json);
    if (isOutput())
        return;
    setValue(json["value"].toDouble());
}

template <> void Param<bool>::read(const QJsonObject &json)
{
    ParamBase::read(json);
    if (isOutput())
        return;
    setValue(json["value"].toBool());
}

template <> void Param<Lightcolor>::read(const QJsonObject &json)
{
    ParamBase::read(json);
    if (isOutput())
        return;
    _value = Lightcolor(json["red"].toInt(), json["green"].toInt(), json["blue"].toInt());
}
