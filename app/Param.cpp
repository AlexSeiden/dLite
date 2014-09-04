#include "Param.h"
#include "lightcolor.h"

// These constants defined for convinience & speed when doing type checks.
const std::type_info & paramTypeFloat = typeid(Param<float>);
const std::type_info & paramTypeInt = typeid(Param<int>);
const std::type_info & paramTypeLcolor = typeid(Param<Lightcolor>);

bool ParamBase::isConnectableTo(ParamBase *otherParam)
{
    if (!this->isConnectable())
        return false;

    if (!otherParam->isConnectable())
        return false;

    // Check that the types match
    if (typeid(this) != typeid(otherParam))
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

   //qDebug() << "ERROR";

}
