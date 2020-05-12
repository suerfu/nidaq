#ifndef H5FILEMANAGER_H
    #define H5FILEMANAGER_H 1

#include "H5Cpp.h"

#include <map>
#include <vector>
#include <iostream>

using std::string;
using std::map;
using std::vector;

class H5FileManager{

public:

    // =====================================
    // Constructor & Destructor
    // =====================================
    
    H5FileManager();
    virtual ~H5FileManager();


    // =====================================
    // File operations
    // =====================================
    
    unsigned int GetFileSize(){
        if( file_ptr==0 )
            return 0;
        return file_ptr->getFileSize();
    }

    bool OpenFile( const string& name, const string& flag="rw");
    bool CloseFile();

    string GetFileName(){ return filename;}


    // =====================================
    // Group operations
    // =====================================
    
    bool OpenGroup( string name);
    bool CloseGroup( const string& name);

    
    // =====================================
    // Data operations
    // =====================================
    
    // Add data by specifying all arguments
    template < class PtrType >
    bool WriteData( PtrType* data, string name, const H5::DataType& type, unsigned int rank, unsigned int dims[] );

    // Add data by external dataset.
    template < class PtrType >
    bool WriteData( PtrType* data, const H5::DataType& type, H5::DataSet* dset);

    
    // =====================================
    // Dataset operations
    // =====================================
 
    H5::DataSet CreateDataSet( string name, const H5::DataType& type, unsigned int rank, unsigned int dims[] ){
        std::cout << "Creating dataset" << std::endl;
        if( DataSetExist( name ) ){
            std::cout << "Dataset already exists" << std::endl;
            return file_ptr->openDataSet( name.c_str());
        }

        hsize_t h5dims[rank];
        for( unsigned int i=0; i<rank; i++)
            h5dims[i] = dims[i];
        return file_ptr->createDataSet( name.c_str(), type, H5::DataSpace(rank, h5dims) );
    }

    H5::DataSet OpenDataSet( string name){
        return file_ptr->openDataSet( name.c_str());
    }


    // =====================================
    // Attributes
    // =====================================
    template < class D >
    bool AddAttribute( string app_name, const std::map<string, vector<D>> atr_map, const H5::DataType& );

    template < class D>
    bool AddAttribute( string app_name, const std::map<string, vector<D>> atr_map );

private:

    bool GroupExist( string name){
        if( list_group.find( name )==list_group.end() )
            return false;
        return true;
    }

    bool DataSetExist( string name){
        H5::DataSet ds;
        try{
            H5::Exception::dontPrint();
            ds = file_ptr->openDataSet( name.c_str() );
        }
        catch( H5::FileIException not_found_error){
            std::cout << "Dataset does not exist in exception"<< std::endl;
            return false;
        }
        return true;
    }


private:

    H5::H5File* file_ptr;
    string filename;

    std::map<string, H5::Group*> list_group;
};



template <class PtrType>
bool H5FileManager::WriteData( PtrType* data, string name, const H5::DataType& type, unsigned int rank, unsigned int dims[]){
    
    std::cout << "dataset creating\n";
    H5::DataSet dset( CreateDataSet( name, type, rank, dims) );
    std::cout << "dataset created\n";
    dset.write( data, type );
    std::cout << "dataset written\n";
    return true;
}


template <class PtrType>
bool H5FileManager::WriteData( PtrType* data, const H5::DataType& type, H5::DataSet* dset){

    if( dset==0 )
        return false;
    
    dset->write( data, type );

    return true;
}


template< class D >
bool H5FileManager::AddAttribute( string app_name, const std::map<string, vector<D>> atr_map, const H5::DataType& datatype ){

    if( !file_ptr )
        return false;

    void* foo = 0;  // pointer to object to which attribute is added.
    bool file_attr = false;
    bool group_attr = false;

    if( app_name=="" || app_name=="/"){
        file_attr = true;
        foo = file_ptr;
        std::cout << "file attribute: " << app_name << std::endl;
    }
    else if( GroupExist( app_name) ){
        group_attr = true;
        foo = list_group[app_name];
        std::cout << "group attribute: " << app_name << std::endl;
    }
    else if( DataSetExist( app_name) ){
        foo =  new H5::DataSet( this->OpenDataSet( app_name ) );
        std::cout << "dataset attribute: " << app_name << std::endl;
    }
    else
        return false;

    typename map<string, vector<D>>::const_iterator itr;
    for( itr = atr_map.begin(); itr!=atr_map.end(); itr++){
        
        string attr_name = itr->first;
        vector<D> attr_vec = itr->second;
        if( attr_vec.empty() )
            continue;
        for( unsigned int i=0; i< attr_vec.size(); i++)
            std::cout << "adding " << attr_name << "\t" << attr_vec[i] << std::endl;

        hsize_t dims[1] = { attr_vec.size() };
        H5::DataSpace attr_ds;
        if( attr_vec.size()==1 )
            attr_ds = H5::DataSpace( H5S_SCALAR);
        else
            attr_ds = H5::DataSpace( 1, dims );

        H5::Attribute attr;
        if( file_attr )
            attr = file_ptr->createAttribute( attr_name.c_str(), datatype, attr_ds);
        else if( group_attr)
            attr = reinterpret_cast<H5::Group*>(foo)->createAttribute( attr_name.c_str(), datatype, attr_ds);
        else
            attr = reinterpret_cast<H5::DataSet*>(foo)->createAttribute( attr_name.c_str(), datatype, attr_ds);

        std::cout << "attribute created" << std::endl;

        attr.write( datatype, attr_vec.data());
        
        std::cout << "attribute written" << std::endl;
    }

    return true;
}

#endif
