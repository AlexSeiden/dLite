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

    QAction *m_loadsongAction;
    QAction *m_pauseAction;
    QAction *m_playAction;
    QAction *m_settingsAction;
    QAction *m_saveAction;
    QAction *m_openAction;

    QIcon                   m_pauseIcon;
    QIcon                   m_playIcon;
    QIcon                   m_settingsIcon;

    QShortcut*              m_playPauseShortcut;
    QShortcut*              m_rewindShortcut;

};

#endif // TRANSPORTCONTROL_H
