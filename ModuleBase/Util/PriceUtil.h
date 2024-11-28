#include "ModuleBase/ModuleBaseApi.h"
#include <QString>

namespace Base
{
    /** 金额工具类 */
    class MODULE_BASE_API PriceUtil
    {
    public:
        /** 金额转字符串 */
        static QString AmountToString(double number, qint8 n = 2);

        /** '分'转字符串 */
        static QString FenToString(qint64 number, qint8 n = 2);

        /**
        * @brief    double数据四舍五入
        * @param data 待四舍五入的数据
        * @param num  保留位数
        * @return   四舍五入后的数据
        */
        static double Round(double data, int num = 0);

        /**
        * @brief    double数据保留0位四舍五入后取整
        * @param data 待四舍五入的数据
        * @return   四舍五入后的数据
        */
        static qint64 ZeroRound(double data);
    };
};
