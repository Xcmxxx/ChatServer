#include "friendmodel.hpp"
#include "db.h"

vector<User> FriendModel::query(int userid)
{
    char sql[1024] = {0};

    sprintf(sql, "select a.id,a.name,a.state from User a inner join Friend b on b.friendid=a.id where b.userid =%d", userid);
    vector<User> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (nullptr != res)
        {
            MYSQL_ROW row;
            // 将row的消息推入vec并返回
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            // 释放res资源
            mysql_free_result(res);
            return vec;
        }
    }
}

void FriendModel:: insert(int userid, int friendid)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into Friend value(%d,'%d')", userid, friendid);
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
