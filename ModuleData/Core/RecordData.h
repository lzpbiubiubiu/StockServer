// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include <QObject>
#include <QDateTime>
#include <QSharedPointer>
#include "ModuleData/ModuleDataApi.h"

namespace Core
{
    /** POS/PVT记录数据 */
    struct RecordData
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

        /**（N）rovingPos门店ID */
        QString rovingPosOrgId;

        /**（N）收银员ID */
        QString cashierId;

        /**（N）收银员编号 */
        QString cashierNo;

        /**（N）小票号 */
        QString receiptNo;

        /**（Y）入库时间 */
        QDateTime createTime;

        /**（Y）POS/PVT推送的离线任务全部数据 */
        QString pushData;

        /**（Y）任务数据 */
        QString extension;
    };
    using RecordDataPtr = QSharedPointer<RecordData>;
    using RecordDataPtrList = QList<QSharedPointer<RecordData>>;
}

Q_DECLARE_METATYPE(Core::RecordData)
Q_DECLARE_METATYPE(Core::RecordDataPtr)
Q_DECLARE_METATYPE(Core::RecordDataPtrList)