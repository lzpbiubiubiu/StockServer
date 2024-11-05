#include <QCoreApplication>
#include "ModuleData/Core/GlobalStructData.h"
#include "ModuleData.h"

// 模块初始化
static void InitModule(){

    qRegisterMetaType<Core::ConfigWare>("ConfigWare");
    qRegisterMetaType<Core::ConfigWarePtr>("ConfigWarePtr");
    qRegisterMetaType<Core::ConfigWarePtrList>("ConfigWarePtrList");

    qRegisterMetaType<Core::Order>("Order");
    qRegisterMetaType<Core::OrderPtr>("OrderPtr");
    qRegisterMetaType<Core::OrderPtrList>("OrderPtrList");
}
Q_COREAPP_STARTUP_FUNCTION(InitModule)