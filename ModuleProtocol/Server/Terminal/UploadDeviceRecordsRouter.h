#pragma once

#include "ModuleBase/Http/Server/Router.h"
#include "ModuleProtocol/ModuleProtocolApi.h"

namespace Http
{
    /** 设备记录上传 */
    class MODULE_PROTOCOL_API UploadDeviceRecordsRouter : public Router
    {
        /** 上传数据类型 */
        enum UploadDataType
        {
            /** 未知 */
            UPLOAD_UNKNOWN = 0,

            /** 离线数据 */
            UPLOAD_OFFLINE_DATA = 1,

            /** 聚合记录数据 */
            UPLOAD_RECORD_DATA = 2,
        };

    public:
        UploadDeviceRecordsRouter() = default;
        virtual ~UploadDeviceRecordsRouter() = default;

    public:
        /** 获取路由方法名称 */
        virtual const QString GetName() const override;

        /** 获取路由方法描述 */
        virtual const QString GetDescription() const override;

        /** 获取路由策略 */
        virtual const RoutePolicy GetPolicy() const override;

        /** 是否校验必须的headers */
        virtual const bool IsCheckHeader() const override;

        /** 校验POST请求的必须参数 */
        virtual bool CheckRequiredParams(const httplib::Request& req, QString& error) override;

        /** 路由处理方法 */
        virtual void OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse) override;

    private:
        /** 处理离线任务数据 */
        void OnOfflineDataHandler(const QJsonObject& object,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse);

        /** 处理聚合记录数据 */
        void OnRecordDataHandler(const QJsonObject& object,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse);

        /** 处理未知类型数据 */
        void OnUnknownDataHandler(const QJsonObject& object,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse);
    };

    using UploadDeviceRecordsRouterPtr = QSharedPointer<UploadDeviceRecordsRouter>;
}