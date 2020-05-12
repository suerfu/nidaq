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
 *  This program illustrates the usage of the H5A Interface functions.
 *  It creates and writes a dataset, and then creates and writes array,
 *  scalar, and string attributes of the dataset.
 *  Program reopens the file, attaches to the scalar attribute using
 *  attribute name and reads and displays its value. Then index of the
 *  third attribute is used to read and display attribute values.
 *  The H5Aiterate function is used to iterate through the dataset attributes,
 *  and display their names. The function is also reads and displays the values
 *  of the array attribute.
 */

#include <stdlib.h>
#include <iostream>
using std::cout;
using std::endl;

#include "hdf5.h"

#define H5FILE_NAME "Attributes.h5"

#define RANK  1   /* Rank and size of the dataset  */
#define SIZE  7

#define ARANK  2   /* Rank and dimension sizes of the first dataset attribute */
#define ADIM1  4
#define ADIM2  8
#define ANAME  "Float attribute"      /* Name of the array attribute */
#define ANAMES "Character attribute" /* Name of the string attribute */

static herr_t attr_info(hid_t loc_id, const char *name, const H5A_info_t *ainfo, void *opdata);
                                     /* Operator function */

int main (void){

   hid_t   file, dataset;       /* File and dataset identifiers */

   hid_t   fid;                 /* Dataspace identifier */
   hid_t   attr1; /* Attribute identifiers */
   hid_t   attr;
   hid_t   aid1;    /* Attribute dataspace identifiers */
   hid_t   atype, atype_mem;    /* Attribute type */
   H5T_class_t  type_class;

   hsize_t fdim[] = {SIZE};
   hsize_t adim[2] = {ADIM1, ADIM2};  /* Dimensions of the first attribute  */

   // *  C/C++ Static * //
   float matrix1[ADIM1][ADIM2]; /* Attribute data */
   for ( int i=0; i < ADIM1; i++) {            /* Values of the array attribute */
       for ( int j=0; j < ADIM2; j++)
            cout <<"("<< i << ' ' << j << ") " << &matrix1[i][j]-&matrix1[0][0] << ' ';
        cout << endl;
   }
   cout << endl;

   // * C++ dynamic * //
   // float** matrix = new float*[ADIM1];
   // for( unsigned int i=0; i<ADIM1; i++)
   //  matrix[i] = new float[ADIM2];

   // * Continuous block * //
   float* matrix_back = new float[ADIM1*ADIM2];
   float** matrix = new float*[ADIM1];

//   float** matrix = (float**) malloc( ADIM1*ADIM2*sizeof(float*));
//   matrix[0] = 

   for( int i=0; i<ADIM1; i++ )
        matrix[i] = matrix_back + i*ADIM2;

   for ( int i=0; i < ADIM1; i++) {            /* Values of the array attribute */
       for ( int j=0; j < ADIM2; j++)
            cout <<"("<< i << ' ' << j << ") " << &matrix[i][j]-&matrix[0][0] << ' ';
        cout << endl;
   }

   herr_t  ret;                /* Return value */
   unsigned i, j;              /* Counters */

   int vector[] = {1, 2, 3, 4, 5, 6, 7};  /* Dataset data */
   int point = 1;                         /* Value of the scalar attribute */
   char string[] = "ABCD";                /* Value of the string attribute */


   for (i=0; i < ADIM1; i++) {            /* Values of the array attribute */
       for (j=0; j < ADIM2; j++){
            matrix[i][j] = -.1*(i+j);
            matrix1[i][j] = -.1*(i+j);
       }
   }
   cout << endl;
   for ( int i=0; i < ADIM1; i++) {            /* Values of the array attribute */
       for ( int j=0; j < ADIM2; j++)
        cout << "(" << i << ' ' << j << ") " << matrix[i][j] << ' ';
        cout << endl;
   }

   /*
    * Create a file.
    */
   file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

   /*
    * Create the dataspace for the dataset in the file.
    */
   fid = H5Screate(H5S_SIMPLE);
   ret = H5Sset_extent_simple(fid, RANK, fdim, NULL);

   /*
    * Create the dataset in the file.
    */
   dataset = H5Dcreate2(file, "Dataset", H5T_NATIVE_INT, fid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

   /*
    * Write data to the dataset.
    */
   ret = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL , H5S_ALL, H5P_DEFAULT, vector);

   /*
    * Create dataspace for the first attribute.
    */
   aid1 = H5Screate(H5S_SIMPLE);
   ret  = H5Sset_extent_simple(aid1, 2, adim, NULL);

   /*
    * Create array attribute.
    */
   attr1 = H5Acreate2(file, ANAME, H5T_NATIVE_FLOAT, aid1, H5P_DEFAULT, H5P_DEFAULT);

   /*
    * Write array attribute.
    */
   ret = H5Awrite(attr1, H5T_NATIVE_FLOAT, matrix_back);

   ret = H5Sclose(aid1);
   ret = H5Sclose(fid);
   //ret = H5Tclose(atype);

   /*
    * Close the attributes.
    */
   ret = H5Aclose(attr1);

   /*
    * Close the dataset.
    */
   ret = H5Dclose(dataset);

   /*
    * Close the file.
    */
   ret = H5Fclose(file);
   return 0;
}

