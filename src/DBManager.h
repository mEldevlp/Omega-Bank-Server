#pragma once
#include <QString>

class DBManager 
{
public:
    static bool openConnection(const QString& host, int port,
        const QString& dbname, const QString& user,
        const QString& password, QString& errorMsg);

    static void closeConnection();
};