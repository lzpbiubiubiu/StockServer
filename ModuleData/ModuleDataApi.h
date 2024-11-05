#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MODULE_DATA_LIB)
#  define MODULE_DATA_API Q_DECL_EXPORT
# else
#  define MODULE_DATA_API Q_DECL_IMPORT
# endif
#else
# define MODULE_DATA_API
#endif