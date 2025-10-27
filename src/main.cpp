#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include "MainWindow.h"
#include "LoginDialog.h"
#include "DBManager.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    bool connected = false;
    QString error;
    QString host = "localhost";
    int port = 5432;
    QString dbname = "Bank";
    QString user = "postgres";
    QString password = "root";

    for (int attempt = 0; attempt < 3 && !connected; ++attempt) 
    {
        bool ok;
        host = QInputDialog::getText(nullptr, QObject::tr("DB Host"), QObject::tr("Host:"), QLineEdit::Normal, host, &ok);
        if (!ok) return 0;
        port = QInputDialog::getInt(nullptr, QObject::tr("DB Port"), QObject::tr("Port:"), port, 1, 65535, 1, &ok);
        if (!ok) return 0;
        dbname = QInputDialog::getText(nullptr, QObject::tr("DB Name"), QObject::tr("Database:"), QLineEdit::Normal, dbname, &ok);
        if (!ok) return 0;
        user = QInputDialog::getText(nullptr, QObject::tr("DB User"), QObject::tr("User:"), QLineEdit::Normal, user, &ok);
        if (!ok) return 0;
        password = QInputDialog::getText(nullptr, QObject::tr("DB Password"), QObject::tr("Password:"), QLineEdit::Password, QString(), &ok);
        if (!ok) return 0;

        connected = DBManager::openConnection(host, port, dbname, user, password, error);
        if (!connected) 
        {
            QMessageBox::warning(nullptr, QObject::tr("DB connect failed"), error + "\nTry again or cancel to exit.");
        }
    }

    if (!connected) return 0;

    LoginDialog login;
    if (login.exec() != QDialog::Accepted) 
    {
        DBManager::closeConnection();
        return 0;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
