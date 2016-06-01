// Copyright (C) 2014-2016 Alex Seiden 
//
#include "engine/dispatch.h"

Dispatch::Dispatch(QObject *parent) :
    QObject(parent)
{ }

Dispatch * Dispatch::Singleton() {
    static Dispatch  _singleton;
    return &_singleton;
}

