#include "DBManager.h"
#include <QtSql>

bool DBManager::openConnection(const QString& host, int port,
    const QString& dbname, const QString& user,
    const QString& password, QString& errorMsg)
{
    if (QSqlDatabase::contains()) 
    {
        QSqlDatabase db = QSqlDatabase::database();
        if (db.isOpen()) return true;
        db.close();
    }

    if (!QSqlDatabase::isDriverAvailable("QPSQL")) 
    {
        errorMsg = QStringLiteral("QPSQL driver is not available. Available drivers: %1")
            .arg(QSqlDatabase::drivers().join(", "));
        qWarning() << errorMsg;
        return false;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(dbname);
    db.setUserName(user);
    db.setPassword(password);

    if (!db.open()) 
    {
        errorMsg = QStringLiteral("Failed to open DB: %1").arg(db.lastError().text());
        qWarning() << errorMsg;
        return false;
    }

    qInfo() << "DB connected to" << host << ":" << port << "/" << dbname;
    return true;
}

void DBManager::closeConnection()
{
    if (QSqlDatabase::contains()) 
    {
        QSqlDatabase db = QSqlDatabase::database();
        if (db.isOpen()) db.close();
    }
}