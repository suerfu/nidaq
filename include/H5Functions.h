//----------- Utility functions related to HDF5-------//

#ifndef H5FUNCTIONS_H
#define H5FUNCTIONS_H 1

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "hdf5.h"


//----------- File operations ------------------------//
hid_t H5CreateFile( string file_name );

herr_t H5CloseFile( hid_t file_id );


//----------- Group operations -----------------------//
hid_t H5CreateGroup( hid_t file_id, string grp_name);

herr_t H5CloseGroup( hid_t id );


//----------- Dataspace operations ------------------//
hid_t H5CreateDataSpace( unsigned int d );

hid_t H5CreateDataSpace( unsigned int d, unsigned int e );


//----------- Functions to add attributes ----------//

template< typename T >
int AddAttribute( hid_t ID, string attr_name, T a, hid_t type ){

    hid_t   ds_id = H5CreateDataSpace( 1 );
    hid_t   attr_id = H5Acreate2( ID, attr_name.c_str(), type, ds_id, H5P_DEFAULT, H5P_DEFAULT );
    herr_t  status = H5Awrite( attr_id, type, &a );
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}



template< typename T >
int AddAttribute( hid_t ID, string attr_name, unsigned int dim, T* a, hid_t type ){
    
    hid_t   ds_id = H5CreateDataSpace( dim );
    hid_t   attr_id = H5Acreate2( ID, attr_name.c_str(), type, ds_id, H5P_DEFAULT, H5P_DEFAULT );
    herr_t  status = H5Awrite( attr_id, type, a );
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}


template< typename T >
int AddAttribute( hid_t ID, string attr_name, unsigned int nrow, unsigned int ncol, T* a, hid_t type ){

    hid_t   ds_id = H5CreateDataSpace( nrow, ncol );
    hid_t   attr_id = H5Acreate2( ID, attr_name.c_str(), type, ds_id, H5P_DEFAULT, H5P_DEFAULT );
    herr_t  status = H5Awrite( attr_id, type, a );
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}


template< typename T >
int AddAttribute( hid_t ID, string attr_name, vector<T> a, hid_t type ){
    return AddAttribute<T>( ID, attr_name, a.size(), &a[0], type);
}


template< typename T >
hid_t AddDataset( hid_t ID, string ds_name, unsigned int nrow, unsigned int ncol, T* data, hid_t type){

    hid_t ds_id = H5CreateDataSpace( nrow, ncol );

    hid_t dataset = H5Dcreate2( ID, ds_name.c_str(), type, ds_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Dwrite( dataset, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    H5Sclose( ds_id );

    return dataset;
}


#endif
