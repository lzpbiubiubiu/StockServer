#include "QdOrmWhere.h"

namespace Orm
{
    QdOrmWhere::QdOrmWhere()
    {
        clear();
    }

    QdOrmWhere::~QdOrmWhere()
    {
        clear();
    }

    QdOrmWhere& QdOrmWhere::where(const QString& column)
    {
        addOperateParams(column, QdOrmWhereItem::ORM_OM_WHERE);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::and_(const QString& column)
    {
        addOperateParams(column, QdOrmWhereItem::ORM_OM_AND);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::and_OpenParenthesis(const QString& column)
    {
        addOperateParams(column, QdOrmWhereItem::ORM_OM_AND_OPEN_PARENTHESIS);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::or_(const QString& column)
    {
        addOperateParams(column, QdOrmWhereItem::ORM_OM_OR);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::or_OpenParenthesis(const QString& column)
    {
        addOperateParams(column, QdOrmWhereItem::ORM_OM_OR_OPEN_PARENTHESIS);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::openParenthesis()
    {
        addOperateParams(QString(), QdOrmWhereItem::ORM_OM_OPEN_PARENTHESIS);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::closeParenthesis()
    {
        addOperateParams(QString(), QdOrmWhereItem::ORM_OM_CLOSE_PARENTHESIS);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::orderAsc(const QStringList& columns)
    {
        QdOrmOrderByItemPtr item = QdOrmOrderByItemPtr::create();
        item->flag = QdOrmOrderByItem::ASC;
        item->cloumns = columns;
        m_orderByItems << item;

        return *this;
    }

    QdOrmWhere& QdOrmWhere::orderAsc(const QString& col1)
    {
        return orderAsc(QStringList() << col1);
    }

    QdOrmWhere& QdOrmWhere::orderDesc(const QStringList& columns)
    {
        QdOrmOrderByItemPtr item = QdOrmOrderByItemPtr::create();
        item->flag = QdOrmOrderByItem::DESC;
        item->cloumns = columns;
        m_orderByItems << item;

        return *this;
    }

    QdOrmWhere& QdOrmWhere::orderDesc(const QString& col1)
    {
        return orderDesc(QStringList() << col1);
    }

    QdOrmWhere& QdOrmWhere::limit(int rowsCount, int startRow)
    {
        m_limitItems.clear();
        m_limitItems << rowsCount << startRow;
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isEqualTo(const QVariant& val)
    {
        addCompareParams(QVariantList() << val, QdOrmWhereItem::ORM_CM_EQUAL);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isNotEqualTo(const QVariant& val)
    {
        addCompareParams(QVariantList() << val, QdOrmWhereItem::ORM_CM_NOT_EQUAL);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isGreaterThan(const QVariant& val)
    {
        addCompareParams(QVariantList() << val, QdOrmWhereItem::ORM_CM_GREATER);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isLessThan(const QVariant& val)
    {
        addCompareParams(QVariantList() << val, QdOrmWhereItem::ORM_CM_LESS);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isGreaterThanOrEqualTo(const QVariant& val)
    {
        addCompareParams(QVariantList() << val, QdOrmWhereItem::ORM_CM_GREATER_OR_EQUAL);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isLessThanOrEqualTo(const QVariant& val)
    {
        addCompareParams(QVariantList() << val, QdOrmWhereItem::ORM_CM_LESS_OR_EQUAL);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isNull()
    {
        addCompareParams(QVariantList(), QdOrmWhereItem::ORM_CM_NULL);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isNotNull()
    {
        addCompareParams(QVariantList(), QdOrmWhereItem::ORM_CM_NOT_NULL);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isBetween(const QVariant& val1, const QVariant& val2)
    {
        addCompareParams(QVariantList() << val1 << val2, QdOrmWhereItem::ORM_CM_BETWEEN);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::isNotBetween(const QVariant& val1, const QVariant& val2)
    {
        addCompareParams(QVariantList() << val1 << val2, QdOrmWhereItem::ORM_CM_NOT_BETWEEN);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::in(const QVariantList& values)
    {
        addCompareParams(values, QdOrmWhereItem::ORM_CM_IN);
        return *this;
    }

    QdOrmWhere& QdOrmWhere::notIn(const QVariantList& values)
    {
        addCompareParams(values, QdOrmWhereItem::ORM_CM_NOT_IN);
        return *this;
    }

    void QdOrmWhere::clear()
    {
        m_whereItems.clear();
        m_orderByItems.clear();
        m_limitItems.clear();
    }

    const QList<QdOrmWhereItemPtr>& QdOrmWhere::getOrmWhereItems() const
    {
        return m_whereItems;
    }

    const QList<QdOrmOrderByItemPtr>& QdOrmWhere::getOrmOrderByItems() const
    {
        return m_orderByItems;
    }

    const QVariantList& QdOrmWhere::getLimitItems() const
    {
        return m_limitItems;
    }

    void QdOrmWhere::addOperateParams(const QString& column, const QdOrmWhereItem::OperateMode& oMode)
    {
        QdOrmWhereItemPtr item = QdOrmWhereItemPtr::create();
        item->cloumn = column;
        item->oMode = oMode;
        m_whereItems << item;
    }

    void QdOrmWhere::addCompareParams(const QVariantList& value, const QdOrmWhereItem::CompareMode& cMode)
    {
        auto& last = m_whereItems.last();
        last->value = value;
        last->cMode = cMode;
    }
}