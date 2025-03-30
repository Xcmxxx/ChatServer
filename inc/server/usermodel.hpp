#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"
//USER表的数据操作类

class UserModel
{
public:
//将User插入数据库，并返回更新后的userId
bool insert(User &user);
//根据ID查询User信息
User query(int id);
//更新用户的状态信息
bool updateState(User user);
//重置用户状态信息
void resetState();
};

#endif