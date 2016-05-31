#include <QDialogButtonBox>
#include <QPushButton>
#include <QBoxLayout>

#include "views/rename_tab_dialog.h"
#include "engine/utils.h"

RenameTabDialog::RenameTabDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);

    m_titleLabel = new QLabel(QString("Change name of cuesheet"), this);
    dialogLayout->addWidget(m_titleLabel);

    m_tabNameEditWidget = new QLineEdit(this);
    m_tabNameEditWidget->setText("");
    QScopedPointer<QHBoxLayout> editLayout (new QHBoxLayout);
    QLabel *editLabel = new QLabel(tr("New name:"), this);
    editLayout->addWidget(editLabel);
    editLayout->addWidget(m_tabNameEditWidget);
    dialogLayout->addLayout(editLayout.data());
    editLayout.take(); // ownership transferred to dialogLayout

    CHECKED_CONNECT(m_tabNameEditWidget, SIGNAL(textChanged(QString)),
                    this, SLOT(nameChanged(QString)));

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

void RenameTabDialog::setTabname(const QString name)
{
    m_tabName = name;
    m_tabNameEditWidget->setText(name);
    m_tabNameEditWidget->selectAll();
}

void RenameTabDialog::setIndex(int index)
{
    m_index = index;
    m_titleLabel->setText(QString("Change name of cuesheet #%1").arg(m_index));
}

void RenameTabDialog::nameChanged(const QString text)
{
    m_tabName = text;
}
