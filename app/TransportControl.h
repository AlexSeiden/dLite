#ifndef TRANSPORTCONTROL_H
#define TRANSPORTCONTROL_H

#include <QToolBar>
#include <QIcon>
#include <QPushButton>
#include <QShortcut>
#include <QAction>


class TransportControl : public QToolBar
{
    Q_OBJECT
public:
    explicit TransportControl(QWidget *parent = 0);

signals:

public slots:
    void updateButtonStates();

private:
    void connectUi();

    QAction *loadsongAction;
    QAction *pauseAction;
    QAction *playAction;
    QAction *settingsAction;
    QAction *saveAction;
    QAction *openAction;

    QIcon                   m_pauseIcon;
    QIcon                   m_playIcon;
    QIcon                   m_settingsIcon;

    QShortcut*              m_playPauseShortcut;
    QShortcut*              m_rewindShortcut;

};

#endif // TRANSPORTCONTROL_H
