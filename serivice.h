#ifndef SERIVICE_H
#define SERIVICE_H

#include <iostream>
#include <memory>
#include <list>
#include <unordered_map>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>

#include <winsock2.h>
#include <windows.h>
#include <boost/asio.hpp>

#include <rwhandle.h>
#include <threadpool.h>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

#define MAX_CONNECT_NUM 65535
#define MAX_THREAD_GROUP 20

/*** 服务器 ***/
class Service
{
    typedef void(*func)(void);

public:
    Service(const string& ip_adress,const short& port);
    ~Service();
    void Accept();
    void StopAccept();
    void StartThreadGroup();

private:
    shared_ptr<RWHandle> CreateHandle();

private:
    io_service m_io_s;
    tcp::acceptor m_acceptor;
    list<int> m_connect_id_pool;//记录TCP连接号
    unordered_map<int,shared_ptr<RWHandle>> m_rw_handler;//将socket放到读写事件器中去，每次accept一个连接之后，都存入map，以方便以后RW操作
    vector<shared_ptr<std::thread>> asio_thread;
    mutex socket_mutex;

    ThreadPool<function<void(void)>> tp;
};

#endif // SERIVICE_H
