
#include <iostream>
#include <string>
#include "hdf5.h"

#define FILENAME    "extend.h5"
#define DATASETNAME "ExtendibleArray"
#define RANK         2

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

    hid_t        file;                          /* handles */
    hid_t        dataspace, dataset;  
    hid_t        filespace, memspace;
    hid_t        prop;                     

    hsize_t      dims[2]  = { atoi(argv[2]), atoi(argv[3]) };           /* dataset dimensions at creation time */     
    hsize_t      maxdims[2] = {H5S_UNLIMITED, H5S_UNLIMITED};
    herr_t       status;                             
    hsize_t      chunk_dims[2] = {1, dims[1]};
    int**        data =  new int*[ dims[0]];
    for( int i=0; i<dims[0]; i++){
        data[i] =  new int[ dims[1]];
        for( int j=0; j<dims[1]; j++){
            data[i][j] = 0;
        }
    }

    auto start = high_resolution_clock::now();

    /* Create the data space with unlimited dimensions. */
    dataspace = H5Screate_simple (RANK, dims, maxdims); 

    /* Create a new file. If file exists its contents will be overwritten. */
    file = H5Fcreate ( argv[1], H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /* Modify dataset creation properties, i.e. enable chunking  */
    prop = H5Pcreate (H5P_DATASET_CREATE);
    status = H5Pset_chunk (prop, RANK, chunk_dims);

    /* Create a new dataset within the file using chunk creation properties.  */
    dataset = H5Dcreate2 (file, "extendible", H5T_NATIVE_INT, dataspace, H5P_DEFAULT, prop, H5P_DEFAULT);

    /* Write data to dataset */
    status = H5Dwrite (dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /* Variables used in extending and writing to the extended portion of dataset */
    hsize_t      size[2];
    size[0] = dims[0];
    size[1] = dims[1];

    for( int i=0; i<Nwrite; i++){

        /* Extend the dataset. Dataset becomes 10 x 3  */
        size[0] += dims[0];
        status = H5Dset_extent (dataset, size);

        /* Select a hyperslab in extended portion of dataset  */
        hsize_t      offset[2];
        offset[0] = (i+1)*dims[0];
        offset[1] = 0;
        filespace = H5Dget_space (dataset);
        status = H5Sselect_hyperslab (filespace, H5S_SELECT_SET, offset, NULL, dims, NULL);  

        /* Define memory space */
        memspace = H5Screate_simple (RANK, dims, NULL); 

        /* Write the data to the extended portion of dataset  */
        status = H5Dwrite (dataset, H5T_NATIVE_INT, memspace, filespace, H5P_DEFAULT, data);

        status = H5Sclose (memspace);
    }

    /* Close resources no longer needed*/
    status = H5Dclose (dataset);    
    status = H5Sclose (dataspace);
    status = H5Pclose (prop);
    status = H5Sclose (filespace);
    status = H5Fclose (file);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    return 0;
}
