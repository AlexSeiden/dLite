#include "OKCupid.h"

// This is the class that handles connections between objects.
OKCupid::OKCupid(QObject *parent) :
    QObject(parent)
{
}

void OKCupid::inputSeeksOutput(ParamBase *param)
{

}

void OKCupid::outputSeeksInput(ParamBase *param)
{

}

OKCupid * OKCupid::Singleton() {
    static OKCupid  _singleton;
    return &_singleton;
}
