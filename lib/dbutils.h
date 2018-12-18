#ifndef DBUTILS_H
#define DBUTILS_H

#include <QObject>
#include <QDebug>

class DbUtils
{
public:
    static void log2(const QString& msg);

private:
    DbUtils();
};

#endif // DBUTILS_H
