#include "LoginDataDml.h"
#include "ModuleData/Core/ConvertUtil.h"

namespace Sql
{
    LoginDataDml::LoginDataDml(QObject* parent)
        : QObject(parent)
    {
    }

    LoginDataDml::~LoginDataDml()
    {
    }

    void LoginDataDml::SetDatabase(QSharedPointer<DBBase> database)
    {
        m_dmlCashierLogin.SetDatabase(database);
    }

    bool LoginDataDml::AddCashierLogin(Core::CashierLoginDataPtr cashier, QString& sqlError)
    {
        auto entity = Core::ConvertUtil::ToEntity(cashier);
        return m_dmlCashierLogin.Add(entity, sqlError);
    }

    bool LoginDataDml::QueryCashierLogin(const QString& cashierId, const QDateTime& startDateTime, const QDateTime& endDateTime, Core::CashierLoginDataPtrList& cashierList, QString& sqlError)
    {
        Sql::CashierLoginEntityPtrList entityList;
        auto query = Orm::QdOrmWhere();
        query.where(CashierLoginEntity::ColumnId()).isEqualTo(cashierId)
            .and_(CashierLoginEntity::ColumnLoginTime()).isBetween(startDateTime, endDateTime);

        bool result = m_dmlCashierLogin.FindByQuery(entityList, query, sqlError);
        if(!result)
        {
            return false;
        }

        cashierList = Core::ConvertUtil::FromEntityList(entityList);
        return true;
    }
    bool LoginDataDml::ClearCashierLogin(const QDateTime& dateTime, QString& sqlError)
    {
        auto query = Orm::QdOrmWhere();
        query.where(CashierLoginEntity::ColumnLoginTime()).isLessThan(dateTime);
        bool result = m_dmlCashierLogin.DeleteByQuery(query, sqlError); 
        return result;
    }
}