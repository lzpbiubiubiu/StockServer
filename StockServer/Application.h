#pragma once
#include <QApplication>
#include <QQmlEngine>
#include <QSharedMemory>
#include "UI/MainWindow.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <ImageHlp.h>
#pragma comment(lib, "DbgHelp.lib")
#endif

namespace UI
{
    /** 提供应用程序运行时数据访问、页面管理等功能 */
    class Application : public QApplication
    {
        Q_OBJECT

    public:
        Application(int& argc, char** argv);
        ~Application();

        /** 获取唯一实例 */
        static Application* Get();

        /** 获取唯一主窗口 */
        static MainWindow* GetMainWindow();

        /** 获取当前QML解释器 */
        QSharedPointer<QQmlEngine> GetQmlEngine() const;

        /** 启动程序 */
        int Launch();

        /** 结束程序 */
        void Exit();

    protected:
        /** 初始化 */
        void Init();

#if defined(_WIN32) || defined(_WIN64)
        /** 崩溃回调处理 */
        static LONG WINAPI ApplicationCrashHandler(EXCEPTION_POINTERS* exception);
#endif

    private Q_SLOTS:
        void OnExit();
        void OnException();

    private:
        // QML解释器
        QSharedPointer<QQmlEngine> m_qmlEngine = nullptr;

        // 主窗口
        MainWindow* m_mainWindow = nullptr;

        // 程序互斥共享内存
        QSharedMemory* m_sharedMemory = nullptr;
    };
}