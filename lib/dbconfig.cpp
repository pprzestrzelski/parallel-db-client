#include "dbconfig.h"

DbConfig::DbConfig()
{
    setDbEngine(DbEngine::SQLITE);
    setIp(QHostAddress(DbConstants::LOCALHOST));
    setPort(DbConstants::DEFAULT_PORT);
    setDbname("none");
    setUsername("anonymous");
    setPassword("anonymous");
    initDbDrivers();
}


DbConfig::DbConfig(
        DbEngine type,
        const QString& address,
        quint16 port,
        const QString& dbname,
        const QString& username,
        const QString& password,
        const QString& odbcName,
        QObject* parent)
    : QObject(parent),
    mEngine(type),
    mIp(DbConstants::LOCALHOST),
    mPort(port),
    mDbname(dbname),
    mUsername(username),
    mPassword(password),
    mOdbcName(odbcName)
{
    setIp(address);
    initDbDrivers();
}


DbConfig& DbConfig::operator=(const DbConfig& conf)
{
    mEngine = conf.getDbEngine();
    mIp = conf.getIp();
    mPort = conf.getPort();
    mDbname = conf.getDbname();
    mUsername = conf.getUsername();
    mPassword = conf.getPassword();
    mOdbcName = conf.getOdbcName();

    return *this;
}


DbConfig::~DbConfig()
{

}


void DbConfig::initDbDrivers()
{
    mDrivers.insert(DbEngine::SQLITE, "QSQLITE");
    mDrivers.insert(DbEngine::MYSQL, "QMYSQL");
    mDrivers.insert(DbEngine::MSSQL_SQLAUTH, "QODBC3");
    mDrivers.insert(DbEngine::MSSQL_WINAUTH, "QODBC3");
}


QString DbConfig::getDbDriver()
{
    return mDrivers.value(mEngine);
}


void DbConfig::setIp(const QString& domain)
{
    QHostInfo hi = QHostInfo::fromName(domain);
    if (verifyDomain(domain))
    {
        mIp = hi.addresses().first();
        if (isDomainAddress(domain))
        {
            mDomain = domain;
        }
        else
        {
            mDomain = QString();
        }
    }
    else
    {
        setIp(QHostAddress(DbConstants::LOCALHOST));
    }
}


void DbConfig::setIp(const QHostAddress& ip)
{
    mIp = ip;
    mDomain = QString();
}


bool DbConfig::isDomainAddress(const QString& value)
{
    QList<QString> list = value.split(".");
    if (list.length() != 4)
        return true;

    QRegExp re("\\d*");
    QString sequence = list.join("");
    if (!re.exactMatch(sequence))
        return true;

    return false;
}


bool DbConfig::verifyDomain(const QString& name)
{
    bool succ = true;

    QHostInfo hi = QHostInfo::fromName(name);
    if (hi.addresses().isEmpty())
    {
        switch (hi.error())
        {
        case QHostInfo::NoError:
            qDebug() << "No error, no address, is it possible... WTF?!";
            break;
        case QHostInfo::HostNotFound:
        case QHostInfo::UnknownError:
            emit hostError("WARNING: " + name +
                           "domain name or IP is not valid.",
                           hi.errorString());
            break;
        //default:
        //    break;
        }

        succ = false;
    }

    return succ;
}
