#include "LoginDialog.h"
#include "AuthManager.h"
#include <QtWidgets>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Login"));
    usernameEdit = new QLineEdit;
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    statusLabel = new QLabel;

    QPushButton* loginBtn = new QPushButton(tr("Login"));
    QPushButton* cancelBtn = new QPushButton(tr("Cancel"));

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &LoginDialog::reject);

    QFormLayout* form = new QFormLayout;
    form->addRow(tr("Username:"), usernameEdit);
    form->addRow(tr("Password:"), passwordEdit);

    QHBoxLayout* btns = new QHBoxLayout;
    btns->addStretch();
    btns->addWidget(loginBtn);
    btns->addWidget(cancelBtn);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addWidget(statusLabel);
    main->addLayout(btns);

    setLayout(main);
    resize(360, 150);
}

void LoginDialog::onLoginClicked()
{
    const QString username = usernameEdit->text().trimmed();
    const QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) 
    {
        statusLabel->setText(tr("Enter username and password"));
        return;
    }

    bool ok = AuthManager::authenticate(username, password);

    if (!ok) 
    {
        statusLabel->setText(tr("Invalid credentials"));
        return;
    }

    accept();
}