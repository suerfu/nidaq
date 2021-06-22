
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

#include <string>
#include "H5FileManager.h"

const int FSPACE_RANK = 2;
const int FSPACE_DIM1 = 10;
const int FSPACE_DIM2 = 20;
const int SIZE = FSPACE_DIM1*FSPACE_DIM2;

int main (void){

    H5FileManager h5man;

    h5man.OpenFile( "Select.h5", "w" );
    cout << "Created output file\n";
    h5man.OpenGroup( "/Data" );
    cout << "Opened output group\n";

    unsigned int dim[2] = { FSPACE_DIM1, FSPACE_DIM2 };

    cout << "Initializing data\n";
    float vector[SIZE];
    for ( int i = 0; i < SIZE; i++)
        vector[i] = 0.5*i;

    h5man.WriteData( vector, "/Data/Matrix", H5::PredType::NATIVE_FLOAT, FSPACE_RANK, dim);

    for ( int i = 0; i < SIZE; i++)
        vector[i] = 2*i;
    h5man.WriteData( vector, "/Data/Matrix2", H5::PredType::NATIVE_FLOAT, FSPACE_RANK, dim);
    
    for ( int i = SIZE-1; i >=0; i--)
        vector[SIZE-1-i] = i;
    h5man.WriteData( vector, "/Data/Matrix3", H5::PredType::NATIVE_FLOAT, FSPACE_RANK, dim);
    
    cout << "Data written\n";

    //H5File* file = new H5File( FILE_NAME, H5F_ACC_TRUNC );
    //Group* group = new Group( file->createGroup( "/Data" ));
    //int fillvalue = 0;   /* Fill value for the dataset */
    //DSetCreatPropList plist;
    //plist.setFillValue(PredType::NATIVE_INT, &fillvalue);
    //hsize_t fdim[] = {FSPACE_DIM1, FSPACE_DIM2}; // dim sizes of ds (on disk)
    //DataSpace fspace( FSPACE_RANK, fdim );
    //DataSet* dataset = new DataSet(file->createDataSet(
    //    DATASET_NAME, PredType::NATIVE_INT, fspace, plist));

    /*
     * Close the dataset and the file.
     */
    //delete dataset;
    //delete group;
    //delete file;

    h5man.CloseGroup("/Data");
    cout << "Group closed\n";
    h5man.CloseFile();
    cout << "File closed\n";

    char c;
    cout << "File finished writing. Enter any character to proceed.\n";
    cin >> c;



   return 0;
}
