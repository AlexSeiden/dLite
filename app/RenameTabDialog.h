#ifndef RENAMETABDIALOG_H
#define RENAMETABDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QLabel;
QT_END_NAMESPACE

class RenameTabDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RenameTabDialog(QWidget *parent = 0);

    QString tabname() const     { return _tabName;}
    void setTabname(QString n);
    void setIndex(int i);

private slots:
    void nameChanged(const QString text);

private:
    QLineEdit*              _tabNameEditWidget;
    QString                 _tabName;
    QLabel*                 _titleLabel;
    int                     _index;
};

#endif // RENAMETABDIALOG_H
