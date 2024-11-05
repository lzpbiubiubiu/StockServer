#include <QDirIterator>
#include "GarbageManager.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleBase/Common/System.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/DatabaseManager.h"

namespace Extras
{
    // 默认每天3点清理一次
    const int LOOP_CLEAN_HOUR = 3;

    // 保留多少天的数据
    const int MAX_RETENTION_DAY = 60;

    GarbageManager::GarbageManager(QObject* parent)
        : Base::AsyncService(parent)
    {}

    GarbageManager::~GarbageManager() {}

    void GarbageManager::Clean(GarbageFiles files)
    {
        CHECK_AND_SET_FROZEN_FLAG();
        QMetaObject::invokeMethod(this, "onClean", Q_ARG(GarbageFiles, files));
    }

    bool GarbageManager::OnStart()
    {
        m_checkTimer = new QTimer(this);
        connect(m_checkTimer, &QTimer::timeout, this, &GarbageManager::onCheckTimout);
        qint64 interval = CalcTimeDelta(LOOP_CLEAN_HOUR);
        m_checkTimer->start(interval);
        return true;
    }

    void GarbageManager::OnStop()
    {
        m_checkTimer->disconnect();
        m_checkTimer->stop();
    }

    void GarbageManager::onClean(GarbageFiles files)
    {
        // 收集垃圾文件
        QList<QString> garbageFiles;
        if(files.testFlag(GarbageFile::GF_SQLITE_FILE))
        {
            // 拉通检查数据目录下所有符合条件的 *.db，*.db-journal文件
            QDirIterator it(Base::PathUtil::GetDatabaseDir(),
                QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
            while(it.hasNext())
            {
                it.next();
                QString fileName = it.fileInfo().fileName();
                if(fileName.startsWith("Bussiness") && (fileName.endsWith(".db") || fileName.endsWith(".db-journal")))
                {
                    QString datestr = fileName.remove("Bussiness").remove(".db-journal").remove(".db");
                    QDate date = QDate::fromString(datestr, "yyyy-MM-dd");

                    // 清理MAX_RETENTION_DAY天以前的文件
                    if(date.daysTo(Base::System::GetCurrentDateTime().date()) >= MAX_RETENTION_DAY)
                        garbageFiles << it.fileInfo().absoluteFilePath();
                }
            }
        }

        // 删除垃圾文件
        for(auto& i : garbageFiles)
            QFile::remove(i);

        RESET_FROZEN_FLAG();
    }

    void GarbageManager::onCheckTimout()
    {
        m_checkTimer->stop();
        Clean(GarbageFile::GF_SQLITE_FILE);
        qint64 interval = CalcTimeDelta(LOOP_CLEAN_HOUR);
        m_checkTimer->start(interval);
    }

    qint64 GarbageManager::CalcTimeDelta(int hour)
    {
        QDateTime currentTime = QDateTime::currentDateTime();
        QDateTime targetTime = QDateTime(currentTime.date(), QTime(hour, 0));

        // 如果当前时间超过了目标时间，则计算出第二天的目标时间
        if(currentTime >= targetTime)
        {
            targetTime = targetTime.addDays(1);
        }

        // 计算时间间隔
        return currentTime.msecsTo(targetTime);
    }
}