
#include "NIdaq.h"
#include "NIDAQdata.h"

#include <sstream>
#include <iostream>
#include <cstring>


extern "C" NIdaq* create_NIdaq( plrsController* c ){ return new NIdaq(c);}


extern "C" void destroy_NIdaq( NIdaq* p ){ delete p;}


extern "C" void PrintUsage(){
    std::cout << "\t-f,\n\t\tPrefix of output file name.\n";
    std::cout << "\t--event,\n\t\tNumber of events to record. Default is 2 billion.\n";
    std::cout << "\t--comment,\n\t\tComment from commandline to record in the HDF5 output.\n\t\tPair of \" is needed if there is space in the comment.\n\n";
}

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
    chan_array = cparser->GetStrArray("/"+modadc+"/channel");
    if( chan_array.empty() ){
        Print( "Cannot find /"+modadc+"/channel.\n", ERR);
        SetStatus(ERROR);
        return;
    }

    string chan_prefix = chan_array[0].substr(0, 7);
        // NOTE: Only one device is supported at the moment.
        // This way of extracting the Dev?/ai? part is valid up to 0 devices.

    channels = GetChannelsEnabled( chan_array );
        // NOTE: Channels specified by range. In the future, channel should also be specified by listing individual channels.
        // NOTE: It is not clear what happens if channels are specified in a wrong order.

    if( channels.empty() ){
        Print("Channel might be in a wrong format. It should be Dev?/ai?\n", ERR);
        SetStatus(ERROR);        
    }


    // ****************************************************
    // Create task
    // ****************************************************

    Print("Creating NI task...\n", DETAIL);
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
        Print("The number of Vmin/Vmax is different fromchannels enabled.\n", ERR);
        SetStatus(ERROR);
        return;
    }


    // ****************************************************
    // Configure input channel
    // ****************************************************
    
    Print("Configuring channels...\n", DETAIL);
    error = ConfigChannel( chan_prefix, channels, Vmin, Vmax );
    if( error < 0 ){
        stringstream ss;
        ss << "Failed to configure ADC channels. Error code " << error << "\n";

        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        ss << errBuff << "\n";

        Print( ss.str(), ERR);
        SetStatus(ERROR);
        return;
    }


    // ****************************************************
    // Configure sampling parameters.
    // This is common for all channels.
    // ****************************************************
    
    sample_freq = cparser->GetFloat("/"+modadc+"/sample_rate", 1000.);
    buff_per_chan = cparser->GetFloat("/"+modadc+"/nb_samples", 10000);

    trig_mode = cparser->GetString("/"+modadc+"/data_mode", "");

    // First check if mode is specified.
    if( trig_mode=="" ){
        Print( "/"+modadc+"/mode not specified.\n", ERR);
        SetStatus(ERROR);
        return;        
    }
    // Continuous mode.
    else if( trig_mode=="cont" ){
        nimode = DAQmx_Val_ContSamps;
    }
    // Finite mode, check if external trigger or internal software trigger
    else if( trig_mode=="trig-ext" || trig_mode=="trig-int" ){
        nimode = DAQmx_Val_FiniteSamps;
        // For ext mode, need the trigger input channel.
        if( trig_mode=="trig-ext" ){
            trig_channel = cparser->GetString( "/"+modadc+"/trigger_channel", "");
            if( trig_channel=="" ){
                Print( "Trigger channel not specified.\n", ERR);
                SetStatus(ERROR);
                return;
            }
        }
        // For int mode, need the period.
        else if( trig_mode=="trig-int" ){
            trig_period_us = cparser->GetInt( "/"+modadc+"/trigger_period_us", 0);
            if( trig_period_us==0 ){
                Print( "Trigger period not specified.\n", ERR);
                SetStatus(ERROR);
                return;
            }
        }
    }
    else{
        Print( "/"+modadc+"/mode not recognized.\n", ERR);
        SetStatus(ERROR);
        return;
    }

    
    // ****************************************************
    // Configure clock
    // This is common for all channels.
    // ****************************************************
    
    Print("Configuring clock...\n", DETAIL);
    error = ConfigClock( nimode, sample_freq, buff_per_chan);
    if( error < 0 ){
        stringstream ss;
        ss << "Failed to configure ADC clock. Error code " << error << "\n";

        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        ss << errBuff << "\n";

        Print( ss.str(), ERR);
        SetStatus( ERROR );

        return;
    }


    // ****************************************************
    // Configure trigger
    // ****************************************************
    Print("Configuring HW trugger...\n", DETAIL);
    error = ConfigTrigger( trig_mode, trig_channel );
    if( error < 0 ){
        stringstream ss;
        ss << "Failed to configure trigger. Error code " << error << "\n";

        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        ss << errBuff << "\n";

        Print( ss.str(), ERR);
        SetStatus( ERROR );

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

    Print("Allocating FIFO buffer...\n", DETAIL);
    for( int i=0; i<buff_depth; i++ ){
        
        // Create object to hold NI ADC configuration and data.
        // Then record clock frequency and voltage ranges.
        NIDAQdata* foo = new NIDAQdata( channels, buff_per_chan );
        foo->SetClockFrequency( sample_freq );
        foo->SetVoltageRange( Vmin, Vmax);
        foo->SetDataMode( nimode );

        // Get device calibration information and write them onto the NIDAQ object.
        for( unsigned int j=0; j<channels.size(); j++){
            stringstream ss;
            ss << chan_prefix << channels[j];
            error = DAQmxGetAIDevScalingCoeff( task, ss.str().c_str(), foo->GetCalCoeff()[j], 4 );

            if( error < 0 ){
                stringstream ss;
                ss << "Failed to get ADC conversion factor. Error code " << error << "\n";
                char errBuff[2048];
                DAQmxGetExtendedErrorInfo(errBuff,2048);
                ss << errBuff << "\n";
                Print( ss.str(), ERR);
                SetStatus( ERROR );
            }
        }

        // Hand one copy to recorder to configure metadata. Send the rest to DAQ module's own FIFO buffer.
        if( i!=0 ){
            Print("Sending empty data template to other modules...\n", DETAIL);
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

    DAQmxStartTask( task );
    
    start_time = ctrl->GetMSTimeStamp();
    if( trig_mode=="trig-int" ){
        ResetTimer();
    }
}



void NIdaq::Run(){

    // If max event has reached, inform the controller about maximum event and simply return.
    if( evt_counter>=max_evt){
        SendUserCommand( "/ctrl/max-evt");
        return;
    }

    // If trigger mode is internal periodic, then make sure the specified period has elapsed.
    if( trig_mode=="trig-int" ){
        if( GetTimerMicroSec()<trig_period_us ){
            return;
        }
        else{
            Print( "Triggering...\n", INFO);
            ResetTimer();
        }
    }

    // Get pointer to available memory. If failed, return and try again (in the next loop iteration).
    void* rdo = PullFromBuffer();
    if( rdo==0 )
        return;

    // In finite sample mode, first wait and check if ready.
    if( nimode==DAQmx_Val_FiniteSamps ){   
        bool32 done = false;
        do{
            DAQmxIsTaskDone( task, &done);
        }while( GetState()==RUN && done==false );
    }

    // First cast void* to something usable
    NIDAQdata* data = reinterpret_cast<NIDAQdata*>(rdo);

    // Readout actual data into the NIDAQdata object using the NI driver's function.
    error = DAQmxReadBinaryI16( task, data->GetBufferPerChannel(), -1, data->GetGroupMode(), data->GetBufferMem(), data->GetBufferSize(), &(data->read), NULL);

    // Check the error code.
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
    if( nimode==DAQmx_Val_FiniteSamps ){
        evt_counter++;
        if( evt_counter>= max_evt ){
            PushCommand(0, "max-evt");
            return;
        }
        else{
            // In finite sample mode, hardware setting is not kept between different runs. Need to start over.
            DAQmxClearTask( task );
            ReConfigure( chan_array );
            DAQmxStartTask( task );
        }
    }
}



void NIdaq::PostRun(){
    DAQmxStopTask( task );
    Print( "DAQ stopping\n", DETAIL);
}


void NIdaq::ReConfigure( vector<string> input ){

    if( channels.empty() ){
        Print("ReConfigure is invoked before channels are configured.\n", ERR);
        return;
    }
    if( nimode==DAQmx_Val_ContSamps ){
        Print("Warning: calling ReConfigure for continuous mode.\n", ERR);
    }

    string chan_prefix = input[0].substr(0, 7);

    if( CreateTask()<0 ){
        SetStatus(ERROR);   // Report error to polaris.
        return;
    }
    
    if( ConfigChannel( chan_prefix, channels, Vmin, Vmax )<0 ){
        SetStatus(ERROR);
        return;
    }

    if( ConfigClock( nimode, sample_freq, buff_per_chan) < 0 ){
        SetStatus(ERROR);
        return;
    }
//    error = DAQmxCfgDigEdgeStartTrig( task, "/Dev1/pfi0", DAQmx_Val_Rising);
}



int32 NIdaq::ConfigClock( int32 mod, float freq, float buff_pchan ){
    return DAQmxCfgSampClkTiming( task, "", freq, DAQmx_Val_Rising, mod, buff_pchan);
}



int32 NIdaq::ConfigTrigger( string trig_mode, string trig_channel){
    if( trig_mode!= "trig-ext" )
        return 0;
    return DAQmxCfgDigEdgeStartTrig( task, trig_channel.c_str(), DAQmx_Val_Rising);
}


int32 NIdaq::ConfigChannel( string prefix, vector<int> ch, vector<float> vmin, vector<float> vmax){
    
    int32 err;
    
    for( unsigned int c=0; c<ch.size(); c++){
        stringstream chan_name;
        chan_name << prefix << ch[c];
            // Channel name consists of Dev?/ai part + the actual index of channel(s)
            // A range of channels can be specified with : as Dev?/ai1:2
        err = DAQmxCreateAIVoltageChan( task, chan_name.str().c_str(), "", DAQmx_Val_Cfg_Default/*DAQmx_Val_Cfg_Default*/, vmin[c], vmax[c], DAQmx_Val_Volts, NULL);
            // Channel termination options are:
            //  DAQmx_Val_Cfg_Default
            //  DAQmx_Val_RSE
            //  DAQmx_Val_NRSE
            //  DAQmx_Val_Diff
            //  DAQmx_Val_PseudoDiff

        if(err<0)
            break;
    }    
    return err;
}



int32 NIdaq::CreateTask(){
    Print("Creating task...\n", DEBUG);
    int32 err = DAQmxCreateTask( "", &task);
    Print("Task created.\n", DEBUG);
    if( err<0 ){
        stringstream msg;
        msg << "Cannot create task due to error " << error << "\n";
        Print( msg.str(), ERR);
    }
    return err;
}



vector<int> NIdaq::GetChannelsEnabled( vector<string> in ){

    vector<int> ret;    // Return vector that consists of the indices of channels enabled.
    
    for( vector<string>::iterator itr = in.begin(); itr!=in.end(); itr++){

        string input = *itr;

        // If a range of channels are specified with :, find out how many are in use.
        if( input.find(":")==string::npos ){
            ret.push_back( input.back()-'0' );
            // return ret;
                // if no range is speciied in channels, the channel index is the last character
                // when dealing with an array, do not return.
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
                continue;
                // return ret;
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
//                cout << c << endl;
            }
        }
    }

    return ret;
}

