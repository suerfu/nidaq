
#include "NIdaq.h"
#include "NIDAQdata.h"

#include <sstream>
#include <iostream>
#include <cstring>


extern "C" NIdaq* create_NIdaq( plrsController* c ){ return new NIdaq(c);}


extern "C" void destroy_NIdaq( NIdaq* p ){ delete p;}


NIdaq::NIdaq( plrsController* ctrl) : plrsModuleDAQ( ctrl){
    buff_depth = 20;
        // NOTE: depth of FIFO buffer is set to be 20 by default.
    max_evt = 0;
    evt_counter = 0;
}



NIdaq::~NIdaq(){}



void NIdaq::Configure(){

    Print("Configuring NI DAQ card...\n", DETAIL);

    // Obtain ID/address of the module to which to send data to.
    // If not found, returns default value of ""
    string next_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/next", "");
    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );   // nonnegative if valid
    }
    if( next_module=="" || next_addr<0 ){
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }


    string modadc = "adc1";
        // NOTE: In current version, only one ADC card is supported.
        // For future multiple devices, make this part a for loop.

    // ***********************************
    // Find out max run number. Default to 2 billion
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

    // Original channel string specified by the user.
    // If channel is not specified, report error and exit.
    chan = cparser->GetString("/"+modadc+"/channel");
    if( chan=="" ){
        Print( "Cannot find /"+modadc+"/channel.\n", ERR);
        SetStatus(ERROR);
        return;
    }

    string chan_prefix = chan.substr(0, 7);
        // NOTE: Only one device is supported at the moment.
        // This way of extracting the Dev?/ai? part is valid up to 0 devices.

    channels = GetChannelsEnabled( chan );
        // NOTE: Channels specified by range. In the future, channel should also be specified by listing individual channels.
        // NOTE: It is not clear what happens if channels are specified in a wrong order.

    if( channels.empty() ){
        Print("Channel might be in a wrong format. It should be Dev?/ai?\n", ERR);
        SetStatus(ERROR);        
    }


    // ****************************************************
    // Create task
    // ****************************************************

    if( CreateTask()<0 ){
        Print("Failed to create NI new task.\n", ERR);
        SetStatus(ERROR);
        return;
    }
    

    // ****************************************************
    // Configure ranges for input channel
    // ****************************************************
    
    Vmin = cparser->GetFloatArray("/"+modadc+"/Vmin");
    Vmax = cparser->GetFloatArray("/"+modadc+"/Vmax");

    // if no value is specified, report error.
    if( Vmin.size()!=channels.size() || Vmax.size()!=channels.size() ){
        Print("The number of Vmin/Vmax is less than channels enabled.\n", ERR);
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
    
    sample_freq = cparser->GetFloat("/"+modadc+"/sample_rate", 1000.);
    buff_per_chan = cparser->GetFloat("/"+modadc+"/nb_samples", 10000);

    if( cparser->GetString("/"+modadc+"/data_mode", "")=="finite" ){
        mode = DAQmx_Val_FiniteSamps;
    }
    else if( cparser->GetString("/"+modadc+"/data_mode", "")=="cont" ){
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

    // Always use 16-bit form and group by channel. Therefore following code is obsolete.
    // bool32 group = DAQmx_Val_GroupByChannel;
    // bool useI16 = true;


    // Allocate memory and fill the circular FIFO buffer of this module with the pointers to the allocated memory.
    int id_self = ctrl->GetIDByName( this->GetModuleName() );
    int id_rec = next_addr;

    for( int i=0; i<buff_depth; i++ ){
        
        // Create object to hold NI ADC configuration and data.
        // Then record clock frequency and voltage ranges.
        NIDAQdata* foo = new NIDAQdata( channels, buff_per_chan );
        foo->SetClockFrequency( sample_freq );
        foo->SetVoltageRange( Vmin, Vmax);
        foo->SetDataMode( mode );

        // Get device calibration information and write them onto the NIDAQ object.
        for( unsigned int j=0; j<channels.size(); j++){
            stringstream ss;
            ss << chan_prefix << channels[j];
            DAQmxGetAIDevScalingCoeff( task, ss.str().c_str(), foo->GetCalCoeff()[j], 4 );
        }

        // Hand one copy to recorder to configure metadata. Send the rest to DAQ module's own FIFO buffer.
        if( i!=0 ){
            PushToBuffer( id_self, foo );
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
    Print( "DAQ starting\n", DETAIL);
    start_time = ctrl->GetMSTimeStamp();
    DAQmxStartTask( task );
}



void NIdaq::Run(){

    // If max event has reached, inform the controller about maximum event and simply return.
    if( evt_counter>=max_evt){
        SendUserCommand( "/ctrl/max-evt");
        return;
    }
    
    // Get pointer to available memory. If failed, return and try again (in the next loop iteration).
    void* rdo = PullFromBuffer();
    if( rdo==0 )
        return;

    // In finite sample mode, first wait and check if ready.
    if( mode==DAQmx_Val_FiniteSamps ){   
        bool32 done = false;
        do{
            DAQmxIsTaskDone( task, &done);
        }while( GetState()==RUN && done==false );
    }


    // Readout actual data into the NIDAQdata object using the NI driver's function.
    NIDAQdata* data = reinterpret_cast<NIDAQdata*>(rdo);
    error = DAQmxReadBinaryI16( task, data->GetBufferPerChannel(), -1, data->GetGroupMode(), data->GetBufferMem(), data->GetBufferSize(), &(data->read), NULL);

    // If read is Successful, hand the data over to next module (which is usually a recorder object).
    if( error==0 ){
        PushToBuffer( next_addr, rdo);
    }
    // Failed due to buffer not ready. This error is recoverable, pointer recycled to itself for reuse and do not report error.
    else if( error==-200284 ){
        Print("Buffer not ready\n", DETAIL);
        PushToBuffer( ctrl->GetIDByName(this->GetModuleName()), rdo);
        return;
    }
    // Other unknown errors. Print out error message and terminate.
    else{
        stringstream ss;
        ss << "Error reading from buffer. Error code " << error << "\n";
        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        ss << errBuff << "\n";

        Print( ss.str(), ERR);

        PushToBuffer( ctrl->GetIDByName(this->GetModuleName()), rdo);   // return pointer to buffer to self.
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
            // In finite sample mode, hardware setting is not kept between different runs. Need to start over.
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
        SetStatus(ERROR);   // Report error to polaris.
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



int32 NIdaq::ConfigChannel( string prefix, vector<int> ch, vector<float> vmin, vector<float> vmax){
    
    int32 err;
    
    for( unsigned int c=0; c<ch.size(); c++){
        stringstream chan_name;
        chan_name << prefix << ch[c];
            // Channel name consists of Dev?/ai part + the actual index of channel(s)
            // A range of channels can be specified with : as Dev?/ai1:2
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



vector<int> NIdaq::GetChannelsEnabled( string input ){

    vector<int> ret;    // Return vector that consists of the indices of channels enabled.

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

