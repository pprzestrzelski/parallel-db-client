#ifndef PARALLELDBMETAINFO_H
#define PARALLELDBMETAINFO_H

#include <QObject>
#include <QSqlDriverCreator>

#if defined(_WIN32) && defined(_MSC_VER)
    #include <Windows.h>
#endif

#include <sqltypes.h>

class ParallelDbMetainfo : public QObject
{
    Q_OBJECT

public:
    ParallelDbMetainfo(QObject* parent = nullptr);

    static QStringList availableConnections();
    static QStringList availableDbDrivers();
    static bool isDriverAvailable(const QString& name);
    static void registerSqlDriver(
            const QString& name,
            QSqlDriverCreatorBase* creator);

    virtual QSqlError lastError() const = 0;
    virtual QStringList tables() const = 0;
//    virtual QSqlIndex primaryIndex(const QString& tablename) const = 0;
    virtual QSqlRecord record(const QString& tablename) const = 0;
    virtual bool driverHasFeature() const = 0;
    virtual bool transaction() const = 0;
    virtual bool commit() const = 0;
    virtual bool rollback() const = 0;
    virtual bool connected(
            SQLRETURN& reqRetCode,
            QMap<QString, QString>& diagData) const = 0;
    virtual bool isOpen() const = 0;
};

#endif // PARALLELDBMETAINFO_H
