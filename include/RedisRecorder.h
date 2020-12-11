//////////////////////////////////////////////////////////////
// 
// Class Name:      RedisRecorder
// Author (s):      B. Suerfu ( suerfu@berkeley.edu)
// Date      :      May 10, 2019
// Description:     polaris module to send data to redis db.
//
//////////////////////////////////////////////////////////////


#ifndef REDISRECORDER_H
    #define REDISRECORDER_H 1

#include "plrsStateMachine.h"
#include "plrsController.h"
#include "NIDAQdata.h"

#include <sw/redis++/redis++.h>

#include <string>
#include <sstream>

using namespace std;
using Redis = sw::redis::Redis;

class RedisRecorder : public plrsStateMachine{

public:
    //----------- Constructors and destructor ------------//
    RedisRecorder( plrsController* c);    //!< Constructor.
    virtual ~RedisRecorder();  //!< Destructor

private:
    //----------- Mandatory polaris virtual functions ----//
    void Configure();
    void PreRun();
    void Run();
    void PostRun();
    void Deconfigure();

private:
    //----------- Private functions for communicating with redis //
    Redis* Connect( const string& host, int port, const string& password);
    void Disconnect();

    void SetRunState(int state);
    int GetRunState();

    void SetEventNum(int event_num);
    int GetEventNum();
  
    void SetTransitionRequest(int transition_request);
    int GetTransitionRequest();
 
    void WriteRunInfo(const std::string& key, int val);
    void WriteRunInfo(const std::string& key, const std::string& val_str);
    string ReadRunInfo(const std::string& key);
  
    // --------------------------
    // Data Stream
    // --------------------------
    void WriteData(int16_t* data, int num_channels, int num_samples);
    void GetData();

private:
    //----------- Private variables for storing run information.
    
    int next_addr;
  
    Redis* redis;

    string stream_name_;
};

extern "C" RedisRecorder* create_RedisRecorder( plrsController* c);

extern "C" void destroy_RedisRecorder( RedisRecorder* p );


#endif
