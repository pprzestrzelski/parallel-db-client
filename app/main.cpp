#include <QStringList>
#include <QString>
#include <QSqlQuery>
#include <QFutureWatcher>
#include <QCoreApplication>
#include <QSqlError>
#include <QObject>

#include "paralleldbfactory.h"
#include "paralleldbclient.h"
#include "dbconfig.h"

#include <iostream>

void handleDbError(QSqlError err)
{
    qDebug() << "HANDLE DB ERROR:" << err.text();
}

void handleHostError(QString msg, QString err)
{
    Q_UNUSED(err)
    qDebug() << "HOST ERROR:" << msg;
}

int main(int argc, char * argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "Available drivers:"
             << ParallelDbMetainfo::availableDbDrivers();

    QString domain = "ebienias.pl";  //"81.18.214.46",
    DbConfig conf(
                DbConfig::DbEngine::MSSQL_SQLAUTH,
                domain,
                1433,
                "agent",
                "sa",
                "Komputer13");
    QObject::connect(&conf, &DbConfig::hostError, handleHostError);
    conf.verifyDomain(domain);

    ParallelDbFactory& dbf = ParallelDbFactory::getInstance();
    dbFactory.createDbClient( "mssql", "main-db", conf);

    //ParallelDbClient* db = dbf.getDbClient("mssql");
    ParallelDbClient* db = DB_CLIENT("mssql");
    QObject::connect(db, &ParallelDbClient::dbError, handleDbError);

    if (db->isOpen())
    {
        // === Make select queries
        QString q1("SELECT * FROM T_USER;");
        QFuture<QList<QSqlRecord>> ans1 = db->select(q1);

        ans1.waitForFinished();

        qDebug() << "\n======================================";
        qDebug().noquote() << q1 << "result:";
        for (int i = 0; i < ans1.result().length(); ++i)
        {
            QSqlRecord rec = ans1.result().at(i);
            qDebug() << rec.value(0).toString() << rec.value(1).toString();
        }
        qDebug() << "======================================";
    }
    else
    {
        qDebug() << "DB is not opened :(";
        qDebug() << db->lastError().text() << "\n";
    }

    // ==== TEST BLOB FILE INSERTION ===============================
    DbConfig sqliteConf;
    sqliteConf.setDbEngine(DbConfig::DbEngine::SQLITE);
    sqliteConf.setDbname("/home/pawel/test.db");
    dbf.createDbClient("sqlite", "aux-db", sqliteConf);

    ParallelDbClient* sqliteDb = dbf.getDbClient("sqlite");

    // Drop table
    QFuture <bool> ans1 = sqliteDb->insert("DROP TABLE data;");
    ans1.waitForFinished();
    if (ans1.result() == false)
    {
        qDebug() << "Failed to dop table";
    }

    // Create table
    QString qCreateTable("CREATE TABLE data(file BLOB);");
    QFuture<bool> ans2 = sqliteDb->insert(qCreateTable);
    ans2.waitForFinished();
    if (ans2.result() == true)
    {
        qDebug() << "Table created!";
    }
    else
    {
        qDebug() << "Failed to create table in db";
    }

    QByteArray data = QByteArray{1024, 0xF};
    QList<QString> placeholders;
    QList<QByteArray> binaries;
    QString qInsertData("INSERT INTO data VALUES(:data);");
    placeholders.append(QString(":data"));
    binaries.append(data);
    QFuture<bool> ans3 = sqliteDb->insert(
                qInsertData, placeholders, binaries);
    ans3.waitForFinished();
    if (ans3.result() == true)
    {
        qDebug() << "Data added to a given table.";
    }
    else
    {
        qDebug() << "Failed to add data to a given table";
    }

    QFuture<QList<QSqlRecord>> ans4 = sqliteDb->select(
                QString("SELECT (file) FROM data;"));
    ans4.waitForFinished();
    if (ans4.result().size() > 0)
    {
        qDebug() << "DB read some data...";
        QVariant value = ans4.result().at(0).value(0);
        qDebug() << value.toByteArray().size();
        qDebug() << value;
    }
    else
    {
        qDebug() << "DB was unable to read data";
    }

    if (sqliteDb->commit())
    {
        qDebug() << "DB commited with success!";
    }
    else
    {
        qDebug() << "Failed to commit :(";
        qDebug() << sqliteDb->lastError();
    }

    qDebug() << "You can press Ctrl + C to close...";
    return app.exec();
}
