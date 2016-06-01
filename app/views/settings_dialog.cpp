// Copyright (c) 2014-2016 Alex Seiden
// Portions Copyright (C) Digia Plc as noted below.
//
/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**/

#include "views/settings_dialog.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QLineEdit>

// TODO get starting values from dancefloor, engine, etc.
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
