#include "ApiIotReportEquipmentCommandResultRequest.h"

namespace Http
{
    ApiIotReportEquipmentCommandResultRequest::ApiIotReportEquipmentCommandResultRequest(QObject *parent)
        : PosRequest(parent)
    {
    }

    ApiIotReportEquipmentCommandResultRequest::~ApiIotReportEquipmentCommandResultRequest()
    {}

    void ApiIotReportEquipmentCommandResultRequest::SetExternalCode(const QString& externalCode)
    {
        m_externalCode = externalCode;
    }

    void ApiIotReportEquipmentCommandResultRequest::SetProductKey(const QString& productKey)
    {
        m_productKey = productKey;
    }

    void ApiIotReportEquipmentCommandResultRequest::SetPayload(const QString& payload)
    {
        m_payload = payload;
    }

    void ApiIotReportEquipmentCommandResultRequest::MakeJsonParam(QJsonObject& val)
    {
        ASSIGN_FROM_STRING(val, "externalCode", m_externalCode);
        ASSIGN_FROM_STRING(val, "productKey", m_productKey);
        ASSIGN_FROM_STRING(val, "payload", m_payload);
    }
}