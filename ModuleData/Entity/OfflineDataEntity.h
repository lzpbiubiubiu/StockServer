// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include "ModuleData/ModuleDataApi.h"
#include "ModuleBase/Database/Base/EntityBase.h"

#define OFFLINE_DATA_TABLE_VERSION_V1 (1)

namespace Sql
{
    /** [tb_offline_data] 离线任务表  */
    class MODULE_DATA_API OfflineDataEntity : public Sql::EntityBase
    {
        Q_OBJECT

        ORM_REGISTER_FRIEND_CLASS(OfflineDataEntity)

        ORM_REGISTER_TABLE("tb_offline_data")

        // 任务唯一标识
        ORM_REGISTER_COLUMN(QString, Uuid, "uuid")

        // 设备类型:0-默认值/1-POS/2-PVT
        ORM_REGISTER_COLUMN(int, DeviceType, "device_type")

        // 设备ID
        ORM_REGISTER_COLUMN(QString, DeviceId, "device_id")

        // POS/PVT 银台号
        ORM_REGISTER_COLUMN(QString, TerminalNo, "terminal_no")

        // POS/PVT MAC地址
        ORM_REGISTER_COLUMN(QString, DeviceCode, "device_code")

        // 商家id
        ORM_REGISTER_COLUMN(QString, TenantId, "tenant_id")

        // 门店id
        ORM_REGISTER_COLUMN(QString, OrgId, "org_id")

        // 0-默认值/1-销售/2-售后/3-押金收费/4-存取大钞/5-COD回款
        ORM_REGISTER_COLUMN(int, SaleType, "sale_type")

        // 销售金额/售后金额/收费金额/存取大钞金额/回款金额
        ORM_REGISTER_COLUMN(qint64, Amount, "amount")

        // 收银员ID
        ORM_REGISTER_COLUMN(QString, CashierId, "cashier_id")

        // 收银员编号
        ORM_REGISTER_COLUMN(QString, CashierNo, "cashier_no")

        // 正逆向小票号
        ORM_REGISTER_COLUMN(QString, ReceiptNo, "receipt_no")

        // 离线任务创建时间
        ORM_REGISTER_COLUMN(QDateTime, CreateTime, "create_time")

        // 离线任务入库时间
        ORM_REGISTER_COLUMN(QDateTime, RecordTime, "record_time")

        // POS/PVT推送的离线任务全部数据
        ORM_REGISTER_COLUMN(QString, PushData, "push_data")

        // 扩展数据
        ORM_REGISTER_COLUMN(QString, Extension, "extension")
    };

    using OfflineDataEntityPtr = QSharedPointer<OfflineDataEntity>;
    using OfflineDataEntityPtrList = QList<QSharedPointer<OfflineDataEntity>>;
}

using namespace Sql;
ORM_REGISTER_HPP_POS_DATABASE(OfflineDataEntity)