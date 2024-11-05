#pragma once

#include <mutex>

template <typename T>
class Singleton
{
public:
    static T* Get()
    {
        std::call_once(s_oc, [&](){ s_instance = new T(); s_gc.DoNothing(); });
        return s_instance;
    };

    static void Destroy()
    {
        if(s_instance != nullptr)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    };

    static bool HasInstance()
    {
        return s_instance != nullptr;
    };

protected:
    Singleton() {}
    ~Singleton() {}

private:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    static T* s_instance;
    static std::once_flag s_oc;

    struct GarbageCollector
    {
        using FunctionPointer = void(*)(void);
        GarbageCollector(FunctionPointer function) : destructor(function) {}
        ~GarbageCollector() { destructor(); }

        // 防止编译器优化
        inline void DoNothing() {}
        FunctionPointer destructor;
    };

    static GarbageCollector s_gc;
};
template<typename T> T* Singleton<T>::s_instance = nullptr;
template<typename T> std::once_flag Singleton<T>::s_oc;
template<typename T> typename Singleton<T>::GarbageCollector Singleton<T>::s_gc(Singleton<T>::Destroy);

#define TO_BE_SINGLETON(T)\
public:\
    friend class Singleton<T>;\
    static T* Get() { return Singleton<T>::Get(); }\
    static bool HasInstance() { return Singleton<T>::HasInstance(); }\
    static void Release() { Singleton<T>::Destroy(); }
