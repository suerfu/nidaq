
#include <iostream>
#include <string>
#include <fstream>

#include <chrono> 

using namespace std::chrono;
using namespace std;

int main ( int argc, char* argv[]){

    if( argc<4 ){
        std::cout << "\nusage: " << argv[0] << "   filename.h5    Nchan    Nsamp    Nsec(=1)\n" << std::endl;
        return -1;
    }

    int Nwrite = 1;
    if( argc>4)
        Nwrite = atoi(argv[4]);

    int      dims[2]  = { atoi(argv[2]), atoi(argv[3]) };           /* dataset dimensions at creation time */     
    int**        data =  new int*[ dims[0]];
    for( int i=0; i<dims[0]; i++){
        data[i] =  new int[ dims[1]];
        for( int j=0; j<dims[1]; j++){
            data[i][j] = 0;
        }
    }

    ofstream file;
    file.open(argv[1], ios_base::out | ios::binary);

    auto start = high_resolution_clock::now();


    for( int i=0; i<Nwrite+1; i++){
        for( int f=0; f<dims[0]; f++){
            file.write(reinterpret_cast<char*>(data[f]), sizeof(int)*dims[1]);
        }
    }

    file.close();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    //auto duration = duration_cast<microseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    return 0;
}
