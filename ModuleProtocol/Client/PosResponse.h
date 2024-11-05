#pragma once

/**
* @file         PosResponse.h
* @brief        Pos响应基类
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include "ModuleBase/Http/Client/Response.h"
#include "ModuleProtocol/ModuleProtocolApi.h"

namespace Http
{
    /** 交易网关HTTP响应基类 */
    class MODULE_PROTOCOL_API PosResponse : public Response
    {
        Q_OBJECT
    public:
        PosResponse(QObject* parent = nullptr);
        virtual ~PosResponse();

        /** 请求是否成功 */
        virtual bool IsSuccess() const override;

        /** 反序列化响应数据 */
        virtual void Deserialize(const QByteArray& ba) override;

        /** 错误信息相关 */
        virtual QString GetError() const override;
        virtual QString GetErrorMessage() const override;
        QString GetDisplayErrorCode() const;
        
    protected:
        /** 解析对象型JSON参数 */
        virtual void ParseJsonParam(const QJsonObject& val);

        /** 解析数组型JSON参数 */
        virtual void ParseJsonParam(const QJsonArray& val);

        /** 解析布尔型Bool参数 */
        virtual void ParseJsonParam(bool val);

        /** 解析整型Double参数*/
        virtual void ParseJsonParam(double val);

    private:
        // 网关错误码
        QString m_resultCode;

        // 网关错误描述
        QString m_resultMessage;

        // 界面要显示的错误码
        QString m_resultDisplayCode;
    };
}

//Q_DECLARE_METATYPE(Http::PosResponse)
