#include <QQmlContext>
#include <QSharedMemory>
#include <QTimer>
#include <QIcon>
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Http/Client/Client.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleBase/Common/System.h"
#include "Extras/HttpServerManager.h"
#include "Application.h"

namespace UI
{
    Application::Application(int& argc, char** argv)
        : QApplication(argc, argv)
    {
        m_qmlEngine = QSharedPointer<QQmlEngine>::create();

        connect(this, &QCoreApplication::aboutToQuit, this, &Application::OnExit);
    }

    Application::~Application()
    {
    }

    Application* Application::Get()
    {
        return qobject_cast<Application*>(instance());
    }

    MainWindow* Application::GetMainWindow()
    {
        return qobject_cast<Application*>(instance())->m_mainWindow;
    }

    QSharedPointer<QQmlEngine> Application::GetQmlEngine() const
    {
        return m_qmlEngine;
    }

    int Application::Launch()
    {
        // 互斥判断
        m_sharedMemory = new QSharedMemory("B733452A-8D8B-4D70-AAA0-0D6FAEC19A39", this); 

        // 共享内存被占用则直接返回
        if(!m_sharedMemory->create(1))
            return -1;

        Init();
        return exec();
    }

    void Application::Exit()
    {
        quit();
    }

    void Application::Init()
    {
#if defined(_WIN32) || defined(_WIN64)
        // 注册崩溃回调
        SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif
        // 添加import指令模块搜索路径
        m_qmlEngine->addImportPath("qrc:/Qml");

        // 注入Application对象
        m_qmlEngine->rootContext()->setContextProperty("app", this);

        // 设置程序图标
        setWindowIcon(QIcon(":/Resources/Images/stock_server.ico"));

        // 创建主窗口
        m_mainWindow = new MainWindow(m_qmlEngine);

        // 加载启动页面
        m_mainWindow->LoadPage("mainPage");

        // 显示主窗口
        m_mainWindow->Show();   
    }

#if defined(_WIN32) || defined(_WIN64)
    LONG WINAPI Application::ApplicationCrashHandler(EXCEPTION_POINTERS* exception)
    {
        // 安装包名称
        auto& config = Core::GlobalData::Get()->GetConfig();
        QDateTime datetime = QDateTime::fromString(config.version.date, "yyyy-MM-dd HH:mm:ss");
        QString packageName = QString("PosServerSetup-Win32-%1-%2-%3-%4")
            .arg(config.version.env)
            .arg(config.version.string)
            .arg(datetime.toString("yyyyMMdd"))
            .arg(datetime.toString("HHmmss"));

        // Dump文件名：DumpFile [安装包名称] [崩溃时间] [商家名称 商家编码] [门店名称 门店编码] [机台号] [设备ID].dmp
        QString dumpFileName = QString("DumpFile [%1] [%2] [StockServer].dmp")
            .arg(packageName)
            .arg(Base::System::GetCurrentDateTime().toString("yyyy-MM-dd HH_mm_ss"));

        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = exception;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;

        QString filePath = Base::PathUtil::GetDumpDir() + dumpFileName;
        std::wstring wlpstr = filePath.toStdWString();
        LPCWSTR lpFileName = wlpstr.c_str();
        HANDLE hFile = ::CreateFileW(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
        {
            if(!::MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &dumpInfo, NULL, NULL))
            {
                LOG_ERROR(QStringLiteral("Write dump file failed, error:%1").arg(GetLastError()).toStdString());
            }
            ::CloseHandle(hFile);
        }
        else
        {
            LOG_ERROR(QStringLiteral("create dump file failed, error:%1").arg(GetLastError()).toStdString());
        }

        // 崩溃回调
        Application::Get()->OnException();

        // 重启StockServer
        Base::System::StartStockServer();

        return EXCEPTION_EXECUTE_HANDLER;
    }
#endif

    void Application::OnExit()
    {
        m_mainWindow->ShowLoading(QStringLiteral("Exiting, please wait..."));

        Base::ServiceManager::Get()->Unload();
        QEventLoop eventloop;
        connect(Base::ServiceManager::Get(), &Base::ServiceManager::signalUnloaded, [&] { eventloop.quit(); });
        eventloop.exec();

        m_mainWindow->CloseLoading();
        m_mainWindow->SetTrayVisible(false);
        m_sharedMemory->detach();

        // 主动释放http客户端单例，避免QThreadPool在C/C++运行时析构报错
        Http::Client::Get()->Release();

        if(Core::GlobalData::Get()->GetConfig().exit.restart)
        {
            LOG_INFO(QStringLiteral("重启Stock Server...").toStdString());
            Base::System::StartStockServer();
        }
        else
        {
            LOG_INFO(QStringLiteral("退出Stock Server...").toStdString());
        }
    }

    void Application::OnException()
    {
        LOG_ERROR("===========>>Application has an exception<<===========");
    }
}