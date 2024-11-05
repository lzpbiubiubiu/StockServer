#pragma once

#include <QString>

namespace Orm
{
    /** 创建数据库表 */
    const QString create_table_sql = "CREATE TABLE %1 (%2);";

    /** 删除数据库表 */
    const QString drop_table_sql = "DROP TABLE %1;";

    /** 主键数据是否存在 */
    const QString exist_sql = "SELECT * FROM %1 WHERE %2;";

    /** 插入数据行 */
    const QString insert_sql = "INSERT INTO %1 (%2) VALUES(%3);";

    /** 保存数据行 */
    const QString save_sql = "REPLACE INTO %1 (%2) VALUES(%3);";

    /** 删除数据行 */
    const QString delete_sql = "DELETE FROM %1 WHERE %2;";

    /** 删除所有数据行 */
    const QString delete_all_sql = "DELETE FROM %1%2;";

    /** 更新数据行 */
    const QString update_sql = "UPDATE %1 SET %2 WHERE %3;";

    /** 更新全部数据行 */
    const QString update_all_sql = "UPDATE %1 SET %2%3;";

    /** 获取数据 */
    const QString fetch_sql = "SELECT %1 FROM %2 WHERE %3;";

    /** 获取全部数据 */
    const QString fetch_all_sql = "SELECT %1 FROM %2%3;";

    /** 聚合函数 */
    const QString count_all_sql = "SELECT COUNT(%1) FROM %2%3;";
    const QString sum_all_sql = "SELECT SUM(%1) FROM %2%3;";
    const QString avg_all_sql = "SELECT AVG(%1) FROM %2%3;";
    const QString min_all_sql = "SELECT MIN(%1) FROM %2%3;";
    const QString max_all_sql = "SELECT MAX(%1) FROM %2%3;";
}