// #ifndef CHATSERVER_H
// #define CHATSERVER_H
#pragma once
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

class ChatServer
{
public:
    ChatServer(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr,
               const std::string &nameArg);
    // 启动函数
    void start();

private:
    // 连接相关回调函数
    void onconnection(const muduo::net::TcpConnectionPtr& conn);
    // 读写事件相关函数
    void onmessage(const muduo::net::TcpConnectionPtr& conn,
        muduo::net::Buffer* buffer,
        muduo::Timestamp receiveTime);

    muduo::net::TcpServer _server; // muduo服务器类
    muduo::net::EventLoop *_loop;  // 指向事件循环的指针
};

// #endif