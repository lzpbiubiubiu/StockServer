#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MODULE_7ZIP_LIB)
#  define MODULE_7ZIP_API Q_DECL_EXPORT
# else
#  define MODULE_7ZIP_API Q_DECL_IMPORT
# endif
#else
# define MODULE_7ZIP_API
#endif