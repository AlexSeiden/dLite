#include "TransportControl.h"
#include "Cupid.h"
#include <QStyle>
#include <QLayout>

TransportControl::TransportControl(QWidget *parent) :
    QToolBar(parent)
{
    setWindowTitle("Transport Control");
    loadsongAction = new QAction(tr("Open..."), this);
    loadsongAction->setIcon(QIcon(":/images/settings.png"));
    loadsongAction->setShortcut(QKeySequence("Ctrl+Shift+O"));
    loadsongAction->setShortcutContext(Qt::ApplicationShortcut);
    loadsongAction->setStatusTip(tr("Load song..."));
    addAction(loadsongAction);
    CHECKED_CONNECT(loadsongAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(showLoadSongDialog()));

    pauseAction = new QAction(tr("Pause"), this);
    pauseAction->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    pauseAction->setStatusTip(tr("Pause"));
    addAction(pauseAction);
    CHECKED_CONNECT(pauseAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(suspend()));

    playAction = new QAction(tr("Play"), this);
    playAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playAction->setStatusTip(tr("Play"));
    addAction(playAction);
    CHECKED_CONNECT(playAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(startPlayback()));
    playAction->setEnabled(false);

    settingsAction = new QAction(tr("Settings..."), this);
    settingsAction->setIcon(QIcon(":/images/settings.png"));
    settingsAction->setStatusTip(tr("Settings..."));
    addAction(settingsAction);
    CHECKED_CONNECT(settingsAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(showSettingsDialog()));

    saveAction = new QAction(tr("Save..."), this);
    saveAction->setIcon(QIcon(":/images/settings.png"));
    saveAction->setShortcut(QKeySequence("Ctrl+S"));
    saveAction->setShortcutContext(Qt::ApplicationShortcut);
    saveAction->setStatusTip(tr("Save..."));
    addAction(saveAction);
    CHECKED_CONNECT(saveAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(showSaveDialog()));


    openAction = new QAction(tr("Open dLite file..."), this);
    openAction->setIcon(QIcon(":/images/settings.png"));
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    openAction->setShortcutContext(Qt::ApplicationShortcut);
    openAction->setStatusTip(tr("Open dLite file..."));
    addAction(openAction);
    CHECKED_CONNECT(openAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(showOpenDialog()));

    // ----------------------------------------
    // Transport shortcuts
    m_playPauseShortcut = new QShortcut(Qt::Key_Space, this);
    m_playPauseShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_playPauseShortcut, SIGNAL(activated()),
                    Cupid::Singleton(), SIGNAL(togglePlayback()));

    m_rewindShortcut = new QShortcut(QKeySequence("Ctrl+0"), this);
    m_rewindShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_rewindShortcut, SIGNAL(activated()),
                    Cupid::Singleton(), SIGNAL(rewind()));

    // Incoming connections
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(updateButtonStates()),
                    this, SLOT(updateButtonStates()));
}

void TransportControl::updateButtonStates()
{
    const bool pauseEnabled = (QAudio::ActiveState == Cupid::Singleton()->getEngine()->state() ||
                               QAudio::IdleState == Cupid::Singleton()->getEngine()->state());
    pauseAction->setEnabled(pauseEnabled);

    const bool playEnabled = ( (QAudio::ActiveState != Cupid::Singleton()->getEngine()->state() &&
                                QAudio::IdleState != Cupid::Singleton()->getEngine()->state()));
    playAction->setEnabled(playEnabled);
}

