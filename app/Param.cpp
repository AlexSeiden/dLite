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

    // Everything looks OK!
    return true;

}
