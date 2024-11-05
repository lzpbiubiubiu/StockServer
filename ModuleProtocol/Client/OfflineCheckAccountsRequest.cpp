#include <QJsonObject>
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleBase/Common/System.h"
#include "OfflineCheckAccountsRequest.h"

namespace Http
{
    OfflineCheckAccountsRequest::OfflineCheckAccountsRequest(QObject *parent)
        : PosRequest(parent)
    {
        QString mac = Base::System::GetMAC();
        SetDeviceCode(mac);
        SetExternalCode(mac);
    }

    OfflineCheckAccountsRequest::~OfflineCheckAccountsRequest()
    {}

    void OfflineCheckAccountsRequest::SetExternalCode(const QString & code)
    {
        m_externalCode = code;
    }

    void OfflineCheckAccountsRequest::SetOfflineCheckInfo(const QJsonArray& info)
    {
        m_offlineCheckInfo = info;
    }

    void OfflineCheckAccountsRequest::MakeJsonParam(QJsonObject& val)
    {
        ASSIGN_FROM_STRING(val, "uuid", Base::MathUtil::GetUUID());
        ASSIGN_FROM_INT64_IF_GREATER_THAN_ZERO(val, "tenantId", m_venderId.toLongLong());
        ASSIGN_FROM_INT64_IF_GREATER_THAN_ZERO(val, "orgId", m_storeId.toLongLong());
        ASSIGN_FROM_STRING(val, "mac", m_externalCode);
        //对账信息列表
        ASSIGN_FROM_ARRAY(val, "checkAccountsInfos", m_offlineCheckInfo);
    }
}