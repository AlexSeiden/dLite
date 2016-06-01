// Copyright (C) 2014-2016 Alex Seiden 
//
// Classes that encapsulate individual parameters on Nodes & Cues.

#ifndef NODEPARAMETERS_H
#define NODEPARAMETERS_H

#include <QList>
#include <QString>
#include <QVariant>
#include <QUuid>
#include <functional>
#include <typeinfo>
#include "engine/lightcolor.h"


// ParamBase
//      An abstract base class.

class Node;
class QJsonObject;
class QWidget;
class QObject;

enum param_t {NONE, FLOAT, INT, BOOL, COLOR, STRING, REGION};

class ParamBase
{
public:
    ParamBase() :
        m_provider(nullptr),
        m_connectedParam(nullptr),
        m_isOutput(false),
        m_isConnectable(true),
        m_type(typeid(this)),
        m_uuid(QUuid::createUuid()),
        m_useminmax(false),
        m_minVal(0.),
        m_maxVal(1.)
    { }
    virtual ~ParamBase() { }

    QString     getName() const                {return m_name;}
    void        setName(QString name)          {m_name = name;}
    bool        isOutput() const               {return m_isOutput;}
    bool        isConnectable() const          {return m_isConnectable;}
    void        setOutput(bool status)         {m_isOutput = status;}
    void        setConnectable(bool status)    {m_isConnectable = status;}
    ParamBase*  connectedParam()               {return m_connectedParam;}
    QUuid&      getUuid()                      {return m_uuid;}

    bool        isConnectableTo(ParamBase *otherParam);
    void        connectTo(ParamBase *server);
    void        copyValueAndConnection(ParamBase *rhs);
    void        disconnect() {m_connectedParam = nullptr; m_provider = nullptr;}

    void        setRange(bool userange, double min=0.0, double max=1.0, double step=1.0);

    virtual void eval() {
        if (m_provider)
            m_provider();
    }

    virtual const std::type_info & getType() {return m_type;}

    std::function<void()> getProvider();

    // This applies to input parameters only: it sets the provider "function"
    // that is excuted to compute the value of this parameter.
    void setProvider(std::function<void()> provider) {m_provider = provider;}

    // Returns a pointer of the Node that this is a parameter for.
    Node *  getParentNode() {return m_parentNode;}
    void    setParentNode(Node *parent) {m_parentNode = parent;}

    // Valid only for input nodes
    Node *getServer() {
        if (m_connectedParam)
            return m_connectedParam->getParentNode();
        return nullptr;
    }

    virtual QWidget* getEditorWidget(QObject* sendValueChangesHere);

    // Serialization
    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

    // Would it be useful to maintain a linked list of connections here?
    // Both for inputs and outputs?  Would that break modularity?
    //QList<ParamBase *>  getConnections();
    // Would be valid only for output nodes
    //Node *getClients();

    // Any OUTPUT params need to stick their value in here.
    QVariant    _qvOutput;

protected:
    std::function<void()>   m_provider;
    ParamBase *             m_connectedParam;
    Node *                  m_parentNode;      // The node that this param is a part of.

    QString                 m_name;
    bool                    m_isOutput;
    bool                    m_isConnectable;
    const std::type_info &  m_type;
    QUuid                   m_uuid;

    // Only used for ranges on numeric types.
    bool                    m_useminmax;
    float                   m_minVal, m_maxVal, _stepVal;

    friend class NodeFactory;
};


// ------------------------------------------------------------------------------
// Templated Param class
//      Instatiated with the following types:
//          float
//          int
//          Lightcolor
//          bool (for triggers)
//          positions
//          regions

template <class PARAMT>
class Param : public ParamBase
{
public:
    Param(PARAMT value=PARAMT()) :
        m_value(value)
        {}

    virtual ~Param() {}

    virtual void getValue(PARAMT &value)  {
        // All parameter values are returned by reference, even for fundamental
        // types like ints and floats. This is for generality with
        // all types of nodes, such as Lightcolor & regions.
        value = m_value;
    }

    virtual void eval() {
        if (m_provider) {
            m_provider();
            m_value = m_connectedParam->_qvOutput.value<PARAMT>();
        }
    }

    void setValue(const PARAMT &value) {m_value=value; _qvOutput.setValue(value);}

    // ??? do we need this?  Would it be better just to assign a _type field?
    virtual const std::type_info & getType() {return typeid(this);}

    PARAMT m_value;

    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

    virtual QWidget* getEditorWidget(QObject* sendValueChangesHere);
};

// ------------------------------------------------------------------------------
// For convinience & speed when doing RTTI
extern const std::type_info & paramTypeFloat;
extern const std::type_info & paramTypeInt;
extern const std::type_info & paramTypeLightcolor;
extern const std::type_info & paramTypeBool;
extern const std::type_info & paramTypeRegion;

#endif // NODEPARAMETERS_H
