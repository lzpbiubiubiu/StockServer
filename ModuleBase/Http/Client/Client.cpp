#include <QSemaphore>

#include "Client.h"
#include "TaskAdapter.h"

// 线程池默认线程数
const int THREAD_POOL_DEFAULT_COUNT = 4;

namespace Http
{
    Client::Client()
    {
        m_threadPool = new QThreadPool(this);

        // 优先使用CPU核心数作为线程数
        int count = QThread::idealThreadCount();
        if(count <= 0)
            count = THREAD_POOL_DEFAULT_COUNT;

        m_threadPool->setMaxThreadCount(count);

        // 设置线程池中线程永不退出
        m_threadPool->setExpiryTimeout(-1);
    }

    Client::~Client()
    {
        m_threadPool->waitForDone();
    }

    void Client::ProcessRequest(QSharedPointer<Request> request, QSharedPointer<Response> response, bool async/* = false*/)
    {
        TaskAdapter* taskAdapter = new TaskAdapter(TaskType::NORMAL, request, response);

        auto RunTask = [taskAdapter, this](){
            taskAdapter->setAutoDelete(true);
            m_threadPool->start(taskAdapter);
        };

        if(async)
        {
            RunTask();
        }
        else
        {
            QSemaphore semaphore;
            QObject::connect(taskAdapter, &TaskAdapter::signalFinished, [&semaphore](){ semaphore.release(); });
            RunTask();
            semaphore.acquire();
        }
    }

    void Client::Download(QSharedPointer<Request> request, QSharedPointer<Response> response, bool async/* = false*/)
    {
        TaskAdapter* taskAdapter = new TaskAdapter(TaskType::DOWNLOAD, request, response);

        auto RunTask = [taskAdapter, this](){
            taskAdapter->setAutoDelete(true);
            m_threadPool->start(taskAdapter);
        };

        if(async)
        {
            RunTask();
        }
        else
        {
            QSemaphore semaphore;
            QObject::connect(taskAdapter, &TaskAdapter::signalFinished, [&semaphore]() { semaphore.release(); });
            RunTask();
            semaphore.acquire();
        }
    }

    void Client::Upload(QSharedPointer<Request> request, QSharedPointer<Response> response, bool async/* = false*/)
    {
        TaskAdapter* taskAdapter = new TaskAdapter(TaskType::UPLOAD, request, response);

        auto RunTask = [taskAdapter, this]() {
            taskAdapter->setAutoDelete(true);
            m_threadPool->start(taskAdapter);
            };

        if(async)
        {
            RunTask();
        }
        else
        {
            QSemaphore semaphore;
            QObject::connect(taskAdapter, &TaskAdapter::signalFinished, [&semaphore]() { semaphore.release(); });
            RunTask();
            semaphore.acquire();
        }
    }
}