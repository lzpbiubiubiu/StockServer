#include <QEventLoop>

#include "Client.h"
#include "DownloadTask.h"
#include "TaskAdapter.h"
#include "NormalTask.h"
#include "UploadTask.h"

namespace Http
{
    TaskAdapter::TaskAdapter(TaskType type, QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent/* = nullptr*/)
        : QObject(parent)
        , m_type(type)
        , m_request(request)
        , m_response(response)
    {}

    void TaskAdapter::run()
    {
        QSharedPointer<Task> task;
        if(m_type == NORMAL)
            task = QSharedPointer<Task>(new NormalTask(m_request, m_response));
        else if(m_type == DOWNLOAD)
            task = QSharedPointer<Task>(new DownloadTask(m_request, m_response));
        else if(m_type == UPLOAD)
            task = QSharedPointer<Task>(new UploadTask(m_request, m_response));
        else
            return;

        auto eventloop = QSharedPointer<QEventLoop>::create();

        // 设置线程的网络访问管理器，并传递给task
        {
            QMutexLocker locker(&Client::Get()->m_mutex);
            if(!Client::Get()->m_networkAccessManagerMap.contains(QThread::currentThread()))
            {
                QNetworkAccessManager* networkAccessManager = new QNetworkAccessManager;
                task->SetNetworkAccessManager(networkAccessManager);
                Client::Get()->m_networkAccessManagerMap[QThread::currentThread()] = networkAccessManager;
            }
            else
            {
                task->SetNetworkAccessManager(Client::Get()->m_networkAccessManagerMap[QThread::currentThread()]);
            }
        }

        QObject::connect(task.data(), &Task::signalFinished, [eventloop, this](HttpResultPtr result) {
            eventloop->quit();
            emit signalFinished(result);
            });

        task->Run();

        eventloop->exec();
    }
}