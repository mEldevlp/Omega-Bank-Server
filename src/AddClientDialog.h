#pragma once
#include <QDialog>
#include <QList>
#include <QPair>
#include <QString>

class QComboBox;
class QLineEdit;
class QDateEdit;
class QLabel;

class AddCardDialog;

class AddClientDialog : public QDialog 
{
    Q_OBJECT
public:
    explicit AddClientDialog(QWidget* parent = nullptr);

signals:
    void clientCreated(const QString& customerId, const QString& accountId, const QString& accountNumber);

private slots:
    void onOk();

private:
    QLineEdit* firstNameEdit = nullptr;
    QLineEdit* lastNameEdit = nullptr;
    QDateEdit* dobEdit = nullptr;
    QLineEdit* emailEdit = nullptr;
    QLineEdit* phoneEdit = nullptr;
    QLabel* statusLabel = nullptr;
};
