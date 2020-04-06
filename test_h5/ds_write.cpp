
#include <iostream>
#include <string>
#include <sstream>
#include "hdf5.h"

#include <chrono> 
using namespace std::chrono;

int main ( int argc, char* argv[]){

    if( argc<4 ){
        std::cout << "\nusage: " << argv[0] << "   filename.h5    Nchan    Nsamp    Nsec(=1)\n" << std::endl;
        return -1;
    }

    int Nwrite = 1;
    if( argc>4)
        Nwrite = atoi(argv[4]);

    hid_t       file_id, group_id, dataset_id, dataspace_id;  /* identifiers */
    hsize_t     dims[2];
    herr_t      status;

    /* Create a new file using default properties. */
    file_id = H5Fcreate( argv[1], H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /* Create the data space for the dataset. */
    dims[0] = atoi( argv[2] ); 
    dims[1] = atoi( argv[3] ); 
    dataspace_id = H5Screate_simple(2, dims, NULL);

    int** dset_data = new int*[dims[0]];
    for( int i=0; i<dims[0]; i++){
        dset_data[i] = new int[dims[1]];
        for( int j=0; j<dims[1]; j++){
            dset_data[i][j] = i+j;
        }
    }

    auto start = high_resolution_clock::now();

    for( unsigned int i=0; i<Nwrite; i++){
        std::stringstream ss;
        ss << "trace_" << i;

        /* Create the dataset. */
        dataset_id = H5Dcreate2( file_id, ss.str().c_str(), H5T_STD_I32BE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        /* Write the dataset. */
        status = H5Dwrite( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data);
    }

    auto stop = high_resolution_clock::now();
    //auto duration = duration_cast<milliseconds>(stop - start);
    auto duration = duration_cast<microseconds>(stop - start);
    std::cout << duration.count()/1000 << std::endl;


    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);

    /* Terminate access to the data space. */ 
    status = H5Sclose(dataspace_id);

    /* Close the file. */
    status = H5Fclose(file_id);

    return 0;
}

