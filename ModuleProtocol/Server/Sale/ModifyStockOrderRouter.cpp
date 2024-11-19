#include "ModuleBase/Database/Base/DmlBase.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/GlobalStructData.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "ModuleBase/Log/Log.h"
#include "ModifyStockOrderRouter.h"

namespace Http
{
    const QString ModifyStockOrderRouter::ModifyStockOrderRouter::GetName() const
    {
        return "/stockServer/sale/modifyStockOrder";
    }

    const QString ModifyStockOrderRouter::GetDescription() const
    {
        return "modifyStockOrder";
    }

    const RoutePolicy ModifyStockOrderRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool ModifyStockOrderRouter::IsCheckHeader() const
    {
        return true;
    }

    bool ModifyStockOrderRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "orderNo"))
            {
                error = "orderNo is required";
                break;
            }
            if (!CHECK_REQUIRED_PARAM(object, "orderUuid"))
            {
                error = "orderUuid is required";
                break;
            }
            if(!CHECK_REQUIRED_PARAM(object, "operate"))
            {
                error = "operate is required";
                break;
            }
            if(!CHECK_REQUIRED_PARAM(object, "stockOrderDetail"))
            {
                error = "stockOrderDetail is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void ModifyStockOrderRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;
        auto uniqueId = object["uniqueId"].toString();
        auto orderNo = object["orderNo"].toString();
        auto orderUuid = object["orderUuid"].toString();
        auto operate = object["operate"].toInt();
        auto stockOrderDetailObj = object["stockOrderDetail"].toObject();

        auto db = Base::GetService<Extra::DatabaseManager>()->GetTempDatabase();
        Sql::DmlBase<Sql::OrderEntity> dml;
        dml.SetDatabase(db);

        auto wareDb = Base::GetService<Extra::DatabaseManager>()->GetStockServerDatabase();
        Sql::DmlBase<Sql::ConfigWareEntity> wareDml;
        wareDml.SetDatabase(wareDb);

        QString sqlError;
        Sql::OrderEntityPtr orderEntity = Sql::OrderEntityPtr::create();

        // 赋值返回数据
        reponse = HttpServerResponsePtr::create();

        bool result = false;

        // 操作类型operate, 1:出库 2:修改订单 3:作废订单
        if (1 == operate)
        {
            orderEntity->SetReceiptNo(orderNo);
            result = dml.FindById(orderEntity, sqlError);
            if(!result)
            {
                LOG_ERROR("查询备货清单Sql执行失败: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "select stock order failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("查询备货清单Sql执行失败: %1").arg(sqlError);
                reponse->traceId = header.traceId;
                reponse->data["uniqueId"] = uniqueId;
                return;
            }

            QMap<QString, Sql::ConfigWareEntityPtr> wareEntityMap;
            QJsonArray tradeErrorWareArr;

            for(auto item : Base::JsonUtil::ToJsonArray(orderEntity->GetWares()))
            {
                QJsonObject wareObj = item.toObject();
                auto wareCode = wareObj["code"].toString();
                auto warName = wareObj["name"].toString();
                int saleCount = wareObj["count"].toInt();
                Sql::ConfigWareEntityPtr wareEntity = nullptr;
                int wareStock = 0;
                if(wareEntityMap.contains(wareCode))
                {
                    wareEntity = wareEntityMap[wareCode];
                    wareStock = wareEntity->GetStock();
                    if(wareStock < saleCount)
                    {
                        LOG_ERROR("商品：{} {}同时存在卖品和赠品，库存不足", wareCode.toStdString(), warName.toStdString());
                        reponse->code = HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR;
                        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR;
                        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR];
                        reponse->displayMsg = QStringLiteral("商品：%1同时存在卖品和赠品，库存不足").arg(warName);
                        reponse->traceId = header.traceId;
                        reponse->data["uniqueId"] = uniqueId;
                        return;
                    }
                }
                else
                {
                    wareEntity = Sql::ConfigWareEntityPtr::create();
                    wareEntity->SetCode(wareCode);
                    bool result = wareDml.FindById(wareEntity, sqlError);
                    if(!result)
                    {
                        LOG_ERROR("查询商品：{} {}Sql执行失败: {}", wareCode.toStdString(), warName.toStdString(), sqlError.toStdString());
                        reponse->code = HTTP_SERVER_SQL_ERROR;
                        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                        reponse->msg = "update config wares stock failed sql error:" + sqlError;
                        reponse->displayMsg = QStringLiteral("查询商品：%1Sql执行失败").arg(warName);
                        reponse->traceId = header.traceId;
                        reponse->data["uniqueId"] = uniqueId;
                        return;
                    }

                    wareStock = wareEntity->GetStock();
                    if(wareStock < saleCount)
                    {
                        LOG_ERROR("{} {}商品库存补不足", wareCode.toStdString(), warName.toStdString());
                        reponse->code = HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR;
                        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR;
                        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR];
                        reponse->displayMsg = QStringLiteral("商品: %1库存补不足").arg(warName);
                        reponse->traceId = header.traceId;
                        reponse->data["uniqueId"] = uniqueId;
                        return;
                    }

                    wareEntity->SetStock(wareStock - saleCount);
                    wareEntityMap[wareCode] = wareEntity;
                }

            }

            Sql::ConfigWareEntityPtrList entities = wareEntityMap.values();
            if(!wareDml.UpdateList(entities, sqlError))
            {
                LOG_ERROR("更新商品列表库存Sql执行失败: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "update config wares stock failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("商品库存扣减Sql执行失败: %1").arg(sqlError);
                reponse->traceId = header.traceId;
                return;
            }

            // 订单入库，插入新订单到正式库
            auto bussinessDb = Base::GetService<Extra::DatabaseManager>()->GetBusinessDatabase(orderEntity->GetTime().date());
            Sql::DmlBase<Sql::OrderEntity> bussinessDml;
            bussinessDml.SetDatabase(bussinessDb);
            if(!bussinessDml.Add(orderEntity, sqlError))
            {
                LOG_ERROR("插入新订单Sql执行失败: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "insert order failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("订单入库Sql执行失败: %1").arg(sqlError);
                reponse->traceId = header.traceId;
                return;
            }

            // 删除临时库订单
            result = dml.DeleteById(orderEntity, sqlError);
            if(!result)
            {
                LOG_ERROR("备货订单删除，Sql执行失败: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "select stock order failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("备货订单删除，Sql执行失败: %1").arg(sqlError);
                reponse->traceId = header.traceId;
                reponse->data["uniqueId"] = uniqueId;
                return;
            }
        }
        else if (2 == operate)
        {
            orderEntity->SetReceiptNo(orderNo);
            result = dml.FindById(orderEntity, sqlError);
            if(!result)
            {
                LOG_ERROR("查询备货清单Sql执行失败: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "select stock order failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("查询备货清单Sql执行失败: %1").arg(sqlError);
                reponse->traceId = header.traceId;
                reponse->data["uniqueId"] = uniqueId;
                return;
            }
            
            auto order = Core::ConvertUtil::FromEntity(orderEntity);
            order->orderAmount = stockOrderDetailObj["orderAmount"].toVariant().toLongLong();
            order->totalPromotionAmount = stockOrderDetailObj["totalPromotionAmount"].toVariant().toLongLong();
            for(auto item : stockOrderDetailObj["wares"].toArray())
            {
                QJsonObject wareObj = item.toObject();
                for(auto& wareItem : order->wareItems)
                {
                    if(wareItem.code == wareObj["code"].toString())
                    {
                        wareItem.count = wareObj["count"].toInt();
                        wareItem.amount = wareObj["amount"].toVariant().toLongLong();
                        break;
                    }
                }
            }

            orderEntity = Core::ConvertUtil::ToEntity(order);
            result = dml.UpdateById(orderEntity, QStringList(), sqlError);
            if(!result)
            {
                LOG_ERROR("更新备货订单Sql执行失败: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "update stock order failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("更新备货订单Sql执行失败: %1").arg(sqlError);
                reponse->traceId = header.traceId;
                reponse->data["uniqueId"] = uniqueId;
                return;
            }
        }
        else if(3 == operate)
        {
            orderEntity->SetReceiptNo(orderNo);
            result = dml.DeleteById(orderEntity, sqlError);
            if(!result)
            {
                LOG_ERROR("备货订单删除，Sql执行失败: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "select stock order failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("备货订单删除，Sql执行失败: %1").arg(sqlError);
                reponse->traceId = header.traceId;
                reponse->data["uniqueId"] = uniqueId;
                return;
            }

        }
        else
        {
            reponse->code = HTTP_SERVER_REQUIRD_PARAM_ERROR;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_REQUIRD_PARAM_ERROR;
            reponse->msg = "operate is invalid";
            reponse->displayMsg = QStringLiteral("操作类型operate无效");
            reponse->traceId = header.traceId;
            reponse->data["uniqueId"] = uniqueId;
            return;
        }

        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
    }
}