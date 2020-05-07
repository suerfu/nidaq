
#include "plrsBaseData.h"
#include "HDF5Recorder.h"

#include <ctime>
#include <vector>

#include <unistd.h>

extern "C" HDF5Recorder* create_HDF5Recorder( plrsController* c ){ return new HDF5Recorder(c);}


extern "C" void destroy_HDF5Recorder( HDF5Recorder* p ){ delete p;}


HDF5Recorder::HDF5Recorder( plrsController* c) : plrsStateMachine(c){

    dump_index = 0;
    bytes_written = 0;
    event_counter = 0;

    name_dataset = "adc1";
    name_detconfig = "detconfig1";
    name_event = "event_";
}


HDF5Recorder::~HDF5Recorder(){}


void HDF5Recorder::Configure(){

    // obtain ID/address of the module to which to send data to.
    string next_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/next", "");
        // if not found, returns default value of ""
    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );   // nonnegative if valid
    }
    if( next_module=="" || next_addr<0 ){
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }


    // Configure output file name.
    // The filename is prefix_DYYYYMMDD_THHMMSS_Fxxxx.hdf5

    // Find beginning part of file prefix from config file
    //
    if( cparser->Find("/cmdl/f") )
        file_prefix = cparser->GetString( "/cmdl/f", "");
    else
        file_prefix = cparser->GetString( "/adc/prefix", "default");


    // Next get current time in the desired format.
    //
    time_t t = ctrl->GetTimeStamp();
    struct tm tm = *localtime(&t);

    // File prefix does not change between different segments.
    stringstream ss;
    ss << file_prefix;
    ss << "_D" << tm.tm_year+1900;
    ss << setfill('0') << setw(2) << tm.tm_mon+1 << tm.tm_mday;
    ss << "_T" << tm.tm_hour << tm.tm_min << tm.tm_sec <<"_";
    file_prefix = ss.str();


    // Get maximum size per file
    // Default is 1 GB
    max_bytes_per_file = cparser->GetInt("/adc/max_size", 1000);
    max_bytes_per_file *= 1000000;


    // Configure the global part of the header.
    // Get a sample data from DAQ module and use it to configure metadata.
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

    ConfigureOutput( reinterpret_cast<NIDAQdata*>(rdo) );

	PushToBuffer( next_addr, rdo);
}


void HDF5Recorder::Deconfigure(){
    CloseOutput();
    return;
}


void HDF5Recorder::PreRun(){}


void HDF5Recorder::Run(){

    void* rdo = PullFromBuffer();
    if( rdo==0 )
        return;
        
    if( bytes_written>=max_bytes_per_file ){

        CloseOutput();

        bytes_written = 0;
        dump_index++;

        ConfigureOutput( reinterpret_cast<NIDAQdata*>(rdo) );
    }
    
    // Write the data to HDF5 file.
    NIDAQdata* data = reinterpret_cast< NIDAQdata* >(rdo);

    stringstream ss;
    ss << name_event << event_counter;
    AddDataset<int16>( ID_dataset, ss.str(), data->GetNChannels(), data->GetBufferPerChannel(), data->GetBufferMem(), H5T_NATIVE_INT16);

    int bytes_read = 2*data->GetNChannels()*data->read;
        // read is sample per channel, convert to total bytes.

	PushToBuffer( next_addr, rdo);

    event_counter++;
    bytes_written += bytes_read;

    return;
}


void HDF5Recorder::CloseOutput(){
    H5CloseGroup( ID_detconfig );
    H5CloseGroup( ID_dataset );
    H5CloseFile( ID_file );
}


void HDF5Recorder::ConfigureOutput( NIDAQdata* header){

    // Update filename and increment dump index.
    filename = GetFileName( file_prefix, dump_index );

    Print("Opening file "+filename+" for output\n", INFO);

    // ---------------------
    // Create output file.
    // ---------------------
    ID_file = H5CreateFile( filename );


    // ---------------------
    // Add timestamp, software version and user note.
    // ---------------------
    AddAttribute<unsigned int>( ID_file, "timestamp", ctrl->GetTimeStamp() , H5T_NATIVE_UINT);

    unsigned int ver[3] = {1,0,0};
    AddAttribute<unsigned int>( ID_file, "version", 3, ver, H5T_NATIVE_UINT);

/*
    string note = "";
    vector<string> note_arr = cparser->GetStrArray("/cmdl/note");
    for( unsigned int i=0; i< note_arr.size(); i++){
        if( i ==0 )
            note = note_arr[i];
        else
            note += " " + note_arr[i];
    }
    AddStringAttr( id_file, "note", note);
*/


    // ---------------------
    // Add ADC configuration from the header of the input file.
    // ---------------------
    ID_detconfig = H5CreateGroup( ID_file, name_detconfig);

    // Add dataset
    ID_dataset = H5CreateGroup( ID_file, name_dataset);


    // ---------------------
    // Add data shape to ADC dataset.
    // ---------------------
    unsigned int shape[2] = { header->GetNChannels(), header->GetBufferPerChannel()};
    AddAttribute<unsigned int>( ID_dataset, "/"+name_dataset+"/shape", 2, shape, H5T_NATIVE_UINT);


    // ---------------------
    // Add index of channels to metadata
    // ---------------------
    AddAttribute<unsigned int>( ID_dataset, "/"+name_dataset+"/index", header->GetChannelIndex(), H5T_NATIVE_UINT );


    // ---------------------
    // Add voltage range as Nx2 matrix.
    // ---------------------
    hsize_t dim[2] = { header->GetNChannels(), 4 };
    float* vrange = new float[ dim[0]*dim[1] ];

    for( unsigned int j=0; j<dim[0]; j++ ){
        vrange[0+dim[1]*j] = (header->GetVmin())[j];
        vrange[1+dim[1]*j] = (header->GetVmax())[j];
    }
    AddAttribute<float>( ID_dataset, "/"+name_dataset+"/vrange", dim[0], 2, vrange, H5T_NATIVE_FLOAT );


    // ---------------------
    // Add sampling frequency
    // ---------------------
    AddAttribute<float>( ID_dataset, "/"+name_dataset+"/fsamp", header->GetClockFrequency(), H5T_NATIVE_FLOAT );


    // ---------------------
    // Add calibration coefficient.
    // ---------------------
    for( unsigned int j=0; j<dim[0]; j++ ){
        for( unsigned int k=0; k<dim[1]; k++){
            vrange[k+dim[1]*j] = header->GetCalCoeff()[j][k];
        }
    }
    AddAttribute<float>( ID_dataset, "/"+name_dataset+"/calib", dim[0], dim[1], vrange, H5T_NATIVE_FLOAT );

    delete[] vrange;

    // Add DAQ mode
    //H5AddStringAttr( ID_dataset, "/"+name_dataset+"/mode", cparser->GetString("/adc/mode") );

    return;
}

    
/*    


herr_t HDF5Recorder::H5AddStringAttr( hid_t id, std::string attr_name, std::string a ){

    hid_t aid3  = H5Screate( H5S_SCALAR );
    hid_t atype = H5Tcopy( H5T_C_S1 );
                  H5Tset_size( atype, a.size()+1 );
                  H5Tset_strpad( atype, H5T_STR_NULLTERM );
    hid_t attr3 = H5Acreate2( id, attr_name.c_str(), atype, aid3, H5P_DEFAULT, H5P_DEFAULT);
    hid_t status = H5Awrite(attr3, atype, a.c_str());

    H5Aclose( attr3 );
    H5Sclose( aid3 );

    return status;
}
*/

// Returns the full filename including the dump index and extension.
string HDF5Recorder::GetFileName( string prefix, int index ){

    stringstream ss;
    ss << prefix;
    ss << 'F' << setfill('0') << setw(4) << index << ".hdf5";
    return ss.str();

}

