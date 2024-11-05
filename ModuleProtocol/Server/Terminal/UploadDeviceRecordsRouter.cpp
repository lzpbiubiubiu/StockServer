#include "UploadDeviceRecordsRouter.h"
#include "ModuleData/Core/OfflineData.h"
#include "ModuleData/Core/RecordData.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Dml/OfflineDataDml.h"
#include "ModuleData/Dml/RecordDataDml.h"
#include "ModuleBase/Log/Log.h"

namespace Http
{
    const QString UploadDeviceRecordsRouter::GetName() const
    {
        return "/instore/terminal/uploadDeviceRecords";
    }

    const QString UploadDeviceRecordsRouter::GetDescription() const
    {
        return "upload device records";
    }

    const RoutePolicy UploadDeviceRecordsRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool UploadDeviceRecordsRouter::IsCheckHeader() const
    {
        return true;
    }

    bool UploadDeviceRecordsRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "uuid"))
            {
                error = "uuid is required";
                break;
            }
            if(!CHECK_REQUIRED_PARAM(object, "uploadType"))
            {
                error = "uploadType is required";
                break;
            }
            if(!CHECK_REQUIRED_PARAM(object, "content"))
            {
                error = "content is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void UploadDeviceRecordsRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        // post参数
        QJsonObject object = GetThreadLocal().object;

        int uploadType = 0;
        ASSIGN_POST_PARAM_TO_INT32(object, "uploadType", uploadType);
        UploadDataType type = static_cast<UploadDataType>(uploadType);

        switch(type)
        {
            case UPLOAD_OFFLINE_DATA:
            {
                OnOfflineDataHandler(object, header, reponse);
                break;
            }
            case UPLOAD_RECORD_DATA:
            {
                OnRecordDataHandler(object, header, reponse);
                break;
            }
            default:
            {
                OnUnknownDataHandler(object, header, reponse);
                break;
            }
        }
    }

    void UploadDeviceRecordsRouter::OnOfflineDataHandler(const QJsonObject& object,
        const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        do
        {
            Core::OfflineDataPtr offlineData = Core::OfflineDataPtr::create();
            ASSIGN_POST_PARAM_TO_STRING(object, "uuid", offlineData->uuid);

            // content中离线数据
            QString strContent;
            ASSIGN_POST_PARAM_TO_STRING(object, "content", strContent);
            QJsonObject content = Base::JsonUtil::ToJsonObject(strContent);
            if(content.isEmpty())
            {
                LOG_ERROR("content is not a object");
                reponse = HttpServerResponsePtr::create();
                reponse->code = HTTP_SERVER_UNPACK_OFFLINE_DATA_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_UNPACK_OFFLINE_DATA_ERROR;
                reponse->msg = "content is not a object";
                reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_UNPACK_OFFLINE_DATA_ERROR];
                reponse->traceId = header.traceId;
                reponse->success = false;
                return;
            }

            // 离线数据生成时间
            QString offlineCreateTime;
            ASSIGN_POST_PARAM_TO_STRING(content, "offlineCreateTime", offlineCreateTime);
            if(offlineCreateTime.isEmpty())
            {
                LOG_ERROR("offlineCreateTime is empty");
                reponse = HttpServerResponsePtr::create();
                reponse->code = HTTP_SERVER_UNPACK_OFFLINE_CREATE_TIME_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_UNPACK_OFFLINE_CREATE_TIME_ERROR;
                reponse->msg = "offlineCreateTime is empty";
                reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_UNPACK_OFFLINE_CREATE_TIME_ERROR];
                reponse->traceId = header.traceId;
                reponse->success = false;
                return;
            }
            offlineData->createTime = QDateTime::fromString(offlineCreateTime, "yyyy-MM-dd hh:mm:ss");
            offlineData->recordTime = QDateTime::currentDateTime();

            auto db = Base::GetService<Extra::DatabaseManager>()->GetRecordDatabase(offlineData->createTime.date());
            Sql::PushDml pushDml;
            pushDml.SetDatabase(db);

            // 判断是否存在相同主键数据
            QString sqlError;
            if(pushDml.IsJobGroupExist(offlineData, sqlError))
            {
                LOG_INFO("database has same uuid data,current uuid:{}", offlineData->uuid.toStdString());
                break;
            }

            qint64 cashierId = 0;
            ASSIGN_POST_PARAM_TO_INT64(content, "cashierId", cashierId);
            offlineData->cashierId = QString::number(cashierId);
            ASSIGN_POST_PARAM_TO_STRING(content, "cashierNo", offlineData->cashierNo);
            ASSIGN_POST_PARAM_TO_STRING(content, "posOrderNo", offlineData->receiptNo);
            ASSIGN_POST_PARAM_TO_STRING(object, "content", offlineData->pushData);

            QJsonArray offlineDataArray;
            ASSIGN_POST_PARAM_TO_ARRAY(content, "offlineDatas", offlineDataArray);
            for(const auto& offlineDataItem : offlineDataArray)
            {
                QJsonObject data = offlineDataItem.toObject();

                int dataType = 0;
                ASSIGN_POST_PARAM_TO_INT32(data, "dataType", dataType);
                if(2 == dataType || 17 == dataType)
                {
                    // 销售数据和0元单数据
                    offlineData->saleType = 1;
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    ASSIGN_POST_PARAM_TO_INT64(dataObject["amountDetail"].toObject(), "orderAmt", offlineData->amount);
                    break;
                }

                if(5 == dataType || 4 == dataType)
                {
                    // 有小票售后数据和无小票售后数据
                    offlineData->saleType = 2;
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    QJsonArray refundDetails = dataObject["refundDetails"].toArray();
                    for(const auto& refundDetail : refundDetails)
                    {
                        QJsonObject refundDetailObject = refundDetail.toObject();
                        qint64 refundAmount = 0;
                        ASSIGN_POST_PARAM_TO_INT64(refundDetailObject["extras"].toObject(), "paidActAmt", refundAmount);
                        offlineData->amount += refundAmount;
                    }
                    break;
                }

                if(13 == dataType)
                {
                    // 收费订单
                    offlineData->saleType = 3;
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    ASSIGN_POST_PARAM_TO_INT64(dataObject["amountDetail"].toObject(), "orderAmt", offlineData->amount);
                    break;
                }

                if(9 == dataType)
                {
                    // 存取大钞
                    offlineData->saleType = 4;
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    QJsonArray pickUpDetails = dataObject["amtDetails"].toArray();
                    for(const auto& pickUpDetail : pickUpDetails)
                    {
                        QJsonObject pickUpDetailObject = pickUpDetail.toObject();
                        qint64 amount = 0;
                        ASSIGN_POST_PARAM_TO_INT64(pickUpDetailObject, "amt", amount);
                        offlineData->amount += amount;
                    }
                    break;
                }
                if(24 == dataType)
                {
                    //Magento COD订单回款支付状态更新
                    offlineData->saleType = 5;
                    QString dataContent;
                    ASSIGN_POST_PARAM_TO_STRING(data, "dataContent", dataContent);
                    QJsonObject dataObject = Base::JsonUtil::ToJsonObject(dataContent);
                    ASSIGN_POST_PARAM_TO_INT64(dataObject, "codColletAmount", offlineData->amount);
                    break;
                }
            }
            // header中门店机台信息
            offlineData->deviceType = header.deviceType;
            offlineData->deviceId = header.deviceId;
            offlineData->terminalNo = header.terminalNo;
            offlineData->deviceCode = header.deviceCode;
            offlineData->tenantId = header.tenantId;
            offlineData->orgId = header.orgId;

            // 插入数据
            if(!pushDml.AddJobGroup(offlineData, sqlError))
            {
                LOG_ERROR(QStringLiteral("sql error: {%1}").arg(sqlError).toStdString());
                reponse = HttpServerResponsePtr::create();
                reponse->code = HTTP_SERVER_ADD_OFFLINE_DATA_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_ADD_OFFLINE_DATA_ERROR;
                reponse->msg = sqlError;
                reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_ADD_OFFLINE_DATA_ERROR];
                reponse->traceId = header.traceId;
                reponse->success = false;
                return;
            }
        } while(0);

        reponse = HttpServerResponsePtr::create();
        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
    }

    void UploadDeviceRecordsRouter::OnRecordDataHandler(const QJsonObject& object,
        const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        do
        {
            Core::RecordDataPtr recordData = Core::RecordDataPtr::create();
            ASSIGN_POST_PARAM_TO_STRING(object, "uuid", recordData->uuid);
            recordData->createTime = QDateTime::currentDateTime();

            auto db = Base::GetService<Extra::DatabaseManager>()->GetRecordDatabase(recordData->createTime.date());
            Sql::RecordDataDml recordDataDml;
            recordDataDml.SetDatabase(db);

            // 判断是否存在相同主键数据
            QString sqlError;
            if(recordDataDml.IsRecordExist(recordData, sqlError))
            {
                LOG_INFO("database has same uuid data, current uuid:{}", recordData->uuid.toStdString());
                break;
            }

            // content中离线数据
            QString strContent;
            ASSIGN_POST_PARAM_TO_STRING(object, "content", strContent);
            QJsonObject content = Base::JsonUtil::ToJsonObject(strContent);
            if(content.isEmpty())
            {
                LOG_ERROR("content is not a object");
                reponse = HttpServerResponsePtr::create();
                reponse->code = HTTP_SERVER_UNPACK_RECORD_DATA_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_UNPACK_RECORD_DATA_ERROR;
                reponse->msg = "content is not a object";
                reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_UNPACK_RECORD_DATA_ERROR];
                reponse->traceId = header.traceId;
                reponse->success = false;

                return;
            }

            // 收银员id
            qint64 cashierId = 0;
            ASSIGN_POST_PARAM_TO_INT64(content, "cashierId", cashierId);
            recordData->cashierId = QString::number(cashierId);
            ASSIGN_POST_PARAM_TO_STRING(content, "cashierNo", recordData->cashierNo);
            ASSIGN_POST_PARAM_TO_STRING(content, "rovingPosOrgId", recordData->rovingPosOrgId);
            ASSIGN_POST_PARAM_TO_STRING(object, "content", recordData->pushData);

            // header中门店机台信息
            recordData->deviceType = header.deviceType;
            recordData->deviceId = header.deviceId;
            recordData->terminalNo = header.terminalNo;
            recordData->deviceCode = header.deviceCode;
            recordData->tenantId = header.tenantId;
            recordData->orgId = header.orgId;

            // 插入数据
            if(!recordDataDml.AddRecord(recordData, sqlError))
            {
                LOG_ERROR(QStringLiteral("sql error: {%1}").arg(sqlError).toStdString());
                reponse = HttpServerResponsePtr::create();
                reponse->code = HTTP_SERVER_ADD_RECORD_DATA_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_ADD_RECORD_DATA_ERROR;
                reponse->msg = sqlError;
                reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_ADD_RECORD_DATA_ERROR];
                reponse->traceId = header.traceId;
                reponse->success = false;
                return;
            }
        } while(0);

        reponse = HttpServerResponsePtr::create();
        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
    }

    void UploadDeviceRecordsRouter::OnUnknownDataHandler(const QJsonObject& object,
        const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        reponse = HttpServerResponsePtr::create();
        reponse->code = HTTP_SERVER_UPLOAD_DATA_TYPE_ERROR;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_UPLOAD_DATA_TYPE_ERROR;
        reponse->msg = "upload data type is unknown";
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_UPLOAD_DATA_TYPE_ERROR];
        reponse->traceId = header.traceId;
        reponse->success = false;
    }
}