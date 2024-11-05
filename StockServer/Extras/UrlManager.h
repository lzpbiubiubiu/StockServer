#pragma once

/**
* @file         UrlManager.h
* @brief        url管理类
* @details      负责客户端请求url管理
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QHash>
#include <QString>

#include "ModuleBase/Service/SyncService.h"

namespace Extra
{
    /** url管理类 */
    class UrlManager : public Base::SyncService
    {
        Q_OBJECT

    public:
        /** URL标识 */
        enum Key
        {
            /** 心跳 */
            HEART = 0,

            /** 上传 */
            FILE_UPLOAD,

            /** 设备指令回执上报 */
            API_IOT_REPORT_EQUIPMENT_COMMAND_RESULT,

            /** 设备信息 */
            GALAXY_DEVICE_INFO,

            /** 数据对账 */
            OFFLINE_CHECK_ACCOUNTS,

            /** 设备版本 */
            GALAXY_VERSION,

            /** 上报设备版本号*/
            GALAXY_REPORT_VERSION
        };

        /** URL参数 */
        struct Param
        {
            /** 主机 */
            QString host;

            /** 路径 */
            QString path;

            /** 超时（毫秒）*/
            int timeout = 0;

            /** URL */
            QString url;

            /** 描述 */
            QString description;
        };
    
        /** HOST数据结构 */
        struct HostInfo
        {
            /** scheme协议类型 */
            QString scheme;

            /** user 协议用户 */
            QString user;

            /** password 密码 */
            QString password;

            /** host 主机 */
            QString host;

            /** port 端口 */
            int port = 0;
        };
    public:
        UrlManager(QObject* parent = nullptr);
        virtual ~UrlManager();

        /**
        * @brief    添加/注册URL
        * @param    key         对应的枚举Key
        * @param    host        host
        * @param    path        url路径
        * @param    timeout     超时时间（毫秒）
        * @param    description 描述
        */
        void AddUrl(Key key, const QString& host, const QString& path, int timeout, const QString& description = QString());

        /**
        * @brief    获取URL
        * @return   URL参数
        */
        Param GetUrl(Key key) const;

        /**
        * @brief    获取GW主机信息
        * @return   GW主机信息
        */
        HostInfo GetGWHostInfo() const;

    protected:
        virtual bool OnStart() override;

        virtual void OnStop() override;

    private:
        QHash<Key, Param> m_urlTabs;
    };
}// namespace Extra