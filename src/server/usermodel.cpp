#include "usermodel.hpp"
#include "db.h"
#include <iostream>
using namespace std;

//插入
bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into User(name,password,state) value('%s','%s','%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取插入成功用户的id
            // mysql_insert_id函数用于获取插入后自增的id值
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
        else
            return false;
        // 此处应有LOG
    }
    else
        return false;
}

//查询
User UserModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "select * from User where id =%d", id);
    MySQL mysql;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (nullptr != res)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (nullptr != row)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);

                // mysql.query()是动态分配内存，必须手动释放
                mysql_free_result(res);
                return user;
            }
        }
    }
}
//更新
bool UserModel::updateState(User user)
{
     // 组装sql语句
     char sql[1024] = {0};
     sprintf(sql, "update User set state ='%s' where id = %d" , user.getState().c_str(),user.getId());
    MySQL mysql;
     if(mysql.connect())
    {
        MYSQL_RES *res=mysql.query(sql);
        if(mysql.update(sql))
        {
            return true;
        }
    }    
}
//服务器异常，重置用户状态
void UserModel::resetState()
{
    char sql[1024] = 
    "update User set state = 'offline' where state = 'online' ";
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.query(sql);
    }    
}
   