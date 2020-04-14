

#include "H5Functions.h"

//----------- File operations ------------------------//
hid_t H5CreateFile( string file_name ){
    return H5Fcreate( file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
}

herr_t H5CloseFile( hid_t file_id ){
    return H5Fclose( file_id ); 
}


//----------- Group operations -----------------------//
hid_t H5CreateGroup( hid_t file_id, string grp_name){
    return H5Gcreate2( file_id, grp_name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
}

herr_t H5CloseGroup( hid_t id ){
    return H5Gclose( id);
}


//----------- Dataspace operations ------------------//
hid_t H5CreateDataSpace( unsigned int d ){
    hsize_t dim[1] = {d};
    return H5Screate_simple( 1, dim, NULL);
}

hid_t H5CreateDataSpace( unsigned int d, unsigned int e ){
    hsize_t dim[2] = {d, e};
    return H5Screate_simple( 2, dim, NULL);
}



