#include <QJsonObject>
#include "FunctionPanel.h"
#include "ModuleBase/Log/Log.h"

namespace UI
{
    FunctionPanel::FunctionPanel(QObject* parent)
        : QObject(parent)
    {
        InitFunctionList();
    }

    FunctionPanel::~FunctionPanel()
    {}

    QJsonArray FunctionPanel::GetFunctionModel()
    {
        return m_funcs;
    }

    void FunctionPanel::InitFunctionList(const QStringList& payways)
    {
        m_funcs = QJsonArray();

        QJsonObject obj;
        obj["funcName"] = "Import Offline Data";

        obj["funcType"] = FuncType::IMPORT_DATA;
        m_funcs.append(obj);

        obj["funcName"] = "Export Transaction Data";

        obj["funcType"] = FuncType::EXPORT_DATA;
        m_funcs.append(obj);

        emit signalFunctionModelChanged();

    }
}