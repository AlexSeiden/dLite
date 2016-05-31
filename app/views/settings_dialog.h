#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "engine/spectrum.h"
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

/*
 * Dialog used to control global settings:
 *      the windowing function used by the spectum analyser
 *      the update interval for spectral analysis and dance floor
 *      whether to send info to the LED hardware.
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(int interval, QWidget *parent = 0);
    ~SettingsDialog();

    WindowFunction windowFunction() const { return m_windowFunction; }
    int interval() const { return m_interval; }
    bool useHardware() const { return m_useHardware; }

private slots:
    void windowFunctionChanged(int index);
    void intervalChanged(const QString text);
    void useHardwareChanged(int index);

private:
    WindowFunction          m_windowFunction;
    int                     m_interval;
    bool                    m_useHardware;

    QComboBox*              m_windowFunctionComboBox;
    QLineEdit*              m_intervalLineEdit;
    QSpinBox*				m_numBandsSpinBox;
    QSpinBox*				m_specMinSpinBox;
    QSpinBox*				m_specMaxSpinBox;
    QCheckBox*              m_useHardwareCheckbox;

};

#endif // SETTINGSDIALOG_H
