#ifndef FSM_H
#define FSM_H

//Head
/*   0xFF 0xFF 0xFF size state  */

//Tail
/*   data  verify  */

#include "rwhandle.h"

#define HEAD_LEN 5

class RWHandle;

class State_Command
{
public:
    State_Command();
    virtual State_Command* Operation(unsigned char *data,RWHandle *handle) = 0;
    virtual ~State_Command();
    int nextSize;
};

class Head : public State_Command
{
public:
    Head();
    virtual State_Command* Operation(unsigned char *data,RWHandle *handle) override;
    virtual ~Head();
};

class One_Mode: public State_Command{
public:
    One_Mode();
    virtual State_Command* Operation(unsigned char *data,RWHandle *handle) override;
    virtual ~One_Mode() override;
};

class Two_Mode: public State_Command{
public:
    Two_Mode();
    virtual State_Command* Operation(unsigned char *data,RWHandle *handle) override;
    virtual ~Two_Mode() override;
};

class Three_Mode: public State_Command{
public:
    Three_Mode();
    virtual State_Command* Operation(unsigned char *data,RWHandle *handle) override;
    virtual ~Three_Mode() override;
};

#endif // FSM_H
