#include "chatservice.hpp"
#include "public.hpp"

#include <muduo/base/Logging.h>
#include <string>
#include <vector>
using namespace std;
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 将处理函数加入Map表，通过给定id获取处理函数
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
}

// 获取对应消息的处理函数
MsgHandler ChatService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (_msgHandlerMap.end() == it)
    {
        //
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid:" << msgid << "can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

// 服务器异常，业务重置
void ChatService::reset()
{
    _usermodel.resetState();
}
// 登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "login service";
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _usermodel.query(id);
    if (-1 != user.getId() && pwd == user.getPwd())
    {
        if (user.getState() == "online")
        {
            // 该用户已登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账号已登录";
            conn->send(response.dump());
        }
        else
        {

            // 登录成功，更新用户状态信息
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["error"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            user.setState("online");
            _usermodel.updateState(user);
            // 登录成功，记录用户连接信息

            { // 注意线程互斥安全
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // 登录成功，离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;

                // 读取离线消息后，把用户的离线消息删除
                _offlineMsgModel.remove(id);
            }
            // 查询好友信息并返回
            vector<User> userVec = _friendmodel.query(id);
            if (!userVec.empty())
            {

                vector<string> vec2;
                for (User &user : userVec)
                {
                    json js;
                    js["id"]=user.getId();
                    js["name"]=user.getName();
                    js["state"]=user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"]=vec2;
            }
            conn->send(response.dump());
        }
    }

    else
    {
        //
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或密码错误";
        conn->send(response.dump());
    }
}

// 注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 将接受到的json字符串序列化成User对象，插入数据库
    LOG_INFO << "reg service";
    string name = js["name"];
    string pwd = js["password"];
    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _usermodel.insert(user);
    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0; // 消息无误
        response["id"] = user.getId();
        conn->send(response.dump());
    }

    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1; // 消息无误
        conn->send(response.dump());
    }
}
// 异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);

        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {

            if (it->second == conn)
            {

                // 从map表中删除用户连接
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 数据库更新状态信息
    if (-1 != user.getId())
    {
        user.setState("offline");
        _usermodel.updateState(user);
    }
}

// 一对一聊天
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{

    int toid = js["to"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        // 找到目标TCP连接
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // 在线
            it->second->send(js.dump());
            return;
        }

        else
        {
            // 不在线
        }
    }
    // 不在线,存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 添加好友信息
    _friendmodel.insert(userid, friendid);
}
