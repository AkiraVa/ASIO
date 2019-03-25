#ifndef RWHANDLE_H
#define RWHANDLE_H

#include <iostream>
#include <memory>
#include <array>
#include <functional>
#include <mutex>

#include <winsock2.h>
#include <windows.h>
#include <boost/asio.hpp>

#include <fsm.h>
#include <threadpool.h>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

#define MAX_PACKAGE_SIZE 65535

class State_Command;

/*** 读写事件处理器 ***/
class RWHandle : public std::enable_shared_from_this<RWHandle>
{
    typedef void(*func)(void);

public:
    RWHandle(io_service& s, ThreadPool<function<void(void)> > &t);
    ~RWHandle();

    void HandleRead();
    void HandleWrite(unsigned char *data, int len);
    void SocketClose();
    tcp::socket& GetSocket();
    void SetConnectID(const int& id);
    int& GetConnectID();

    template<class F>
    void SetCallbackFunction(F f)
    {
        m_error_callback = f;
    }

private:
    tcp::socket m_socket;
    array<unsigned char,MAX_PACKAGE_SIZE> m_buff;
    int connect_id;
    function<void(int)> m_error_callback;
    //mutex socket_mutex;
    State_Command *data_reader;
    boost::asio::io_service::strand m_strand;

    ThreadPool<function<void(void)>> &tp_;
};

#endif // RWHANDLE_H
