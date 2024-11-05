#include "ApiIotReportEquipmentCommandResultResponse.h"

namespace Http
{
    ApiIotReportEquipmentCommandResultResponse::ApiIotReportEquipmentCommandResultResponse(QObject *parent)
        : PosResponse(parent)
    {}

    ApiIotReportEquipmentCommandResultResponse::~ApiIotReportEquipmentCommandResultResponse()
    {}

    void ApiIotReportEquipmentCommandResultResponse::ParseJsonParam(const QJsonObject& val)
    {
    }

    const ApiIotReportEquipmentCommandResultResponse::Result& ApiIotReportEquipmentCommandResultResponse::GetResult() const
    {
        return m_result;
    }
}