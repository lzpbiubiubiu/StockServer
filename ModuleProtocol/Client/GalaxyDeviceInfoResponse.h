// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include "PosResponse.h"

namespace Http
{
    /** 银河设备信息返回，/galaxy/deviceInfo */
    class MODULE_PROTOCOL_API GalaxyDeviceInfoResponse : public PosResponse
    {
    public:
        struct EquipmentExtensionVo
        {
            // 外设ID
            qint64 equipmentId = 0;

            // 外设模板ID
            qint64 equipmentTemplateId = 0;

            // 外设模板名称
            QString equipmentTemplateName;

            // 外设型号ID
            qint64 equipmentExtensionModelId = 0;

            // 外设型号名称
            QString equipmentExtensionModelName;

            // 自定义字段json
            QJsonObject customField;
        };

        struct SimpleEquipmentVoExt
        {
            /** 设备id */
            qint64 id = 0;

            /** 设备编码，唯一索引 */
            QString externalCode;

            /** 设备名称 */
            QString name;

            /** 自定义字段 */
            QJsonObject customField;
        };

        struct CustomFieldJson
        {
            /** pos外部id */
            QString posExternalId;

            /** 是否触屏版，1 是， 0 不是 */
            QString posTouch;

            /** pos机台号 */
            QString deskNum;

            /** 前置机ip*/
            QString inStoreServerIp;

            /** 前置机端口*/
            QString inStoreServerPort;

            /** SM 硬件序列号 */
            QString hardwareSerialNo;

            /** SM BIR MIN机器编码 */
            QString birMinNo;

            /** SM BIR PTU机器编码 */
            QString birPtuNo;

            /** SM Roving POS 归属门店 */
            QString roving;
        };

        struct Result
        {
            // 设备id
            qint64 id = 0;

            // 设备编码
            QString externalCode;

            // 设备名称
            QString name;

            // 位置
            QString location;

            // 租户ID
            qint64 tenantId = 0;

            // 租户名称
            QString tenantName;

            // 机构ID
            qint64 orgId = 0;

            // 机构编码
            QString orgCode;

            // 机构名称
            QString orgName;

            // 集团ID
            qint64 groupId = 0;

            // 门店地址
            QString address;

            // 门店类型
            int orgType = 0;

            // 设备状态 1-启用 2-停用
            int status = 1;

            // 服务器时间
            qint64 serverTime = 0;

            // 外设信息列表
            QList<EquipmentExtensionVo> equipmentExtensionVoList;

            // 自定义字段
            CustomFieldJson customField;

            // 查询In-Store POS Server时,返回的当前门店的winPos设备列表
            QList<SimpleEquipmentVoExt> winPosSimpleEquipmentVoList;

            // 查询In-Store POS Server时,返回的当前门店的PVT设备列表
            QList<SimpleEquipmentVoExt> pvtSimpleEquipmentVoList;
        };

        GalaxyDeviceInfoResponse(QObject *parent = Q_NULLPTR);
        ~GalaxyDeviceInfoResponse();

        const Result& GetResult() const;

    protected:
        virtual void ParseJsonParam(const QJsonObject& val) override;

    private:
        Result m_result;
    };
}