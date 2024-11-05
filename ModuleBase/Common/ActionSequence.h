// Copyright (c) 2023 Dmall Pos Team

#pragma once

#include <QList>
#include <QMutex>
#include <functional>
#include "ModuleBase/ModuleBaseApi.h"

namespace Base
{
    /** 动作队列，辅助异步串行动作的调度 */
    class MODULE_BASE_API ActionSequence : public QObject
    {
        Q_OBJECT
    public:
        /** 动作回调 */
        using ActionCallback = std::function<void()>;

        /** 动作定义 */
        struct Action
        {
            int id = 0;
            ActionCallback callback;
        };

        ActionSequence(QObject* parent = nullptr);
        virtual ~ActionSequence();

        /** 动作新增 */
        void Add(const Action& action);

        /** 动作清空 */
        void Clear();

        /** 动作执行 */
        void Next();

        /** 动作重定向到指定指定位置 */
        void SeekTo(int id);

        /** 动作重定向到序列开始位置 */
        void SeekToBegin();

        /** 动作数量 */
        int GetSize();

    Q_SIGNALS:
        /** 全部动作信号 */
        void signalAllActionFinished();

    private:
        // 互斥锁
        QMutex m_mtx;

        // 全部Action
        QList<Action> m_actions;

        // 当前Action
        int m_cursor = 0;
    };
}