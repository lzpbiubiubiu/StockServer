#include "DatabaseManager.h"
#include "Config.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleData/Database/StockServerDB.h"
#include "ModuleData/Database/BusinessDB.h"

namespace Extra
{
    // 数据库密码
    static const char* DATABASE_PASSWORD_TABLE[] =
    {
        // DEV
        "test",

        // UAT
        "76123D9D-B14C-4522-A669-84FED70CBFDB",

        // PRO
        "76123D9D-B14C-4522-A669-84FED70CBFDB"
    };

    DatabaseManager::DatabaseManager(QObject* parent)
        : Base::SyncService(parent)
    {}

    DatabaseManager::~DatabaseManager()
    {}

    QSharedPointer<Sql::DBBase> DatabaseManager::GetStockServerDatabase()
    {
        // Pos.db
        QString fileName = Base::PathUtil::GetDatabaseDir() + STOCK_SERVER_DATABASE_NAME + ".db";
        if(m_databasePool.contains(fileName))
            return m_databasePool[fileName];
        else
        {
            QSharedPointer<Sql::StockServerDB> posDb = CreateDatabase<Sql::StockServerDB>(fileName);
            //数据库有问题
            if(posDb.isNull())
            {
                posDb = RecreateDatabase<Sql::StockServerDB>(STOCK_SERVER_DATABASE_NAME, fileName);
            }
           
            return posDb;
        }
    }

    QSharedPointer<Sql::DBBase> DatabaseManager::GetBusinessDatabase(const QDate& date)
    {
        // Recordyyyy-MM-dd.db
        QString fileName = Base::PathUtil::GetDatabaseDir() + BUSINESS_DATABASE_NAME + date.toString("yyyy-MM-dd") + ".db";
        if(m_databasePool.contains(fileName))
        {
            return m_databasePool[fileName];
        }
        else
        {
            QSharedPointer<Sql::BusinessDB> db = CreateDatabase<Sql::BusinessDB>(fileName);
            if(db.isNull())
                db = RecreateDatabase<Sql::BusinessDB>(BUSINESS_DATABASE_NAME, fileName);
            return db;
        }
    }

    QSharedPointer<Sql::DBBase> DatabaseManager::GetBusinessDatabase(const QString& name)
    {
        QString fileName = Base::PathUtil::GetDatabaseDir() + name;
        if(m_databasePool.contains(fileName))
            return m_databasePool[fileName];
        else
            return CreateDatabase<Sql::BusinessDB>(fileName);
    }

    QSharedPointer<Sql::DBBase> DatabaseManager::GetTempDatabase()
    {
        QString fileName = Base::PathUtil::GetDatabaseDir() + TEMP_DATABASE_NAME + ".db";
        if(m_databasePool.contains(fileName))
            return m_databasePool[fileName];
        else
        {
            QSharedPointer<Sql::BusinessDB> saleDb = CreateDatabase<Sql::BusinessDB>(fileName);
            //数据库有问题
            if(saleDb.isNull())
            {
                //直接删除数据库
                QFile::remove(fileName);

                //再创建新的数据库一次
                saleDb = CreateDatabase<Sql::BusinessDB>(fileName);
            }
            return saleDb;
        }
    }

    template<typename T>
    QSharedPointer<T> DatabaseManager::CreateDatabase(const QString& name)
    {
        QMutexLocker locker(&m_mtx);
        if(m_databasePool.contains(name))
            return qSharedPointerCast<T>(m_databasePool[name]);

        auto database = QSharedPointer<T>::create();
        Sql::DatabaseInfo info;
        info.databaseName = name;
        info.supportCipher = QSqlDatabase::isDriverAvailable("SQLITECIPHER");
        info.databaseType = info.supportCipher ? "SQLITECIPHER" : "QSQLITE";
        //ServiceEnvironment env = GlobalData::Get()->GetConfig().app.env;
        //info.databasePwd = DATABASE_PASSWORD_TABLE[env];

        database->SetDatabaseInfo(info);
        if(!database->Connect())
        {
            Q_ASSERT_X(0, "CreateDatabase()", "Connect failed");
            return QSharedPointer<T>();
        }

        if(!database->CreateTable())
        {
            Q_ASSERT_X(0, "CreateDatabase()", "Create table failed");
            return QSharedPointer<T>();
        }

        if(!UpgradeDatabase(database))
        {
            Q_ASSERT_X(0, "CreateDatabase()", "Upgrade database failed");
            return QSharedPointer<T>();
        }

        m_databasePool[name] = database;
        return database;
    }

    template<typename T>
    QSharedPointer<T> DatabaseManager::RecreateDatabase(const QString& baseName, const QString& fileName)
    {
        LOG_ERROR("database has an error,file:{}", fileName.toStdString());
        //先重命名现有数据库
        QString errorName = Base::PathUtil::GetDatabaseDir() + "error_" + baseName + ".db";
        QFile::remove(errorName);
        QFile::remove(errorName + "-journal");
        if(!QFile::rename(fileName, errorName) || !QFile::rename(fileName + "-journal", errorName + "-journal"))
        {
            LOG_ERROR("database rename failed, file:{}", fileName.toStdString());
        }

        //再创建新的数据库一次
        return CreateDatabase<T>(fileName);
    }

    VersionEntityPtr DatabaseManager::QueryTableVersion(const Orm::QdOrmClass* ormClass)
    {
        Orm::QdOrmWhere query;
        query.where(VersionEntity::ColumnName()).isEqualTo(ormClass->tableName()).limit(1);
        QString error;
        VersionEntityPtr version;
        VersionEntityPtrList records;
        if(m_versionDML.FindByQuery(records, query, error) && !records.isEmpty())
            version = records.first();
        return version;
    }

    bool DatabaseManager::RegisterTableVersion(const Orm::QdOrmClass* ormClass)
    {
        // 表版本已注册，直接返回
        if(QueryTableVersion(ormClass) != nullptr)
            return true;

        auto versionEntity = QSharedPointer<VersionEntity>::create();
        versionEntity->SetName(ormClass->tableName());
        versionEntity->SetVersion(ormClass->version());
        QString error;
        return m_versionDML.Add(versionEntity, error);
    }

    bool DatabaseManager::UpdateTableVersion(const Orm::QdOrmClass* ormClass)
    {
        // 表版本不存在，更新失败
        auto versionEntity = QueryTableVersion(ormClass);
        if(versionEntity == nullptr)
            return false;

        // 表版本一致不需要更新
        if(ormClass->version() <= versionEntity->GetVersion())
            return true;

        versionEntity->SetVersion(ormClass->version());
        QString error;
        return m_versionDML.UpdateById(versionEntity, QStringList(), error);
    }

    bool DatabaseManager::CreateTableIndex(const Orm::QdOrmClass* ormClass, const QSqlDatabase& database)
    {
        QSqlQuery query(database);

        const auto& dataMembers = ormClass->ormMetaMembers();
        for(const auto& ormMember : dataMembers)
        {
            if(ormMember->isIndex())
            {
                QString sql = "CREATE INDEX " + ormClass->tableName() + "_" + ormMember->fieldName() + "_idx" + " ON " + ormClass->tableName() + " (" + ormMember->fieldName() + ");";
                if(!query.exec(sql))
                    return false;
            }
        }
        return true;
    }

    bool DatabaseManager::UpgradeTable(const Orm::QdOrmClass* ormClass, VersionEntityPtr version, const QSqlDatabase& database)
    {
        QSqlQuery query(database);
        const auto& dataMembers = ormClass->ormMetaMembers();

        for(const auto& ormMember : dataMembers)
        {
            if(ormMember.isNull() || (ormMember->fieldVersion() <= version->GetVersion()))
                continue;

            // 新增列
            QString sql = "ALTER TABLE " + ormClass->tableName() + " ADD COLUMN " + ormMember->fieldName() + " " + ormMember->fieldSqlForm() + ";";
            if(!query.exec(sql))
            {
                QString s = query.lastError().text();
                return false;
            }

            // 新增索引
            if(ormMember->isIndex())
            {
                sql = "CREATE INDEX " + ormClass->tableName() + "_" + ormMember->fieldName() + "_idx" + " ON " + ormClass->tableName() + " (" + ormMember->fieldName() + ");";
                if(!query.exec(sql))
                    return false;
            }
        }
        return true;
    }

    bool DatabaseManager::UpgradeDatabase(QSharedPointer<Sql::DBBase> database)
    {
        if(database.isNull())
            return false;

        m_versionDML.SetDatabase(database);
        QStringList tables = database->GetSqlDatabase().tables();
        for(const auto& table : tables)
        {
            const Orm::QdOrmClass* ormClass = Orm::QdOrmFactory::Get()->getAppointClass(table);
            if(!ormClass)
                continue;

            // 根据类名获取version表中记录的版本
            VersionEntityPtr version = QueryTableVersion(ormClass);
            if(version != nullptr)
            {
                // 过滤已更新的类
                if(ormClass->version() > version->GetVersion())
                {
                    if(!UpgradeTable(ormClass, version, database->GetSqlDatabase()))
                        return false;

                    if(!UpdateTableVersion(ormClass))
                        return false;
                }
            }
            else
            {
                // 注册新表的版本号
                if(!RegisterTableVersion(ormClass))
                    return false;

                // 创建表索引
                if(!CreateTableIndex(ormClass, database->GetSqlDatabase()))
                    return false;
            }
        }
        return true;
    }

    bool DatabaseManager::OnStart()
    {
        if (GetStockServerDatabase().isNull())
            return false;

        if(GetBusinessDatabase(QDate::currentDate()).isNull())
            return false;

        if(GetTempDatabase().isNull())
            return false;
        
        return true;
    }

    void DatabaseManager::OnStop()
    {
        for(auto& i : m_databasePool)
            i->Disconnect();
    }
}