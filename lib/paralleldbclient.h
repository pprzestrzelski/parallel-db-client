#ifndef PARALLELDBCLIENT_H
#define PARALLELDBCLIENT_H

#if defined(LIB_LIBRARY)
# define LIBSHARED_EXPORT Q_DECL_EXPORT
#else
# define LIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <QtCore/QtGlobal>
#include <QDebug>
#include <QHostAddress>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QList>
#include "paralleldbmetainfo.h"
#include "dbconfig.h"
#include "constants.h"
#include <ctime>

#include <QMutex>
#include <QMutexLocker>

class ParallelDbFactory;

class LIBSHARED_EXPORT ParallelDbClient : public ParallelDbMetainfo
{
    Q_OBJECT
    friend ParallelDbFactory;

public:
    ~ParallelDbClient();
    ParallelDbClient(const ParallelDbClient&) = delete;
    void operator=(const ParallelDbClient&) = delete;

    void setConnectionName(const QString& connectionName);
    void setDbConfig(const DbConfig& conf);
    void setDbEngine(DbConfig::DbEngine type);
    void setIp(const QHostAddress& ip);
    void setPort(quint16 port);
    void setDbName(const QString& dbname);
    void setAddress(const QHostAddress& ip, quint16 port);
    void setUsername(const QString& username);
    void setPassword(const QString& password);
    void setOdbcName(const QString& odbcName);
    QString getConnectionName() const;
    DbConfig& getDbConfig();
    DbConfig::DbEngine getDbEngine() const;
    QHostAddress getIp() const;
    quint16 getPort() const;
    QString getDbname() const;
    QString getUsername() const;
    QString getPassword() const;
    QString getOdbcName() const;
    void applyConfig();

    QFuture<QList<QSqlRecord>> sendQuery(const QString& queryString);
    QFuture<QList<QSqlRecord>> sendBindedQuery(
            const QString& queryString,
            const QList<QString>& placeholders,
            const QList<QByteArray>& binaries);
    QFuture<bool> sendNonQuery(const QString& queryString);
    QFuture<bool> sendBindedNonQuery(
            const QString& queryString,
            const QList<QString>& placeholders,
            const QList<QByteArray>& binaries);

    QFuture<QList<QSqlRecord>> select(const QString& queryString);
    QFuture<QList<QSqlRecord>> select(
            const QString& queryString,
            const QList<QString>& placeholders,
            const QList<QByteArray>& binaries);
    QFuture<bool> insert(const QString& queryString);
    QFuture<bool> insert(
            const QString& queryString,
            const QList<QString>& placeholders,
            const QList<QByteArray>& binaries);
    QFuture<bool> update(const QString& queryString);
    QFuture<bool> update(
            const QString& queryString,
            const QList<QString>& placeholders,
            const QList<QByteArray>& binaries);
    QFuture<bool> del(const QString& queryString);
    QSqlError lastError() const;
    QStringList tables() const;
//    QSqlIndex primaryIndex(const QString& tablename) const;
    QSqlRecord record(const QString& tablename) const;
    bool driverHasFeature() const;
    bool transaction() const;
    bool commit() const;
    bool rollback() const;
    bool connected() const;
    bool isOpen() const;

private:
    explicit ParallelDbClient(
            const QString& connectionName,
            const DbConfig& config,
            QObject* parent = nullptr);
    bool verifyPresenceOfRequestedDriver();
    void addDb();
    void removeDbConnection(const QString& connectionName);
    void closeDb();
    void openDb();
    QList<QSqlRecord> executeQuery(const QString& queryString);
    QList<QSqlRecord> executeBindedQuery(
            const QString& queryString,
            const QList<QString>& placeholders,
            const QList<QByteArray>& binaries);
    bool executeNonQuery(const QString& queryString);
    bool executeBindedNonQuery(
            const QString& queryString,
            const QList<QString>& placeholders,
            const QList<QByteArray>& binaries);
    void log(const QString& msg);

    QString mConnectionName;
    DbConfig mConfig;

    QMutex mMutex;

signals:
    void dbError(QSqlError error);
};

#endif // PARALLELDBCLIENT_H
