#include "paralleldbmetainfo.h"

ParallelDbMetainfo::ParallelDbMetainfo(QObject* parent)
    : QObject(parent)
{
    // never used
}


QStringList ParallelDbMetainfo::availableConnections()
{
    return QSqlDatabase::connectionNames();
}


QStringList ParallelDbMetainfo::availableDbDrivers()
{
    return QSqlDatabase::drivers();
}


bool ParallelDbMetainfo::isDriverAvailable(const QString& name)
{
    return QSqlDatabase::isDriverAvailable(name);
}


void ParallelDbMetainfo::registerSqlDriver(
        const QString& name,
        QSqlDriverCreatorBase* creator)
{
    QSqlDatabase::registerSqlDriver(name, creator);
}
