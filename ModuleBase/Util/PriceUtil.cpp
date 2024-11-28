#include "PriceUtil.h"
#include "Log/Log.h"
#include <QtMath>

namespace Base
{
    QString PriceUtil::AmountToString(double number, qint8 n /*= 2*/)
    {
        // TODO 银行行家算法
        // 先按保留位数四舍五入
        number = qRound64(number * qPow(10, n)) / qPow(10, n);
        QString amountStr = QString::number(number, 'f', n);
        return amountStr;
    }

    QString PriceUtil::FenToString(qint64 number, qint8 n)
    {
        return AmountToString(number / 100.0, n);
    }

    double PriceUtil::Round(double data, int num/* = 0*/)
    {
        if(std::isnan(data) || std::isinf(data))
        {
            LOG_ERROR("PriceUtil::Round data is nan or finite, data: {}", data);
            return data;
        }

        // 转为字符串
        std::string str_data = std::to_string(data);

        if(str_data.length() >= 19)
        {
            LOG_ERROR("PriceUtil::Round data is too large, data: {}", str_data.c_str());
            return data;
        }

        // 判断正负数
        double pre_num = 1.0;
        if(str_data[0] == '-')
        {
            pre_num = -1.0;
            str_data = str_data.substr(1);
        }

        // 切割整数和小数部分
        size_t decimal_pos = str_data.find('.');
        std::string str_int_data = decimal_pos != std::string::npos ? str_data.substr(0, decimal_pos) : str_data;
        std::string str_float_data = decimal_pos != std::string::npos ? str_data.substr(decimal_pos + 1) : "0";

        // 如果num小于等于0，返回整数部分值
        if(num <= 0)
        {
            // 判断第一位小数是否大于4，大于4进位
            if(str_float_data[0] > '4')
                return (std::stod(str_int_data) + 1.0) * pre_num;
            else
                return std::stod(str_int_data) * pre_num;
        }

        // 获取小数部分并根据长度补0
        if(str_float_data.length() < static_cast<size_t>(num + 1))
            str_float_data += std::string(num + 1 - str_float_data.length(), '0');
        else
            str_float_data = str_float_data.substr(0, num + 1);

        // 将整数和小数部分拼接
        std::string str_data_new = str_int_data + str_float_data;
        long long int_data = std::stoll(str_data_new.substr(0, str_data_new.length() - 1));

        // 判断最后一位是否大于4，大于4进位
        if(str_data_new[str_data_new.length() - 1] > '4')
        {
            std::string str_int_data_new = std::to_string(int_data + 1);
            str_int_data_new = std::string(str_data_new.length() - 1 - num, '0') + str_int_data_new;
            str_data_new = str_int_data_new.substr(0, str_int_data_new.length() - num) + "." +
                str_int_data_new.substr(str_int_data_new.length() - num);
        }
        else
        {
            std::string str_int_data_new = std::to_string(int_data);
            str_int_data_new = std::string(str_data_new.length() - 1 - num, '0') + str_int_data_new;
            str_data_new = str_int_data_new.substr(0, str_int_data_new.length() - num) + "." +
                str_int_data_new.substr(str_int_data_new.length() - num);
        }

        // 转为double正负数
        double rtn_float = std::stod(str_data_new) * pre_num;
        if(pre_num == -1.0 && rtn_float == 0.0)
        {
            return -0.0;
        }

        return rtn_float;
    }

    qint64 PriceUtil::ZeroRound(double data)
    {
        return qRound64(Round(data));
    }
}
