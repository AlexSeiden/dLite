#ifndef CUESHEET_H
#define CUESHEET_H

#include <QWidget>

namespace Ui {
class CueSheet;
}

class CueSheet : public QWidget
{
    Q_OBJECT

public:
    explicit CueSheet(QWidget *parent = 0);
    ~CueSheet();

private:
    Ui::CueSheet *ui;
};

#endif // CUESHEET_H
