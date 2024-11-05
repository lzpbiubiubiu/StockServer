// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include <QObject>
#include <QDateTime>
#include <QSharedPointer>
#include "ModuleData/ModuleDataApi.h"

namespace Core
{
    /** 离线任务 */
    struct MODULE_DATA_API OfflineData
    {
        /**（Y）离线订单唯一标志 */
        QString uuid;

        /**（Y）设备类型:0-默认值/1-POS/2-PVT */
        int deviceType = 0;

        /**（Y）POS机ID */
        QString deviceId;

        /**（Y）POS/PVT 银台号 */
        QString terminalNo;

        /**（Y）POS/PVT MAC地址*/
        QString deviceCode;

        /**（Y）商家ID */
        QString tenantId;

        /**（Y）门店ID */
        QString orgId;

        /**（Y）0-默认值/1-销售/2-售后/3-押金收费/4-存取大钞/5-COD回款 */
        int saleType = 0;

        /**（N）销售金额/售后金额/押金收费/存取大钞金额/COD回款金额 */
        qint64 amount = 0;

        /**（N）收银员ID */
        QString cashierId;

        /**（N）收银员编号 */
        QString cashierNo;

        /**（N）小票号 */
        QString receiptNo;

        /**（Y）离线任务创建时间 */
        QDateTime createTime;

        /**（Y）离线任务入库时间 */
        QDateTime recordTime;

        /**（Y）POS/PVT推送的离线任务全部数据 */
        QString pushData;

        /**（Y）任务数据 */
        QString extension;

        //Q_GADGET
    };

    using OfflineDataPtr = QSharedPointer<OfflineData>;
    using OfflineDataPtrList = QList<QSharedPointer<OfflineData>>;
}

Q_DECLARE_METATYPE(Core::OfflineData)
Q_DECLARE_METATYPE(Core::OfflineDataPtr)
Q_DECLARE_METATYPE(Core::OfflineDataPtrList)