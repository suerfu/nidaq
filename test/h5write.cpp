/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 *  This program shows how the select_hyperslab and select_elements
 *  functions are used to write selected data from memory to the file.
 *  Program takes 48 elements from the linear buffer and writes them into
 *  the matrix using 3x2 blocks, (4,3) stride and (2,4) count.
 *  Then four elements  of the matrix are overwritten with the new values and
 *  file is closed. Program reopens the file and reads and displays the result.
 */
#ifdef OLD_HEADER_FILENAME
#include <iostream.h>
#else
#include <iostream>
#endif
using std::cout;
using std::cin;
using std::endl;
#include <string>
#include "H5Cpp.h"
using namespace H5;

const H5std_string FILE_NAME( "Select.h5" );
const H5std_string DATASET_NAME( "Matrix in file" );

const int   FSPACE_RANK = 2;   // Rank of the first dataset in memory
const int   FSPACE_DIM1 = 5;   // Dataset size in memory
const int   FSPACE_DIM2 = 8;   // Dataset size in memory

int main (void)
{
    /*
     * Try block to detect exceptions raised by any of the calls inside it
     */
    /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
    Exception::dontPrint();
    /*
     * Create a file.
     */
    H5File* file = new H5File( FILE_NAME, H5F_ACC_TRUNC );
    /*
    * Create property list for a dataset and set up fill values.
    */
    int fillvalue = 0;   /* Fill value for the dataset */
    DSetCreatPropList plist;
    plist.setFillValue(PredType::NATIVE_INT, &fillvalue);
    /*
     * Create dataspace for the dataset in the file.
     */
    hsize_t fdim[] = {FSPACE_DIM1, FSPACE_DIM2}; // dim sizes of ds (on disk)
    DataSpace fspace( FSPACE_RANK, fdim );
    /*
     * Create dataset and write it into the file.
     */
    DataSet* dataset = new DataSet(file->createDataSet(
        DATASET_NAME, PredType::NATIVE_INT, fspace, plist));

    const int SIZE = FSPACE_DIM1*FSPACE_DIM2;
    int    vector[SIZE]; // vector buffer for dset
    /*
     * Buffer initialization.
     */
    for ( int i = 0; i < SIZE; i++)
        vector[i] = i;
    dataset->write( vector, PredType::NATIVE_INT );
    /*
     * Close the dataset and the file.
     */
    delete dataset;
    delete file;


    char c;
    cout << "File finished writing. Enter any character to proceed.\n";
    cin >> c;



   return 0;
}
