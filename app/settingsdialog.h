#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "spectrum.h"
#include <QDialog>
#include <QAudioDeviceInfo>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QSlider;
class QSpinBox;
class QGridLayout;
class QLineEdit;
QT_END_NAMESPACE

/**
 * Dialog used to control settings such as the audio input / output device
 * and the windowing function.
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(int interval,
                   QWidget *parent = 0);
    ~SettingsDialog();

    WindowFunction windowFunction() const { return m_windowFunction; }
    const QAudioDeviceInfo &inputDevice() const { return m_inputDevice; }
    const QAudioDeviceInfo &outputDevice() const { return m_outputDevice; }
    int interval() const { return m_interval; }

private slots:
    void windowFunctionChanged(int index);
    void inputDeviceChanged(int index);
    void outputDeviceChanged(int index);
    void intervalChanged(const QString text);

private:
    WindowFunction   m_windowFunction;
    QAudioDeviceInfo m_inputDevice;
    QAudioDeviceInfo m_outputDevice;
    int m_interval;
    int      				m_spectrumNumBands;
    int      				m_spectrumLow;
    int      				m_spectrumHi;

    QComboBox *m_inputDeviceComboBox;
    QComboBox *m_outputDeviceComboBox;
    QComboBox *m_windowFunctionComboBox;
    QLineEdit *m_intervalLineEdit;

#if 0
    QSpinBox*				m_spectrumNumBandsSpinbox;
    QSpinBox*				m_spectrumLowSpinbox;
    QSpinBox*				m_spectrumHiSpinbox;
#endif

};

#endif // SETTINGSDIALOG_H
