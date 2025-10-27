#include "AddClientDialog.h"
#include <QtWidgets>
#include <QtSql>

AddClientDialog::AddClientDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Add Client"));
    firstNameEdit = new QLineEdit;
    lastNameEdit = new QLineEdit;
    dobEdit = new QDateEdit;
    dobEdit->setCalendarPopup(true);
    dobEdit->setDisplayFormat("yyyy-MM-dd");
    dobEdit->setDate(QDate::fromString("1990-01-01", "yyyy-MM-dd"));
    emailEdit = new QLineEdit;
    phoneEdit = new QLineEdit;
    statusLabel = new QLabel;

    QPushButton* ok = new QPushButton(tr("Create"));
    QPushButton* cancel = new QPushButton(tr("Cancel"));
    connect(ok, &QPushButton::clicked, this, &AddClientDialog::onOk);
    connect(cancel, &QPushButton::clicked, this, &AddClientDialog::reject);

    QFormLayout* form = new QFormLayout;
    form->addRow(tr("First name:"), firstNameEdit);
    form->addRow(tr("Last name:"), lastNameEdit);
    form->addRow(tr("Date of birth:"), dobEdit);
    form->addRow(tr("Email:"), emailEdit);
    form->addRow(tr("Phone:"), phoneEdit);

    QHBoxLayout* btns = new QHBoxLayout;
    btns->addStretch();
    btns->addWidget(ok);
    btns->addWidget(cancel);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addWidget(statusLabel);
    main->addLayout(btns);
    setLayout(main);
    resize(480, 280);
}

void AddClientDialog::onOk()
{
    QString firstName = firstNameEdit->text().trimmed();
    QString lastName = lastNameEdit->text().trimmed();
    QString dob = dobEdit->date().toString(Qt::ISODate);
    QString email = emailEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty())
    {
        statusLabel->setText(tr("Please fill first and last name"));
        return;
    }

    QSqlQuery q(QSqlDatabase::database());

    q.prepare(R"(
        SELECT customer_id::text, account_id::text, account_number
        FROM create_client_with_account(:first, :last, :dob::date, :email, :phone)
    )");

    q.bindValue(":first", firstName);
    q.bindValue(":last", lastName);
    q.bindValue(":dob", dob);
    q.bindValue(":email", email);
    q.bindValue(":phone", phone);

    if (!q.exec()) 
    {
        statusLabel->setText(tr("Failed to create client/account: %1").arg(q.lastError().text()));
        qWarning() << "create_client_with_account failed:" << q.lastError().text();
        return;
    }

    if (q.next()) 
    {
        QString newCustomerId = q.value(0).toString();
        QString newAccountId = q.value(1).toString();
        QString newAccountNum = q.value(2).toString();

        emit clientCreated(newCustomerId, newAccountId, newAccountNum);

        QMessageBox::information(this, tr("Created"),
            tr("Client created (id=%1)\nAccount created %2 (id=%3)")
            .arg(newCustomerId, newAccountNum, newAccountId));

        accept();
        return;
    }

    statusLabel->setText(tr("Unknown error while creating client/account"));
}