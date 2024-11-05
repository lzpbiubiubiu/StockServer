#include "RecordDataEntity.h"

ORM_REGISTER_CPP_POS_DATABASE(RecordDataEntity)

namespace Orm
{
    template<>
    void register_class(QdOrmClassDao<RecordDataEntity>& t)
    {
        ORM_REGISTER_QD_TABLE(RecordDataEntity, RECORD_DATA_TABLE_VERSION_V1)
        OrmMetaMemberPtr member = nullptr;

        // RECORD_DATA_TABLE_VERSION_V1
        member = ORM_REGISTER_QD_ID(RecordDataEntity, Uuid)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, DeviceType, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, DeviceId, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, TerminalNo, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, DeviceCode, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, TenantId, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, OrgId, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, RovingPosOrgId, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, CashierId, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, CashierNo, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, ReceiptNo, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, CreateTime, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, PushData, RECORD_DATA_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(RecordDataEntity, Extension, RECORD_DATA_TABLE_VERSION_V1)
    }
}