
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
}


/// Destructor. Nothing needs to be done.
NIdaq::~NIdaq(){}



void NIdaq::Configure(){

    Print("Configuring NI DAQ card...\n", DETAIL);

    // Find out which channel(s) to use.
    chan = cparser->GetString("/module/"+GetModuleName()+"/channel");
    if( chan=="" ){
        chan = "Dev1/ai0";
        Print( "Cannot find /module/"+GetModuleName()+"/channel, using default (Dev1/ai0)\n", ERR);
    }
    Print( "Channels configured as "+chan+"\n", INFO);

    // Find out how many channels are in use.
    if( chan.find(":")==string::npos ){
        nchan = 1;
            // if no range is speciied in channels, then number of channels is 1
    }
    else{
        // find out the channel number of the beginning channel
        size_t delim = chan.find(":");
        size_t range0_low = chan.find("ai");
        size_t range0_cap = chan.find("AI");
        size_t range0 = (range0_low!=string::npos) ? range0_low : range0_cap;
        if( range0==string::npos ){
            Print("Channel might be in a wrong format. It should be Dev?/ai?\n", ERR);
            SetStatus(ERROR);
        }
        string sub0 = chan.substr( range0+2, delim-range0-2);

        // channel number of the ending channel.
        string sub1;
        size_t range1_low = chan.find("ai", delim); // search from the delimiting character.
        size_t range1_cap = chan.find("AI", delim); // search from the delimiting character.
        size_t range1 = ( range1_low!=string::npos ) ? range1_low : range1_cap;

        if( range1==string::npos ){  // channel of the form Dev1/ai0:4
            sub1 = chan.substr( delim+1, chan.size()-delim);
        }
        else{
            sub1 = chan.substr( range1+2, chan.size()-delim);
        }
        nchan = stoi(sub1) - stoi(sub0) + 1;
        if( nchan<0 )
            nchan = -nchan;
    }

    // Create task
    error = DAQmxCreateTask( "", &task);
    if( error<0 ){
        stringstream msg;
        msg << "Cannot create task due to error " << error << "\n";
        Print( msg.str(), ERR);
        SetStatus(ERROR);
        return;
    }
    
    // Configure input channel.
    Vmin = cparser->GetFloat("/module/"+GetModuleName()+"/Vmin", -10);
    Vmax = cparser->GetFloat("/module/"+GetModuleName()+"/Vmax", 10);
    
    DAQmxCreateAIVoltageChan( task, chan.c_str(), "", DAQmx_Val_Cfg_Default, Vmin, Vmax, DAQmx_Val_Volts, NULL);
    //DAQmxCreateAIVoltageChan( task, chan.c_str(), "", DAQmx_Val_PseudoDiff, Vmin, Vmax, DAQmx_Val_Volts, NULL);


    // Configure sampling clock.
    sample_freq = cparser->GetFloat("/module/"+GetModuleName()+"/freq", 1000.);
    buff_size = cparser->GetFloat("/module/"+GetModuleName()+"/buff_per_chan", 10000);

    mode = DAQmx_Val_ContSamps;

    DAQmxCfgSampClkTiming( task, "", sample_freq, DAQmx_Val_Rising, mode, buff_size);


    // How to group data, either by channel or by scan number
    bool32 group;
    if( cparser->GetBool("/module/"+GetModuleName()+"/groupbychan", true) ){
        group = DAQmx_Val_GroupByChannel;
    }
    else{
        group = DAQmx_Val_GroupByScanNumber;
    }

    // Configure I16 or F64
    bool useI16 = true;
    if( ! cparser->GetBool("/module/"+GetModuleName()+"/int16", true) )
        useI16 = false;

    // fill in circular FIFO buffer with resources.
    int id = ctrl->GetIDByName( this->GetModuleName() );
    for( int i=0; i<buff_depth; ++i ){
        PushToBuffer( id, new NIDAQdata( nchan, useI16, buff_size, sample_freq, group) );
    }

    for( unsigned int i=0; i<nchan; i++){
        float64 cal_coeff[8] = {0,0,0,0,0,0,0,0};
        stringstream ss;
        ss << "Dev1/ai" << i;
        DAQmxGetAIDevScalingCoeff( task, ss.str().c_str(), cal_coeff, 8 );
        for( int j=0; j<8; j++)
            ss << ": " << cal_coeff[j] << " ";
        ss << "\n";
        Print( ss.str(), INFO);
    }

    Print("ADC configured.\n",DETAIL);

    return;
}



void NIdaq::Deconfigure(){
    Print( "Closing NI DAQ card...\n", DETAIL);
    DAQmxClearTask( task );
}



void NIdaq::PreRun(){
    Print( "DAQ starting\n", DETAIL);
    start_time = ctrl->GetMSTimeStamp();
    DAQmxStartTask( task );
}



void NIdaq::Run(){
    
    void* rdo = 0;

    while( GetState()==RUN ){
    	if( rdo==0 ){
            rdo = PullFromBuffer();
	    if( rdo==0 ){
            Print("Failed to get buffer\n", INFO);
	    	continue;
            }
        }

        NIDAQdata* data = reinterpret_cast<NIDAQdata*>(rdo);
        if( data->FmtI16() )
            error = DAQmxReadBinaryI16( task, data->buff_per_chan, -1, data->group, data->GetBufferI16(), data->buffsize, &(data->read), NULL);
        else
            error = DAQmxReadAnalogF64( task, data->buff_per_chan, -1, data->group, data->GetBufferF64(), data->buffsize, &(data->read), NULL);

	    if( error==0 ){
    	    PushToBuffer( addr_nxt, rdo);
            rdo = 0;
	    }
	    else if( error==-200284 ){
            Print("Buffer not ready\n", INFO);
	        continue;
	        // buffer not ready yet.
	    }
	    else{
	        stringstream ss;
	        ss << "Error reading from buffer. Error code " << error << "\n";
            char errBuff[2048];
            DAQmxGetExtendedErrorInfo(errBuff,2048);
            ss << errBuff << "\n";
	        Print( ss.str(), ERR);
	        PushToBuffer( ctrl->GetIDByName(this->GetModuleName()), rdo);
                // return pointer to buffer to self.
	        SetStatus(ERROR);
            break;
	    }
//        sched_yield();
    }
}



void NIdaq::PostRun(){
    DAQmxStopTask( task );
    Print( "DAQ stopping\n", DETAIL);
}



