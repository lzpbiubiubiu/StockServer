#pragma once

#include <QObject>
#include "ModuleBase/Database/Base/DmlBase.h"
#include "ModuleData/ModuleDataApi.h"
#include "ModuleData/Core/LoginData.h"
#include "ModuleData/Entity/CashierLoginEntity.h"

namespace Sql
{
class MODULE_DATA_API LoginDataDml : public QObject
{
    Q_OBJECT

public:
    LoginDataDml(QObject *parent = Q_NULLPTR);
    ~LoginDataDml();

    void SetDatabase(QSharedPointer<DBBase> database);

    /** 添加记录 */
    bool AddCashierLogin(Core::CashierLoginDataPtr cashier, QString& sqlError);

    /** 查询登录记录 */
    bool QueryCashierLogin(const QString& cashierId, const QDateTime& startDateTime, const QDateTime& endDateTime, Core::CashierLoginDataPtrList& cashierList, QString& sqlError);

    /** 清理多余的登录数据*/ 
    bool ClearCashierLogin(const QDateTime& dateTime, QString& sqlError);

private:
    Sql::DmlBase<Sql::CashierLoginEntity> m_dmlCashierLogin;
};

} // namespace Sql