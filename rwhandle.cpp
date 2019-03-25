#include "rwhandle.h"

RWHandle::RWHandle(io_service& s, ThreadPool<function<void(void)>> &t):m_socket(s),m_strand(s),data_reader(new Head()),tp_(t)//这里使用io_serivice对socket进行赋值绑定
{
    data_reader->nextSize = HEAD_LEN;
}

RWHandle::~RWHandle()
{

}

void RWHandle::HandleRead()
{
    //注意这里其实shared_ptr引用了3次，lambda表达式引用了1次！！！
    //self在函数结束后，被销毁一次。如果对象在异步返回之前被销毁，还有lambda的一次引用
    auto self = shared_from_this();
    //三种情况会调用回调的lambda：1.缓存区满 2.transfer_at_least到指定的字节 3.发生错误
    async_read(m_socket,buffer(m_buff,data_reader->nextSize), m_strand.wrap([this,self](const boost::system::error_code &ec, size_t size)
    {
        //lock_guard<mutex> guard(socket_mutex);
        if(ec)
        {
            SocketClose();
            cout<<"Read Error:"<<ec.message()<<"  Error value:"<<ec.value()<<endl;
            m_error_callback(connect_id);
            return;
        }
        State_Command* tmp = data_reader->Operation(m_buff.data(),this);
        tmp->nextSize = data_reader->nextSize;
        delete data_reader;
        data_reader = tmp;

        //继续下一次异步读取
        HandleRead();

    }));
}

void RWHandle::HandleWrite(unsigned char* data, int len)
{
    /*异步发送*/
//    auto self = shared_from_this();
//    async_write(m_socket, buffer(data, len), m_strand.wrap([self,this](const boost::system::error_code &ec, size_t size)
//    {
//        //lock_guard<mutex> guard(socket_mutex);
//        if(ec)
//        {
//            SocketClose();
//            cout<<"Write Error:"<<ec.message()<<"  Error value:"<<ec.value()<<endl;
//             m_error_callback(connect_id);
//            return;
//        }
//    }));
    /*同步发送*/
    auto self = shared_from_this();
    tp_.Add([this,self,data,len]()
    {
        boost::system::error_code ec;
        write(m_socket, buffer(data, len),ec);
        if(ec)
        {
            SocketClose();
            cout<<"Write Error:"<<ec.message()<<"  Error value:"<<ec.value()<<endl;
            m_error_callback(connect_id);
            return;
        }
    });
}

void RWHandle::SocketClose()
{
    boost::system::error_code ec;
    m_socket.shutdown(tcp::socket::shutdown_send,ec);
    m_socket.close(ec);
}

tcp::socket& RWHandle::GetSocket()
{
    return m_socket;
}


void RWHandle::SetConnectID(const int& id)
{
    connect_id = id;
}

int& RWHandle::GetConnectID()
{
    return connect_id;
}
