// Copyright (c) 2022 Dmall Pos Team

#pragma once

#include "PosResponse.h"

namespace Http
{
    class MODULE_PROTOCOL_API GalaxyVersionResponse : public PosResponse
    {
    public:
        struct  VersionInfo
        {
            QString versionName;
            QString versionPath;
            QString md5;
            QString basicVersionCode;
            qint32  packageType = 1;   //包类型 1 - 完整包，2 - 补丁包
            qint32  forceUpdate = 2;   //是否强制更新，1-是，2-否
        };
        struct Result
        {
            QList<VersionInfo> versionList;
        };

        GalaxyVersionResponse(QObject *parent = Q_NULLPTR);
        ~GalaxyVersionResponse();

        const Result& GetResult() const;

    protected:
        /** 解析数组型JSON参数 */
        virtual void ParseJsonParam(const QJsonArray& val) override;

    private:
        Result m_result;
    };

}