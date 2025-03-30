#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <mutex>

#include "json.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"

using namespace muduo::net;
using namespace muduo;
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

// 单列业务类
//  处理事件的方法类型
using MsgHandler =
    std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;
// 聊天服务器业务类
class ChatService
{
public:
    // 获取单列对象的接口函数
    static ChatService *instance();
    // 登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 客户端异常关闭
    void clientCloseException(const TcpConnectionPtr &conn);
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    // 一对一额聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 服务器异常关闭、
    void reset();

private:
    ChatService();
    // 消息ID和对应的事件处理方法
    std::unordered_map<int, MsgHandler> _msgHandlerMap;

    // 存储在线用户的通信连接
    std::unordered_map<int, TcpConnectionPtr> _userConnMap;

    // 数据操作类对象
    UserModel _usermodel;
    //
    OfflineMsgModel _offlineMsgModel;
    //
    FriendModel _friendmodel;
    // 定义互斥锁，保证_userConnectionMap的线程安全
    std::mutex _connMutex;
};
#endif