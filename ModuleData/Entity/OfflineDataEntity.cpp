#include "OfflineDataEntity.h"

ORM_REGISTER_CPP_POS_DATABASE(OfflineDataEntity)

namespace Orm
{
    template<>
    void register_class(QdOrmClassDao<OfflineDataEntity>& t)
    {
        ORM_REGISTER_QD_TABLE(OfflineDataEntity, OFFLINE_DATA_TABLE_VERSION_V1)
        OrmMetaMemberPtr member = nullptr;

        // OFFLINE_DATA_TABLE_VERSION_V1
        member = ORM_REGISTER_QD_ID(OfflineDataEntity, Uuid)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, DeviceType, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, DeviceId, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, TerminalNo, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, DeviceCode, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, TenantId, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, OrgId, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, SaleType, OFFLINE_DATA_TABLE_VERSION_V1)
        member->setIsIndex(true);
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, Amount, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, CashierId, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, CashierNo, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, ReceiptNo, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, CreateTime, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, RecordTime, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, PushData, OFFLINE_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OfflineDataEntity, Extension, OFFLINE_DATA_TABLE_VERSION_V1)
    }
}