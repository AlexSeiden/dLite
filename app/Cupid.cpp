#include "Cupid.h"

// This is the class that handles connections between objects.
Cupid::Cupid(QObject *parent) :
    QObject(parent)
{ }

#if 0 // NUKEMEMAYBE
void Cupid::inputSeeksOutput(ParamBase *param) {  }
void Cupid::outputSeeksInput(ParamBase *param) {  }
#endif

Cupid * Cupid::Singleton() {
    static Cupid  _singleton;
    return &_singleton;
}

