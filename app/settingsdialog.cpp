#include "settingsdialog.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QLineEdit>

// TODO get starting values from dancefloor engine etc.
SettingsDialog::SettingsDialog(int interval, QWidget *parent)
    :   QDialog(parent)
    ,   m_windowFunction(DefaultWindowFunction)
    ,   m_interval(interval)
    ,   m_windowFunctionComboBox(new QComboBox(this))
    ,   m_intervalLineEdit(new QLineEdit(this))
    ,   m_useHardwareCheckbox(new QCheckBox(this))
{
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);

    // Populate pull down menu with choices of windowing functions for spectral analysis
    m_windowFunctionComboBox->addItem(tr("None"), QVariant::fromValue(int(NoWindow)));
    m_windowFunctionComboBox->addItem("Hann", QVariant::fromValue(int(HannWindow)));
    m_windowFunctionComboBox->setCurrentIndex(m_windowFunction);

    // Add widgets to layout

    QScopedPointer<QHBoxLayout> windowFunctionLayout(new QHBoxLayout);
    QLabel *windowFunctionLabel = new QLabel(tr("Window function"), this);
    windowFunctionLayout->addWidget(windowFunctionLabel);
    windowFunctionLayout->addWidget(m_windowFunctionComboBox);
    dialogLayout->addLayout(windowFunctionLayout.data());
    windowFunctionLayout.take(); // ownership transferred to dialogLayout

    m_intervalLineEdit->setValidator(new QIntValidator(m_intervalLineEdit));
    m_intervalLineEdit->setText(QString::number(m_interval));
    QScopedPointer<QHBoxLayout> intervalLayout (new QHBoxLayout);
    QLabel *intervalLabel = new QLabel(tr("Update Interval (ms)"), this);
    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(m_intervalLineEdit);
    dialogLayout->addLayout(intervalLayout.data());
    intervalLayout.take(); // ownership transferred to dialogLayout

    QScopedPointer<QHBoxLayout> checkboxLayout (new QHBoxLayout);
    QLabel *checkboxLabel = new QLabel(tr("Use LED hardware"), this);
    checkboxLayout->addWidget(checkboxLabel);
    checkboxLayout->addWidget(m_useHardwareCheckbox);
    dialogLayout->addLayout(checkboxLayout.data());
    checkboxLayout.take(); // ownership transferred to dialogLayout

    CHECKED_CONNECT(m_windowFunctionComboBox, SIGNAL(activated(int)),
                    this, SLOT(windowFunctionChanged(int)));
    CHECKED_CONNECT(m_intervalLineEdit, SIGNAL(textChanged(QString)),
                    this, SLOT(intervalChanged(QString)));
    CHECKED_CONNECT(m_useHardwareCheckbox, SIGNAL(stateChanged(int)),
                    this, SLOT(useHardwareChanged(int)));

    // Add standard OK/Cancel buttons to layout
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    dialogLayout->addWidget(buttonBox);

    CHECKED_CONNECT(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
                    this, SLOT(accept()));
    CHECKED_CONNECT(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
                    this, SLOT(reject()));

    setLayout(dialogLayout);
}

SettingsDialog::~SettingsDialog() { }

void SettingsDialog::windowFunctionChanged(int index)
{
    m_windowFunction = static_cast<WindowFunction>(
            m_windowFunctionComboBox->itemData(index).value<int>());
}

void SettingsDialog::intervalChanged(const QString text)
{
    m_interval = text.toInt();
}

void SettingsDialog::useHardwareChanged(int val)
{
    m_useHardware = val;
}
