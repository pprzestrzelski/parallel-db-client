#include "paralleldbfactory.h"

ParallelDbFactory& ParallelDbFactory::getInstance()
{
    static ParallelDbFactory instance;
    return instance;
}


ParallelDbFactory::ParallelDbFactory()
{
    //
}


ParallelDbFactory::~ParallelDbFactory()
{

}


ParallelDbClient* ParallelDbFactory::getDbClient(
        const QString& dbClientName)
{
    ParallelDbClient* db = nullptr;
    if (mDbClients.contains(dbClientName))
    {
        db = mDbClients.value(dbClientName);
    }
    else
    {
        emit requestedDberror(
                    "Database " +
                    dbClientName +
                    " does not exist.");
    }

    return db;
}

void ParallelDbFactory::createDbClient(
        const QString& dbClientName,
        const QString& connectionName,
        const DbConfig& config)
{
    if (mDbClients.contains(dbClientName))
    {
        mDbClients.remove(dbClientName);
        emit warning(
                    "Database client " +
                    dbClientName +
                    " exists. It will be removed" +
                    " and replaced with a new one.");
    }

    mDbClients.insert(
                dbClientName,
                new ParallelDbClient(connectionName, config, this));
}


bool ParallelDbFactory::removeDbClient(const QString& dbClientName)
{
    bool succ = false;

    if (mDbClients.contains(dbClientName))
    {
        mDbClients.remove(dbClientName);
        succ = true;
    }

    return succ;
}
