#pragma once

#include <QString>
#include "ModuleBase/ModuleBaseApi.h"

namespace Base
{
    /** 数学工具类，提供常用算法封装 */
    class MODULE_BASE_API MathUtil
    {
    public:
        /** （小写）生成32位UUID字符串 */
        static QString GetUUID();

        /** （小写）生成16位随机字符串 */
        static QString GetNonce();

        /** （小写）计算MD5摘要字符串 */
        static QString GetMD5(const QString& content);

        /** （小写）计算文件MD5摘要字符串 */
        static QString GetFileMD5(const QString& fileName);
    };
};