#include "paralleldbclient.h"

#ifdef _MSC_VER
   #define LOG(msg, useLog) \
    if (useLog) qDebug() << __FUNCTION__ << "=>" << msg;
#elif __GNUC__
    #define LOG(msg, useLog) \
    if (useLog) qDebug() << __PRETTY_FUNCTION__ << "=>" << msg;
#endif


ParallelDbClient::ParallelDbClient(
        const QString& connectionName,
        const DbConfig& config,
        QObject* parent)
    : ParallelDbMetainfo(parent),
    mConnectionName(connectionName),
    mUseLog(false)
{
    // mConfig has to be assigned here!
    // In list initialization it causes the following error:
    //   call to implicitly-deleted copy constructor of DbConfig
    //   copy constructor of 'DbConfig' is implicitly deleted because base class 'QObject' has a deleted copy constructor
    // It is because DbConfig, as inherited from QOBject,
    // can not have copy c-tor!
    mConfig = config;
    verifyPresenceOfRequestedDriver();
    applyConfig();
}


ParallelDbClient::~ParallelDbClient()
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    if (db.isOpen())
    {
        db.close();
    }

    QSqlDatabase::removeDatabase(mConnectionName);
}


bool ParallelDbClient::verifyPresenceOfRequestedDriver()
{
    bool succ = true;
    if (!ParallelDbMetainfo::isDriverAvailable(mConfig.getDbDriver()))
    {
        succ = false;
        LOG(
            "Requested database driver " +
            mConfig.getDbDriver() +
            " is NOT available!",
            mUseLog);
    }

    return succ;
}


void ParallelDbClient::removeDbConnection(const QString& connectioName)
{
    if (QSqlDatabase::contains(connectioName))
    {
        QSqlDatabase::removeDatabase(connectioName);
    }
}


void ParallelDbClient::setConnectionName(const QString& connectionName)
{
    removeDbConnection(mConnectionName);
    mConnectionName = connectionName;
    applyConfig();
}


void ParallelDbClient::setDbConfig(const DbConfig& conf)
{
    mConfig = conf;
}


void ParallelDbClient::setDbEngine(DbConfig::DbEngine type)
{
    removeDbConnection(mConnectionName);
    mConfig.setDbEngine(type);
}


void ParallelDbClient::setIp(const QHostAddress &ip)
{
    mConfig.setIp(ip);
}


void ParallelDbClient::setPort(quint16 port)
{
    mConfig.setPort(port);
}


void ParallelDbClient::setDbName(const QString &dbName)
{
    mConfig.setDbname(dbName);
}


void ParallelDbClient::setAddress(const QHostAddress &ip, quint16 port)
{
    mConfig.setIp(ip);
    mConfig.setPort(port);
}


void ParallelDbClient::setUsername(const QString &username)
{
    mConfig.setUsername(username);
}


void ParallelDbClient::setPassword(const QString &password)
{
    mConfig.setPassword(password);
}


void ParallelDbClient::setOdbcName(const QString &odbcName)
{
    mConfig.setOdbcName(odbcName);
}


QString ParallelDbClient::getConnectionName() const
{
    return mConnectionName;
}


DbConfig& ParallelDbClient::getDbConfig()
{
    return mConfig;
}


DbConfig::DbEngine ParallelDbClient::getDbEngine() const
{
    return mConfig.getDbEngine();
}


QHostAddress ParallelDbClient::getIp() const
{
    return mConfig.getIp();
}


quint16 ParallelDbClient::getPort() const
{
    return mConfig.getPort();
}


QString ParallelDbClient::getDbname() const
{
    return QSqlDatabase::database(mConnectionName).databaseName();
}


QString ParallelDbClient::getUsername() const
{
    return mConfig.getUsername();
}


QString ParallelDbClient::getPassword() const
{
    return mConfig.getPassword();
}


QString ParallelDbClient::getOdbcName() const
{
    return mConfig.getOdbcName();
}


void ParallelDbClient::applyConfig()
{
    addDb();
    closeDb();

    QSqlDatabase db = QSqlDatabase::database(mConnectionName, false);

    if (mConfig.getDbEngine() == DbConfig::DbEngine::SQLITE)
    {
        // TODO: What if sqlite is secured?!
        db.setDatabaseName(getDbname());
    }
    else if (mConfig.getDbEngine() == DbConfig::DbEngine::MYSQL)
    {
        db.setHostName(getIp().toString());
        db.setPort(getPort());
        db.setDatabaseName(getDbname());
        db.setUserName(getUsername());
        db.setPassword(getPassword());
    }
    else if (mConfig.getDbEngine() == DbConfig::DbEngine::MSSQL_SQLAUTH)
    {
        db.setDatabaseName(
                    QString("DRIVER={%1};SERVER=%2,%3;DATABASE=%4;UID=%5;PWD=%6")
                    .arg(mConfig.getOdbcName())
                    .arg(mConfig.getIp().toString())
                    .arg(mConfig.getPort())
                    .arg(mConfig.getDbname())
                    .arg(mConfig.getUsername())
                    .arg(mConfig.getPassword()));
    }
    else if (mConfig.getDbEngine() == DbConfig::DbEngine::MSSQL_WINAUTH)
    {
        db.setDatabaseName(
                    QString("DRIVER={%1};SERVER=%2,%3;DATABASE=%4")
                    .arg(mConfig.getOdbcName())
                    .arg(mConfig.getIp().toString())
                    .arg(mConfig.getPort())
                    .arg(mConfig.getDbname()));
    }
}

// TODO: add db per thread?! Include thread id
// https://stackoverflow.com/questions/47457478/using-qsqlquery-from-multiple-threads
void ParallelDbClient::addDb()
{
    if (!QSqlDatabase::contains(mConnectionName))
    {
        QSqlDatabase::addDatabase(
                    mConfig.getDbDriver(),
                    mConnectionName);
    }
}


void ParallelDbClient::closeDb()
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName, false);
    if (db.isOpen())
    {
        db.close();
    }
}


void ParallelDbClient::openDb()
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName, true);
    if (!db.isOpen())
    {
        bool succ = db.open();
        if (!succ)
            emit dbError(db.lastError());
    }
}


QFuture<QList<QSqlRecord>> ParallelDbClient::sendQuery(
        const QString& queryString)
{
    // Copy of an operand is always passed by QtConcurrent::run() to the function!
    QFuture<QList<QSqlRecord>> future = QtConcurrent::run(
                this,
                &ParallelDbClient::executeQuery,
                queryString);
    return future;
}


QFuture<QList<QSqlRecord>> ParallelDbClient::sendBindedQuery(
        const QString& queryString,
        const QList<QString>& placeholders,
        const QList<QByteArray>& binaries)
{
    QFuture<QList<QSqlRecord>> future = QtConcurrent::run(
                this,
                &ParallelDbClient::executeBindedQuery,
                queryString,
                placeholders,
                binaries);
    return future;
}


QFuture<bool> ParallelDbClient::sendNonQuery(
        const QString& queryString)
{
    QFuture<bool> future = QtConcurrent::run(
                this,
                &ParallelDbClient::executeNonQuery,
                queryString);
    return future;
}


QFuture<bool> ParallelDbClient::sendBindedNonQuery(
        const QString& queryString,
        const QList<QString>& placeholders,
        const QList<QByteArray>& binaries)
{
    QFuture<bool> future = QtConcurrent::run(
                this,
                &ParallelDbClient::executeBindedNonQuery,
                queryString,
                placeholders,
                binaries);
    return future;
}


QFuture<QList<QSqlRecord>> ParallelDbClient::select(const QString& queryString)
{
    // Copy of an operand is always passed by QtConcurrent::run() to the function!
    QFuture<QList<QSqlRecord>> future = QtConcurrent::run(
                this,
                &ParallelDbClient::executeQuery,
                queryString);
    return future;
}


QFuture<QList<QSqlRecord>> ParallelDbClient::select(
        const QString& queryString,
        const QList<QString>& placeholders,
        const QList<QByteArray>& binaries)
{
    QFuture<QList<QSqlRecord>> future = QtConcurrent::run(
                this,
                &ParallelDbClient::executeBindedQuery,
                queryString,
                placeholders,
                binaries);
    return future;
}


QFuture<bool> ParallelDbClient::insert(const QString &queryString)
{
    QFuture<bool> future = QtConcurrent::run(
                this,
                &ParallelDbClient::executeNonQuery,
                queryString);
    return future;
}


QFuture<bool> ParallelDbClient::update(const QString &queryString)
{
    return insert(queryString);
}


QFuture<bool> ParallelDbClient::del(const QString &queryString)
{
    return insert(queryString);
}


QFuture<bool> ParallelDbClient::insert(
        const QString& queryString,
        const QList<QString>& placeholders,
        const QList<QByteArray>& binaries)
{
    QFuture<bool> future = QtConcurrent::run(
                this,
                &ParallelDbClient::executeBindedNonQuery,
                queryString,
                placeholders,
                binaries);
    return future;
}


QFuture<bool> ParallelDbClient::update(
        const QString& queryString,
        const QList<QString>& placeholders,
        const QList<QByteArray>& binaries)
{
    return insert(queryString, placeholders, binaries);
}


QList<QSqlRecord> ParallelDbClient::executeQuery(
        const QString& queryString)
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    QList<QSqlRecord> ans;

    QMutexLocker lock(&mMutex);
    openDb();
    if (db.isOpen())
    {
        LOG("database " + db.databaseName() + " opened", mUseLog);

        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(queryString);
        // TODO: consider using query.exec(queryString)
        if (query.exec())
        {
            while (query.next())
            {
                ans.append(query.record());
            }

            LOG("query executed successfully!", mUseLog);
        }
        else
        {
            LOG("query not executed " + query.lastError().text(), mUseLog);
            emit dbError(db.lastError());
        }
    }
    else
    {
        LOG("database " + db.databaseName() + " is not opened", mUseLog);
        emit dbError(db.lastError());
    }

    return ans;
}


QList<QSqlRecord> ParallelDbClient::executeBindedQuery(
        const QString& queryString,
        const QList<QString>& placeholders,
        const QList<QByteArray>& binaries)
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    QList<QSqlRecord> ans;

    if (placeholders.size() != binaries.size() ||
        placeholders.size() == 0 ||
        binaries.size() == 0)
    {
        LOG(QString("Placeholders and/or binaries") +
            " are empty or of different size.",
            mUseLog);
        return ans;
    }

    openDb();
    if (db.isOpen())
    {
        LOG("database " + db.databaseName() + " opened", mUseLog);

        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(queryString);
        int i = 0;
        for (auto& placeholder : placeholders)
        {
            query.bindValue(
                        placeholder,
                        binaries[i++],
                        QSql::In | QSql::Binary);
        }

        if (query.exec())
        {
            while (query.next())
            {
                ans.append(query.record());
            }
            LOG("query executed successfully!", mUseLog);
        }
        else
        {
            LOG("query not executed " + query.lastError().text(), mUseLog);
            emit dbError(db.lastError());
        }
    }
    else
    {
        log("database " + db.databaseName() + " is not opened");
        emit dbError(db.lastError());
    }

    return ans;
}


bool ParallelDbClient::executeNonQuery(const QString& queryString)
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    bool succ = false;

    openDb();
    if (db.isOpen())
    {
        LOG("database " + db.databaseName() + " opened", mUseLog);

        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(queryString);
        if (query.exec())
        {
            succ = true;
            LOG("query executed successfully!", mUseLog);
        }
        else
        {
            LOG("query not executed " + query.lastError().text(), mUseLog);
            emit dbError(db.lastError());
        }
    }
    else
    {
        LOG("database " + db.databaseName() + " is not opened", mUseLog);
        emit dbError(db.lastError());
    }

    return succ;
}


bool ParallelDbClient::executeBindedNonQuery(
        const QString& queryString,
        const QList<QString>& placeholders,
        const QList<QByteArray>& binaries)
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    bool succ = false;

    if (placeholders.size() != binaries.size() ||
        placeholders.size() == 0 ||
        binaries.size() == 0)
    {
        LOG(QString("binded query not executed. Placeholders and/or binaries") +
            " are empty or of different size.", mUseLog);
        return succ;
    }

    openDb();
    if (db.isOpen())
    {
        LOG("database " + db.databaseName() + " opened", mUseLog);

        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(queryString);
        int i = 0;
        for (auto& placeholder : placeholders)
        {
            query.bindValue(
                        placeholder,
                        binaries[i++],
                        QSql::In | QSql::Binary);
        }

        if (query.exec())
        {
            succ = true;
            LOG("query executed successfully!", mUseLog);
        }
        else
        {
            LOG("query not executed " + query.lastError().text(), mUseLog);
            emit dbError(db.lastError());
        }
    }
    else
    {
        LOG("database " + db.databaseName() + " is not opened", mUseLog);
        emit dbError(db.lastError());
    }

    return succ;
}


QSqlError ParallelDbClient::lastError() const
{
    return QSqlDatabase::database(mConnectionName).lastError();
}


QStringList ParallelDbClient::tables() const
{
    return QSqlDatabase::database(mConnectionName).tables();
}


//QSqlIndex ParallelDbClient::primaryIndex(const QString& tablename) const
//{
//    // TODO: implement
//    Q_UNUSED(tablename)
//    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
//    db.primaryIndex(tablename);
//}


QSqlRecord ParallelDbClient::record(const QString& tablename) const
{
    return QSqlDatabase::database(mConnectionName).record(tablename);
}


bool ParallelDbClient::driverHasFeature() const
{
    // TODO: implement
    return 0;
}


bool ParallelDbClient::transaction() const
{
    return QSqlDatabase::database(mConnectionName).transaction();
}


bool ParallelDbClient::commit() const
{
    return QSqlDatabase::database(mConnectionName).commit();
}


bool ParallelDbClient::rollback() const
{
    return QSqlDatabase::database(mConnectionName).rollback();
}


bool ParallelDbClient::connected(SQLRETURN& reqRetCode) const
{
    // Database has to be opened here, otherwise
    // QVariant::isNull() will return true!
    QSqlDatabase db = QSqlDatabase::database(mConnectionName, true);
    if (db.isOpenError())
    {
        LOG("Error while opening DB", mUseLog);
        return false;
    }

    QVariant v = db.driver()->handle();

    if (v.isValid() && !v.isNull())
    {
        SQLHDBC hdbc = *static_cast<SQLHDBC**>(v.data());
        SQLUINTEGER uIntVal;
        SQLRETURN retcode = SQLGetConnectAttr(
                            hdbc,
                            SQL_ATTR_CONNECTION_DEAD,
                            static_cast<SQLPOINTER>(&uIntVal),
                            static_cast<SQLINTEGER>(sizeof (uIntVal)),
                            nullptr);
        reqRetCode = retcode;
        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        {
            LOG("Failed to read connection attributes", mUseLog);
            return false;
        }

        if (uIntVal == SQL_CD_TRUE)
        {
            LOG("Connection is closed/dead", mUseLog);
            return false;
        }
        else if (uIntVal == SQL_CD_FALSE)
        {
            LOG("Connection is available", mUseLog);
            return true;
        }
    }

    // equivalent to closeDb()!
    if (!db.isOpen())
        db.open();

    return false;
}


bool ParallelDbClient::isOpen() const
{
    return QSqlDatabase::database(mConnectionName).isOpen();
}


void ParallelDbClient::log(const QString& msg)
{
    if (mUseLog)
    {
    #ifdef _MSC_VER
        qDebug().nospace() << __FUNCTION__ << " => " << msg;
    #endif

    #ifdef __GNUC__
        qDebug().nospace() << __PRETTY_FUNCTION__ << " => " << msg;
    #endif
    }
}


void ParallelDbClient::useLog(bool v)
{
    QMutexLocker locker(&mMutex);
    mUseLog = v;
}
