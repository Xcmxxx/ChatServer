#include "db.h"
#include <mysql/mysql.h>
#include <string>
#include <muduo/base/Logging.h>

using namespace std;


// 数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat";

// 初始化数据库，相当于给了一块存储空间,句柄
MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}
// 释放资源
MySQL::~MySQL()
{
    if (nullptr != _conn)
        mysql_close(_conn);
}
// 连接数据库
bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                  password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (nullptr != p)
    {
        // 默认是ASCII，设置GBK可以显示中文
        mysql_query(_conn, "set names gbk");
        LOG_INFO<<"connect mysql success!";
    }
    else{
        LOG_INFO<<"connect mysql fail";
    }
    return p;

}
// 更新操作
bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "更新失败";
        return false;
    }
    return true;
}
// 查询操作
MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {

        // 当前的源文件名和程序行数
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "查询失败";
        return nullptr;
    }
    return mysql_use_result(_conn);
}
//获取连接
MYSQL *MySQL:: getConnection()
{
    return _conn;
}