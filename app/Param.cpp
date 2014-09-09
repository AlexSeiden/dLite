#include "Param.h"
#include "lightcolor.h"
#include <QDebug>
#include "Node.h"

// These constants defined for convinience & speed when doing type checks.
const std::type_info & paramTypeFloat = typeid(Param<float>);
const std::type_info & paramTypeInt = typeid(Param<int>);
const std::type_info & paramTypeLcolor = typeid(Param<Lightcolor>);

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


void ParamBase::connectParams(ParamBase *server, ParamBase *client)
{
    // This assumes that server and client have already
    // been verified as being compatable:  i.e., server is an output,
    // client is an input; they have the same type; etc.

   Param<int> *s = dynamic_cast<Param<int> *>(server);
   Param<int> *c = dynamic_cast<Param<int> *>(client);
   if (s && c) {
       c->setProvider(s->getProvider());
       return;
   }

   qDebug() << "ERROR--connectParams";
}


void ParamBase::connectTo(ParamBase *server)
{
    // This assumes that server and client have already
    // been verified as being compatable:  i.e., server is an output,
    // client is an input; they have the same type; etc.

    this->_connectedNode = server->getParent();
    this->_connectedParam = server;
    // XXX theres got to be a better way to do this.
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

    qDebug() << "ERROR--connectTo";

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

