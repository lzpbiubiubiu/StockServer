#include <QFontDatabase>
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleBase/Common/System.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "Extras/HttpServerManager.h"
#include "Extras/GarbageManager.h"
#include "Extras/ConfigManager.h"
#include "Application.h"

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    QCoreApplication::setOrganizationName("Some organization");

    // 是否开启QtQuick 2D Renderer
    if(!Base::System::IsHardwareRenderEnabled())
        qputenv("QMLSCENE_DEVICE", "softwarecontext");

    // 高分屏缩放
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    double factor = Base::System::GetScreenScaleFactor();
    if(factor > 1.0)
        qputenv("QT_SCALE_FACTOR", QString::number(factor).toUtf8());

    UI::Application app(argc, argv);

    // 日志库初始化
    QString logDir = Base::PathUtil::GetLogDir();
    Base::InitLogging(logDir);
    LOG_INFO("===========>>Stock Server start<<===========");

    // 加载外部字体
    int fontId = QFontDatabase::addApplicationFont(Base::PathUtil::GetApplicationDir() + "PlusJakartaSans-Regular.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    fontId = QFontDatabase::addApplicationFont(Base::PathUtil::GetApplicationDir() + "PlusJakartaSans-Bold.ttf");
    families << QFontDatabase::applicationFontFamilies(fontId);

    LOG_INFO("External Fonts: {}", families.join(",").toStdString());
    LOG_INFO("QMLSCENE_DEVICE: {}", qgetenv("QMLSCENE_DEVICE").toStdString());
    LOG_INFO("QT_SCALE_FACTOR: {}", qgetenv("QT_SCALE_FACTOR").toStdString());
    //LOG_INFO("SqlDrivers Support: {}", QSqlDatabase::drivers().join(",").toStdString());

    /**
    * 核心服务定义
    * 字段定义顺序将决定初始化回调函数OnStart()的执行顺序
    * 如果服务在初始化期间存在依赖调用，则必须按照依赖关系定义字段顺序
    */

    Base::ServiceManager::Get()->AddService<Extra::DatabaseManager>();
    Base::ServiceManager::Get()->AddService<Extra::ConfigManager>();
    Base::ServiceManager::Get()->AddService<Extra::HttpServerManager>();
    Base::ServiceManager::Get()->AddService<Extras::GarbageManager>();
    

    // 程序启动
    int code = app.Launch();

    // 日志库关闭
    LOG_INFO("===========>>Stock Server stop<<===========");
    Base::ShutdownLogging();

    return code;
}