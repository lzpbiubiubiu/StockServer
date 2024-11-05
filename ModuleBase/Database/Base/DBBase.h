#pragma once

#include "ModuleBase/Database/Base/Qd.h"

namespace Sql
{
    /** 数据库信息 */
    struct DatabaseInfo
    {
        /** 数据库名称 */
        QString databaseName;

        /** 是否支持加密 */
        bool supportCipher = false;

        /** 数据库类型 */
        QString databaseType;

        /** 数据库加密密码 */
        QString databasePwd;
    };

    /** 数据库对象基类 */
    class MODULE_BASE_API DBBase
    {
    public:
        /** 设置数据库信息 */
        void SetDatabaseInfo(const DatabaseInfo& info);

        /** 获取数据库操作锁 */
        QReadWriteLock& GetLock();

        /** 获取数据库名称 */
        QString GetName() const;

        /** 获取数据库操作对象 */
        QSqlDatabase GetSqlDatabase();

        /** 检查数据库完整性 */
        bool CheckIntegrity(const QSqlDatabase& database);

        /** 连接到数据库 */
        bool Connect();

        /** 断开数据库连接 */
        void Disconnect();

        /** 创建数据表 */
        virtual bool CreateTable();

    private:
        QReadWriteLock m_lock;
        DatabaseInfo m_info;
        QHash<Qt::HANDLE, QString> m_connectionTable;
    };
}
