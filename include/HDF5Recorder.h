#ifndef HDF5RECORDER_H
    #define HDF5RECORDER_H 1

#include <string>
#include <sstream>

#include "plrsController.h"
#include "plrsModuleRecorder.h"

#include "NIDAQdata.h"

//#include "hdf5.h"
#include "H5FileManager.h"

using namespace std;


/// This module writes data to file on disk.
/// Default behavior ( if not overrided with polymorphism )

/// 1) output file name is configured in the following order:
/// --- specified with commandline option --file
/// --- specified in config file with /module/recorder/file

/// 2) after data is processed data is sent to the next module determined in the following order
/// --- specified with /module/record/next_module
/// --- search if there is interface module
/// --- search if there is graphics module
/// --- send data back to controller.

/// 3) default behavior of Run module is to interpret incoming void pointer as array of two floats and writes them separately in two columns.


class HDF5Recorder : public plrsStateMachine{

public:

    HDF5Recorder( plrsController* c);    //!< Constructor.

    virtual ~HDF5Recorder();  //!< Destructor

private:

    //----------- Mandatory polaris virtual functions ----//

    void Configure();

    void Deconfigure();

    void PreRun();

    void Run();
    
    void PostRun();


private:

    int next_addr;

    unsigned int dim[2];

    //----------- Action methods to configure outputs ----//

    void ConfigureOutput( NIDAQdata* d);
    
    void CloseOutput();

    int GetNextModuleID();
        //!< Retrieves the ID/address of next module.
        //!< If not specified, it returns the module's own address.

    string GetFilePrefix();
        //!< Get file prefix from config file or commandline, with commandline having higher priority.

    string GetFileName( string prefix, int fn, const time_t& tm_stamp,  int dmp_id );
        //!< Concatenates the dump index and extention to the prefix and returns it.
        //!< The filename is Prefix_Ix_DYYYYMMDD_THHMMSS_Fxxxx.hdf5

    string name_dataset;
        //!< HDF5 group name for ADC dataset.

    string name_event;
        //!< Prefix for naming each 

    string name_detconfig;
        //!< Group name for detector configuration.

    //----------- Variables to track progress

    int dump_index;

    unsigned long long bytes_written;
    unsigned long long max_MB_per_file;
    unsigned long long MB;

    unsigned int event_counter;
        // Local ID. Resets to 1 for each dump.
    unsigned int event_id;
        // Global ID. Continuous increment starting from 1.
    unsigned int max_event_per_file;

private:

    H5FileManager* h5man;

    string file_prefix;
    string filename;

    uint32_t timestamp_g;   // Global timestamp at the beginning of first dump.
    uint32_t timestamp_l;   // Local timestamp of each dump.

    int facility_num;

    static const string daq_version;
    static const string format_version;

    set<string> parameter_added;
        // Used to keep track of added parameters.
        // In the end, all other parameters not added will be added as strings.

    
    template< typename T>
    void AddAttribute(string  app_name, string attr_name, vector<T> attr, int rank=1, unsigned int dim[]={} ){
        h5man->AddAttribute<T>( app_name, attr_name, attr, rank, dim );
        parameter_added.insert(attr_name);
    }

    template< typename T>
    void AddAttribute(string  app_name, string attr_name, T attr ){
        h5man->AddAttribute<T>( app_name, attr_name, attr );
        parameter_added.insert(attr_name);
    }

    template< typename T>
    void AddAttribute( string full_name, T attr){
        h5man->AddAttribute<T>( GetDirectory(full_name), GetName(full_name), attr);
    }

    string GetDirectory( string foo ){
        if( foo[0]!='/' )
            foo = "/"+foo;
        string sub = foo.substr(0, foo.find_last_of('/'));
        if( sub=="" )
            sub = "/";
        return sub;
    }

    string GetName( string foo ){
        string sub = foo.substr(foo.find_last_of('/')+1);
        if( sub=="" )
            sub = foo;
        return sub;
    }

    uint64_t GetSeriesNumber( int facility_num, const time_t& tmstamp);
};

extern "C" HDF5Recorder* create_HDF5Recorder( plrsController* c);

extern "C" void destroy_HDF5Recorder( HDF5Recorder* p );


#endif
