#include "HeartResponse.h"

namespace Http
{
    HeartResponse::HeartResponse(QObject *parent)
        : PosResponse(parent)
    {}

    HeartResponse::~HeartResponse()
    {}

    void HeartResponse::ParseJsonParam(const QJsonObject& val)
    {
        auto commandList = val["commandList"].toArray();
        for(auto value : commandList)
        {
            m_result.commandList << value.toString();
        }
    }

    const HeartResponse::Result& HeartResponse::GetResult() const
    {
        return m_result;
    }
}