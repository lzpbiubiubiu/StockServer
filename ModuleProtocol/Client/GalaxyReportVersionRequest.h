#pragma once

#include "ModuleProtocol/ModuleProtocolApi.h"
#include "PosRequest.h"

namespace Http
{
    /** 上报版本，${domain}/galaxy/reportversion */
    class MODULE_PROTOCOL_API GalaxyReportVersionRequest : public PosRequest
    {
    public:
        GalaxyReportVersionRequest(QObject* parent = Q_NULLPTR);
        ~GalaxyReportVersionRequest();

        void SetDeviceId(qint64 deviceId);
        void SetVersionCode(const QString& versionCode);
        void SetVersionName(const QString& versionName);
        void SetPackageName(const QString& packageName);
        void SetAppId(const QString& appId);

    protected:
        virtual void MakeJsonParam(QJsonObject& val) override;

    private:

        // 设备id
        qint64      m_deviceId;         

        // 版本号
        QString     m_versionCode;      

        // 版本名称
        QString     m_versionName;      

        //包名
        QString     m_packageName;

        //appId
        QString     m_appId;
    };

}