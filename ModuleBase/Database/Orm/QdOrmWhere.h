#pragma once

#include <QObject>
#include <QVariant>
#include <QSharedPointer>
#include "ModuleBase/ModuleBaseApi.h"

namespace Orm
{
    struct MODULE_BASE_API QdOrmWhereItem
    {
        /** 操作类型 */
        enum OperateMode
        {
            ORM_OM_NONE = 0,
            ORM_OM_WHERE,
            ORM_OM_AND,
            ORM_OM_AND_OPEN_PARENTHESIS,
            ORM_OM_OR,
            ORM_OM_OR_OPEN_PARENTHESIS,
            ORM_OM_OPEN_PARENTHESIS,
            ORM_OM_CLOSE_PARENTHESIS
        };

        /** 比较类型 */
        enum CompareMode
        {
            ORM_CM_NONE = 0,
            ORM_CM_EQUAL,
            ORM_CM_NOT_EQUAL,
            ORM_CM_GREATER,
            ORM_CM_GREATER_OR_EQUAL,
            ORM_CM_LESS,
            ORM_CM_LESS_OR_EQUAL,
            ORM_CM_NULL,
            ORM_CM_NOT_NULL,
            ORM_CM_BETWEEN,
            ORM_CM_NOT_BETWEEN,
            ORM_CM_IN,
            ORM_CM_NOT_IN
        };

        /** 列名 */
        QString cloumn;

        /** 值 */
        QVariantList value;

        /** 操作类型 */
        OperateMode oMode = ORM_OM_NONE;

        /** 比较类型 */
        CompareMode cMode = ORM_CM_NONE;
    };
    using QdOrmWhereItemPtr = QSharedPointer<QdOrmWhereItem>;

    struct MODULE_BASE_API QdOrmOrderByItem
    {
        enum OrderFlag
        {
            /** 升序 */
            ASC,

            /** 降序 */
            DESC,
        };

        /** 排序模式 */
        OrderFlag flag = ASC;

        /** 数据列 */
        QStringList cloumns;
    };
    using QdOrmOrderByItemPtr = QSharedPointer<QdOrmOrderByItem>;

    class MODULE_BASE_API QdOrmWhere
    {
    public:
        QdOrmWhere();
        ~QdOrmWhere();

        QdOrmWhere& where(const QString& column);

        QdOrmWhere& and_(const QString& column);
        QdOrmWhere& and_OpenParenthesis(const QString& column);

        QdOrmWhere& or_(const QString& column);
        QdOrmWhere& or_OpenParenthesis(const QString& column);

        QdOrmWhere& openParenthesis();
        QdOrmWhere& closeParenthesis();

        QdOrmWhere& orderAsc(const QStringList& columns);
        QdOrmWhere& orderAsc(const QString& col1);

        QdOrmWhere& orderDesc(const QStringList& columns);
        QdOrmWhere& orderDesc(const QString& col1);

        QdOrmWhere& limit(int rowsCount, int startRow = 0);

        QdOrmWhere& isEqualTo(const QVariant& val);
        QdOrmWhere& isNotEqualTo(const QVariant& val);

        QdOrmWhere& isGreaterThan(const QVariant& val);
        QdOrmWhere& isLessThan(const QVariant& val);

        QdOrmWhere& isGreaterThanOrEqualTo(const QVariant& val);
        QdOrmWhere& isLessThanOrEqualTo(const QVariant& val);

        QdOrmWhere& isNull();
        QdOrmWhere& isNotNull();

        QdOrmWhere& isBetween(const QVariant& val1, const QVariant& val2);
        QdOrmWhere& isNotBetween(const QVariant& val1, const QVariant& val2);

        QdOrmWhere& in(const QVariantList& values);
        QdOrmWhere& notIn(const QVariantList& values);

        void clear();
        const QList<QdOrmWhereItemPtr>& getOrmWhereItems() const;
        const QList<QdOrmOrderByItemPtr>& getOrmOrderByItems() const;
        const QVariantList& getLimitItems() const;

    private:
        void addOperateParams(const QString& column, const QdOrmWhereItem::OperateMode& oMode);
        void addCompareParams(const QVariantList& value, const QdOrmWhereItem::CompareMode& cMode);

    private:
        QList<QdOrmWhereItemPtr> m_whereItems;
        QList<QdOrmOrderByItemPtr> m_orderByItems;
        QVariantList m_limitItems;
    };
}