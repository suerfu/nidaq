#include "plrsBaseData.h"
#include "HDF5Recorder.h"

#include <ctime>
#include <vector>

extern "C" HDF5Recorder* create_HDF5Recorder( plrsController* c ){ return new HDF5Recorder(c);}


extern "C" void destroy_HDF5Recorder( HDF5Recorder* p ){ delete p;}


HDF5Recorder::HDF5Recorder( plrsController* c) : plrsStateMachine(c){

    dump_index = 0;
    bytes_written = 0;

    name_dataset = "/adc1";
    name_detconfig = "/detconfig1";
    name_event = "event_";
}


HDF5Recorder::~HDF5Recorder(){;}


void HDF5Recorder::Configure(){

    // Configure output file name.
    // The filename is prefix_DYYYYMMDD_Thhmmss_xxxx.hdf5
    // where YYYY - year, MM -  month, DD - day
    // hh - hour (24-hr), mm - minutes, ss - seconds

    if( cparser->Find("/adc/prefix") )
        file_prefix = cparser->GetString( "/cmdl/prefix", "");
    else
        file_prefix = cparser->GetString( "/adc/prefix", "default");

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


    // Configure the global part of the header.
    // Get a sample data from DAQ module and use it to configure metadata.
    void* rdo = 0;
    while( rdo==0 ){
        rdo = PullFromBuffer();
    }
    ConfigureOutput( reinterpret_cast<NIDAQdata*>(rdo) );

	PushToBuffer( addr_nxt, rdo);

}


void HDF5Recorder::Deconfigure(){
    CloseOutput();
    return;
}


void HDF5Recorder::PreRun(){
/*
    if( output_file ){
        if( cparser->GetString("/cmdl/no-header")=="" && cparser->GetString("/module/"+GetModuleName()+"/no-header")=="" ){
            cparser->Print( output_file, "# " );

            time_t rawtime;
            struct tm* loctime;
            time( &rawtime );
            loctime = localtime( &rawtime);

            output_file << "# " << asctime( loctime) << endl;
        }
    }
*/
    return;
}


void HDF5Recorder::Run(){
    return;
/*
    void* rdo = PullFromBuffer();

    while( GetState()==RUN ){
        if( rdo==0 ){
            rdo = PullFromBuffer();
	    if( rdo==0 )
	    	continue;
        }
	    if( output_file ){

            NIDAQdata* data = reinterpret_cast< NIDAQdata* >(rdo);
            if( data->FmtI16() )
                output_file.write( reinterpret_cast<char*>(data->buffer), sizeof(int16)*data->buffsize );
            else
                output_file.write( reinterpret_cast<char*>(data->buffer), sizeof(float64)*data->buffsize );
	    }

	    PushToBuffer( addr_nxt, rdo);
	    rdo = 0;
    }
*/
}


void HDF5Recorder::CloseOutput(){
    H5CloseGroup( id_detconfig );
    H5CloseGroup( id_dataset );
    H5CloseFile( id_file );
}


void HDF5Recorder::ConfigureOutput( NIDAQdata* header){

    bytes_written = 0;

    // Update filename and increment dump index.
    filename = GetFileName( file_prefix, dump_index );
    dump_index ++;


    // Create output file.
    id_file = H5CreateFile( filename );

    // Add timestamp, software version and user note to the global header.
    vector<unsigned int> time = { ctrl->GetTimeStamp() };
    H5AddUIntAttr( id_file, "timestamp", time);

    vector<int> ver = {1,0,0};
    H5AddIntAttr( id_file, "version", ver);

    string note = "";
    vector<string> note_arr = cparser->GetStrArray("/cmdl/note");
    for( unsigned int i=0; i< note_arr.size(); i++){
        if( i ==0 )
            note = note_arr[i];
        else
            note += " " + note_arr[i];
    }
    H5AddStringAttr( id_file, "note", note);


    // Add ADC configuration from the header of the input file.
    id_detconfig = H5CreateGroup( id_file, name_detconfig);

    // Add dataset
    id_dataset = H5CreateGroup( id_file, name_dataset);


    // Add data shape to ADC dataset.
    vector<int> temp;
    temp.push_back( header->GetNChannels() );
    temp.push_back( header->GetBufferPerChannel() );
    H5AddIntAttr( id_dataset, name_dataset+"/shape", temp);
    temp.clear();


    // Add index of channels to metadata
    H5AddIntAttr( id_dataset, name_dataset+"/index", header->GetChannelIndex() );


    // Add voltage range as Nx2 matrix.
    hsize_t dim[2] = { header->GetNChannels(), 4 };
    float* vrange = new float[ dim[0]*dim[1] ];

    dim[1] = 2;
    for( unsigned int j=0; j<dim[0]; j++ ){
        vrange[0+dim[1]*j] = (header->GetVmin())[j];
        vrange[1+dim[1]*j] = (header->GetVmax())[j];
        cout << j << '\t' << vrange[0+dim[1]*j] <<'\t'<< vrange[0+dim[1]*j] << endl;
    }
    H5AddFloatMatrixAttr( id_dataset, name_dataset+"/vrange", dim, vrange );
    //H5AddFloatAttr( id_dataset, name_dataset+"/vrange", dim[1], vrange[0] );


    // Add sampling frequency
    vector<float> ftemp;
    ftemp.push_back( header->GetClockFrequency() );
    H5AddFloatAttr( id_dataset, name_dataset+"/fsamp", ftemp);
    ftemp.clear();

    // Add calibration coefficient.
    dim[1] = 4;
    for( unsigned int j=0; j<dim[0]; j++ ){
        for( unsigned int k=0; k<dim[1]; k++){
            vrange[k+dim[1]*j] = header->GetCalCoeff()[j][k];
            cout << j << '\t' << k << '\t' << vrange[k+dim[1]*j]  << endl;
        }
    }
    H5AddFloatMatrixAttr( id_dataset, name_dataset+"/calib", dim, vrange );

    delete[] vrange;

    // Add DAQ mode
    H5AddStringAttr( id_dataset, name_dataset+"/mode", cparser->GetString("/adc/mode") );

    return;
}

    
    
herr_t HDF5Recorder::H5AddIntAttr( hid_t id, std::string attr_name, std::vector<int> a ){

    if( a.empty() )
        return 0;

    hsize_t dims = a.size();
    hid_t   ds_id = H5Screate_simple( 1, &dims, NULL);
    hid_t   attr_id = H5Acreate2( id, attr_name.c_str(), H5T_NATIVE_INT, ds_id, H5P_DEFAULT, H5P_DEFAULT);
    herr_t  status = H5Awrite( attr_id, H5T_NATIVE_INT, &a[0]);
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}


herr_t HDF5Recorder::H5AddUIntAttr( hid_t id, std::string attr_name, std::vector<unsigned int> a ){

    if( a.empty() )
        return 0;

    hsize_t dims = a.size();
    hid_t   ds_id = H5Screate_simple( 1, &dims, NULL);
    hid_t   attr_id = H5Acreate2( id, attr_name.c_str(), H5T_NATIVE_UINT, ds_id, H5P_DEFAULT, H5P_DEFAULT);
    herr_t  status = H5Awrite( attr_id, H5T_NATIVE_UINT, &a[0]);
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}


herr_t HDF5Recorder::H5AddFloatAttr( hid_t id, std::string attr_name, std::vector<float> a ){

    if( a.empty() )
        return 0;

    hsize_t dims = a.size();
    hid_t   ds_id = H5Screate_simple( 1, &dims, NULL);
    hid_t   attr_id = H5Acreate2( id, attr_name.c_str(), H5T_NATIVE_FLOAT, ds_id, H5P_DEFAULT, H5P_DEFAULT);
    herr_t  status = H5Awrite( attr_id, H5T_NATIVE_FLOAT, &a[0]);
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}



herr_t HDF5Recorder::H5AddFloatAttr( hid_t id, std::string attr_name, hsize_t d, float a[] ){

    hsize_t dims = d;
    hid_t   ds_id = H5Screate_simple( 1, &dims, NULL);
    hid_t   attr_id = H5Acreate2( id, attr_name.c_str(), H5T_NATIVE_FLOAT, ds_id, H5P_DEFAULT, H5P_DEFAULT);
    herr_t  status = H5Awrite( attr_id, H5T_NATIVE_FLOAT, a);
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}



herr_t HDF5Recorder::H5AddFloatMatrixAttr( hid_t id, std::string attr_name, hsize_t dims[], float* a ){

    hid_t ds_id = H5Screate_simple( 2, dims, NULL);
    //hid_t ds_id = H5Screate( H5S_SIMPLE );
    //H5Sset_extent_simple( ds_id, 2, dims, NULL);

    hid_t   attr_id = H5Acreate2( id, attr_name.c_str(), H5T_NATIVE_FLOAT, ds_id, H5P_DEFAULT, H5P_DEFAULT);
    herr_t  status = H5Awrite( attr_id, H5T_NATIVE_FLOAT, a);
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}



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

