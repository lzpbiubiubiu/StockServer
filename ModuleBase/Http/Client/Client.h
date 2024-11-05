#pragma once

/**
* @file         Client.h
* @brief        Http客户端封装
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QMutex>
#include <QNetworkAccessManager>
#include <QSharedPointer>
#include <QThreadPool>

#include "ModuleBase/Common/Singleton.h"
#include "ModuleBase/ModuleBaseApi.h"
#include "Request.h"
#include "Response.h"

namespace Http
{
    // Http客户端单例封装类
    class MODULE_BASE_API Client : public QObject
    {
        Q_OBJECT
        TO_BE_SINGLETON(Client)

    public:
        friend class TaskAdapter;
        virtual ~Client();

        /**
        * @brief    发起http请求
        * @warn     设置异步请求时，request需设置回调函数来接收响应，回调函数在工作线程执行，
        *           如果需要在自己的线程执行，需要注意多线程下数据安全问题，request和response都不是线程安全的
        * @param    request     请求对象
        * @param    response    响应对象
        * @param    async       是否异步
        */
        void ProcessRequest(QSharedPointer<Request> request, QSharedPointer<Response> response, bool async = false);

        /**
        * @brief    http文件下载
        * @warn     设置异步请求时，request需设置回调函数来接收响应，回调函数在工作线程执行，
        *           如果需要在自己的线程执行，需要注意多线程下数据安全问题，request和response都不是线程安全的
        * @param    request     请求对象
        * @param    response    响应对象
        * @param    async       是否异步
        */
        void Download(QSharedPointer<Request> request, QSharedPointer<Response> response, bool async = false);

        /**
        * @brief    http文件上传
        * @warn     设置异步请求时，request需设置回调函数来接收响应，回调函数在工作线程执行，
        *           如果需要在自己的线程执行，需要注意多线程下数据安全问题，request和response都不是线程安全的
        * @param    request     请求对象
        * @param    response    响应对象
        * @param    async       是否异步
        */
        void Upload(QSharedPointer<Request> request, QSharedPointer<Response> response, bool async = false);

    private:
        Client();

    private:
        // 任务执行线程池
        QThreadPool* m_threadPool = nullptr;

        // 网络访问管理器映射表，根据线程获取对应的网络访问管理器
        QMap<QThread*, QNetworkAccessManager*> m_networkAccessManagerMap;

        // 网络访问管理器映射表互斥锁
        QMutex m_mutex;
    };
}