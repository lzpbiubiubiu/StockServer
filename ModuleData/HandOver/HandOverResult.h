#pragma once

#include <QString>
#include <QJSonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSharedPointer>

namespace Core {
		/**
		* 点算面额信息
		*/
		class CashMoneyType {

		public:
			/** 币种类型 (1:纸币(B)  2:硬币(C)) */
			int currencyType;
			/** 面额 */
			long long faceValue = 0LL;
			/** 数量 */
			int count = 0;
		};

		/**
		* 点算金额详情
		*/
		class CountCashDetail {

		public:
			/** 支付方式ID */
			QString paymentMethodId;
			/** 支付方式名称 */
			QString paymentMethodName;
			/** 点算总金额 */
			long long totalAmount = 0LL;
			/** 点算面额明细 */
			QList<QSharedPointer<CashMoneyType>> cashMoneyTypes;
		};

		/**
		* 支付详情
		*/
		class PaymentDetail {

		public:
			/** 支付方式ID */
			QString paymentMethodId;
			/** 支付方式名称 */
			QString paymentMethodName;
			/** 支付金额 */
			long long paymentAmount = 0LL;
		};

		/**
	 	* 支付方式汇总
		*/
	   class PaymentSummary {
	   public:
		   /** 支付方式ID */
		   QString paymentMethodId;
		   /** 支付方式名称 */
		   QString paymentMethodName;
		   /** 支付金额 */
		   long long totalAmount = 0LL;
		   /** 支付次数 */
		   long long totalCount = 0LL;

		   QJsonObject toQJsonObject() const {
			   QJsonObject jsonObject;

			   jsonObject["paymentMethodId"] = paymentMethodId;
			   jsonObject["paymentMethodName"] = paymentMethodName;
			   jsonObject["totalAmount"] = totalAmount;
			   jsonObject["totalCount"] = totalCount;

			   return jsonObject;
		   }
	   };

	   /**
		* 点算金额信息
		*/
	   class CashMoneyInfo {

	   public:
		   /** 币种类型 (1:纸币  2:硬币) */
		   int currencyType;
		   /** 面额 */
		   long long faceValue = 0LL;
		   /** 数量 */
		   int count = 0;
		   /** 总金额 */
		   long long totalAmount = 0LL;

		   QJsonObject toQJsonObject() const {
			   QJsonObject jsonObject;

			   jsonObject["currencyType"] = currencyType;
			   jsonObject["faceValue"] = faceValue;
			   jsonObject["count"] = count;
			   jsonObject["totalAmount"] = totalAmount;

			   return jsonObject;
		   }
	   };

	   class CountCashInfo {

	   public:
		   /** 支付方式ID */
		   QString paymentMethodId;
		   /** 支付方式名称 */
		   QString paymentMethodName;
		   /** 点算总金额 */
		   long long totalAmount = 0LL;
		   /** 点算金额明细列表 */
		   QList<QSharedPointer<CashMoneyInfo>> cashMoneyInfos;

		   QJsonObject toQJsonObject() const {
			   QJsonObject jsonObject;

			   jsonObject["paymentMethodId"] = paymentMethodId;
			   jsonObject["paymentMethodName"] = paymentMethodName;
			   jsonObject["totalAmount"] = totalAmount;
			   QJsonArray cashMoneyInfosArray;
			   for(const auto& item : cashMoneyInfos) {
				   if(item) {
					   cashMoneyInfosArray.append(item->toQJsonObject());
				   }
			   }
			   jsonObject["cashMoneyInfos"] = cashMoneyInfosArray;

			   return jsonObject;
		   }
	   };

	   /**
		* 取大钞信息
		*/
	   class GetBigMoneyInfo {

	   public:
		   /** 取大钞次数 */
		   int count = 0;
		   /** 取大钞总金额 */
		   long long totalAmount = 0LL;
		   /** 取大钞支付方式汇总列表 */
		   QList<QSharedPointer<PaymentSummary>> paymentSummarys;

		   QJsonObject toQJsonObject() const {
			   QJsonObject jsonObject;

			   jsonObject["count"] = count;
			   jsonObject["totalAmount"] = totalAmount;
			   QJsonArray paymentSummarysArray;
			   for(const auto& item : paymentSummarys) {
				   if(item) {
					   paymentSummarysArray.append(item->toQJsonObject());
				   }
			   }
			   jsonObject["paymentSummarys"] = paymentSummarysArray;

			   return jsonObject;
		   }
	   };

	   /**
		 * POS机台SI号信息
		 */
	   class PosSiNoInfo {

	   public:
		   //机台号
		   QString posNo;
		   /** 交班内机台第一个销售单SI号 */
		   QString handOverBeginSiNo;
		   /** 交班内机台最后一个销售单SI号 */
		   QString handOverEndSiNo;
		   /** 销售单最早完成时间 */
		   long long saleOrderMinFinished = LLONG_MAX;
		   /** 销售单最晚完成时间 */
		   long long saleOrderMaxFinished = LLONG_MIN;

		   QJsonObject toQJsonObject() const {
			   QJsonObject jsonObject;

			   jsonObject["posNo"] = posNo;
			   jsonObject["handOverBeginSiNo"] = handOverBeginSiNo;
			   jsonObject["handOverEndSiNo"] = handOverEndSiNo;
			   jsonObject["saleOrderMinFinished"] = saleOrderMinFinished;
			   jsonObject["saleOrderMaxFinished"] = saleOrderMaxFinished;

			   return jsonObject;
		   }
	   };

		/**
		 * 交班Result
		 */
		class HandOverResult {

		public:
			/** 门店ID */
			QString orgId;
			/** POS银台号 */
			QString posNo;
			/** 收营员编号 */
			QString cashierNo;
			/** 授权人ID */
			QString authorizerId;
			/** 交班时间 */
			long long handTime = 0LL;
			/** 交班统计开始时间 */
			long long handStartTime = 0LL;
			/** 交班统计结束时间 */
			long long handEndTime = 0LL;
			/** 交班内机台第一个销售单SI号 */
			QString handBeginSiNo;
			/** 交班内机台最后一个销售单SI号 */
			QString handEndSiNo;
			/** 销售单数量 */
			int saleCount = 0LL;
			/** 销售单应收总金额 */
			long long saleTotalAmount = 0LL;
			/** 售后单数量 */
			int afterSaleCount = 0LL;
			/** 售后单应对总金额 */
			long long afterSaleTotalAmount = 0LL;
			/** Net Sales 和 Net Received */
			long long netSalesAmount = 0LL;
			/** 收费单总金额 */
			long long chargeTotalAmount = 0LL;
			/** 手动开钱箱次数 */
			int openBoxCount = 0LL;
			/** 固定备用金金额 */
			long long fixedReadyAmount = 0LL;
			/** 当前备用金金额 */
			long long readyAmount = 0LL;
			/** 收银员录入现金实缴金额 */
			long long cashierCashAmount = 0LL;
			/** Cash Received */
			long long cashReceivedAmount = 0LL;
			/** 现金实缴差异金额 */
			long long cashDiffAmount = 0LL;
			/** 系统现金应缴金额 */
			long long systemCashAmount = 0LL;
			/** 点算现金总金额 */
			long long cashTotalAmount = 0LL;
			/** 班次内COD未回款金额 */
			long long classCodNoCollectAmount = 0LL;
			/** 非班次内COD回款金额 */
			long long noClassCodCollectAmount = 0LL;
			/** 所有支付方式总金额 */
			long long allPaymentTotalAmount = 0LL;
			/** 销售单支付方式汇总列表 ---所有销售去掉COD未回款单,加上非班次COD回款单*/
			QList<QSharedPointer<PaymentSummary>> salePaymentSummarys;
			/** 退款单支付方式列表 */
			QList<QSharedPointer<PaymentSummary>> afterPaymentSummarys;
			/** 点算金额明细列表 */
			QList<QSharedPointer<CountCashInfo>> countCashInfos;
			/** 取大钞信息 */
			QSharedPointer<GetBigMoneyInfo> getBigMoneyInfo;

			QJsonObject toQJsonObject() const {
				QJsonObject jsonObject;

				jsonObject["orgId"] = orgId;
				jsonObject["posNo"] = posNo;
				jsonObject["cashierNo"] = cashierNo;
				jsonObject["authorizerId"] = authorizerId;
				jsonObject["handTime"] = handTime;
				jsonObject["handStartTime"] = handStartTime;
				jsonObject["handEndTime"] = handEndTime;
				jsonObject["handBeginSiNo"] = handBeginSiNo;
				jsonObject["handEndSiNo"] = handEndSiNo;
				jsonObject["saleCount"] = saleCount;
				jsonObject["saleTotalAmount"] = saleTotalAmount;
				jsonObject["afterSaleCount"] = afterSaleCount;
				jsonObject["afterSaleTotalAmount"] = afterSaleTotalAmount;
				jsonObject["netSalesAmount"] = netSalesAmount;
				jsonObject["chargeTotalAmount"] = chargeTotalAmount;
				jsonObject["openBoxCount"] = openBoxCount;
				jsonObject["fixedReadyAmount"] = fixedReadyAmount;
				jsonObject["readyAmount"] = readyAmount;
				jsonObject["cashTotalAmount"] = cashTotalAmount;
				jsonObject["cashierCashAmount"] = cashierCashAmount;
				jsonObject["cashReceivedAmount"] = cashReceivedAmount;
				jsonObject["cashDiffAmount"] = cashDiffAmount;
				jsonObject["systemCashAmount"] = systemCashAmount;
				jsonObject["cashTotalAmount"] = cashTotalAmount;
				jsonObject["classCodNoCollectAmount"] = classCodNoCollectAmount;
				jsonObject["noClassCodCollectAmount"] = noClassCodCollectAmount;
				jsonObject["allPaymentTotalAmount"] = allPaymentTotalAmount;
				if(getBigMoneyInfo != nullptr) {
					jsonObject["getBigMoneyInfo"] = getBigMoneyInfo->toQJsonObject();
				}
				
				if(!salePaymentSummarys.isEmpty()) {
					QJsonArray salePaymentSummaryArray;
					for(const auto& item : salePaymentSummarys) {
						if(item) {
							salePaymentSummaryArray.append(item->toQJsonObject());
						}
					}
					jsonObject["salePaymentSummarys"] = salePaymentSummaryArray;
				}

				if(!afterPaymentSummarys.isEmpty()) {
					QJsonArray afterSalePaymentSummaryArray;
					for(const auto& item : afterPaymentSummarys) {
						if(item) {
							afterSalePaymentSummaryArray.append(item->toQJsonObject());
						}
					}
					jsonObject["afterPaymentSummarys"] = afterSalePaymentSummaryArray;
				}

				if(!countCashInfos.isEmpty()) {
					QJsonArray countCashInfosArray;
					for(const auto& item : countCashInfos) {
						if(item) {
							countCashInfosArray.append(item->toQJsonObject());
						}
					}
					jsonObject["countCashInfos"] = countCashInfosArray;
				}

				return jsonObject;
			}
		};
}

using HandOverResultPtr = QSharedPointer<Core::HandOverResult>;