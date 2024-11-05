#pragma once

#include "ModuleBase/Http/Server/Router.h"
#include "ModuleProtocol/ModuleProtocolApi.h"
#include "ModuleData/HandOver/HandOverResult.h"
#include "ModuleData/Dml/OfflineDataDml.h"

namespace Http
{
    /** 交班 */
    class MODULE_PROTOCOL_API HandoverRouter : public Router
    {
    public:
        HandoverRouter() = default;
        virtual ~HandoverRouter() = default;

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

    protected:
        /** SM交班统计服务 */
        HandOverResultPtr handOverStatictis(const QJsonObject& jsonObject);

        /**交班透传参数 */
        void handOverCommonBuild(HandOverResultPtr handOverResult, const QJsonObject& jsonObject);

        /** 初始化交班信息 */
        void initHandOverListInfo(HandOverResultPtr handOverResult, const QJsonObject& jsonObject);

        /**交班销售统计 */
        void handOverTradeSaleStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject);

        /**交班售后统计 */
        void handOverTradeAfterSaleStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject);

        /**交班押金收费统计 */
        void handOverTradeChargeStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject);

        /**交班存取大数统计 */
        void handOverTradePickUpStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject);

        /**交班其他信息统计 */
        void handOverTradeOtherInfoStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject) const;
        
        /**交班点算金额统计 */
        void handOverCashMoneyInfoStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject);

    protected:
        //数据库操作对象
        Sql::PushDml m_pushDml;

        //交班时间范围内所有销售数据列表
        Core::OfflineDataPtrList m_saleOrdersAllList;

        //交班时间范围内所有售后数据列表
        Core::OfflineDataPtrList m_afterSaleOrdersAllList;
       
        //交班时间范围内所有押金收费数据列表
        Core::OfflineDataPtrList m_chargeAllList;

        //交班时间范围内所有存取大钞数据列表
        Core::OfflineDataPtrList m_pickUpAllList;

        //交班时间范围内所有COD更新回款数据列表
        Core::OfflineDataPtrList m_CODColletAllList;

        QMap<QString, Core::OfflineDataPtr> m_CODColletAllMap;

        //交班时间范围内所有点算明细数据列表
        QList<QSharedPointer<Core::CountCashDetail>> m_countCashDetails;

        //交班时间范围内销售单现金应收总金额 = 现金销售单总金额(不包括班次内COD未回款金额) + 非班次内COD回款金额
        long long m_saleCashTotalAmount = 0L;

        //交班时间范围内售后单现金总金额 
        long long m_afterSaleCashTotalAmount = 0L;

        //收费单现金总金额
        long long m_chargeCashTotalAmount = 0L;
    };
    using HandoverRouterPtr = QSharedPointer<HandoverRouter>;
}