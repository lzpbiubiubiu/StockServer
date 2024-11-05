#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MODULE_PROTOCOL_LIB)
#  define MODULE_PROTOCOL_API Q_DECL_EXPORT
# else
#  define MODULE_PROTOCOL_API Q_DECL_IMPORT
# endif
#else
# define MODULE_PROTOCOL_API
#endif
