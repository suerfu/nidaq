#ifndef NIDAQDATA_H
#define NIDAQDATA_H 1

#include <NIDAQmx.h>
#include <stdio.h>

class NIDAQdata{

public:

    NIDAQdata( int nc, bool i16, unsigned int buffpc, float frq, bool32 grp ){
        nchan = nc;
        useI16 = i16;
        buff_per_chan = buffpc;
        buffsize = nc*buff_per_chan;
        clk = frq;
        if( useI16 )
            buffer = new int16[buffsize];
        else
            buffer = new float64[buffsize];
        read = 0;
        group = grp;
    }

    ~NIDAQdata(){
        if( buffer ){
            if( useI16 )
                delete GetBufferI16();
            else
                delete GetBufferF64();
        }
    }

    bool FmtI16(){
        return useI16;
    }

    float64* GetBufferF64(){
        return reinterpret_cast<float64*>(buffer);
    }

    int16* GetBufferI16(){
        return reinterpret_cast<int16*>(buffer);
    }

    int nchan;
    bool useI16;
    unsigned int buff_per_chan;
    unsigned int buffsize;

    void* buffer;
   
    float clk;

    int read;
        // number of samples read

    bool32 group;
};

#endif
