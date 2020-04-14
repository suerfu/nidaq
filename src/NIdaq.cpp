
#include "NIdaq.h"
#include "NIDAQdata.h"

#include <sstream>
#include <iostream>
#include <cstring>


/// creator function for loading the module.
extern "C" NIdaq* create_NIdaq( plrsController* c ){ return new NIdaq(c);}


/// destructor function for releasing the module.
extern "C" void destroy_NIdaq( NIdaq* p ){ delete p;}


/// Constructor. buff_depth will control depth of FIFO buffer.
NIdaq::NIdaq( plrsController* ctrl) : plrsModuleDAQ( ctrl){
    buff_depth = 20;
    max_evt = 0;
    evt_counter = 0;
}


/// Destructor. Nothing needs to be done.
NIdaq::~NIdaq(){}



void NIdaq::Configure(){

    Print("Configuring NI DAQ card...\n", DETAIL);

    string modadc = "adc";
    // ***********************************
    // Find out max run number. Default to 100
    // ***********************************
    
    max_evt = cparser->GetInt("/cmdl/event", 0);
        // max_evt is unsigned int. Do not set it to negative values.
    if( max_evt==0 )
        max_evt = cparser->GetInt("/"+modadc+"/event", 0);
    if( max_evt==0 )
        max_evt = 2000000000;


    // ***********************************
    // Find out which channel(s) to use.
    // ***********************************

    chan = cparser->GetString("/"+modadc+"/channel");
        // original channel string specified by the user.

    // if channel is not specified, report error and exit.
    if( chan=="" ){
        Print( "Cannot find /"+modadc+"/channel.\n", ERR);
        SetStatus(ERROR);
        return;
    }

    string chan_prefix = chan.substr(0, 7);

    channels = GetChannelsEnabled( chan );
    if( channels.empty() ){
        Print("Channel might be in a wrong format. It should be Dev?/ai?\n", ERR);
        SetStatus(ERROR);        
    }


    // ****************************************************
    // Create task
    // ****************************************************

    if( CreateTask()<0 ){
        SetStatus(ERROR);
        return;
    }
    

    // ****************************************************
    // Configure ranges for input channel
    // ****************************************************
    
    Vmin = cparser->GetFloatArray("/"+modadc+"/Vmin");
    Vmax = cparser->GetFloatArray("/"+modadc+"/Vmax");
        // if no value is specified, use the largest range as default.

    if( Vmin.size()!=channels.size() || Vmax.size()!=channels.size() ){
        Print("Number of Vmin/Vmax is smaller than channels.\n", ERR);
        SetStatus(ERROR);
        return;
    }


    // ****************************************************
    // Configure input channel
    // ****************************************************
    
    if( ConfigChannel( chan_prefix, channels, Vmin, Vmax )<0 ){
        SetStatus(ERROR);
        return;
    }


    // ****************************************************
    // Configure sampling parameters.
    // This is common for all channels.
    // ****************************************************
    
    sample_freq = cparser->GetFloat("/"+modadc+"/freq", 1000.);
    buff_per_chan = cparser->GetFloat("/"+modadc+"/buff_per_chan", 10000);

    if( cparser->GetString("/"+modadc+"/mode", "")=="finite" ){
        mode = DAQmx_Val_FiniteSamps;
    }
    else if( cparser->GetString("/"+modadc+"/mode", "")=="cont" ){
        mode = DAQmx_Val_ContSamps;
    }
    else{
        Print("/"+modadc+"/mode not recognized or not specified.\n", ERR);
        SetStatus(ERROR);
        return;
    }

    
    // ****************************************************
    // Configure clock
    // This is common for all channels.
    // ****************************************************
    
    if( ConfigClock( mode, sample_freq, buff_per_chan) < 0 ){
        SetStatus(ERROR);
        return;
    }


    // ****************************************************
    // Allocate memory for reading data.
    // ****************************************************

    // Always use 16-bit form and group by channel.
    // bool32 group = DAQmx_Val_GroupByChannel;
    // bool useI16 = true;


    // Allocate memory and fill the circular FIFO buffer of this module with the pointers to the allocated memory.
    int id = ctrl->GetIDByName( this->GetModuleName() );
    int id_rec = addr_nxt;

    for( int i=0; i<buff_depth; ++i ){

        NIDAQdata* foo = new NIDAQdata( channels.size(), buff_per_chan );
        foo->SetClockFrequency( sample_freq );
        foo->SetChannelIndex( channels );
        foo->SetVoltageRange( Vmin, Vmax);

        // Get device calibration information
        for( unsigned int j=0; j<channels.size(); j++){
            stringstream ss;
            ss << chan_prefix << channels[j];
            DAQmxGetAIDevScalingCoeff( task, ss.str().c_str(), foo->GetCalCoeff()[j], 4 );
        }

        // Hand one copy to recorder to configure metadata.
        if(i!=0){
            PushToBuffer( id, foo );
        }
        else{
            PushToBuffer( id_rec, foo );
        }
    }

    Print("DAQ configured.\n", DETAIL);

    return;
}



void NIdaq::Deconfigure(){
    Print( "Closing NI DAQ card...\n", DETAIL);
    DAQmxClearTask( task );
}



void NIdaq::PreRun(){
/*
    Print( "DAQ starting\n", DETAIL);
    start_time = ctrl->GetMSTimeStamp();
    DAQmxStartTask( task );
*/
}



void NIdaq::Run(){

    // this function will be called repeatedly in a while-loop as long as STATE is RUN

    // if max event has reached, simply return
    if( evt_counter>=max_evt)
        return;
    

    // first get pointer to available memory
    void* rdo = 0;
    do{
        rdo = PullFromBuffer();
    }while ( rdo==0 && GetState()==RUN );


    // in finite sample mode, first wait and check if ready.
    if( mode==DAQmx_Val_FiniteSamps ){   
        bool32 done = false;
        do{
            DAQmxIsTaskDone( task, &done);
        }while( GetState()==RUN && done==false );
    }


    // readout actual data
    NIDAQdata* data = reinterpret_cast<NIDAQdata*>(rdo);
    error = DAQmxReadBinaryI16( task, data->GetBufferPerChannel(), -1, data->GetGroupMode(), data->GetBufferMem(), data->GetBufferSize(), &(data->read), NULL);

    if( error==0 ){     // Successful read.
        PushToBuffer( addr_nxt, rdo);
        rdo = 0;
    }
    else if( error==-200284 ){      // This error is recoverable
        Print("Buffer not ready\n", DETAIL);
        return;
        // buffer not ready yet.
    }
    else{   // Other unknown errors. Print out error message and terminate.
        stringstream ss;
        ss << "Error reading from buffer. Error code " << error << "\n";
        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        ss << errBuff << "\n";
        Print( ss.str(), ERR);
        PushToBuffer( ctrl->GetIDByName(this->GetModuleName()), rdo);
            // return pointer to buffer to self.
        SetStatus(ERROR);
        return;
    }

    // In a finite mode, after each read, configuration information is not kept, so reconfigure.
    if( mode==DAQmx_Val_FiniteSamps ){

        evt_counter++;
        if( evt_counter>= max_evt ){
            PushCommand(0, "max-evt");
            return;
        }
        else{
            // in finite sample mode, hardware setting is not kept between different runs.
            // have to start over.
            DAQmxClearTask( task );
            ReConfigure( chan );
            DAQmxStartTask( task );
        }
    }
}



void NIdaq::PostRun(){
    DAQmxStopTask( task );
    Print( "DAQ stopping\n", DETAIL);
}


void NIdaq::ReConfigure( string input ){

    if( channels.empty() ){
        Print("ReConfigure is invoked before channels are configured.\n", ERR);
        return;
    }
    if( mode==DAQmx_Val_ContSamps ){
        Print("Warning: calling ReConfigure for continuous mode.\n", ERR);
    }

    string chan_prefix = input.substr(0, 7);

    if( CreateTask()<0 ){
        SetStatus(ERROR);
        return;
    }
    
    if( ConfigChannel( chan_prefix, channels, Vmin, Vmax )<0 ){
        SetStatus(ERROR);
        return;
    }

    if( ConfigClock( mode, sample_freq, buff_per_chan) < 0 ){
        SetStatus(ERROR);
        return;
    }
}



int32 NIdaq::ConfigClock( int32 mod, float freq, float buff_pchan ){
    return DAQmxCfgSampClkTiming( task, "", freq, DAQmx_Val_Rising, mod, buff_pchan);
}



int32 NIdaq::ConfigChannel( string prefix, vector<unsigned int> ch, vector<float> vmin, vector<float> vmax){
    
    int32 err;
    
    for( unsigned int c=0; c<ch.size(); c++){
        stringstream chan_name;
        chan_name << prefix << ch[c];   //  Dev?/ai part + the actual index
        err = DAQmxCreateAIVoltageChan( task, chan_name.str().c_str(), "", DAQmx_Val_Cfg_Default, vmin[c], vmax[c], DAQmx_Val_Volts, NULL);

        if(err<0)
            break;
    }
    
    return err;
}



int32 NIdaq::CreateTask(){
    int32 err = DAQmxCreateTask( "", &task);
    if( err<0 ){
        stringstream msg;
        msg << "Cannot create task due to error " << error << "\n";
        Print( msg.str(), ERR);
    }
    return err;
}



vector<unsigned int> NIdaq::GetChannelsEnabled( string input ){

    vector<unsigned int> ret;

    // If a range of channels are specified with :, find out how many are in use.
    if( input.find(":")==string::npos ){
        ret.push_back( input.back()-'0' );
        return ret;
            // if no range is speciied in channels, the channel index is the last character
    }
    else{
        // find out the channel number of the beginning channel
        size_t delim = input.find(":");
        size_t range0_low = input.find("ai");
        size_t range0_cap = input.find("AI");
            // check for both lower case and capital case.

        size_t range0 = (range0_low!=string::npos) ? range0_low : range0_cap;
            // now range0 is the beginning location of first analog input
        if( range0==string::npos ){
            return ret;
        }

        string sub0 = input.substr( range0+2, delim-range0-2);
            // channel number is the second character since ai/AI up to the delim character

        // find the ending channel number after :
        string sub1;
        size_t range1_low = input.find("ai", delim); // search from the delimiting character.
        size_t range1_cap = input.find("AI", delim); // search from the delimiting character.
        size_t range1 = ( range1_low!=string::npos ) ? range1_low : range1_cap;

        if( range1==string::npos ){  // channel of the form Dev1/ai0:4
            sub1 = input.substr( delim+1, input.size()-delim);
        }
        else{
            sub1 = input.substr( range1+2, input.size()-delim);
        }

        // the beginning and ending index of channels are found. Fill the channels array.
        int order = stoi(sub1) > stoi(sub0) ? 1 : -1;
        for( int c = stoi(sub0); c!=stoi(sub1)+order; c+=order){
            // in termination condition, order is added because the end is inclusive.
            ret.push_back( c );
        }
    }

    return ret;
}

