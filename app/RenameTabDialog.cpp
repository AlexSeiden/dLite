#include "RenameTabDialog.h"
#include <QLineEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include "utils.h"

RenameTabDialog::RenameTabDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);

    _titleLabel = new QLabel(QString("Change name of cuesheet"), this);
    dialogLayout->addWidget(_titleLabel);

    _tabNameEditWidget = new QLineEdit(this);
    _tabNameEditWidget->setText("");
    QScopedPointer<QHBoxLayout> editLayout (new QHBoxLayout);
    QLabel *editLabel = new QLabel(tr("New name:"), this);
    editLayout->addWidget(editLabel);
    editLayout->addWidget(_tabNameEditWidget);
    dialogLayout->addLayout(editLayout.data());
    editLayout.take(); // ownership transferred to dialogLayout

    CHECKED_CONNECT(_tabNameEditWidget, SIGNAL(textChanged(QString)),
                    this, SLOT(nameChanged(QString)));

    // Add standard OK/Cancel buttons to layout
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    dialogLayout->addWidget(buttonBox);

    // Connect OK/Cancel
    CHECKED_CONNECT(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
                    this, SLOT(accept()));
    CHECKED_CONNECT(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
                    this, SLOT(reject()));

    setLayout(dialogLayout);
}

void RenameTabDialog::setTabname(const QString name)
{
    _tabName = name;
    _tabNameEditWidget->setText(name);
    _tabNameEditWidget->selectAll();
}

void RenameTabDialog::setIndex(int index)
{
    _index = index;
    _titleLabel->setText(QString("Change name of cuesheet #%1").arg(_index));
}

void RenameTabDialog::nameChanged(const QString text)
{
    _tabName = text;
}
