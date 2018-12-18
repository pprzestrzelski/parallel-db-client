#ifndef PARALLELDBFACTORY_H
#define PARALLELDBFACTORY_H

#define dbFactory ParallelDbFactory::getInstance()
#define DB_CLIENT(name) ParallelDbFactory::getInstance().getDbClient(name)

#include <QObject>
#include <QMap>
#include "paralleldbclient.h"
#include "dbconfig.h"

class LIBSHARED_EXPORT ParallelDbFactory : public QObject
{
    Q_OBJECT
public:
    static ParallelDbFactory& getInstance();
    ~ParallelDbFactory();
    ParallelDbFactory(const ParallelDbFactory&) = delete;
    void operator=(const ParallelDbFactory&) = delete;

    ParallelDbClient* getDbClient(const QString& dbClientName);
    void createDbClient(
            const QString& dbClientName,
            const QString& connectionName,
            const DbConfig& config);
    bool removeDbClient(const QString& dbClientName);

private:
    explicit ParallelDbFactory(/*QObject* parent = nullptr*/);
    QMap<QString, ParallelDbClient*> mDbClients;

signals:
    void requestedDberror(QString msg);
    void warning(QString msg);

public slots:
};

#endif // PARALLELDBFACTORY_H
