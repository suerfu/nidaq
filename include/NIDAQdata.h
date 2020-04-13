#ifndef NIDAQDATA_H
#define NIDAQDATA_H 1

#include <NIDAQmx.h>
#include <stdio.h>
#include <iostream>

using std::cout;
using std::endl;

class NIDAQdata{

public:

    NIDAQdata( unsigned int nc, unsigned int buffpc){
        nchan = nc;
        buff_per_chan = buffpc;

        buffer_mem = new int16[nchan*buff_per_chan];
        cal_coeff_mem = new float64[nchan*4];

        buffer_mat = new int16*[nchan];
        cal_coeff_mat = new float64*[nchan];
        for( unsigned int i=0; i<nchan; i++){
            buffer_mat[i] = buffer_mem + i*buff_per_chan;
            cal_coeff_mat[i] = cal_coeff_mem + i*4;
        }
    }

    ~NIDAQdata(){
        delete[] buffer_mem;
        delete[] buffer_mat;
        delete[] cal_coeff_mem;
        delete[] cal_coeff_mat;
    }

    unsigned int GetNChannels(){ return nchan;}

    int GetBufferPerChannel(){ return buff_per_chan;}

    void SetClockFrequency( float c){ clk = c;}
    float GetClockFrequency(){ return clk;}

    void SetChannelIndex( vector<int> a){
        chan_index = a;
    }

    vector<int> GetChannelIndex(){ return chan_index;}

    void SetVoltageRange( vector<float> a, vector<float> b){
        vmin = a;   vmax = b;
    }

    vector<float> GetVmin(){ return vmin;}
    
    vector<float> GetVmax(){ return vmax;}

    int16** GetBuffer(){
        return buffer_mat;
    }

    int16* GetBufferMem(){
        return buffer_mem;
    }

    float64** GetCalCoeff(){ return cal_coeff_mat;}
    
    float64* GetCalCoeffMem(){ return cal_coeff_mem;}
/*
    void Print(){
        cout << "N channels: " << nchan << endl;
        cout << "Clock frequency: " << clk << endl;
        cout << "Buffer per chan: " << buff_per_chan << endl;
        for( unsigned int i=0; i<nchan; i++){
            cout << "Channel " << i << endl;
            cout << "\tChannel index: " << chan_index[i] << endl;
            cout << "\tVoltage range: " << vmin[i] << ' ' << vmax[i] << endl;
            cout << "\tcal coeff: " << cal_coeff[i][0] <<' '<< cal_coeff[i][0] <<' '<< cal_coeff[i][0] <<' '<< cal_coeff[i][0] << endl;
        }
    }
*/
private:

    unsigned int nchan;
        // Number of channels enabled.

    unsigned int buff_per_chan;

    unsigned int buffsize;

    int16** buffer_mat;
    int16* buffer_mem;

    float64** cal_coeff_mat;
    float64* cal_coeff_mem;

    vector<int> chan_index;
    vector<float> vmin;
    vector<float> vmax;
   
    float clk;

    int read;
        // number of samples read
};

#endif
