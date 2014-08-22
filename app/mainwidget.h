#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QAudio>
#include <QIcon>
#include <QWidget>
#include <QIntValidator>
#include <QSpinBox>

class Engine;
class FrequencySpectrum;
class LevelMeter;
class SublevelMeter;
class ProgressBar;
class SettingsDialog;
class Spectrograph;
class Controlpanel;
class Dancefloorwidget;
class Dancefloormodel;

QT_BEGIN_NAMESPACE
class QAction;
class QAudioFormat;
class QLabel;
class QMenu;
class QPushButton;
class QSpinBox;
class QLineEdit;
QT_END_NAMESPACE

/**
 * Main application widget, responsible for connecting the various UI
 * elements to the Engine.
 */
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

    // QObject
    void timerEvent(QTimerEvent *event);

public slots:
    void stateChanged(QAudio::State state);
    void spectrumChanged(qint64 position, qint64 length,
                         const FrequencySpectrum &spectrum);
    void infoMessage(const QString &message, int timeoutMs);
    void errorMessage(const QString &heading, const QString &detail);
    void audioPositionChanged(qint64 position);
    void bufferLengthChanged(qint64 length);

private slots:
    void showFileDialog();
    void showSettingsDialog();
    void updateButtonStates();

private:
    void createUi();
    void createMenus();
    void connectUi();
    void reset();

private:
    Engine*                 m_engine;

    ProgressBar*            m_progressBar;
    Spectrograph*           m_spectrograph;
    LevelMeter*             m_levelMeter;

    QPushButton*            m_fileButton;
    QPushButton*            m_pauseButton;
    QIcon                   m_pauseIcon;
    QPushButton*            m_playButton;
    QIcon                   m_playIcon;
    QPushButton*            m_settingsButton;
    QIcon                   m_settingsIcon;

    QSpinBox *				m_numBandsSpinBox;
    QSpinBox *				m_specMinSpinBox;
    QSpinBox *				m_specMaxSpinBox;

    QLabel*                 m_infoMessage;
    int                     m_infoMessageTimerId;

    SettingsDialog*         m_settingsDialog;

    QAction*                m_loadFileAction;

    Dancefloormodel *       m_dancefloormodel;
    Dancefloorwidget *      m_dancefloorwidget;
    Controlpanel *          m_controlpanel;
};

#endif // MAINWIDGET_H
