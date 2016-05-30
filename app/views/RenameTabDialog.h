#ifndef RENAMETABDIALOG_H
#define RENAMETABDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>

class RenameTabDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RenameTabDialog(QWidget *parent = 0);

    QString tabname() const     { return m_tabName;}
    void setTabname(QString n);
    void setIndex(int i);

private slots:
    void nameChanged(const QString text);

private:
    QLineEdit*              m_tabNameEditWidget;
    QString                 m_tabName;
    QLabel*                 m_titleLabel;
    int                     m_index;
};

#endif // RENAMETABDIALOG_H
