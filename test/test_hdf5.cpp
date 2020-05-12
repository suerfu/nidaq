
#include <iostream>
using namespace std;

#include "H5FileManager.h"

int main(){

    H5FileManager* man = new H5FileManager();
    man->OpenFile("haha.hdf5", "w+");
    man->OpenGroup("this/is/a/long/dir");
    man->OpenGroup("/data0/data1/data2/");

    unsigned int dims[2] = {20, 4};
    float* voltage = new float[ dims[0]*dims[1] ];
    for( unsigned int i=0; i<dims[0]; i++){
//        voltage[i] = i;
        for( unsigned int j = 0; j<dims[1]; j++){
            voltage[ j+i*dims[1] ] = i+0.1*j;
        }
    }

    map<string, vector<int>> attr;
    attr["a"].push_back( 1 );
    attr["a"].push_back( 2 );
    attr["a"].push_back( 3 );
    man->AddAttribute<int>( "/data0/data1", attr, H5::PredType::NATIVE_INT );

    man->CloseGroup("/data0/data1/");
    cout << "Group closed" << endl;
    man->WriteData<float>( voltage, "/data0/data1/data2/voltage0", H5::PredType::NATIVE_FLOAT, 2, dims );
    cout << "Writing data" << endl;

    map<string, vector<string>> attr2;
    attr2["name"].push_back( "Hello World!" );
    attr2["name"].push_back( "Magia Record" );
    man->AddAttribute<string>( "/data0/data1/data2/voltage0", attr2);

    cout << "File Size: " << man->GetFileSize() << endl;
    man->CloseFile();

    return 0;

/*
    hid_t f1 = H5CreateFile("hello.hdf5");
    hid_t g1 = H5OpenGroup( f1, "dir1");
    hid_t g2 = H5OpenGroup( g1, "dir2");


    unsigned int dim = 1;
    int data = 10;

    AddAttribute<int>( f1, string("version"), dim, &data, H5T_NATIVE_INT);
    AddAttribute<int>( g2, string("version"), dim, &data, H5T_NATIVE_INT);
    AddAttribute<int>( g1, string("version"), dim, &data, H5T_NATIVE_INT);


    unsigned int dims[2] = {2,3};
    float* voltage = new float[ dims[0]*dims[1] ];
    for( unsigned int i=0; i<dims[0]; i++){
        for( unsigned int j = 0; j<dims[1]; j++){
            voltage[ j+i*dims[1] ] = i+0.1*j;
        }
    }
    for( unsigned int i=0; i<dims[0]*dims[1]; i++)
        voltage[i] = 0.1*i;


    AddAttribute<float>( g2, string("voltage"), dims[0], dims[1], voltage, H5T_NATIVE_FLOAT);
    AddAttribute<float>( g1, string("voltage"), dims[1], dims[0], voltage, H5T_NATIVE_FLOAT);

    AddDataset<float>( g2, "data1", dims[0], dims[1], voltage, H5T_NATIVE_FLOAT );
    AddDataset<float>( g1, "data1", dims[1], dims[0], voltage, H5T_NATIVE_FLOAT );

//    H5Close();
    
    H5CloseGroup(g2);
    H5CloseGroup(g1);
    H5CloseFile(f1);
*/
    return 0;
}

