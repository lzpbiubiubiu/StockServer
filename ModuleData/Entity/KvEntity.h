#pragma once

#include "Base/Database/Base/EntityBase.h"

#define KV_TABLE_VERSION_V1 (1)

namespace Sql
{
    /** [tb_kv] KV字典表  */
    class KvEntity : public Sql::EntityBase
    {
        Q_OBJECT

        ORM_REGISTER_FRIEND_CLASS(KvEntity)

        ORM_REGISTER_TABLE("tb_kv")

        // Key
        ORM_REGISTER_COLUMN(QString, Key, "key")

        // Value
        ORM_REGISTER_COLUMN(QString, Value, "value")
    };

    using KvEntityPtr = QSharedPointer<KvEntity>;
    using KvEntityPtrList = QList<QSharedPointer<KvEntity>>;
}

using namespace Sql;
