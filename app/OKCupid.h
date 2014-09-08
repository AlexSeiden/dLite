#ifndef OKCUPID_H
#define OKCUPID_H

#include <QObject>
#include "Param.h"

class OKCupid : public QObject
{
    Q_OBJECT
public:
    explicit OKCupid(QObject *parent = 0);

signals:

public slots:
    void    inputSeeksOutput(ParamBase *param);
    void    outputSeeksInput(ParamBase *param);

};

#endif // OKCUPID_H
