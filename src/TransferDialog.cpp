#include "TransferDialog.h"
#include <QtWidgets>

TransferDialog::TransferDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Transfer funds"));

    QLabel* fromLabel = new QLabel(tr("From account:"));
    QLabel* fromVal = new QLabel;
    QLabel* toLabel = new QLabel(tr("To account number:"));
    toAccountEdit = new QLineEdit;

    QLabel* amountLabel = new QLabel(tr("Amount:"));
    amountSpin = new QDoubleSpinBox;
    amountSpin->setMaximum(1e12);
    amountSpin->setDecimals(2);

    QLabel* currencyLabel = new QLabel(tr("Currency (ISO)"));
    currencyEdit = new QLineEdit;
    currencyEdit->setMaxLength(3);

    QLabel* performedByLabel = new QLabel(tr("Performed by employee id (optional)"));
    performedByEdit = new QLineEdit;

    QFormLayout* form = new QFormLayout;
    form->addRow(tr("From account"), fromVal);
    form->addRow(toLabel, toAccountEdit);
    form->addRow(amountLabel, amountSpin);
    form->addRow(currencyLabel, currencyEdit);
    form->addRow(performedByLabel, performedByEdit);

    QPushButton* ok = new QPushButton(tr("OK"));
    QPushButton* cancel = new QPushButton(tr("Cancel"));
    connect(ok, &QPushButton::clicked, this, &TransferDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &TransferDialog::reject);

    QHBoxLayout* btns = new QHBoxLayout;
    btns->addStretch();
    btns->addWidget(ok);
    btns->addWidget(cancel);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addLayout(btns);

    if (!fromAccountNumber.isEmpty()) fromVal->setText(fromAccountNumber);
}

QString TransferDialog::toAccountNumber() const { return toAccountEdit->text().trimmed(); }

double TransferDialog::amount() const { return amountSpin->value(); }

QString TransferDialog::currency() const { return currencyEdit->text().trimmed().toUpper(); }

QString TransferDialog::performedByEmployeeId() const { return performedByEdit->text().trimmed(); }