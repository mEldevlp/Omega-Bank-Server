#pragma once

#include <QDialog>

class QLineEdit;
class QLabel;

class LoginDialog : public QDialog 
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr);

private slots:
    void onLoginClicked();

private:
    QLineEdit* usernameEdit = nullptr;
    QLineEdit* passwordEdit = nullptr;
    QLabel* statusLabel = nullptr;
};