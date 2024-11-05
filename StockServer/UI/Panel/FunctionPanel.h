// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include <QJsonArray>
#include <QObject>
#include "ModuleData/Core/GlobalData.h"

namespace UI
{
    /** 面板  */
    class FunctionPanel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QJsonArray functionModel READ GetFunctionModel NOTIFY signalFunctionModelChanged)

    public:

        enum FuncType
        {
            /** 导入 */
            IMPORT_DATA = 0,

            /** 导出 */
            EXPORT_DATA = 1
        };
        Q_ENUM(FuncType)

        FunctionPanel(QObject* parent);
        ~FunctionPanel();

        QJsonArray GetFunctionModel();

        /** 初始化功能按钮列表 */
        void InitFunctionList(const QStringList& payways = QStringList());

    Q_SIGNALS:
        void signalFunctionModelChanged();

    private:
        // 功能列表
        QJsonArray m_funcs;
    };
}