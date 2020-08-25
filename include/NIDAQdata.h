#ifndef NIDAQDATA_H
    #define NIDAQDATA_H 1

#include <stdio.h>
#include <iostream>
#include <vector>

#include <NIDAQmx.h>

using std::cout;
using std::endl;
using std::vector;

//! Object to hold data/information obtained from National Instrument ADC card.

//! NIDAQdata is an object to hold ADC configuration information and actual
//! ADC data read from National Instrument ADC card.
//! It is typically initialized by the DAQ module and passed to recorder module for writing output.

class NIDAQdata{

public:

    NIDAQdata( const vector<int>& indices, unsigned int buffpc);
        //!< Constructor.
        //!< \param[in] indices Vector of integers representing the indices of channels to be used.
        //!< \param[in] buffpc  Size of buffer (sample size) per channel.
        //!< At this stage all channels are assumed to have the same sample size.

    ~NIDAQdata();
        //!< Destructor which frees allocated memory.

    int GetNChannels(){ return nchan;}
        //!< \return Number of channels enabled.

    unsigned int GetBufferPerChannel(){ return buff_per_chan;}
        //!< \return Size of samples per channel.

    void SetClockFrequency( float frequency){ clk = frequency;}
        //!< Sets clock frequency (sampling rate) in Hz.
        //!< \param[in] frequency   Clock frequency (sampling rate) in Hz.
        //!< Note that the input has to be a reasonable and valid float.
        //!< This is not checked against in the rest of the code.

    float GetClockFrequency(){ return clk;}
        //!< \return Clock frequency (sampling rate) in Hz.

    vector<int> GetChannelIndex(){ return chan_index;}
        //!< \return Vector of channel indices enabled for this data set.

    void SetVoltageRange( vector<float> min, vector<float> max){
        vmin = min;   vmax = max;
    }
        //!< Sets the voltage ranges for channels enabled.
        //!< \param[in] min Vector of floats representing the minimum voltages of channels enabled.
        //!< \param[in] max Vector of floats representing the maximum voltages of channels enabled.
        //!< A proper voltage range is needed for having the most accurate ADC performance.

    vector<float> GetVmin(){ return vmin;}
        //!< \return The minimum voltage ranges of channels enabled.
    
    vector<float> GetVmax(){ return vmax;}
        //!< \return The maximum voltage ranges of channels enabled.

    int16** GetBuffer(){
        return buffer_mat;
    }
        //!< \return 2D int16 pointer.
        //!< This pointer should be used when one wants to access ADC values as elements of a matrix.

    size_t GetBufferSize(){
        return nchan*buff_per_chan;
    }
        //!< \return Total size of buffer (sample sizes).

    int16* GetBufferMem(){
        return buffer_mem;
    }
        //!< \return The underlying address where the data read is stored.
        //!< This address is a contiguous block that can and should be used in writing HDF5 files.

    void SetDataMode( int mode){ data_mode = mode;}
        //!< Sets the mode for data acquisition.
        //!< \param[in] mode DAQ mode. Code representation to be developed later.

    int GetDataMode(){ return data_mode;}
        //!< \return The mode of data acquisition.
        //!< The integer representation will be developed and made official later.

    void SetTrigMode( string mode ){ trig_mode = mode; }

    string GetTrigMode(){ return trig_mode;}

    void SetTrigChannel( string tc ){ trig_channel = tc; }

    string GetTrigChannel(){ return trig_channel; }

    void SetTrigPeriod( int a ){ trig_period = a; }

    int GetTrigPeriod(){ return trig_period; }

    float64** GetCalCoeff(){ return cal_coeff_mat;}
        //!< \return The pointer to memory where calibration coefficients for enabled channels are stored.
        //!< The calibration coefficients are coefficients of a 3rd order polynomial.
        //!< The address returned can be used to access in a matrix convention, i.e. [channel][poly-order].
    
    float64* GetCalCoeffMem(){ return cal_coeff_mem;}
        //!< \return The memory where calibration coefficients are stored.
        //!< This address represents a contiguous block. Coefficients are grouped by channels.
        //!< This address can be used to write HDF5 files.
        //!< The calibration coefficients are coefficients of a 3rd order polynomial.

    bool32 GetGroupMode(){ return DAQmx_Val_GroupByChannel;}
        //!< Obsolete. It is used to get the grouping mode of the data, i.e. by channels or by sample.
        //!< This method is obsolete since it was determined that we always group by channels.

    int read;
        //!< Bytes read from ADC.
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

    int data_mode;
        //!< Mode of data taking. Continuous, finite, triggered, etc.
        //!< Currently, only continuous and finite are supported.
    
    string trig_mode;
        //!< Trigger mode.
        //!< Will be specified in more detail in the future.

    string trig_channel;
        //!< Channel used to accept trigger.

    int trig_period;
        //!< Trigger period in ms.
        //!< Internally inside the program, it will be multiplied by 1000 to increase precision.

    unsigned int nchan;
        //!< Number of channels enabled.

    unsigned int buff_per_chan;
        //!< Size of buffer (number of samples) per channel.

    unsigned int buffsize;
        //!< Total size of buffer.

    int16** buffer_mat;
        //!< Pointer to buffer. Used for accessing elements using matrix convention.
    int16* buffer_mem;
        //!< Pointer to buffer as contiguous block of data.
        //!< Needed for writing HDF5 output.

    float64** cal_coeff_mat;
        //!< Calibration coefficients for access as matrix.
    float64* cal_coeff_mem;
        //!< Calibration coefficients as contiguous block of data.
        //!< Needed for HDF5 output.

    vector<int> chan_index;
        //!< Vector that holds Indices of channels enabled.

    vector<float> vmin;     //!< Minimum range of input voltages.
    vector<float> vmax;     //!< Maximum range of input voltages.
   
    float clk;
        //!< Clock frequency/sampling rate in Hz.
};

#endif
