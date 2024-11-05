#pragma once
#include <QObject>
#include <QDateTime>

namespace Core
{
    struct CashierLoginData
    {
        /** UUID */
        QString uuid;

        /** ID */
        QString id;

        /** 员工编号 */
        QString empno;

        /** 账号 */
        QString account;

        /** 姓名 */
        QString name;

        /** 登录机台 */
        QString deskNo;

        /** 登录时间 */
        QDateTime loginTime;

        /** 登出时间 */
        QDateTime logoutTime;
    };

    using CashierLoginDataPtr = QSharedPointer<CashierLoginData>;
    using CashierLoginDataPtrList = QList<CashierLoginDataPtr>;

}

Q_DECLARE_METATYPE(Core::CashierLoginData)
Q_DECLARE_METATYPE(Core::CashierLoginDataPtr)
Q_DECLARE_METATYPE(Core::CashierLoginDataPtrList)