#ifndef DBCONFIG_H
#define DBCONFIG_H

#if defined(LIB_LIBRARY)
# define LIBSHARED_EXPORT Q_DECL_EXPORT
#else
# define LIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QHostAddress>
#include <QHostInfo>
#include <QMap>
#include <QObject>
#include "constants.h"

class LIBSHARED_EXPORT DbConfig: public QObject
{
    Q_OBJECT

public:
    enum DbEngine { SQLITE, MYSQL, MSSQL_SQLAUTH, MSSQL_WINAUTH, MSSQL_LOCAL };

    DbConfig();
    DbConfig(
            DbEngine type,
            const QString& address,
            quint16 port,
            const QString& dbname,
            const QString& username,
            const QString& password,
            const QString& odbcName = "SQL Server",
            QObject* parent = nullptr);
    DbConfig& operator=(const DbConfig& conf);
    ~DbConfig();

    void setDbEngine(DbEngine type) { this->mEngine = type; }
    void setIp(const QHostAddress& ip) { this->mIp = ip; }
    void setIp(const QString& domain);
    void setPort(quint16 port) { this->mPort = port; }
    void setDbname(const QString &dbName) { this->mDbname = dbName; }
    void setUsername(const QString& username) { this->mUsername = username; }
    void setPassword(const QString& password) { this->mPassword = password; }
    void setOdbcName(const QString& odbcName) { this->mOdbcName = odbcName; }

    DbEngine getDbEngine() const { return mEngine; }
    QHostAddress getIp() const { return mIp; }
    quint16 getPort() const { return mPort; }
    QString getDbname() const { return mDbname; }
    QString getUsername() const { return mUsername; }
    QString getPassword() const { return mPassword; }
    QString getOdbcName() const { return mOdbcName; }
    QString getDbDriver();

    bool verifyDomain(const QString& name);

private:
    void initDbDrivers();

    DbEngine mEngine;
    QHostAddress mIp;
    quint16 mPort;
    QString mDbname;
    QString mUsername;
    QString mPassword;
    QString mOdbcName;
    QMap<DbEngine, QString> mDrivers;

signals:
    void hostError(QString msg, QString errorString);
};

#endif // DBCONFIG_H
