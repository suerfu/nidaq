#include "plrsBaseData.h"
#include "HDF5Recorder.h"

#include <ctime>
#include <vector>

#include <unistd.h>

extern "C" HDF5Recorder* create_HDF5Recorder( plrsController* c ){ return new HDF5Recorder(c);}

extern "C" void destroy_HDF5Recorder( HDF5Recorder* p ){ delete p;}

const string HDF5Recorder::daq_version = "1.0.0";

const string HDF5Recorder::format_version = "1.0.0"; 


HDF5Recorder::HDF5Recorder( plrsController* c) : plrsStateMachine(c){

    facility_num = 0;   // default facility number

    dump_index = 1;     // Dump index start counting from 1.

    bytes_written = 0;

    event_counter = 0;
    event_id = 0;
    max_event_per_file = 100000;
        // Default number of events in each dump. This is not usually editable through config file.

    max_MB_per_file = 1000;
        // Default number of mega bytes per file.
    MB = 1024*1024;

    // the following variables are defined such that naming conventions can be easily changed in the future.
    name_dataset = "adc1";
    name_event = "event_";
    name_detconfig = "detconfig1";    
}



HDF5Recorder::~HDF5Recorder(){}



void HDF5Recorder::Configure(){

    Print("configuring...\n", DEBUG);
    
    // =========================================================
    // Initialize HDF5 manager. Data will be written in ConfigureOutput..
    // =========================================================
    h5man = new H5FileManager();
    Print("Initialized HDF5 file manager.\n", DEBUG);

    // =========================================================
    // Obtain ID/address of the module to which to send data to.
    // =========================================================
    next_addr = GetNextModuleID();
    Print("Data loop set up.\n", DEBUG);

    max_MB_per_file = GetConfigParser()->GetInt("/max_MB_per_file", max_MB_per_file);

    // =========================================================
    // Get output file name.
    // =========================================================

    // Get filre prefix from either commandline or configuration file.
    file_prefix = GetFilePrefix();

    // Get global timestamp to be used in the series of dumps.
    timestamp_g = ctrl->GetTimeStamp();

    // Get Facility ID
    facility_num = GetConfigParser()->GetInt("facility", facility_num);

    // Concatenates and get the file name.
    filename = GetFileName( file_prefix, facility_num, timestamp_g, dump_index );


    // =========================================================
    // Open output HDF5 file. Report error if fail.
    // =========================================================

    Print("Opening file "+filename+" for output...\n", INFO);
    if( h5man->OpenFile( filename, "w+")==false ){
        Print("Failed to create HDF5 file for output.\n", ERR);
        SetStatus(ERROR);
        return;
    }
    Print("File successfully opened for output.\n", DEBUG);


    // =========================================================
    // Get a template data object from DAQ module and use it to configure metadata.
    // =========================================================

    Print("Configuring HDF5 file metadata using empty data template...\n", DEBUG);
    void* rdo = 0;
    while( rdo==0 && GetState()==CONFIG ){
        rdo = PullFromBuffer();
    }

    // If state is no longer CONFIG due to error, return the memory and exit
    if( GetState()!=CONFIG ){
        if( rdo!=0 )
    	    PushToBuffer( next_addr, rdo);
        return;
    }

    // Use the empty data template to configure the output meta data.
    ConfigureOutput( reinterpret_cast<NIDAQdata*>(rdo) );
    Print("HDF5 file metadata configured.\n", DEBUG);

    // After configuring the output, give the data to the next module.
	PushToBuffer( next_addr, rdo);
    
    Print( this->GetModuleName()+" configured.\n", DEBUG);
}



void HDF5Recorder::Deconfigure(){
    CloseOutput();
    return;
}



void HDF5Recorder::PreRun(){}


void HDF5Recorder::PostRun(){
    AddAttribute( "/" + name_dataset, "nb_events", event_counter );
    CloseOutput();
}


void HDF5Recorder::Run(){

    // Obtain a valid/ready pointer to data object from circular FIFO buffer.
    void* rdo = PullFromBuffer();
    if( rdo==0 )
        return;
    
    // If either the bytes written is more than the maximum specified, or event number exceeds the maximum event,
    // close the current output and opens another HDF5 with dump number incremented.
    //
    // It is dangerous to use GetFileSize from HDF5 app due to overflow at around 4 GB. Instead use long long int to count ourselves.
    // cout << h5man->GetFileSize()*100 << '\t' << h5man->GetFileSize()/MB << '\t' << max_MB_per_file << '\t' << (h5man->GetFileSize()/MB >= max_MB_per_file) << endl; 

    if( bytes_written >= MB*max_MB_per_file || event_counter+1 >= max_event_per_file ){
        AddAttribute( "/" + name_dataset, "nb_events", event_counter );
        CloseOutput();

        // reset event and byte counters, increment dump index but do not touch event id (which is global).
        event_counter = 0;
        bytes_written = 0;
        dump_index++;

        filename = GetFileName( file_prefix, facility_num, timestamp_g, dump_index );
        
        Print("Opening file "+filename+" for output\n", INFO);
        if( h5man->OpenFile( filename, "w+")==false ){
            Print("Failed to create HDF5 file for output.\n", ERR);
            SetStatus(ERROR);
            return;
        }

        ConfigureOutput( reinterpret_cast<NIDAQdata*>(rdo) );
        Print("Output file successfully configured.\n", DEBUG);
    }

    // =========================================================
    // Write the data to HDF5 file.
    // =========================================================

    // Since the readout pointer is of type void*, first cast it to NIDAQdata type.
    NIDAQdata* data = reinterpret_cast< NIDAQdata* >(rdo);

    // Configure the name of the event and write the 2D data to HDF5 file.
    stringstream ss;
    ss << "/" << name_dataset << "/" << name_event << event_counter+1;
    string event_name = ss.str();

    dim[0] = data->GetNChannels();
    dim[1] = data->GetBufferPerChannel();

    Print("Writing event data...\n", DEBUG);
    bool b =  h5man->WriteData<int16>( data->GetBufferMem(), event_name, H5::PredType::NATIVE_INT16, 2, dim);
    Print("Event data written.\n", DEBUG);

    if( b==false ){
        Print( "Failed to write event data.\n", ERR);
	    PushToBuffer( next_addr, rdo);
        SetStatus(ERROR);
        return;
    }
    else{
        // Add necessary metadata to the dataset.
        AddAttribute( event_name, "event_index", event_counter+1 );
        AddAttribute( event_name, "event_id", event_id+1 );
        AddAttribute( event_name, "event_num", dump_index*max_event_per_file+event_counter+1 );
        AddAttribute( event_name, "event_time", ctrl->GetTimeStamp() );
        Print("Event attribute added.\n", DEBUG);
    }

    // Return the data pointer back to DAQ module (or the next module in the pipeline).
	PushToBuffer( next_addr, rdo);

    // Below is obsolete since for bytes written, currently HDF5 built-in functions are used.
    int bytes_read = 2*data->GetNChannels()*data->read;
        // read is sample per channel, convert to total bytes.

    event_counter++;
    event_id++;
    bytes_written += bytes_read;

    return;
}



void HDF5Recorder::CloseOutput(){
    h5man->CloseFile();
}



void HDF5Recorder::ConfigureOutput( NIDAQdata* header ){

    // ================================================
    // Global/File-wide Attributes
    // ================================================
    Print( "Configuring global attributes...\n", DEBUG);

    AddAttribute( "/daq_version", daq_version );
    AddAttribute( "/format_version", format_version );

    string comment_cfg =  GetConfigParser()->GetString("/comment");
    string comment_cmd =  GetConfigParser()->GetString("/cmdl/comment");
    AddAttribute( "/comment", comment_cfg+" "+comment_cmd );

    AddAttribute( "/facility", facility_num );
    AddAttribute( "/run_type", facility_num );  // NOTE: load from config.
    AddAttribute( "/dump_num", dump_index );
    if( dump_index==0 )
        AddAttribute( "/timestamp", timestamp_g );
    else{
        timestamp_l = ctrl->GetTimeStamp();
        AddAttribute( "/timestamp", timestamp_l );
    }
    AddAttribute( "/series_num", GetSeriesNumber( facility_num, timestamp_g) );

    Print( "Global attributes configured.\n", DEBUG);

    // ================================================
    // Attributes under ADC Group
    // ================================================
    
    string group_name = name_dataset;
    Print( "Configuring attributes under group " + group_name + "...\n", DEBUG);

    h5man->OpenGroup( "/"+group_name );
    Print( "Created group " + group_name + ".\n", DEBUG );

    AddAttribute( "/"+group_name, "sample_rate", header->GetClockFrequency() );
    Print( "Added sample_rate.\n", DEBUG);
    AddAttribute( "/"+group_name, "nb_channels", header->GetNChannels() );
    Print( "Added nb_channels.\n", DEBUG);
    AddAttribute( "/"+group_name, "nb_samples", header->GetBufferPerChannel() );
    Print( "Added nb_samples.\n", DEBUG);
    AddAttribute( "/"+group_name, "adc_channel_indices", header->GetChannelIndex() );
    Print( "Added adc_channel_indices.\n", DEBUG);
    AddAttribute( "/"+group_name, "data_mode", header->GetDataMode() );
    Print( "Added data_mode.\n", DEBUG);
    // Need to update number of event later
    AddAttribute( "/"+group_name, "adc_name", GetConfigParser()->GetStrArray("/"+group_name+"/adc_name") );
    Print( "Added adc_name.\n", DEBUG);

    const int rank = 2;
    unsigned int dim[2];

    // ---------------------
    // Add voltage range as Nx2 matrix.
    // ---------------------

    dim[0] = header->GetNChannels();
    dim[1] = 2;
    vector<float> foo;
    vector<float> vmin = header->GetVmin();
    vector<float> vmax = header->GetVmax();
    for( unsigned int i=0; i<vmax.size(); i++ ){
        foo.push_back( vmin[i] );
        foo.push_back( vmax[i] );
    }
    AddAttribute( "/"+group_name, "voltage_range", foo, rank, dim );
    Print( "Added voltage_range.\n", DEBUG);


    // ---------------------
    // Add calibration coefficient.
    // ---------------------
    foo.clear();
    dim[1] = 4;
    for( unsigned int j=0; j<dim[0]; j++ ){
        for( unsigned int k=0; k<dim[1]; k++){
            foo.push_back( header->GetCalCoeff()[j][k] );
        }
    }
    AddAttribute( "/"+group_name, "adc_conversion_factor", foo, rank, dim );
    Print( "Added adc_conversion_factor.\n", DEBUG);

    AddAttribute( "/"+group_name, "adc_connections", GetConfigParser()->GetStrArray("/"+group_name+"/adc_connections") );
//  The following code is prone to Seg fault if wrong numbers of parameters are specified in the config file.
//    vector<string> adc_conn = GetConfigParser()->GetStrArray("/"+group_name+"/adc_connections");
//    if( adc_conn.size()>0 ){
//        dim[1] = 2;
//        AddAttribute( "/"+group_name, "adc_connections", adc_conn, rank, dim);
//    }

    Print( "Added adc_connections.\n", DEBUG);
    
    Print( "Group attributes configured.\n", DEBUG);

    // ================================================
    // Attributes under detconfig1
    // ================================================

    group_name = name_detconfig;
    Print( "Configuring attributes under group " + group_name + "...\n", DEBUG);

    h5man->OpenGroup( "/"+group_name );

    AddAttribute( "/"+group_name, "device_name_list", GetConfigParser()->GetStrArray("/"+group_name+"/device_name_list") );
    AddAttribute( "/"+group_name, "adc_channel_indices", header->GetChannelIndex() );
    AddAttribute( "/"+group_name, "adc_conversion_factor", foo, rank, dim );

//  The following code is prone to Seg fault if wrong numbers of parameters are specified in the config file.
//  adc_con is previously defined, and is currently temporarily not supported.
//    if( adc_conn.size()>0 ){
//        dim[1] = 2;
//        AddAttribute( "/"+group_name, "adc_connections", adc_conn, rank, dim);
//    }

    vector<string> param_list;
    param_list.push_back( "tes_bias" );
    param_list.push_back( "squid_bias" );
    param_list.push_back( "preamp_voltage_bias" );
    param_list.push_back( "phi_bias" );
    param_list.push_back( "lock_point" );
    param_list.push_back( "output_offset" );
    param_list.push_back( "signal_gen_amplitude" );
    param_list.push_back( "signal_gen_frequency" );
    for( vector<string>::iterator itr = param_list.begin(); itr!=param_list.end(); itr++ ){
        AddAttribute( "/"+group_name, *itr, GetConfigParser()->GetFloatArray("/"+group_name+"/"+*itr) );
    }
    param_list.clear();

    param_list.push_back( "preamp_gain" );
    param_list.push_back( "lock_mode" );
    param_list.push_back( "output_gain" );
    param_list.push_back( "signal_gen_shape" ); // should it be string?
    for( vector<string>::iterator itr = param_list.begin(); itr!=param_list.end(); itr++ ){
        AddAttribute( "/"+group_name, *itr, GetConfigParser()->GetIntArray("/"+group_name+"/"+*itr) );
    }

    Print( "Group attributes configured.\n", DEBUG);

    return;
}



uint64_t HDF5Recorder::GetSeriesNumber( int facility_num, const time_t& tmstamp){

    // Series number consists of facility ID + the date and time when the DAQ was started.
    struct tm tm = *localtime( &tmstamp );

    stringstream ss;
    ss << facility_num;
    ss << tm.tm_year+1900;
    ss << setfill('0') << setw(2) << tm.tm_mon+1;
    ss << setfill('0') << setw(2) << tm.tm_mday;
    ss << setfill('0') << setw(2) << tm.tm_hour;
    ss << setfill('0') << setw(2) << tm.tm_min;
    ss << setfill('0') << setw(2) << tm.tm_sec;

    return stol( ss.str() );
}



string HDF5Recorder::GetFileName( string prefix, int fn, const time_t& t, int index ){

    struct tm tm = *localtime(&t);

    stringstream ss;
    ss << file_prefix;
    ss << "_I" << fn;
        // Facility ID
    ss << "_D" << tm.tm_year+1900;
    ss << setfill('0') << setw(2) << tm.tm_mon+1 << setfill('0') << setw(2) << tm.tm_mday;
        // Date
    ss << "_T" << setfill('0') << setw(2) << tm.tm_hour;
    ss << setfill('0') << setw(2) << tm.tm_min;
    ss << setfill('0') << setw(2) << tm.tm_sec;
        // Time
    ss <<"_F" << setfill('0') << setw(4) << index;
        // Dump number
    ss << ".hdf5";
        // File extension.

    return ss.str();
}



string HDF5Recorder::GetFilePrefix(){

    if( GetConfigParser()->Find("/cmdl/f") )
        file_prefix = GetConfigParser()->GetString( "/cmdl/f", "");
    else
        file_prefix = GetConfigParser()->GetString( "/prefix", "Default");
    return file_prefix;
}



int HDF5Recorder::GetNextModuleID(){

    int next_addr;

    string next_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/next", "");
        // if not found, returns default value of ""
    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );   // nonnegative if valid
    }
    if( next_module=="" || next_addr<0 ){
        Print("Next module not specified. Setting up loop-back.\n", INFO);
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }

    return next_addr;
}



