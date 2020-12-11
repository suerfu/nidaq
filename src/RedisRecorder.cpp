
#include "RedisRecorder.h"
#include "NIDAQdata.h"

#include <ctime>
#include <vector>

#include <unistd.h>

extern "C" RedisRecorder* create_RedisRecorder( plrsController* c ){ return new RedisRecorder(c);}


extern "C" void destroy_RedisRecorder( RedisRecorder* p ){ delete p;}


RedisRecorder::RedisRecorder( plrsController* c) : plrsStateMachine(c){
    redis = 0;
    stream_name_ = "NIstream";
}


RedisRecorder::~RedisRecorder(){}


void RedisRecorder::Configure(){

    // =========================================================
    // obtain ID/address of the module to which to send data to.
    // =========================================================

    string next_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/next", "");
        // if not found, returns default value of ""
    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );   // nonnegative if valid
    }
    if( next_module=="" || next_addr<0 ){
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }


    // =========================================================
    // Instantiate a redis class and connect to the database
    // =========================================================

    redis = Connect("127.0.0.1", 6400, "");
    SetRunState(1);
    SetTransitionRequest(0);
}


void RedisRecorder::Deconfigure(){
    delete redis;
    redis = 0;
}


void RedisRecorder::PreRun(){
    while ( GetTransitionRequest()!=1 && GetState()==RUN ) {
	    usleep(100);
	}
}


void RedisRecorder::PostRun(){
    SetRunState(0);
    SetTransitionRequest(0);
}


void RedisRecorder::Run(){

    // =========================================================
    // First get available pointer from circular FIFO buffer
    // =========================================================
    void* rdo = PullFromBuffer();
    if( rdo==0 )
        return;
    

    // =========================================================
    // Write the data to Redis file.
    // =========================================================
    NIDAQdata* data = reinterpret_cast< NIDAQdata* >(rdo);

    // actual writing...
    // stringstream ss;
    // ss << name_event << event_counter;
    // AddDataset<int16>( ID_dataset, ss.str(), data->GetNChannels(), data->GetBufferPerChannel(), data->GetBufferMem(), H5T_NATIVE_INT16);

    // int bytes_read = 2*data->GetNChannels()*data->read;
        // read is sample per channel, convert to total bytes.

	PushToBuffer( next_addr, rdo);

    return;
}


Redis* RedisRecorder::Connect( const string& host, int port, const string& password){

    // configure connect option
    sw::redis::ConnectionOptions connection_options;
    connection_options.host = host;  
    connection_options.port = port; 
    
    // connection_options.password = "auth";
            //  Optional. No password by default.
    // connection_options.db = 1;
            //  Optional. Use the 0th database by default.

    connection_options.socket_timeout = std::chrono::milliseconds(1000);
            //  Optional. Timeout before we successfully send request to or receive response from redis.
            // By default, the timeout is 0ms, i.e. never timeout and block until we send or receive successfuly.
    
    return new Redis(connection_options);
}




void RedisRecorder::SetRunState(int state){
    WriteRunInfo("run_state", state);
}


int RedisRecorder::GetRunState(){
    string state_str = ReadRunInfo("run_state");
    return stoi(state_str);
}

void RedisRecorder::SetEventNum(int event_num){
    WriteRunInfo("event_num", event_num);
}


int RedisRecorder::GetEventNum(){
    string event_num_str = ReadRunInfo("event_num");
    return stoi(event_num_str);
}


void RedisRecorder::SetTransitionRequest(int transition_request){
    WriteRunInfo("transition_request", transition_request);
}


int RedisRecorder::GetTransitionRequest(){
    string transition_str = ReadRunInfo("transition_request");
    return stoi(transition_str);
}
 

void RedisRecorder::WriteRunInfo(const std::string& key, int val){
    string val_str = to_string(val);
    WriteRunInfo(key,val_str);
}


void RedisRecorder::WriteRunInfo(const std::string& key, const std::string& val){
    redis->hset("RunInfo",key,val);
}


string RedisRecorder::ReadRunInfo(const std::string& key){
    string val_str;
    auto val = redis->hget("RunInfo",key);
    if (val)
        val_str = *val;

    return val_str;
}


// void WriteData(int16_t* data, int num_channels, int num_samples);
// void GetData();
