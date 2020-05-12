
#include <iostream>

#include <vector>

#include <string>
#include <sstream>

#include "hdf5.h"

herr_t H5WriteIntAttr( hid_t id, std::string attr_name, std::vector<int> a ){

    if( a.empty() )
        return 0;

    hsize_t dims = a.size();
    hid_t   ds_id = H5Screate_simple( 1, &dims, NULL);

    hid_t   attr_id = H5Acreate2( id, attr_name.c_str(), H5T_STD_I32BE, ds_id, H5P_DEFAULT, H5P_DEFAULT);

    herr_t  status = H5Awrite( attr_id, H5T_NATIVE_INT, &a[0]);
    
    H5Aclose( attr_id );
    H5Sclose( ds_id );

    return status;
}


herr_t H5WriteStringAttr( hid_t id, std::string attr_name, std::string a ){

    hid_t aid3  = H5Screate( H5S_SCALAR );
    hid_t atype = H5Tcopy( H5T_C_S1 );
                  H5Tset_size( atype, a.size() );
                  H5Tset_strpad( atype, H5T_STR_NULLTERM );

    hid_t attr3 = H5Acreate2( id, attr_name.c_str(), atype, aid3, H5P_DEFAULT, H5P_DEFAULT);

    hid_t status = H5Awrite(attr3, atype, a.c_str());

    H5Aclose( attr3 );
    H5Sclose( aid3 );

    return status;
}


int main ( int argc, char* argv[]){

    if( argc<2 ){
        std::cout << "\nusage: " << argv[0] << "   filename.hdf5\n" << std::endl;
        return -1;
    }

    herr_t      status;

    /* Create the data space for the dataset. */
    std::cout << "creating output file...\n";
    hid_t   file_id = H5Fcreate( argv[1], H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /* Write attribute */
    std::vector<int> array;
    array.push_back(10);
    array.push_back(10);
    H5WriteIntAttr( file_id, "timestamp", array);

    std::string s = argv[0];
    for( int i=1; i<argc; i++ ){
        s += ' ';
        s += argv[i];
    }
    H5WriteStringAttr( file_id, "ver.", s);

    /* Close file */
    status = H5Fclose( file_id );
    
    return 0;
/*    
    std::cout << "creating group file...\n";
    //hid_t   group_id = H5Gcreate2( file_id, "/info", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    hid_t   group_id = file_id;

    std::cout << "creating dataspace for attribute...\n";
    hsize_t file_attr_dims = 1;
    hid_t   file_ds_id = H5Screate_simple( 1, &file_attr_dims, NULL);
    
    std::cout << "creating attribute file...\n";
    hid_t   file_attr_id = H5Acreate2( group_id, "/timestamp", H5T_STD_I32BE, file_ds_id, H5P_DEFAULT, H5P_DEFAULT);
    
    int timestamp = 1234567;
    std::cout << "writing attribute...\n";
    status = H5Awrite( file_attr_id, H5T_NATIVE_INT, &timestamp);
    
    std::cout << "closing attribute...\n";
    status = H5Aclose( file_attr_id );
    
    std::cout << "closing dataspace...\n";
    status = H5Sclose( file_ds_id );
    
    std::cout << "closing group...\n";
    //status = H5Gclose( group_id );
    
    std::cout << "closing file...\n";
    status = H5Fclose( file_id );
*/
    return 0;

    //
    hsize_t     dims[2] = {3,4};
    
    hid_t dataspace1_id = H5Screate_simple(2, dims, NULL);

    int** dset_data = new int*[dims[0]];
    for( int i=0; i<dims[0]; i++){
        dset_data[i] = new int[dims[1]];
        for( int j=0; j<dims[1]; j++){
            dset_data[i][j] = i+j;
        }
    }



    /*
    for( unsigned int i=0; i<2; i++){
        std::stringstream ss;
        ss << "trace_" << i;
        dataset_id = H5Dcreate2( file_id, ss.str().c_str(), H5T_STD_I32BE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        status = H5Dwrite( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data);
    }
    */

    return 0;
}

