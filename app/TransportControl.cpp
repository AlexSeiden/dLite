#include "TransportControl.h"
#include "Cupid.h"
#include <QStyle>
#include <QLayout>

TransportControl::TransportControl(QWidget *parent) :
    QToolBar(parent)
{
    setWindowTitle("Transport Control");
    m_loadsongAction = new QAction(tr("Open..."), this);
    m_loadsongAction->setIcon(QIcon(":/images/settings.png"));
    m_loadsongAction->setShortcut(QKeySequence("Ctrl+Shift+O"));
    m_loadsongAction->setShortcutContext(Qt::ApplicationShortcut);
    m_loadsongAction->setStatusTip(tr("Load song..."));
    addAction(m_loadsongAction);
    CHECKED_CONNECT(m_loadsongAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(showLoadSongDialog()));

    m_pauseAction = new QAction(tr("Pause"), this);
    m_pauseAction->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    m_pauseAction->setStatusTip(tr("Pause"));
    addAction(m_pauseAction);
    CHECKED_CONNECT(m_pauseAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(suspend()));

    m_playAction = new QAction(tr("Play"), this);
    m_playAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_playAction->setStatusTip(tr("Play"));
    addAction(m_playAction);
    CHECKED_CONNECT(m_playAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(startPlayback()));
    m_playAction->setEnabled(false);

    m_settingsAction = new QAction(tr("Settings..."), this);
    m_settingsAction->setIcon(QIcon(":/images/settings.png"));
    m_settingsAction->setStatusTip(tr("Settings..."));
    addAction(m_settingsAction);
    CHECKED_CONNECT(m_settingsAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(showSettingsDialog()));

    m_saveAction = new QAction(tr("Save..."), this);
    m_saveAction->setIcon(QIcon(":/images/settings.png"));
    m_saveAction->setShortcut(QKeySequence("Ctrl+S"));
    m_saveAction->setShortcutContext(Qt::ApplicationShortcut);
    m_saveAction->setStatusTip(tr("Save..."));
    addAction(m_saveAction);
    CHECKED_CONNECT(m_saveAction, SIGNAL(triggered()),
                    Cupid::Singleton(), SIGNAL(showSaveDialog()));


    m_openAction = new QAction(tr("Open dLite file..."), this);
    m_openAction->setIcon(QIcon(":/images/settings.png"));
    m_openAction->setShortcut(QKeySequence("Ctrl+O"));
    m_openAction->setShortcutContext(Qt::ApplicationShortcut);
    m_openAction->setStatusTip(tr("Open dLite file..."));
    addAction(m_openAction);
    CHECKED_CONNECT(m_openAction, SIGNAL(triggered()),
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
    m_pauseAction->setEnabled(pauseEnabled);

    const bool playEnabled = ( (QAudio::ActiveState != Cupid::Singleton()->getEngine()->state() &&
                                QAudio::IdleState != Cupid::Singleton()->getEngine()->state()));
    m_playAction->setEnabled(playEnabled);
}

