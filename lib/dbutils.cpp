#include "dbutils.h"

DbUtils::DbUtils()
{

}

void DbUtils::log2(const QString& msg)
{
#ifdef _MSC_VER
    qDebug().nospace() << __FUNCTION__ << " => " << msg;
#endif

#ifdef __GNUC__
    qDebug().nospace() << __PRETTY_FUNCTION__ << " => " << msg;
#endif
}
