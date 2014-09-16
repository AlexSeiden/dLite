#include "Cupid.h"

Cupid::Cupid(QObject *parent) :
    QObject(parent)
{ }

Cupid * Cupid::Singleton() {
    static Cupid  _singleton;
    return &_singleton;
}

