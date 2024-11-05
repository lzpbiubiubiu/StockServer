#include <QMutexLocker>
#include "ActionSequence.h"

namespace Base
{
    ActionSequence::ActionSequence(QObject* parent)
        : QObject(parent)
    {}

    ActionSequence::~ActionSequence()
    {}

    void ActionSequence::Add(const Action& action)
    {
        QMutexLocker locker(&m_mtx);
        m_actions << action;
    }

    void ActionSequence::Clear()
    {
        QMutexLocker locker(&m_mtx);
        m_actions.clear();
    }

    void ActionSequence::Next()
    {
        m_mtx.lock();
        if(m_cursor >= m_actions.size())
        {
            m_mtx.unlock();
            emit signalAllActionFinished();
            return;
        }

        Action action = m_actions[m_cursor];
        m_cursor++;
        m_mtx.unlock();
        action.callback();
    }

    void ActionSequence::SeekTo(int id)
    {
        QMutexLocker locker(&m_mtx);
        int index = -1;
        for(int i = 0; i < m_actions.size(); ++i)
        {
            if(m_actions[i].id == id)
            {
                index = i;
                break;
            }
        }

        Q_ASSERT(index != -1);
        if(index != -1)
            m_cursor = index;
    }

    void ActionSequence::SeekToBegin()
    {
        QMutexLocker locker(&m_mtx);
        if(m_actions.isEmpty())
            return;
        m_cursor = 0;
    }

    int ActionSequence::GetSize()
    {
        QMutexLocker locker(&m_mtx);
        return m_actions.size();
    }
}