// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include "ModuleData/ModuleDataApi.h"
#include "ModuleBase/Database/Base/EntityBase.h"

#define USER_TABLE_VERSION_V1 (1)

namespace Sql
{
    /** [tb_user] 用户信息表  */
    class MODULE_DATA_API UserLoginEntity : public Sql::EntityBase
    {
        Q_OBJECT

        ORM_REGISTER_FRIEND_CLASS(CashierEntity)

        ORM_REGISTER_TABLE("tb_user")

        // 账号ID
        ORM_REGISTER_COLUMN(QString, Account, "account")

        // 密码
        ORM_REGISTER_COLUMN(QString, Password, "password")

        // 姓名
        ORM_REGISTER_COLUMN(QString, Name, "name")

        // 电话
        ORM_REGISTER_COLUMN(QString, Tel, "tel")

        // 用户状态
        ORM_REGISTER_COLUMN(int, Status, "status")

    };

    using UserLoginEntityPtr = QSharedPointer<UserLoginEntity>;
    using UserLoginEntityPtrList = QList<QSharedPointer<UserLoginEntity>>;
}

using namespace Sql;
ORM_REGISTER_HPP_STOCK_DATABASE(UserLoginEntity)