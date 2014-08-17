#ifndef CUEVIEW_H
#define CUEVIEW_H

#include <QWidget>

namespace Ui {
class CueView;
}

class CueView : public QWidget
{
    Q_OBJECT

public:
    explicit CueView(QWidget *parent = 0);
    ~CueView();

private:
    Ui::CueView *ui;
};

#endif // CUEVIEW_H
