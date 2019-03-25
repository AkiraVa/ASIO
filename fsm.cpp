#include "fsm.h"
#include <memory>

State_Command::State_Command():nextSize(0){}
State_Command::~State_Command(){}


Head::Head(){}
Head::~Head(){}
State_Command* Head::Operation(unsigned char *data,RWHandle *handle)
{
    //cout << static_cast<int>(data[0] & data[1] & data[2]);
    if(static_cast<int>(data[0] & data[1] & data[2]) == 0xFF)
    {
        nextSize = static_cast<int>(data[3]);
        int nextstate = static_cast<int>(data[4]);
        switch (nextstate){
            case 1:return new One_Mode();break;
            case 2:return new Two_Mode();break;
            case 3:return new Three_Mode();break;
            default:nextSize = HEAD_LEN;return new Head();break;
        }
    }else{
        nextSize = HEAD_LEN;
        return new Head();
    }
}

One_Mode::One_Mode(){}
One_Mode::~One_Mode(){}
State_Command* One_Mode::Operation(unsigned char *data,RWHandle *handle)
{
    //handle
    cout<<this_thread::get_id()<<endl;
    std::cout<<data<<endl;
    handle->HandleWrite((unsigned char*)("\nrec 1:"),6);
    handle->HandleWrite(data,nextSize);


    nextSize = HEAD_LEN;
    return new Head();
}


Two_Mode::Two_Mode(){}
Two_Mode::~Two_Mode(){}
State_Command* Two_Mode::Operation(unsigned char *data,RWHandle *handle)
{
    //handle
    cout<<this_thread::get_id()<<endl;
    std::cout<<data<<endl;
    handle->HandleWrite((unsigned char*)("\nrec 2:"),6);
    handle->HandleWrite(data,nextSize);

    nextSize = HEAD_LEN;
    return new Head();
}


Three_Mode::Three_Mode(){}
Three_Mode::~Three_Mode(){}
State_Command* Three_Mode::Operation(unsigned char *data,RWHandle *handle)
{
    //handle

    std::cout<<data<<endl;
    handle->HandleWrite((unsigned char*)("\nrec 3:"),6);
    handle->HandleWrite(data,nextSize);

    nextSize = HEAD_LEN;
    return new Head();
}
