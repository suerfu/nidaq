
#include "NIDAQdata.h"


NIDAQdata::NIDAQdata( const vector<int>& indices, unsigned int buffpc){

    chan_index = indices;
		// indicex of the enabled channels, not necessarily continuous in nature

    nchan = chan_index.size();
		// total number of enabled channels

    buff_per_chan = buffpc;

    data_mode = -1;

    buffer_mem = new int16[nchan*buff_per_chan];
    cal_coeff_mem = new float64[nchan*4];
        // NOTE: for other models where polynomials of different orders are used, a different order should be used.

    buffer_mat = new int16*[nchan];
    cal_coeff_mat = new float64*[nchan];
    for( unsigned int i=0; i<nchan; i++){
        buffer_mat[i] = buffer_mem + i*buff_per_chan;
        cal_coeff_mat[i] = cal_coeff_mem + i*4;
    }
}



NIDAQdata::~NIDAQdata(){
    delete[] buffer_mem;
    buffer_mem = 0;

    delete[] buffer_mat;
    buffer_mat = 0;

    delete[] cal_coeff_mem;
    cal_coeff_mem = 0;

    delete[] cal_coeff_mat;
    cal_coeff_mat = 0;
}




