#include "Param.h"
#include "lightcolor.h"
#include "Region.h"
#include <QDebug>
#include "Node.h"
#include <QJsonObject>
#include <QJsonArray>

// These view things are for the editor widget callback at the bottom
#include <QSpinBox>
#include <QCheckBox>
#include <QToolButton>
#include <QEvent>
#include "utils.h"
#include "ColorChip.h"
#include "MyDoubleSpinBox.h"

// These constants defined for convinience & speed when doing type checks.
// ??? Are these still needed?  Would it be better just to have use an enum?
const std::type_info & paramTypeFloat       = typeid(Param<float>);
const std::type_info & paramTypeInt         = typeid(Param<int>); const std::type_info & paramTypeLightcolor  = typeid(Param<Lightcolor>);
const std::type_info & paramTypeBool        = typeid(Param<bool>);
const std::type_info & paramTypeRegion      = typeid(Param<Region>);

std::function<void()> ParamBase::getProvider()
{
    // The Node implements operator() to create closure/functor.
    // All output values will be updated.
    return [this]() {(*this->_parentNode)();};
}

// ------------------------------------------------------------------------------
// isConnectableTo
//      returns true if this parameter can be connected to "otherParam"
bool ParamBase::isConnectableTo(ParamBase *otherParam)
{
    if (!this->isConnectable())
        return false;

    if (!otherParam->isConnectable())
        return false;

    // Check that the types match
    if (typeid(*this) != typeid(*otherParam))
        return false;

    // LATER allow for automatic type promotion/conversion--
    // e.g. convert "int" types to "floats",
    //      "floats" to "colors"
    // perhaps even "floats" to "ints" via trucation.

    // Make this is an input param and the other is an output param
    if (this->isOutput() == otherParam->isOutput())
        return false;

    // XXX check for cycles :-P

    // Everything looks OK!
    return true;
}

// ------------------------------------------------------------------------------
// connectTo
//      connects this parameter (the "client") to an output parameter on another
//      node (the "server").
void ParamBase::connectTo(ParamBase *server)
{
    // This assumes that server and client have already
    // been verified as being compatable:  i.e., server is an output,
    // client is an input; they have the same type; etc.

    this->_connectedParam = server;

    // GROSS -- There's got to be a better way to do this.
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

    {
    Param<bool> *s = dynamic_cast<Param<bool> *>(server);
    Param<bool> *c = dynamic_cast<Param<bool> *>(this);
    if (s && c) {
        c->setProvider(s->getProvider());
        return;
    }
    }

    {
    Param<Region> *s = dynamic_cast<Param<Region> *>(server);
    Param<Region> *c = dynamic_cast<Param<Region> *>(this);
    if (s && c) {
        c->setProvider(s->getProvider());
        return;
    }
    }

    qDebug() << "ERROR" << Q_FUNC_INFO;

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

// Copy Value
// XXX  GROSS -- There's got to be a better way to do this.
void ParamBase::copyValueAndConnection(ParamBase *rhs)
{
    _connectedParam = rhs->_connectedParam;
    _provider = rhs->_provider;

    {
    Param<int> *s = dynamic_cast<Param<int> *>(rhs);
    Param<int> *c = dynamic_cast<Param<int> *>(this);
    if (s && c) {
        c->_value = s->_value;
        return;
    }
    }

    {
    Param<float> *s = dynamic_cast<Param<float> *>(rhs);
    Param<float> *c = dynamic_cast<Param<float> *>(this);
    if (s && c) {
        c->_value = s->_value;
        return;
    }
    }

    {
    Param<Lightcolor> *s = dynamic_cast<Param<Lightcolor> *>(rhs);
    Param<Lightcolor> *c = dynamic_cast<Param<Lightcolor> *>(this);
    if (s && c) {
        c->_value = s->_value;
        return;
    }
    }

    {
    Param<bool> *s = dynamic_cast<Param<bool> *>(rhs);
    Param<bool> *c = dynamic_cast<Param<bool> *>(this);
    if (s && c) {
        c->_value = s->_value;
        return;
    }
    }

    {
    Param<Region> *s = dynamic_cast<Param<Region> *>(rhs);
    Param<Region> *c = dynamic_cast<Param<Region> *>(this);
    if (s && c) {
        // XXX this probably isn't correct
        c->_value = s->_value;
        return;
    }
    }

    // XXX this won't handle copying of spectral range node's subrange params.

    // ErrorHandling

    qDebug() << "ERROR" << Q_FUNC_INFO;

    qDebug() << "as per typeid(*this/rhs):";
    qDebug() << "thisType    " << typeid(*this).name();
    qDebug() << "rhsType  " << typeid(*rhs).name() << endl;

    qDebug() << "as per _type:";
    qDebug() << "thisType    " << this->_type.name();
    qDebug() << "rhsType  " << rhs->_type.name() << endl;

    qDebug() << "as per getType:";
    qDebug() << "thisType    " << this->getType().name();
    qDebug() << "rhsType  " << rhs->getType().name() << endl;
}

// ------------------------------------------------------------------------------
// setRange
//      Sets min and max range used by numeric params.
void ParamBase::setRange(bool userange, double min, double max, double step)
{
    _useminmax = userange;
    _minVal = min;
    _maxVal = max;
    _stepVal = step;
}

// ------------------------------------------------------------------------------
// Serialization
//      (or "file i/o," as we called in the old days.)

// Writing (base class)
void ParamBase::writeToJSONObj(QJsonObject &json) const
{
    json["name"] = _name;
    json["uuid"] = _uuid.toString();
    if (_connectedParam)
        json["connectedTo"] = _connectedParam->_uuid.toString();
#if 0
    // These shouldn't change for a given parameter, and so for now
    // we don't output them.
    json["isOutput"] = _isOutput;
    json["isConnectable"] = _isConnectable;
//    json["type"] = _type; // TODO translate
#endif
    // Values of params are written by the type-specialized versions below.
}

// Writing (template specializations)
template <> void Param<Lightcolor>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;

    json["red"] = _value.getRed();
    json["green"] = _value.getGreen();
    json["blue"] = _value.getBlue();
}

template <> void Param<float>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

template <> void Param<bool>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

template <> void Param<int>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;
    json["value"] = _value;
}

template <> void Param<Region>::writeToJSONObj(QJsonObject &json) const
{
    ParamBase::writeToJSONObj(json);
    if (isOutput())
        return;
}

// Reading (base class)
void ParamBase::readFromJSONObj(const QJsonObject &json)
{
    // ErrorHandling
    _uuid = QUuid(json["uuid"].toString());
}

// Reading (template specializations)
template <> void Param<int>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    if (isOutput())
        return;
    setValue(json["value"].toInt());
}

template <> void Param<float>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    if (isOutput())
        return;
    setValue(json["value"].toDouble());
}

template <> void Param<bool>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    if (isOutput())
        return;
    setValue(json["value"].toBool());
}

template <> void Param<Lightcolor>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    if (isOutput())
        return;
    _value = Lightcolor(json["red"].toInt(), json["green"].toInt(), json["blue"].toInt());
}


template <> void Param<Region>::readFromJSONObj(const QJsonObject &json)
{
    ParamBase::readFromJSONObj(json);
    // XXX Node override handling this now.
}


// ------------------------------------------------------------------------------
// Editor widgetry
//      Although we partly break the separation between "model" and "view" here
//      (Since editors are clearly part of the view, and the params themselves
//      could be view-agnostic), it makes the code more cohesive,  rather
//      than implementing the editors in another class.
QWidget* ParamBase::getEditorWidget(QObject *sendValueChangesHere)
{
    Q_UNUSED(sendValueChangesHere);
    return nullptr;
}

// TODO subclass Spin Boxes
// and capture editingFinished signal to perform clearFocus()

template <> QWidget* Param<float>::getEditorWidget(QObject* sendValueChangesHere)
{
    MyDoubleSpinBox *editorWidget = new MyDoubleSpinBox;
    if (_useminmax) {    // TODO finish this
        editorWidget->setRange(_minVal, _maxVal);
        editorWidget->setSingleStep(_stepVal);
    }
    editorWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);

    float val = 0.0;
    getValue(val);
    editorWidget->setValue(val);
    //CHECKED_CONNECT(editorWidget, SIGNAL(valueChanged(double)), sendValueChangesHere, SLOT(setValue(double)));
    // TODO vet connection
    editorWidget->connect(editorWidget, SIGNAL(valueChanged(double)),
                          sendValueChangesHere, SLOT(setValue(double)));

    editorWidget->setFixedSize(60,22); // hardw
    // can't do this bc clearFocus is not a slot.
//    editorWidget->connect(editorWidget, SIGNAL(editingFinished()),
//                          editorWidget, SLOT(clearFocus()));

    // was trying to get centered widget...
//    editorWidget->move(0, -editorWidget->rect().bottom());
    editorWidget->setFocusPolicy(Qt::ClickFocus);
    return editorWidget;
}

template <> QWidget* Param<int>::getEditorWidget(QObject* sendValueChangesHere)
{
    QSpinBox *editorWidget = new QSpinBox;
//    editorWidget->installEventFilter(eventFilter);
    if (_useminmax) {    // TODO finish this
        editorWidget->setRange(_minVal, _maxVal);
        editorWidget->setSingleStep(_stepVal);
    }
//    editorWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);

    int val = 0;
    getValue(val);
    editorWidget->setValue(val);
    editorWidget->connect(editorWidget, SIGNAL(valueChanged(int)),
                          sendValueChangesHere, SLOT(setValue(int)));

    editorWidget->setFixedSize(60,22); // hardw
    return editorWidget;
}

template <> QWidget* Param<bool>::getEditorWidget(QObject* sendValueChangesHere)
{
    QCheckBox *editorWidget = new QCheckBox;
    bool val = true;
    getValue(val);
    editorWidget->setChecked(val);
    editorWidget->connect(editorWidget, SIGNAL(stateChanged(int)),
                          sendValueChangesHere, SLOT(setBoolValue(int)));
    editorWidget->move(0, -editorWidget->rect().bottom());
    editorWidget->setContentsMargins(0,0,0,0);
//    editorWidget->setAutoFillBackground(false);  // doesn't work
    return editorWidget;
}

template <> QWidget* Param<Region>::getEditorWidget(QObject* sendValueChangesHere)
{
    Q_UNUSED(sendValueChangesHere);
    // There's no editor widget for regions--we use the dance floor window.
    return nullptr;
}

template <> QWidget* Param<Lightcolor>::getEditorWidget(QObject* sendValueChangesHere)
{
    ColorChip *editorWidget =  new ColorChip;
    Lightcolor val;
    getValue(val);
    QColor qc = val.toQColor();
    editorWidget->setButtonColor(qc);
    editorWidget->connect(editorWidget, SIGNAL(valueChanged(Lightcolor)),
                          sendValueChangesHere, SLOT(setValue(Lightcolor)));

    return editorWidget;
}
