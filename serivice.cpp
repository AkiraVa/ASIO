#include "serivice.h"

Service::Service(const string& ip_adress,const short& port):m_acceptor(m_io_s,tcp::endpoint(address::from_string(ip_adress),port)),m_connect_id_pool(MAX_CONNECT_NUM)
{
    int cur = 0;
    generate_n(m_connect_id_pool.begin(),MAX_CONNECT_NUM,[&cur](){ return ++cur; });
}

Service::~Service(){}

void Service::Accept()
{
    //创建一个新的读写事件器，主要目的是创建新的tcp::socket
    shared_ptr<RWHandle> Handler = CreateHandle();
    m_acceptor.async_accept(Handler->GetSocket(), [this,Handler](const boost::system::error_code &ec)
    {
        lock_guard<mutex> guard(socket_mutex);

        if(ec)
        {
            cout<<"Read Error:"<<ec.message()<<"  Error value:"<<ec.value()<<endl;
            Handler->SocketClose();
            //StopAccept();
            return;
        }

        //连接成功，将RWHandle加入map
        m_rw_handler.insert(make_pair(Handler->GetConnectID(),Handler));
        Handler->HandleRead();

        Accept();
    });
}

void Service::StopAccept()
{
    boost::system::error_code ec;
    m_acceptor.cancel(ec);
    m_acceptor.close(ec);
    m_io_s.stop();
    for(int i =0; i<MAX_THREAD_GROUP; ++i)
        asio_thread[i]->join();
}

void Service::StartThreadGroup()
{
    for(int i =0; i<MAX_THREAD_GROUP; ++i)
        asio_thread.push_back(make_shared<thread>(std::bind(static_cast<std::size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run),&m_io_s)));
}

shared_ptr<RWHandle> Service::CreateHandle()
{
    int c_id = m_connect_id_pool.front();
    m_connect_id_pool.pop_front();
    shared_ptr<RWHandle> rwh = make_shared<RWHandle>(m_io_s, tp);

    rwh->SetConnectID(c_id);
    rwh->SetCallbackFunction([this](int con_id)
    {
       lock_guard<mutex> guard(socket_mutex);

       auto it = m_rw_handler.find(con_id);
       if(it != m_rw_handler.end())
            m_rw_handler.erase(it);
       cout<<"close id is:"<<con_id<<endl;
       m_connect_id_pool.push_back(con_id);
    });
    return rwh;
}
