#include "GalaxyReportVersionRequest.h"

namespace Http
{
    GalaxyReportVersionRequest::GalaxyReportVersionRequest(QObject *parent)
        : PosRequest(parent)
    {
    }

    GalaxyReportVersionRequest::~GalaxyReportVersionRequest()
    {
    }

    void GalaxyReportVersionRequest::SetDeviceId(qint64 deviceId)
    {
        m_deviceId = deviceId;
    }

    void GalaxyReportVersionRequest::SetVersionCode(const QString& versionCode)
    {
        m_versionCode = versionCode;
    }

    void GalaxyReportVersionRequest::SetVersionName(const QString& versionName)
    {
        m_versionName = versionName;
    }

    void GalaxyReportVersionRequest::SetPackageName(const QString& packageName)
    {
        m_packageName = packageName;
    }

    void GalaxyReportVersionRequest::SetAppId(const QString& appId)
    {
        m_appId = appId;
    }

    void GalaxyReportVersionRequest::MakeJsonParam(QJsonObject& val)
    {
        ASSIGN_FROM_INT64(val, "deviceId", m_deviceId);
        ASSIGN_FROM_STRING(val, "versionCode", m_versionCode);
        ASSIGN_FROM_STRING(val, "versionName", m_versionName);
        ASSIGN_FROM_STRING(val, "packageName", m_packageName);
        ASSIGN_FROM_STRING(val, "appId", m_appId);
    }

}