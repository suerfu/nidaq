
#include "H5FileManager.h"

#include <iostream>

using std::cout;
using std::endl;
using H5::Group;

H5FileManager::H5FileManager(){
    file_ptr = 0;
    filename = "";
}


H5FileManager::~H5FileManager(){
    // if file is still open, close it.
    if( file_ptr!=0 ){
        std::map<string, H5::Group*>::iterator itr;
        for( itr= list_group.begin(); itr!=list_group.end(); itr++){
            H5::Group* foo = itr->second;
            if( foo!=0 ){
                foo->close();
                delete foo;
                foo = 0;
            }
        }
        CloseFile();
        delete file_ptr;
        file_ptr = 0;
    }
}


bool H5FileManager::OpenFile( const string& name, const string& f){

    if( file_ptr!=0 )
        return false;

    unsigned int flag = H5F_ACC_RDWR;
    if( f=="r" )
        flag = H5F_ACC_RDONLY;
    else if( f=="w" )
        flag = H5F_ACC_EXCL;
    else if( f=="w+" )
        flag = H5F_ACC_TRUNC;

    file_ptr = new H5::H5File( name.c_str(), flag);
    filename = name;
    return true;
}


bool H5FileManager::CloseFile(){
    if( file_ptr!=0 ){
        file_ptr->close();
        file_ptr = 0;
        return true;
    }
    return false;
}


bool H5FileManager::OpenGroup( string name ){

    if( file_ptr==0  )
        return false;
    
    if( *(name.end()-1)=='/')
        name.erase( name.end()-1);

    std::size_t pos = 0;

    do{
        pos = name.find( '/', pos+1);   // positions of substrings separated by '/' in the name
        string gname = name.substr( 0, pos);    // group name

        std::map<string, Group*>::iterator itr = list_group.find( gname );
        if( itr==list_group.end() ){
            list_group[gname] = new Group( file_ptr->createGroup( gname.c_str()) );
        }
    }while( pos!=std::string::npos );
    
    return true;
}


bool H5FileManager::CloseGroup( const string& name){

    if( list_group.find( name )!=list_group.end() ){
        list_group[name]->close();
        return true;
    }
    return false;
}




/*
    // File info.
    unsigned int GetNDataSets( const string& name);

    // File operations

    // Group operations

    // Dataset operations
    bool CreateDataset( const string& name, unsigned int dims[]={1} );
    bool CloseDataset( const string& name );

    template<class datatype, class h5type>
    bool WriteData( datatype* data);

    // Attributes
    bool AddAttribute()

*/



template <>
bool H5FileManager::AddAttribute<int>( string app_name, const std::map<string, vector<int>> atr_map ){
    return AddAttribute<int>( app_name, atr_map, H5::PredType::NATIVE_INT);
}

template <>
bool H5FileManager::AddAttribute<unsigned int>( string app_name, const std::map<string, vector<unsigned int>> atr_map ){
    return AddAttribute<unsigned int>( app_name, atr_map, H5::PredType::NATIVE_UINT);
}

template <>
bool H5FileManager::AddAttribute<float>( string app_name, const std::map<string, vector<float>> atr_map ){
    return AddAttribute<float>( app_name, atr_map, H5::PredType::NATIVE_FLOAT);
}

template <>
bool H5FileManager::AddAttribute<double>( string app_name, const std::map<string, vector<double>> atr_map ){
    return AddAttribute<double>( app_name, atr_map, H5::PredType::NATIVE_DOUBLE);
}



template <>
bool H5FileManager::AddAttribute<const char*>( string app_name, const std::map<string, vector<const char*>> atr_map ){
    return AddAttribute<const char*>( app_name, atr_map, H5::StrType(H5::PredType::C_S1, H5T_VARIABLE) );
}


template <>
bool H5FileManager::AddAttribute<string>( string app_name, const std::map<string, vector<string>> atr_map ){
    
    std::map<string, vector<const char*>> cstr;

    std::map<string, vector<string>>::const_iterator itr;
    for( itr = atr_map.begin(); itr!=atr_map.end(); itr++ ){
        vector<string> val = itr->second;
        for( vector<string>::const_iterator itr2 = val.begin(); itr2!=val.end(); itr2++ ){
            cstr[ itr->first ].push_back( itr2->c_str() );
        }
    }
    return AddAttribute<const char*>( app_name, cstr, H5::StrType(H5::PredType::C_S1, H5T_VARIABLE) );
}


