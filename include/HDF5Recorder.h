#ifndef HDF5RECORDER_H
    #define HDF5RECORDER_H 1

#include <string>
#include <sstream>

#include "plrsController.h"
#include "plrsModuleRecorder.h"

#include "NIDAQdata.h"

#include "hdf5.h"
#include "H5Functions.h"

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
    virtual void PreRun();
    void Run();

private:

    int next_addr;

    //----------- Variables to track progress
    string file_prefix;
    string filename;

    int dump_index;
    unsigned long long bytes_written;
    unsigned long long max_bytes_per_file;
    unsigned long long event_counter;


    //----------- Action methods to configure outputs ----//

    void ConfigureOutput( NIDAQdata* d);
    
    void CloseOutput();


    //----------- HDF5 specifid variables ----------------//

    hid_t ID_file;
        // HDF5 output file identifier.

    hid_t ID_detconfig;
        // HDF5 group identifier for detector configuration.
    string name_detconfig;
        // Detector configuration group name.
    
    hid_t ID_dataset;
        // HDF5 group identifier for ADC dataset.
    string name_dataset;
        // Group name for ADC data.

    string name_event;
        // Naming convention for data.


    string GetFileName( string prefix, int index );

};

extern "C" HDF5Recorder* create_HDF5Recorder( plrsController* c);

extern "C" void destroy_HDF5Recorder( HDF5Recorder* p );


#endif
