#include "GalaxyDeviceInfoResponse.h"

namespace Http
{
    GalaxyDeviceInfoResponse::GalaxyDeviceInfoResponse(QObject *parent)
        : PosResponse(parent)
    {}

    GalaxyDeviceInfoResponse::~GalaxyDeviceInfoResponse()
    {}

    void GalaxyDeviceInfoResponse::ParseJsonParam(const QJsonObject& val)
    {
        ASSIGN_TO_INT64(val, "id", m_result.id);
        ASSIGN_TO_STRING(val, "externalCode", m_result.externalCode);
        ASSIGN_TO_STRING(val, "name", m_result.name);
        ASSIGN_TO_INT64(val, "tenantId", m_result.tenantId);
        ASSIGN_TO_STRING(val, "tenantName", m_result.tenantName);
        ASSIGN_TO_INT64(val, "orgId", m_result.orgId);
        ASSIGN_TO_STRING(val, "orgCode", m_result.orgCode);
        ASSIGN_TO_STRING(val, "orgName", m_result.orgName);
        ASSIGN_TO_STRING(val, "address", m_result.address);
        ASSIGN_TO_INT32(val, "status", m_result.status);
        ASSIGN_TO_INT64(val, "serverTime", m_result.serverTime);
        ASSIGN_TO_INT64(val, "groupId", m_result.groupId);
        ASSIGN_TO_INT32(val, "orgType", m_result.orgType);

        for(auto i : val["equipmentExtensionVoList"].toArray())
        {
            EquipmentExtensionVo equipmentExtensionVo;
            QJsonObject obj = i.toObject();
            ASSIGN_TO_INT64(obj, "equipmentId", equipmentExtensionVo.equipmentId);
            ASSIGN_TO_INT64(obj, "equipmentTemplateId", equipmentExtensionVo.equipmentTemplateId);
            ASSIGN_TO_STRING(obj, "equipmentTemplateName", equipmentExtensionVo.equipmentTemplateName);
            ASSIGN_TO_INT64(obj, "equipmentExtensionModelId", equipmentExtensionVo.equipmentExtensionModelId);
            ASSIGN_TO_STRING(obj, "equipmentExtensionModelName", equipmentExtensionVo.equipmentExtensionModelName);
            ASSIGN_TO_OBJECT(obj, "customField", equipmentExtensionVo.customField);
            m_result.equipmentExtensionVoList << equipmentExtensionVo;
        }

        QJsonObject customFieldObj = val["customField"].toObject();
        if(!customFieldObj.isEmpty())
        {
            ASSIGN_TO_STRING(customFieldObj, "location", m_result.location);
            ASSIGN_TO_STRING(customFieldObj, "posExternalId", m_result.customField.posExternalId);
            ASSIGN_TO_STRING(customFieldObj, "posTouch", m_result.customField.posTouch);
            ASSIGN_TO_STRING(customFieldObj, "deskNum", m_result.customField.deskNum);
            ASSIGN_TO_STRING(customFieldObj, "hardwareSerialNo", m_result.customField.hardwareSerialNo);
            ASSIGN_TO_STRING(customFieldObj, "birMinNo", m_result.customField.birMinNo);
            ASSIGN_TO_STRING(customFieldObj, "birPtuNo", m_result.customField.birPtuNo);
            ASSIGN_TO_STRING(customFieldObj, "roving", m_result.customField.roving);
            ASSIGN_TO_STRING(customFieldObj, "ip", m_result.customField.inStoreServerIp);
            ASSIGN_TO_STRING(customFieldObj, "port", m_result.customField.inStoreServerPort);
        }
    
        auto winPosSimpleEquipmentVoList = val["winPosSimpleEquipmentVoList"].toArray();
        for(auto item : winPosSimpleEquipmentVoList)
        {
            SimpleEquipmentVoExt winPosSimpleEquipmentVo;
            QJsonObject obj = item.toObject();
            ASSIGN_TO_INT64(obj, "id", winPosSimpleEquipmentVo.id);
            ASSIGN_TO_STRING(obj, "externalCode", winPosSimpleEquipmentVo.externalCode);
            ASSIGN_TO_STRING(obj, "name", winPosSimpleEquipmentVo.name);
            ASSIGN_TO_OBJECT(obj, "customField", winPosSimpleEquipmentVo.customField);
            m_result.winPosSimpleEquipmentVoList << winPosSimpleEquipmentVo;
        }

        auto pvtSimpleEquipmentVoList = val["pvtSimpleEquipmentVoList"].toArray();
        for(auto item : pvtSimpleEquipmentVoList)
        {
            SimpleEquipmentVoExt pvtSimpleEquipmentVo;
            QJsonObject obj = item.toObject();
            ASSIGN_TO_INT64(obj, "id", pvtSimpleEquipmentVo.id);
            ASSIGN_TO_STRING(obj, "externalCode", pvtSimpleEquipmentVo.externalCode);
            ASSIGN_TO_STRING(obj, "name", pvtSimpleEquipmentVo.name);
            ASSIGN_TO_OBJECT(obj, "customField", pvtSimpleEquipmentVo.customField);
            m_result.pvtSimpleEquipmentVoList << pvtSimpleEquipmentVo;
        }
    }

    const GalaxyDeviceInfoResponse::Result& GalaxyDeviceInfoResponse::GetResult() const
    {
        return m_result;
    }
}