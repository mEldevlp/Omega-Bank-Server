#include "AuthManager.h"
#include <QtSql>
#include <QCryptographicHash>

QString AuthManager::s_currentUser;
QString AuthManager::s_currentRole;

QByteArray AuthManager::hashPassword(const QByteArray& salt, const QString& password)
{
    QByteArray data = salt + password.toUtf8();
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256);
}

bool AuthManager::authenticate(const QString& username, const QString& password)
{
    if (!QSqlDatabase::isDriverAvailable("QPSQL") && !QSqlDatabase::isDriverAvailable("QSQLITE")) 
    {
        qWarning() << "No SQL driver available";
        return false;
    }

    QSqlQuery q(QSqlDatabase::database());
    q.prepare("SELECT password_hash, salt, role FROM employee_accounts WHERE username = :u LIMIT 1");
    q.bindValue(":u", username);

    if (!q.exec()) 
    {
        qWarning() << "Auth query failed:" << q.lastError().text();
        return false;
    }

    if (!q.next()) return false;

    QByteArray stored_hash = q.value(0).toByteArray();
    QByteArray stored_salt = q.value(1).toByteArray();
    QString role = q.value(2).toString();

    QByteArray computed = hashPassword(stored_salt, password);

    if (computed == stored_hash) 
    {
        s_currentUser = username;
        s_currentRole = role;
        QSqlQuery u(QSqlDatabase::database());
        u.prepare("UPDATE employee_accounts SET last_login = now() WHERE username = :u");
        u.bindValue(":u", username);
        u.exec();
        return true;
    }

    return false;
}

QString AuthManager::currentUser() { return s_currentUser; }
QString AuthManager::currentRole() { return s_currentRole; }
void AuthManager::logout() { s_currentUser.clear(); s_currentRole.clear(); }