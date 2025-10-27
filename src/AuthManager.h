#pragma once

#include <QString>
#include <QByteArray>

class AuthManager 
{
public:
    static bool authenticate(const QString& username, const QString& password);
    static QString currentUser();
    static QString currentRole();
    static void logout();

private:
    static QByteArray hashPassword(const QByteArray& salt, const QString& password);
    static QString s_currentUser;
    static QString s_currentRole;
};