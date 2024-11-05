#pragma once

#include <QTimer>
#include "ModuleBase/Service/AsyncService.h"
#include "ModuleBase/Service/ServiceManager.h"

namespace Extras
{
    /** 垃圾文件清理服务 */
    class GarbageManager : public Base::AsyncService
    {
        Q_OBJECT
        SUPPORT_FROZEN_FLAG()
    public:
        GarbageManager(QObject* parent = nullptr);
        ~GarbageManager();

        /** 垃圾文件类型 */
        enum GarbageFile
        {
            /** 数据库文件 */
            GF_SQLITE_FILE = 1,
        };
        Q_ENUM(GarbageFile)
        Q_DECLARE_FLAGS(GarbageFiles, GarbageFile)

        /** 立即清理 */
        void Clean(GarbageFiles files);

    protected:
        virtual bool OnStart() override;
        virtual void OnStop() override;

        Q_INVOKABLE void onClean(GarbageFiles files);

    protected Q_SLOTS:
        void onCheckTimout();

    private:
        /** 计算当前时间到目标时间的秒差值 */
        qint64 CalcTimeDelta(int hour);

    private:
        // 检查定时器
        QTimer* m_checkTimer = nullptr;
    };
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Extras::GarbageManager::GarbageFiles)