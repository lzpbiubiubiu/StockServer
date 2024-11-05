#include "HeartRequest.h"

namespace Http
{
    HeartRequest::HeartRequest(QObject *parent)
        : PosRequest(parent)
    {
    }

    HeartRequest::~HeartRequest()
    {}

    void HeartRequest::SetTimestamp(qint64 timestamp)
    {
        m_timestamp = timestamp;
    }

    void HeartRequest::SetProductKey(const QString& productKey)
    {
        m_productKey = productKey;
    }

    void HeartRequest::SetOperateType(OperateType operateType)
    {
        m_operateType = operateType;
    }

    void HeartRequest::MakeJsonParam(QJsonObject& val)
    {
        ASSIGN_FROM_INT64(val, "timestamp", m_timestamp);
        ASSIGN_FROM_STRING(val, "productKey", m_productKey);
        ASSIGN_FROM_INT32_IF_GREATER_THAN_ZERO(val, "operateType", (int)m_operateType);
    }
}