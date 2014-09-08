#include "CueBox.h"
#include "dancefloormodel.h"
#include <QDebug>
#include "Node.h"

Dancefloormodel *Cue::_dfModel = nullptr;

CueBox::CueBox() :
    Cue(),
    _x(10),
    _y(8),
    _scale(1.0),
    _alpha(1.0),
    _color(Lightcolor(255,255,255))
{
    int numCues = _dfModel->getNumCues();
    setName(QString("BoxCue%1").arg(numCues));

    _x.setName("x");
    _x.setOutput(false);
    _x.setConnectable(true);

    _y.setName("y");
    _y.setOutput(false);
    _y.setConnectable(true);

    _scale.setName("scale");
    _scale.setOutput(false);
    _scale.setConnectable(true);

    _alpha.setName("alpha");
    _alpha.setOutput(false);
    _alpha.setConnectable(true);

    _color.setName("color");
    _color.setOutput(false);
    _color.setConnectable(true);

    _paramList << &_x <<&_y <<&_scale << &_alpha << &_color;
}

void CueBox::operator ()() {
    evaluate();
}

void CueBox::evaluate()
{
    if (!_active) return;

    evalAllInputs();

//    qDebug() << "x getvalue " << x;

//    int otherx;
//    std::function<void(int &value)> lp = _x.getProvider();
//    lp(otherx);
//    qDebug() << "other x    " << otherx;
//    (*_x._connectedNode)(otherx);

//    RandomInt *rn = dynamic_cast<RandomInt *>(_x._connectedNode);
//    int rnx = -1;
//    (*rn)(rnx);
//    qDebug() << "rnx        " << rnx;



    Lightcolor color;
    float alpha;
    int x,y;

    _color.getValue(color);
    _alpha.getValue(alpha);
    _x.getValue(x);
    _y.getValue(y);
    color *= alpha;

    // "Fire" the light with the correct parameters
    Firing *firing = new Firing;
    firing->_color = color;
    firing->_alpha = alpha;
    firing->setDecay(1);    // TODO
    _dfModel->fireLight(x, y, firing);
}


static Registrar<CueBox>     registrar("BoxCue", Node::CUE);
