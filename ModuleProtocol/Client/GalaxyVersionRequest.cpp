#include "GalaxyVersionRequest.h"

namespace Http
{
    GalaxyVersionRequest::GalaxyVersionRequest(QObject* parent)
    {
    }

    GalaxyVersionRequest::~GalaxyVersionRequest()
    {
    }

    void GalaxyVersionRequest::SetDeviceId(const QString& deviceId)
    {
        m_deviceId = deviceId;
    }

    void GalaxyVersionRequest::SetCurrentVersion(const QString& currentVersion)
    {
        m_currentVersion = currentVersion;
    }

    void GalaxyVersionRequest::SetPackageName(const QString& packageName)
    {
        m_packageName = packageName;
    }

    void GalaxyVersionRequest::SetAppId(const QString& appId)
    {
        m_appId = appId;
    }

    void GalaxyVersionRequest::MakeJsonParam(QJsonObject& val)
    {
        ASSIGN_FROM_STRING(val, "deviceId", m_deviceId);
        ASSIGN_FROM_STRING(val, "currentVersionCode", m_currentVersion);
        ASSIGN_FROM_STRING(val, "packageName", m_packageName);
        ASSIGN_FROM_STRING(val, "appId", m_appId);
    }
}