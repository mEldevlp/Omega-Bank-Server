#pragma once

#include <QDialog>

class QLineEdit;
class QDoubleSpinBox;
class QComboBox;

class TransferDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TransferDialog(QWidget* parent = nullptr);

    void setFromAccountId(const QString& id) { fromAccountId = id; }
    void setFromAccountNumber(const QString& num) { fromAccountNumber = num; }

    QString toAccountNumber() const;
    double amount() const;
    QString currency() const;
    QString performedByEmployeeId() const;

private:
    QLineEdit* toAccountEdit = nullptr;
    QDoubleSpinBox* amountSpin = nullptr;
    QLineEdit* currencyEdit = nullptr;
    QLineEdit* performedByEdit = nullptr;

    QString fromAccountId;
    QString fromAccountNumber;
};