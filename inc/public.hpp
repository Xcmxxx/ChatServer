#ifndef PUBLIC_H
#define PUBLIC_H

/*
server和client的公共文件
*/
enum EnMsgType
{
    LOGIN_MSG=1,//登录消息
    LOGIN_MSG_ACK,//登录响应消息
    REG_MSG,//注册消息
    REG_MSG_ACK,//注册响应确认
    ONE_CHAT_MSG,//单独聊天信息
    ADD_FRIEND_MSG//添加好友
};
#endif