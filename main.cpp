#include <iostream>
#include <serivice.h>

int main()
{ 
    Service ser("0.0.0.0", 8090);
    ser.Accept();
    ser.StartThreadGroup();

    system("pause");

    ser.StopAccept();

    return 0;
}
