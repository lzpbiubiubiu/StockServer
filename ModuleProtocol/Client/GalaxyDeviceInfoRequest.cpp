#include <QJsonObject>
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleBase/Common/System.h"
#include "GalaxyDeviceInfoRequest.h"

namespace Http
{
    GalaxyDeviceInfoRequest::GalaxyDeviceInfoRequest(QObject *parent)
        : PosRequest(parent)
    {
        QString mac = Base::System::GetMAC();
        SetDeviceCode(mac);
        SetExternalCode(mac);
    }

    GalaxyDeviceInfoRequest::~GalaxyDeviceInfoRequest()
    {}

    void GalaxyDeviceInfoRequest::SetExternalCode(const QString& code)
    {
        m_externalCode = code;
    }

    void GalaxyDeviceInfoRequest::MakeJsonParam(QJsonObject& val)
    {
        ASSIGN_FROM_STRING(val, "externalCode", m_externalCode);
        ASSIGN_FROM_STRING(val, "productKey", Core::GlobalData::Get()->GetProductKey());
    }
}