#include "engine/dispatch.h"

Dispatch::Dispatch(QObject *parent) :
    QObject(parent)
{ }

Dispatch * Dispatch::Singleton() {
    static Dispatch  _singleton;
    return &_singleton;
}

