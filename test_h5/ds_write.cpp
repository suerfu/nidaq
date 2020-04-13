
#include <iostream>
#include <string>
#include <sstream>
#include "hdf5.h"

#include <chrono> 
using namespace std::chrono;

#define H5FILE_NAME        "haha.h5"
#define DATASETNAME "IntArray"
#define NX     5                      /* dataset dimensions */
#define NY     6
#define RANK   2


int main ( int argc, char* argv[]){

    hid_t       file, dataset;         /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[2];              /* dataset dimensions */
    herr_t      status;
    float         data[NX][NY];          /* data to write */
    int         i, j;

    /*
     * Data  and output buffer initialization.
     */
    for(j = 0; j < NX; j++)
    for(i = 0; i < NY; i++)
        data[j][i] = 0.01*(i + j);
    /*
     * 0 1 2 3 4 5
     * 1 2 3 4 5 6
     * 2 3 4 5 6 7
     * 3 4 5 6 7 8
     * 4 5 6 7 8 9
     */

    /*
     * Create a new file using H5F_ACC_TRUNC access,
     * default file creation properties, and default file
     * access properties.
     */
    file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset.
     */
    dimsf[0] = NX;
    dimsf[1] = NY;
    dataspace = H5Screate_simple(RANK, dimsf, NULL);

    /*
     * Define datatype for the data in the file.
     * We will store little endian INT numbers.
     */
    datatype = H5Tcopy(H5T_NATIVE_FLOAT);
    status = H5Tset_order(datatype, H5T_ORDER_LE);

    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
    dataset = H5Dcreate2(file, DATASETNAME, H5T_NATIVE_FLOAT, dataspace,
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Write the data to the dataset using default transfer properties.
     */
    status = H5Dwrite(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);

    return 0;

}

/*
int main ( int argc, char* argv[]){

    if( argc<4 ){
        std::cout << "\nusage: " << argv[0] << "   filename.h5    Nchan    Nsamp    Nsec(=1)\n" << std::endl;
        return -1;
    }

    int Nwrite = 1;
    if( argc>4)
        Nwrite = atoi(argv[4]);

    hid_t       file_id, group_id, dataset_id, dataspace_id; 
    hsize_t     dims[2];
    herr_t      status;

    file_id = H5Fcreate( argv[1], H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    dims[0] = atoi( argv[2] ); 
    dims[1] = atoi( argv[3] ); 
    dataspace_id = H5Screate_simple(2, dims, NULL);

    float** dset_data = new float*[dims[0]];
    for( int i=0; i<dims[0]; i++){
        dset_data[i] = new float[dims[1]];
        for( int j=0; j<dims[1]; j++){
            dset_data[i][j] = 0.1*(i+j);
        }
    }

    auto start = high_resolution_clock::now();

    for( unsigned int i=0; i<Nwrite; i++){
        std::stringstream ss;
        ss << "trace_" << i;

        dataset_id = H5Dcreate2( file_id, ss.str().c_str(), H5T_NATIVE_FLOAT, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        status = H5Dwrite( dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data);
    }

    auto stop = high_resolution_clock::now();
    //auto duration = duration_cast<milliseconds>(stop - start);
    auto duration = duration_cast<microseconds>(stop - start);
    std::cout << duration.count()/1000 << std::endl;


    status = H5Dclose(dataset_id);

    status = H5Sclose(dataspace_id);

    status = H5Fclose(file_id);

    return 0;
}
*/
