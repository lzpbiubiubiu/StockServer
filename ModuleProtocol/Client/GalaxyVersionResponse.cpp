#include "GalaxyVersionResponse.h"

namespace Http
{
    GalaxyVersionResponse::GalaxyVersionResponse(QObject *parent)
        : PosResponse(parent)
    {
    }

    GalaxyVersionResponse::~GalaxyVersionResponse()
    {
    }

    void GalaxyVersionResponse::ParseJsonParam(const QJsonArray& val)
    {
        VersionInfo versionInfo;
        for (const auto& item : val)
        {
            QJsonObject obj = item.toObject();

            ASSIGN_TO_STRING(obj, "versionName", versionInfo.versionName);
            ASSIGN_TO_STRING(obj, "versionPath", versionInfo.versionPath);
            ASSIGN_TO_STRING(obj, "md5", versionInfo.md5);
            ASSIGN_TO_STRING(obj, "basicVersionCode", versionInfo.basicVersionCode);
            ASSIGN_TO_INT32(obj, "packageType", versionInfo.packageType);
            ASSIGN_TO_INT32(obj, "forceUpdate", versionInfo.forceUpdate);
            m_result.versionList << versionInfo;
        }
    }


    const GalaxyVersionResponse::Result& GalaxyVersionResponse::GetResult() const
    {
        return m_result;
    }
}