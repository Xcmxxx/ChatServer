#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"
#include <functional>
#include <iostream>
#include <string>
using json = nlohmann::json;
ChatServer::ChatServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr,
                       const std::string &nameArg)
    : _server(loop, listenAddr, nameArg),
      _loop(loop)
{

    _server.setConnectionCallback(std::bind(&ChatServer::onconnection, this, std::placeholders::_1));

    _server.setMessageCallback(std::bind(&ChatServer::onmessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    _server.setThreadNum(2);
}

void ChatServer::start()
{
    _server.start();
}

// 连接相关函数
void ChatServer::onconnection(const muduo::net::TcpConnectionPtr &conn)
{

    if (!conn->connected()) // 断开
    {
        
        std::cout<<"断开函数调用"<<std::endl;
        ChatService::instance()->clientCloseException(conn);
        
        conn->shutdown();

    }
}
// 读写事件相关函数
void ChatServer::onmessage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buffer,
                           muduo::Timestamp time)
{
    std::string buf=buffer->retrieveAllAsString();
    //数据反序列化
    json js=json::parse(buf);

    //完全解耦网络与业务代码
    //获取对应msgid的处理函数
    auto msgHandler=ChatService::instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn,js,time);
}