#pragma once
#include <QMutex>
#include <QObject>
#include <QSqlDatabase>
#include "ModuleBase/Service/SyncService.h"
#include "ModuleData/Entity/VersionEntity.h"
#include "ModuleBase/Database/Base/DBBase.h"
#include "ModuleBase/Database/Base/DmlBase.h"
#include "ModuleData/ModuleDataApi.h"

/** 数据库管理器服务，实现数据库文件的管理 */
namespace Extra
{
    class MODULE_DATA_API DatabaseManager : public Base::SyncService
    {
        Q_OBJECT

    public:
        DatabaseManager(QObject* parent = nullptr);
        virtual ~DatabaseManager();

        /** 获取配置数据库 */
        QSharedPointer<Sql::DBBase> GetStockServerDatabase();

        /** 获取交易数据库 */
        QSharedPointer<Sql::DBBase> GetBusinessDatabase(const QDate& date);
        QSharedPointer<Sql::DBBase> GetBusinessDatabase(const QString& name);

    protected:
        /**
         * 创建SQLite数据库
         * @param name: 数据库名称
         * @return 成功返回非空值
         */
        template<typename T>
        QSharedPointer<T> CreateDatabase(const QString& name);

        /** 查询表版本 */
        VersionEntityPtr QueryTableVersion(const Orm::QdOrmClass* ormClass);

        /** 登记表版本 */
        bool RegisterTableVersion(const Orm::QdOrmClass* ormClass);

        /** 更新表版本 */
        bool UpdateTableVersion(const Orm::QdOrmClass* ormClass);

        /** 创建表索引 */
        bool CreateTableIndex(const Orm::QdOrmClass* ormClass, const QSqlDatabase& database);

        /** 升级表 */
        bool UpgradeTable(const Orm::QdOrmClass* ormClass, VersionEntityPtr version, const QSqlDatabase& database);

        /** 升级库 */
        bool UpgradeDatabase(QSharedPointer<Sql::DBBase> database);

        /** 重新生成新库 */
        template<typename T>
        QSharedPointer<T> RecreateDatabase(const QString& baseName, const QString& fileName);

        virtual bool OnStart() override;

        virtual void OnStop() override;

    private:
        // 数据库创建锁
        QMutex m_mtx;

        // 数据库缓存
        QHash<QString, QSharedPointer<Sql::DBBase>> m_databasePool;

        // version表操作对象
        Sql::DmlBase<Sql::VersionEntity> m_versionDML;
    };
}