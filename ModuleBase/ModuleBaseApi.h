#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MODULE_BASE_LIB)
#  define MODULE_BASE_API Q_DECL_EXPORT
# else
#  define MODULE_BASE_API Q_DECL_IMPORT
# endif
#else
# define MODULE_BASE_API
#endif