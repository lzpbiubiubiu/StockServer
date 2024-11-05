#include "HandoverRouter.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/DatabaseManager.h"
#include <QDateTime>

namespace Http
{
    const QString HandoverRouter::GetName() const
    {
        return "/instore/terminal/handover";
    }

    const QString HandoverRouter::GetDescription() const
    {
        return "handover";
    }

    const RoutePolicy HandoverRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool HandoverRouter::IsCheckHeader() const
    {
        return true;
    }

    bool HandoverRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "orgId") || !CHECK_REQUIRED_PARAM(object, "cashierNo") || !CHECK_REQUIRED_PARAM(object, "handTime") ||
                !CHECK_REQUIRED_PARAM(object, "handStartTime") || !CHECK_REQUIRED_PARAM(object, "handEndTime"))
            {
                error = "orgId cashierNo handTime handStartTime handEndTime is required.";
                break;
            }
            else
            {
                qint64 handStartTime = object["handStartTime"].toVariant().toLongLong();
                qint64 handEndTime = object["handEndTime"].toVariant().toLongLong();
                if(handStartTime > handEndTime)
                {
                    error = "handStartTime is greater than handEndTime.";
                    break;
                }
            }
        } while(0);

        return error.isEmpty();
    }

    void HandoverRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;


        QSharedPointer<Core::HandOverResult> handOverResult = handOverStatictis(object);

        // 赋值返回数据
        reponse = HttpServerResponsePtr::create();
        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
        reponse->data = handOverResult->toQJsonObject();
    }

    HandOverResultPtr HandoverRouter::handOverStatictis(const QJsonObject& jsonObject)
    {
        QDateTime startTime = QDateTime::currentDateTime();
        HandOverResultPtr handOverResult = HandOverResultPtr::create();

        //1、透传
        handOverCommonBuild(handOverResult, jsonObject);
        //2、初始化交班信息
        initHandOverListInfo(handOverResult, jsonObject);

        //3、交班销售&售后&收费&取大数 其他信息统计
        handOverTradeSaleStaticis(handOverResult, jsonObject);

        handOverTradeAfterSaleStaticis(handOverResult, jsonObject);

        handOverTradeChargeStaticis(handOverResult, jsonObject);

        handOverTradePickUpStaticis(handOverResult, jsonObject);

        handOverTradeOtherInfoStaticis(handOverResult, jsonObject);

        //4、点算金额
        handOverCashMoneyInfoStaticis(handOverResult, jsonObject);

        QDateTime endTime = QDateTime::currentDateTime();

        LOG_INFO(QString("handOverStatictis time: %1 ms").arg(startTime.msecsTo(endTime)).toStdString());

        return handOverResult;
    }

    void HandoverRouter::handOverCommonBuild(HandOverResultPtr handOverResult, const QJsonObject& jsonObject)
    {
        handOverResult->orgId = jsonObject["orgId"].toString();
        handOverResult->posNo = jsonObject["posNo"].toString();
        handOverResult->cashierNo = jsonObject["cashierNo"].toString();
        handOverResult->authorizerId = jsonObject["authorizerId"].toString();
        handOverResult->handTime = jsonObject["handTime"].toDouble();
        handOverResult->handStartTime = jsonObject["handStartTime"].toVariant().toLongLong();
        handOverResult->handEndTime = jsonObject["handEndTime"].toVariant().toLongLong();
        handOverResult->openBoxCount = jsonObject["openBoxCount"].toInt();
        handOverResult->fixedReadyAmount = jsonObject["fixedReadyAmount"].toVariant().toLongLong();
        handOverResult->cashierCashAmount = jsonObject["cashierCashAmount"].toVariant().toLongLong();
    }

    void HandoverRouter::initHandOverListInfo(HandOverResultPtr handOverResult, const QJsonObject& jsonObject)
    {
        //获取交班范围内所有记录数据库
        QDateTime handStartDate = QDateTime::fromMSecsSinceEpoch(jsonObject["handStartTime"].toVariant().toLongLong());
        QDateTime handEndDate = QDateTime::fromMSecsSinceEpoch(jsonObject["handEndTime"].toVariant().toLongLong());

        m_saleOrdersAllList.clear();
        m_afterSaleOrdersAllList.clear();
        m_chargeAllList.clear();
        m_pickUpAllList.clear();
        m_CODColletAllList.clear();
        m_CODColletAllMap.clear();
        m_countCashDetails.clear();

        for(auto recordDate = handStartDate.date(); recordDate <= handEndDate.date(); recordDate = recordDate.addDays(1))
        {
            auto dbm = Base::GetService<Extra::DatabaseManager>()->GetRecordDatabase(recordDate);
            m_pushDml.SetDatabase(dbm);
            Core::OfflineDataPtrList saleOrdersList;
            QString sqlError;
            if(!m_pushDml.GetAllJobGroups(handOverResult->orgId, handOverResult->cashierNo, 1, 1, handStartDate, handEndDate, true, saleOrdersList, sqlError))
            {
                QString message = QString("HandoverRouter GetAllJobGroups sql error: %1, orgId: %2, cashierNo: %3, saleType: %4").arg(sqlError).arg(handOverResult->orgId).arg(handOverResult->cashierNo).arg(1);
                LOG_ERROR(message.toStdString());
            }
            m_saleOrdersAllList << saleOrdersList;

            Core::OfflineDataPtrList afterSaleOrdersList;
            if(!m_pushDml.GetAllJobGroups(handOverResult->orgId, handOverResult->cashierNo, 1, 2, handStartDate, handEndDate, true, afterSaleOrdersList, sqlError))
            {
                QString message = QString("HandoverRouter GetAllJobGroups sql error: %1, orgId: %2, cashierNo: %3, saleType: %4").arg(sqlError).arg(handOverResult->orgId).arg(handOverResult->cashierNo).arg(2);
                LOG_ERROR(message.toStdString());
            }
            m_afterSaleOrdersAllList << afterSaleOrdersList;

            Core::OfflineDataPtrList chargeList;
            if(!m_pushDml.GetAllJobGroups(handOverResult->orgId, handOverResult->cashierNo, 1, 3, handStartDate, handEndDate, true, chargeList, sqlError))
            {
                QString message = QString("HandoverRouter GetAllJobGroups sql error: %1, orgId: %2, cashierNo: %3, saleType: %4").arg(sqlError).arg(handOverResult->orgId).arg(handOverResult->cashierNo).arg(3);
                LOG_ERROR(message.toStdString());
            }
            m_chargeAllList << chargeList;

            Core::OfflineDataPtrList pickUpList;
            if(!m_pushDml.GetAllJobGroups(handOverResult->orgId, handOverResult->cashierNo, 1, 4, handStartDate, handEndDate, true, pickUpList, sqlError))
            {
                QString message = QString("HandoverRouter GetAllJobGroups sql error: %1, orgId: %2, cashierNo: %3, saleType: %4").arg(sqlError).arg(handOverResult->orgId).arg(handOverResult->cashierNo).arg(4);
                LOG_ERROR(message.toStdString());
            }
            m_pickUpAllList << pickUpList;

            Core::OfflineDataPtrList cODColletList;
            if(!m_pushDml.GetAllJobGroups(handOverResult->orgId, handOverResult->cashierNo, 1, 5, handStartDate, handEndDate, true, cODColletList, sqlError))
            {
                QString message = QString("HandoverRouter GetAllJobGroups sql error: %1, orgId: %2, cashierNo: %3, saleType: %4").arg(sqlError).arg(handOverResult->orgId).arg(handOverResult->cashierNo).arg(5);
                LOG_ERROR(message.toStdString());
            }
            m_CODColletAllList << cODColletList;
            for(auto& cODCollet : cODColletList)
            {
                m_CODColletAllMap.insert(cODCollet->receiptNo, cODCollet);
            }
        }

        //点算明细
        QJsonArray countCashDetailsArray = jsonObject["countCashDetails"].toArray();
        for(int i = 0; i < countCashDetailsArray.size(); ++i)
        {
            QSharedPointer<Core::CountCashDetail> countCashDetail = QSharedPointer<Core::CountCashDetail>::create();
            QJsonObject object = countCashDetailsArray[i].toObject();
            countCashDetail->paymentMethodId = object["paymentMethodId"].toString();
            countCashDetail->paymentMethodName = object["paymentMethodName"].toString();
            countCashDetail->totalAmount = object["totalAmount"].toVariant().toLongLong();
            //点算面额明细
            QList<QSharedPointer<Core::CashMoneyType>> cashMoneyTypes;
            QJsonArray cashMoneyTypesArray = object["cashMoneyTypes"].toArray();
            for(int i = 0; i < cashMoneyTypesArray.size(); ++i)
            {
                QSharedPointer<Core::CashMoneyType> cashMoneyType = QSharedPointer<Core::CashMoneyType>::create();
                QJsonObject moneyTypeObject = cashMoneyTypesArray[i].toObject();
                cashMoneyType->currencyType = moneyTypeObject["currencyType"].toInt();
                cashMoneyType->faceValue = moneyTypeObject["faceValue"].toVariant().toLongLong();
                cashMoneyType->count = moneyTypeObject["count"].toInt();

                cashMoneyTypes << cashMoneyType;
            }
            countCashDetail->cashMoneyTypes = cashMoneyTypes;
            m_countCashDetails << countCashDetail;
        }
    }

    void HandoverRouter::handOverTradeSaleStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject)
    {
        //------- 销售单统计 -------//
        QMap<QString, QSharedPointer<Core::PaymentSummary>> salePaymentSummaryMap; //销售单支付方式汇总Map
        QList<QSharedPointer<Core::PaymentSummary>> salePaymentSummarys; //销售单支付方式汇总List
        //机台序号汇总Map
        QMap<QString, QSharedPointer<Core::PosSiNoInfo>> posSiNoInfoMap;
        int saleCount = 0;//销售单数量
        long long saleTotalAmount = 0LL;//销售单应收总金额
        long long classCodNoCollectAmount = 0LL;// 班次内COD未回款金额
        long long noClassCodCollectAmount = 0LL;// 非班次内COD回款金额
        //int noClassCodCollectCount = 0;//非班次内COD现金回款次数
        long long allPaymentTotalAmount = 0LL;//所有支付方式总金额
        m_saleCashTotalAmount = 0LL;//销售单现金应收总金额

        for(const auto& saleOrder : m_saleOrdersAllList)
        {
            saleCount++;
            saleTotalAmount += saleOrder->amount;

            // 订单额外信息 isMagento:是否Magento订单 isMagentoCod:是否Magento COD订单 isCollect:是否已回款
            int isMagento = 0;
            int isMagentoCod = 0;
            int isCollect = 0;
            QString saleInvoiceNumber;

            // 每笔销售订单的支付详情列表
            QList<QSharedPointer<Core::PaymentDetail>> paymentDetails;

            QString strContent = saleOrder->pushData;
            QJsonObject content = Base::JsonUtil::ToJsonObject(strContent);
            if(content.isEmpty())
            {
                LOG_ERROR("handOverTradeSaleStaticis saleOrder content is not a object");
                return;
            }
            QJsonArray offlineDataArray;
            ASSIGN_POST_PARAM_TO_ARRAY(content, "offlineDatas", offlineDataArray);
            for(const auto& offlineDataItem : offlineDataArray)
            {
                QJsonObject data = offlineDataItem.toObject();
                int dataType = 0;
                ASSIGN_POST_PARAM_TO_INT32(data, "dataType", dataType);
                if(2 == dataType)
                {
                    // 销售单数据
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    QJsonObject customTag = dataObject["customTag"].toObject();
                    saleInvoiceNumber = customTag["saleInvoiceNumber"].toString();
                    if(customTag.contains("codOrder"))
                    {
                        // Magento订单
                        isMagento = 1;
                        // Magento COD订单 true:是 false:不是
                        isMagentoCod = customTag["codOrder"].toBool() ? 1 : 0;
                        if(customTag.contains("codPayStatus"))
                        {
                            // COD订单回款状态   codPayStatus-1:已回款 2:未回款
                            isCollect = customTag["codPayStatus"].toInt() == 1 ? 1 : 0;
                        }
                    }
                }
                else if(3 == dataType)
                {
                    // 对账单数据
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    QJsonArray paymentDetailsArray = dataObject["payTradeRecords"].toArray();
                    for(const auto& paymentDetailItem : paymentDetailsArray)
                    {
                        QSharedPointer<Core::PaymentDetail> paymentDetail = QSharedPointer<Core::PaymentDetail>::create();
                        QJsonObject paymentObject = paymentDetailItem.toObject();
                        paymentDetail->paymentMethodId = paymentObject["paymentMethodId"].toString();
                        // 对账单数据中没有支付方式名称
                        paymentDetail->paymentAmount = paymentObject["transPayableAmt"].toVariant().toLongLong();

                        paymentDetails << paymentDetail;
                    }
                }
            }

            long long orderFinished = saleOrder->createTime.toMSecsSinceEpoch();
            if(posSiNoInfoMap.contains(saleOrder->terminalNo))
            {
                QSharedPointer<Core::PosSiNoInfo> posSiNoInfo = posSiNoInfoMap.value(saleOrder->terminalNo);
                if(orderFinished < posSiNoInfo->saleOrderMinFinished)
                {
                    posSiNoInfo->saleOrderMinFinished = orderFinished;
                    posSiNoInfo->handOverBeginSiNo = saleInvoiceNumber;
                }
                if(orderFinished > posSiNoInfo->saleOrderMaxFinished)
                {
                    posSiNoInfo->saleOrderMaxFinished = orderFinished;
                    posSiNoInfo->handOverEndSiNo = saleInvoiceNumber;
                }
            }
            else
            {
                QSharedPointer<Core::PosSiNoInfo> posSiNoInfo = QSharedPointer<Core::PosSiNoInfo>::create();
                posSiNoInfo->posNo = saleOrder->terminalNo;
                posSiNoInfo->saleOrderMinFinished = orderFinished;
                posSiNoInfo->handOverBeginSiNo = saleInvoiceNumber;
                posSiNoInfo->saleOrderMaxFinished = orderFinished;
                posSiNoInfo->handOverEndSiNo = saleInvoiceNumber;

                posSiNoInfoMap.insert(saleOrder->terminalNo, posSiNoInfo);
            }

            // 通过COD更新回款数据列表筛选确定COD是否回款及回款时间

            if(m_CODColletAllMap.contains(saleOrder->receiptNo))
            {
                isCollect = 1;
                m_CODColletAllMap.remove(saleOrder->receiptNo);
            }

            //计算班次内COD未回款金额: Magento订单 && Magento COD订单 && COD未回款
            if(isMagento == 1 && isMagentoCod == 1 && isCollect == 0)
            {
                classCodNoCollectAmount += saleOrder->amount;
            }
            //支付汇总   POS所有该班次销售单--不包括未回款的 (线下订单 + Magento非COD订单 + Magento COD订单该班次已回款)
            //if (isMagento == 0 || (isMagento == 1 && isMagentoCod == 0) || (isMagento == 1 && isMagentoCod == 1 && isCollect == 1)) 

            //支付汇总   POS所有该班次销售单(只管销售部分，不管是否回款)
            if(1)
            {
                for(const auto& paymentDetail : paymentDetails)
                {
                    //现金支付总金额汇总
                    if(Core::GlobalData::Get()->GetCashPaymentMethodId() == paymentDetail->paymentMethodId)
                    {
                        m_saleCashTotalAmount += paymentDetail->paymentAmount;
                    }
                    if(salePaymentSummaryMap.contains(paymentDetail->paymentMethodId))
                    {
                        QSharedPointer<Core::PaymentSummary> paymentSummary = salePaymentSummaryMap.value(paymentDetail->paymentMethodId);
                        paymentSummary->totalAmount += paymentDetail->paymentAmount;
                        paymentSummary->totalCount += 1;
                    }
                    else
                    {
                        QSharedPointer<Core::PaymentSummary> paymentSummary = QSharedPointer<Core::PaymentSummary>::create();
                        paymentSummary->paymentMethodId = paymentDetail->paymentMethodId;
                        paymentSummary->paymentMethodName = paymentDetail->paymentMethodName;
                        paymentSummary->totalAmount = paymentDetail->paymentAmount;
                        paymentSummary->totalCount = 1;

                        salePaymentSummaryMap.insert(paymentSummary->paymentMethodId, paymentSummary);
                    }
                }
            }
        }

        //------- 销售单BIR序号 -------
        //收营员交班所有操作机台
        QString posNo;
        //收营员交班各个机台第一个销售单SI号
        QString handOverBeginSiNo;
        //收营员交班各个机台最后一个销售单SI号
        QString handOverEndSiNo;
        int count = 0;
        for(QMap<QString, QSharedPointer<Core::PosSiNoInfo>>::iterator it = posSiNoInfoMap.begin(); it != posSiNoInfoMap.end(); ++it)
        {
            QString key = it.key();//获取键
            QSharedPointer<Core::PosSiNoInfo> posSiNoInfo = it.value();//获取值
            posNo.append(key);
            handOverBeginSiNo.append(posSiNoInfo->handOverBeginSiNo);
            handOverEndSiNo.append(posSiNoInfo->handOverEndSiNo);
            if(++count != posSiNoInfoMap.size())
            {
                posNo.append(",");
                handOverBeginSiNo.append(",");
                handOverEndSiNo.append(",");
            }
        }
        if(!posSiNoInfoMap.contains(handOverResult->posNo))
        {
            if (!posSiNoInfoMap.isEmpty())
            { 
                posNo.append(",").append(handOverResult->posNo);
                handOverBeginSiNo.append(",");
                handOverEndSiNo.append(",");
            }
            else
            {
                posNo.append(handOverResult->posNo);
            }
        }
        
        handOverResult->posNo = posNo;
        handOverResult->handBeginSiNo = handOverBeginSiNo;
        handOverResult->handEndSiNo = handOverEndSiNo;

        //计算非班次内COD回款金额: Magento COD订单 && COD已回款 && 回款COD未在销售单中
        for(const auto& cODCollet : m_CODColletAllMap.values())
        {
            noClassCodCollectAmount += cODCollet->amount;
            //noClassCodCollectCount += 1;
        }

        //销售单支付方式汇总列表 (该班次內线下订单 + Magento非COD订单 + Magento COD订单该班次已回款 + 非班次内COD回款)

        //if(salePaymentSummaryMap.contains(Core::GlobalData::Get()->GetCashPaymentMethodId()))
        //{
        //	QSharedPointer<Core::PaymentSummary> paymentSummary = salePaymentSummaryMap.value(Core::GlobalData::Get()->GetCashPaymentMethodId());
        //	paymentSummary->totalAmount += noClassCodCollectAmount;
        //	paymentSummary->totalCount += noClassCodCollectCount;
        //}
        //else
        //{
        //	QSharedPointer<Core::PaymentSummary> paymentSummary = QSharedPointer<Core::PaymentSummary>::create();
        //	paymentSummary->paymentMethodId = Core::GlobalData::Get()->GetCashPaymentMethodId();
        //	paymentSummary->totalAmount = noClassCodCollectAmount;
        //	paymentSummary->totalCount = noClassCodCollectCount;

        //	salePaymentSummaryMap.insert(paymentSummary->paymentMethodId, paymentSummary);
        //}

        for(auto& paymentSummary : salePaymentSummaryMap.values())
        {
            salePaymentSummarys << paymentSummary;
            allPaymentTotalAmount += paymentSummary->totalAmount;
        }

        handOverResult->saleCount = saleCount;
        handOverResult->saleTotalAmount = saleTotalAmount;
        handOverResult->classCodNoCollectAmount = classCodNoCollectAmount;
        handOverResult->noClassCodCollectAmount = noClassCodCollectAmount;
        handOverResult->salePaymentSummarys = salePaymentSummarys;
        handOverResult->allPaymentTotalAmount = allPaymentTotalAmount;

        // 交班时间范围内销售单现金应收总金额 = 现金销售单总金额(不包括班次内COD未回款金额) + 非班次内COD回款金额
        //m_saleCashTotalAmount += noClassCodCollectAmount;
        m_saleCashTotalAmount += noClassCodCollectAmount - classCodNoCollectAmount;
    }

    void HandoverRouter::handOverTradeAfterSaleStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject)
    {
        //------- 售后单统计 -------//
        //售后单支付方式汇总Map
        QMap<QString, QSharedPointer<Core::PaymentSummary>> afterSalePaymentSummaryMap;
        //售后单支付方式汇总List
        QList<QSharedPointer<Core::PaymentSummary>> afterSalePaymentSummarys;
        int afterSaleCount = 0L;//售后单数量
        long long afterSaleTotalAmount = 0L;//售后单总金额 
        m_afterSaleCashTotalAmount = 0LL;//售后单现金总金额

        for(const auto& afterSaleDetail : m_afterSaleOrdersAllList)
        {
            afterSaleCount++;
            afterSaleTotalAmount += afterSaleDetail->amount;

            //每笔售后单支付详情列表
            QList<QSharedPointer<Core::PaymentDetail>> paymentDetails;
            QString strContent = afterSaleDetail->pushData;
            QJsonObject content = Base::JsonUtil::ToJsonObject(strContent);
            if(content.isEmpty())
            {
                LOG_ERROR("handOverTradeAfterSaleStaticis afterSaleDetail content is not a object");
                return;
            }
            QJsonArray offlineDataArray;
            ASSIGN_POST_PARAM_TO_ARRAY(content, "offlineDatas", offlineDataArray);
            for(const auto& offlineDataItem : offlineDataArray)
            {
                QJsonObject data = offlineDataItem.toObject();
                int dataType = 0;
                ASSIGN_POST_PARAM_TO_INT32(data, "dataType", dataType);
                if(4 == dataType || 5 == dataType)
                {
                    // 对账单数据
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    QJsonArray paymentDetailsArray = dataObject["refundDetails"].toArray();
                    for(const auto& paymentDetailItem : paymentDetailsArray)
                    {
                        QSharedPointer<Core::PaymentDetail> paymentDetail = QSharedPointer<Core::PaymentDetail>::create();
                        QJsonObject paymentObject = paymentDetailItem.toObject();
                        paymentDetail->paymentMethodId = paymentObject["refundChannelId"].toString();
                        paymentDetail->paymentMethodName = paymentObject["refundChannelName"].toString();
                        QJsonObject extras = paymentObject["extras"].toObject();
                        paymentDetail->paymentAmount = extras["paidActAmt"].toVariant().toLongLong();

                        paymentDetails << paymentDetail;
                    }
                }
            }

            for(const auto& paymentDetail : paymentDetails)
            {
                //现金支付总金额汇总
                if(Core::GlobalData::Get()->GetCashPaymentMethodId() == paymentDetail->paymentMethodId)
                {
                    m_afterSaleCashTotalAmount += paymentDetail->paymentAmount;
                }
                if(afterSalePaymentSummaryMap.contains(paymentDetail->paymentMethodId))
                {
                    QSharedPointer<Core::PaymentSummary> paymentSummary = afterSalePaymentSummaryMap.value(paymentDetail->paymentMethodId);
                    paymentSummary->totalAmount += paymentDetail->paymentAmount;
                    paymentSummary->totalCount = paymentSummary->totalCount + 1;
                }
                else
                {
                    QSharedPointer<Core::PaymentSummary> paymentSummary = QSharedPointer<Core::PaymentSummary>::create();
                    paymentSummary->paymentMethodId = paymentDetail->paymentMethodId;
                    paymentSummary->paymentMethodName = paymentDetail->paymentMethodName;
                    paymentSummary->totalAmount = paymentDetail->paymentAmount;
                    paymentSummary->totalCount = 1;

                    afterSalePaymentSummaryMap.insert(paymentSummary->paymentMethodId, paymentSummary);
                }
            }
        }

        handOverResult->afterSaleCount = afterSaleCount;
        handOverResult->afterSaleTotalAmount = afterSaleTotalAmount;

        for(const auto& paymentSummary : afterSalePaymentSummaryMap.values())
        {
            afterSalePaymentSummarys << paymentSummary;
        }
        handOverResult->afterPaymentSummarys = afterSalePaymentSummarys;
    }

    void HandoverRouter::handOverTradeChargeStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject)
    {
        //------- 收费单统计 -------//
        long long chargeTotalAmount = 0L;//收费单总金额
        m_chargeCashTotalAmount = 0LL;
        if(!m_chargeAllList.isEmpty())
        {
            for(const auto& chargeFee : m_chargeAllList)
            {
                chargeTotalAmount += chargeFee->amount;

                //每笔收费单支付详情列表
                QList<QSharedPointer<Core::PaymentDetail>> paymentDetails;
                QString strContent = chargeFee->pushData;
                QJsonObject content = Base::JsonUtil::ToJsonObject(strContent);
                if(content.isEmpty())
                {
                    LOG_ERROR("handOverTradeChargeStaticis chargeFee content is not a object");
                    return;
                }

                QJsonArray offlineDataArray;
                ASSIGN_POST_PARAM_TO_ARRAY(content, "offlineDatas", offlineDataArray);
                for(const auto& offineDataItem : offlineDataArray)
                {
                    QJsonObject data = offineDataItem.toObject();
                    int dataType = 0;
                    ASSIGN_POST_PARAM_TO_INT32(data, "dataType", dataType);
                    // dataType -- 14:收费单支付明细数据   13:收费单商品明细数据
                    if(14 == dataType)
                    {
                        QString dataContent;
                        ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                        QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                        QJsonArray paymentDetailsArray = dataObject["payTradeRecords"].toArray();
                        for(const auto& paymentDetailItem : paymentDetailsArray)
                        {
                            QSharedPointer<Core::PaymentDetail> paymentDetail = QSharedPointer<Core::PaymentDetail>::create();
                            QJsonObject paymentObject = paymentDetailItem.toObject();
                            paymentDetail->paymentMethodId = paymentObject["paymentMethodId"].toString();
                            // 收费单支付方式名称没有
                            paymentDetail->paymentAmount = paymentObject["transPayableAmt"].toVariant().toLongLong();

                            paymentDetails << paymentDetail;
                        }
                    }
                }

                for(const auto& paymentDetail : paymentDetails)
                {
                    //现金支付总金额汇总
                    if(Core::GlobalData::Get()->GetCashPaymentMethodId() == paymentDetail->paymentMethodId)
                    {
                        m_chargeCashTotalAmount += paymentDetail->paymentAmount;
                    }
                }
            }

            handOverResult->chargeTotalAmount = chargeTotalAmount;
        }
    }

    void HandoverRouter::handOverTradePickUpStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject)
    {
        //------- 取大数统计 -------//
        //取大钞结果信息
        //取大数支付汇总Map
        QMap<QString, QSharedPointer<Core::PaymentSummary>> pickUpPaymentSummaryMap;
        int pickUpTotalcount = 0;//取大钞总次数
        long long pickUpTotalAmount = 0LL;//取大钞总金额
        QSharedPointer<Core::GetBigMoneyInfo> getBigMoneyInfo = QSharedPointer<Core::GetBigMoneyInfo>::create();

        for(const auto& pickUpInfo : m_pickUpAllList)
        {
            //1:存  2:取
            int pickUpType = 0;

            QString strContent = pickUpInfo->pushData;
            QJsonObject content = Base::JsonUtil::ToJsonObject(strContent);
            if(content.isEmpty())
            {
                LOG_ERROR("handOverTradePickUpStaticis pickUpInfo content is not a object");
                return;
            }
            QJsonArray offlineDataArray;
            ASSIGN_POST_PARAM_TO_ARRAY(content, "offlineDatas", offlineDataArray);
            for(const auto& offlineDataItem : offlineDataArray)
            {
                QJsonObject data = offlineDataItem.toObject();
                int dataType = 0;
                ASSIGN_POST_PARAM_TO_INT32(data, "dataType", dataType);
                if(9 == dataType)
                {
                    // 存取大钞数据
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    pickUpType = dataObject["type"].toInt();

                    if(2 == pickUpType)
                    {
                        pickUpTotalcount++;
                        pickUpTotalAmount += pickUpInfo->amount;
                        //支付汇总
                        QJsonArray amtDetailsArray = dataObject["amtDetails"].toArray();
                        for(const auto& amtDetailsItem : amtDetailsArray)
                        {
                            QJsonObject amtDetailObject = amtDetailsItem.toObject();
                            QString paymentMethodId = amtDetailObject["psId"].toString();
                            // 取大钞支付方式名称没有
                            if(pickUpPaymentSummaryMap.contains(paymentMethodId))
                            {
                                QSharedPointer<Core::PaymentSummary> paymentSummary = pickUpPaymentSummaryMap.value(paymentMethodId);
                                paymentSummary->totalAmount += amtDetailObject["amt"].toVariant().toLongLong();
                                paymentSummary->totalCount = paymentSummary->totalCount + 1;
                            }
                            else
                            {
                                QSharedPointer<Core::PaymentSummary> paymentSummary = QSharedPointer<Core::PaymentSummary>::create();
                                paymentSummary->paymentMethodId = paymentMethodId;
                                paymentSummary->totalAmount = amtDetailObject["amt"].toVariant().toLongLong();
                                paymentSummary->totalCount = 1;

                                pickUpPaymentSummaryMap.insert(paymentSummary->paymentMethodId, paymentSummary);
                            }
                        }
                    }
                }
            }
        }
        getBigMoneyInfo->count = pickUpTotalcount;
        getBigMoneyInfo->totalAmount = pickUpTotalAmount;


        //取大数支付方式汇总列表
        QList<QSharedPointer<Core::PaymentSummary>> pickUpPaymentSummarys;
        for(const auto& pickUpPaymentSummary : pickUpPaymentSummaryMap.values())
        {
            pickUpPaymentSummarys << pickUpPaymentSummary;
        }

        getBigMoneyInfo->paymentSummarys = pickUpPaymentSummarys;

        handOverResult->getBigMoneyInfo = getBigMoneyInfo;
    }

    void HandoverRouter::handOverTradeOtherInfoStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject) const
    {
        //Net Sales 和 Net Received(总销售金额-总应退金额)
        long long netSalesAmount = handOverResult->saleTotalAmount - handOverResult->afterSaleTotalAmount;
        handOverResult->netSalesAmount = netSalesAmount;
        //Cash Received(总销售现金实收+总收费现金实收-总退货现金实退)
        long long cashReceivedAmount = m_saleCashTotalAmount + m_chargeCashTotalAmount - m_afterSaleCashTotalAmount;
        handOverResult->cashReceivedAmount = cashReceivedAmount;
        //系统现金应缴金额(固定备用金 + 现金销售实收 + 收费 - 退货(void + return) - 取大数)
        long long systemCashAmount = handOverResult->fixedReadyAmount + m_saleCashTotalAmount + m_chargeCashTotalAmount - m_afterSaleCashTotalAmount - handOverResult->getBigMoneyInfo->totalAmount;
        handOverResult->systemCashAmount = systemCashAmount;
        //现金实缴差异金额(cashierCashAmount-systemCashAmount)
        long long cashDiffAmount = handOverResult->cashierCashAmount - systemCashAmount;
        handOverResult->cashDiffAmount = cashDiffAmount;
    }

    void HandoverRouter::handOverCashMoneyInfoStaticis(HandOverResultPtr handOverResult, const QJsonObject& jsonObject)
    {
        //------- 交班点算金额信息 -------//

        if(m_countCashDetails.isEmpty())
        {
            LOG_INFO("Handover calculation amount statistics, the calculation amount list is empty.");
            return;
        }

        QList<QSharedPointer<Core::CountCashInfo>> countCashInfos;
        for(const auto& countCashDetail : m_countCashDetails)
        {
            QSharedPointer<Core::CountCashInfo> countCashInfo = QSharedPointer<Core::CountCashInfo>::create();
            countCashInfo->paymentMethodId = countCashDetail->paymentMethodId;
            countCashInfo->paymentMethodName = countCashDetail->paymentMethodName;
            countCashInfo->totalAmount = countCashDetail->totalAmount;
            //现金点算
            QList<QSharedPointer<Core::CashMoneyType>> cashMoneyTypes = countCashDetail->cashMoneyTypes;
            if(!cashMoneyTypes.isEmpty())
            {
                //现金点算金额汇总Map
                QMap<long long, QSharedPointer<Core::CashMoneyInfo>> cashMoneyInfoSummaryMap;
                for(const auto& cashMoneyType : cashMoneyTypes)
                {
                    if(cashMoneyInfoSummaryMap.contains(cashMoneyType->faceValue))
                    {
                        QSharedPointer<Core::CashMoneyInfo> cashMoneyInfo = cashMoneyInfoSummaryMap.value(cashMoneyType->faceValue);
                        cashMoneyInfo->totalAmount += cashMoneyType->faceValue * cashMoneyType->count;
                        cashMoneyInfo->count += cashMoneyType->count;
                    }
                    else
                    {
                        QSharedPointer<Core::CashMoneyInfo> cashMoneyInfo = QSharedPointer<Core::CashMoneyInfo>::create();
                        cashMoneyInfo->faceValue = cashMoneyType->faceValue;
                        cashMoneyInfo->totalAmount = cashMoneyType->faceValue * cashMoneyType->count;
                        cashMoneyInfo->currencyType = cashMoneyType->currencyType;
                        cashMoneyInfo->count = cashMoneyType->count;

                        cashMoneyInfoSummaryMap.insert(cashMoneyInfo->faceValue, cashMoneyInfo);
                    }
                }
                /** 现金点算总金额 */
                long long cashTotalAmount = 0LL;
                //现金点算金额面额汇总列表
                QList<QSharedPointer<Core::CashMoneyInfo>> cashMoneyInfos;
                for(auto& cashMoneyInfo : cashMoneyInfoSummaryMap.values())
                {
                    cashTotalAmount += cashMoneyInfo->totalAmount;
                    cashMoneyInfos << cashMoneyInfo;
                }

                handOverResult->cashTotalAmount = cashTotalAmount;
                countCashInfo->cashMoneyInfos = cashMoneyInfos;
            }

            countCashInfos << countCashInfo;
        }
        handOverResult->countCashInfos = countCashInfos;
    }
}
