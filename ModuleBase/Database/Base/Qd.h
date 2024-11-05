#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QVariant>
#include <QDateTime>
#include <QReadWriteLock>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QMetaEnum>
#include <QThread>
#include <QFile>
#include <QDir>
#include "ModuleBase/Database/Orm/QdOrmFactory.h"


// 注册数据表
#define ORM_REGISTER_TABLE(Name) \
    public:\
        static QString GetTableName() { return Name; }

// 注册数据列
#define ORM_REGISTER_COLUMN(Type,Name,Key) \
    private: \
        Q_PROPERTY(Type Name READ Get##Name WRITE Set##Name) \
    protected: \
        Type m_##Name = Type(); \
    public: \
        static QString Column##Name() { return Key; } \
        static QString Column##Name##Form() { return #Type; } \
        Type Get##Name() const { return m_##Name; } \
        void Set##Name(const Type& value) { m_##Name = value; }

// 注册数据表
#define ORM_REGISTER_QD_TABLE(Entity, Version) \
    t.setName(Entity::GetTableName()); \
    t.setVersion(Version);

// 注册唯一主键
#define ORM_REGISTER_QD_ID(Entity,Name) \
    t.id(Entity::Column##Name(),Entity::Column##Name##Form(),#Name);

// 注册数据列
#define ORM_REGISTER_QD_DATA(Entity,Name,Version) \
    t.data(Entity::Column##Name(), Entity::Column##Name##Form(),#Name, Version);

// 注册数据库实体头文件
#define ORM_REGISTER_HPP_STOCK_DATABASE(Entity) \
    extern Orm::QdOrmFactoryHelper<Entity> G_QD_REGISTER_FACTORY_##Entity;

// 注册数据库实体源文件
#define ORM_REGISTER_CPP_STOCK_DATABASE(Entity) \
    Orm::QdOrmFactoryHelper<Entity> G_QD_REGISTER_FACTORY_##Entity(Entity::GetTableName());

// 注册友元register_class函数
#define ORM_REGISTER_FRIEND_CLASS(className) \
    template <class T> friend void Orm::register_class(T & t);
