#include "AddCardDialog.h"
#include <QtWidgets>
#include <QtSql>
#include "AuthManager.h" // для получения текущего сотрудника (created_by)

AddCardDialog::AddCardDialog(const QList<QPair<QString, QString>>& accounts, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Add Card"));

    accountCombo = new QComboBox;
    for (const auto& p : accounts) accountCombo->addItem(p.second, p.first);

    typeCombo = new QComboBox;
    typeCombo->addItems({ "debit", "credit", "prepaid" });

    panEdit = new QLineEdit;
    panEdit->setEchoMode(QLineEdit::Password);
    panEdit->setPlaceholderText(tr("Card number (digits only)"));

    expiryEdit = new QDateEdit;
    expiryEdit->setDisplayFormat("yyyy-MM-dd");
    expiryEdit->setCalendarPopup(true);
    expiryEdit->setDate(QDate::currentDate().addYears(3));

    statusCombo = new QComboBox;
    statusCombo->addItems({ "active", "blocked" });

    statusLabel = new QLabel;

    QPushButton* createBtn = new QPushButton(tr("Create"));
    QPushButton* cancelBtn = new QPushButton(tr("Cancel"));
    connect(createBtn, &QPushButton::clicked, this, &AddCardDialog::onCreate);
    connect(cancelBtn, &QPushButton::clicked, this, &AddCardDialog::reject);

    QFormLayout* form = new QFormLayout;
    form->addRow(tr("Account:"), accountCombo);
    form->addRow(tr("Type:"), typeCombo);
    form->addRow(tr("PAN:"), panEdit);
    form->addRow(tr("Expiry (YYYY-MM-DD):"), expiryEdit);
    form->addRow(tr("Status:"), statusCombo);

    QHBoxLayout* btns = new QHBoxLayout;
    btns->addStretch();
    btns->addWidget(createBtn);
    btns->addWidget(cancelBtn);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addWidget(statusLabel);
    main->addLayout(btns);
    setLayout(main);
    resize(480, 240);
}

bool AddCardDialog::luhn_check(const QString& s)
{
    QString digits;

    for (QChar c : s) if (c.isDigit()) digits.append(c);

    int sum = 0;
    bool alt = false;

    for (int i = digits.size() - 1; i >= 0; --i) 
    {
        int d = digits[i].digitValue();

        if (alt) 
        {
            d *= 2;
            if (d > 9) d -= 9;
        }

        sum += d;
        alt = !alt;
    }

    return (sum % 10) == 0;
}

QString AddCardDialog::sanitizePan(const QString& pan) const
{
    QString s;
    for (QChar c : pan) if (c.isDigit()) s.append(c);
    return s;
}

bool AddCardDialog::validatePan(const QString& pan) const
{
    QString s = sanitizePan(pan);
    if (s.size() < 12 || s.size() > 19) return false;
    return luhn_check(s);
}

void AddCardDialog::onCreate()
{
    if (accountCombo->count() == 0) 
    {
        statusLabel->setText(tr("No account available for this customer."));
        return;
    }

    QString accountId = accountCombo->currentData().toString();
    QString type = typeCombo->currentText();
    QString pan = panEdit->text().trimmed();
    if (!validatePan(pan)) 
    {
        statusLabel->setText(tr("Invalid card number (Luhn check failed or length)."));
        return;
    }

    QDate expiry = expiryEdit->date();
    if (!expiry.isValid() || expiry < QDate::currentDate()) 
    {
        statusLabel->setText(tr("Invalid expiry date."));
        return;
    }

    QString status = statusCombo->currentText();

    QString createdById;
    QString username = AuthManager::currentUser();

    if (!username.isEmpty()) 
    {
        QSqlQuery qq(QSqlDatabase::database());
        qq.prepare("SELECT id FROM employee_accounts WHERE username = :u LIMIT 1");
        qq.bindValue(":u", username);
        if (qq.exec() && qq.next()) createdById = qq.value(0).toString();
    }

    QSqlQuery q(QSqlDatabase::database());
    q.prepare("SELECT create_card(:acc::uuid, :type, :pan, :expiry::date, :status, :created_by::uuid)");
    q.bindValue(":acc", accountId);
    q.bindValue(":type", type);
    q.bindValue(":pan", sanitizePan(pan));
    q.bindValue(":expiry", expiry.toString(Qt::ISODate));
    q.bindValue(":status", status);
    if (!createdById.isEmpty()) q.bindValue(":created_by", createdById); else q.bindValue(":created_by", QVariant());

    if (!q.exec()) {
        statusLabel->setText(tr("DB error: %1").arg(q.lastError().text()));
        qWarning() << "create_card failed:" << q.lastError().text();
        return;
    }

    if (q.next()) 
    {
        QString newId = q.value(0).toString();
        QMessageBox::information(this, tr("Created"), tr("Card created (id=%1)").arg(newId));
        accept();
        return;
    }

    statusLabel->setText(tr("Unknown error"));
}