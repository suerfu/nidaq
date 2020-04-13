#ifndef HDF5RECORDER_H
    #define HDF5RECORDER_H 1

#include <string>
#include <sstream>

#include "plrsController.h"
#include "plrsModuleRecorder.h"

#include "NIDAQdata.h"

#include "hdf5.h"

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

    //----------- Variables to track progress
    string file_prefix;
    int dump_index;
    string filename;

    string GetFileName( string prefix, int index ){
        stringstream ss;
        ss << prefix;
        ss << 'F' << setfill('0') << setw(4) << index << ".hdf5";
        return ss.str();
    }

    unsigned int bytes_written;
    unsigned int max_bytes_per_file;

    void ConfigureOutput( NIDAQdata* d);
    
    void CloseOutput();

    //----------- HDF5 specifid variables ----------------//

    hid_t id_file;
        // HDF5 output file identifier.

    hid_t id_detconfig;
        // HDF5 group identifier for detector configuration.
    string name_detconfig;
        // Detector configuration group name.
    
    hid_t id_dataset;
        // HDF5 group identifier for ADC dataset.
    string name_dataset;
        // Group name for ADC data.

    string name_event;
        // Naming convention for data.

    unsigned int evt_counter;
        // Event counter.

    //----------- Utility functions related to HDF5-------//

    hid_t H5CreateFile( string file_name){
        return H5Fcreate( file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    }
        // Encapsulate parameters which are unlikely to change in a dedicated file to make function simpler.
    
    herr_t H5CloseFile( hid_t file_id ){
        return H5Fclose( file_id ); 
    }

    hid_t H5CreateGroup( hid_t file_id, string grp_name){
        return H5Gcreate2( file_id, grp_name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    
    herr_t H5CloseGroup( hid_t id ){
        return H5Gclose( id);
    }

    
    herr_t H5AddIntAttr( hid_t id, std::string attr_name, std::vector<int> a );
    herr_t H5AddUIntAttr( hid_t id, std::string attr_name, std::vector<unsigned int> a );
        // id is the HDF5 ID of the object to which attribute is attached.
    

    herr_t H5AddFloatAttr( hid_t id, std::string attr_name, hsize_t dim, float a[] );
    herr_t H5AddFloatAttr( hid_t id, std::string attr_name, std::vector<float> a );
        // id is the HDF5 ID of the object to which attribute is attached.
    
    
    herr_t H5AddFloatMatrixAttr( hid_t id, std::string attr_name, hsize_t dim[], float* a );

    
    herr_t H5AddStringAttr( hid_t id, std::string attr_name, std::string a );
        // id is the HDF5 ID of the object to which attribute is attached.

    
    hid_t H5CreateDataSpace( NIDAQdata h);
        // HDF5 dataspace ID doesn't change, so the same object can be reused.
    
    
    herr_t H5WriteData( hid_t id, string name, NIDAQdata h );
};

extern "C" HDF5Recorder* create_HDF5Recorder( plrsController* c);

extern "C" void destroy_HDF5Recorder( HDF5Recorder* p );


#endif
