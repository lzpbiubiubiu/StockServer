#pragma once

#include <QList>
#include <QSharedPointer>

namespace Orm
{
    template <typename T>
    struct is_smart_ptr : public std::false_type { ; };

    template <typename T>
    struct is_smart_ptr< QSharedPointer<T> > : public std::true_type { ; };

    template <typename T>
    struct is_smart_ptr< QSharedPointer<T>& > : public std::true_type { ; };

    template <typename T>
    struct is_smart_ptr< const QSharedPointer<T> > : public std::true_type { ; };

    template <typename T>
    struct is_smart_ptr< const QSharedPointer<T>& > : public std::true_type { ; };

    template <typename T>
    struct is_container : public std::false_type { ; };

    template <typename T>
    struct is_container< QList<T> > : public std::true_type { ; };

    template <typename T>
    struct is_container< QList<T>& > : public std::true_type { ; };

    template <typename T>
    struct is_container< const QList<T> > : public std::true_type { ; };

    template <typename T>
    struct is_container< const QList<T>& > : public std::true_type { ; };
}