#pragma once

#include "ModuleData/ModuleDataApi.h"
#include "ModuleBase/Database/Base/EntityBase.h"

#define VERSION_TABLE_VERSION_V1 (1)

namespace Sql
{
    /** [tb_version] 版本号表 */
    class MODULE_DATA_API VersionEntity : public EntityBase
    {
        Q_OBJECT

        ORM_REGISTER_FRIEND_CLASS(VersionEntity)

        ORM_REGISTER_TABLE("tb_version")

        // 表名称
        ORM_REGISTER_COLUMN(QString, Name, "name")

        // 版本号
        ORM_REGISTER_COLUMN(int, Version, "version")
    };

    using VersionEntityPtr = QSharedPointer<VersionEntity>;
    using VersionEntityPtrList = QList<QSharedPointer<VersionEntity>>;
}

using namespace Sql;
ORM_REGISTER_HPP_STOCK_DATABASE(VersionEntity)
