#ifndef NIDAQ_H
    #define NIDAQ_H 1

// system libraries
#include <vector>

// driver
#include <NIDAQmx.h>

// polaris-specific
#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"


/// NIdaq is a polaris module for reading data from National Instrument DAQ card (currently it is tested only on PCIe-6376).


class NIdaq : public plrsModuleDAQ {

public:

    NIdaq( plrsController* ctrl);
        //!< Constructor

    ~NIdaq();
        //!< Destructor

protected:

    void Configure();
        //!< Configure method.
        
        //!< This method is called during the CONFIG stage and used to configure the ADC hardware, internal variables and also other modules that rely on ADC data format such as HDF5 recorder.

    void Deconfigure();
        //!< Closes connection to the ADC hardware.

    void PreRun();
        //!< Called before the main acquisition loop. It instruct the hardware to start data taking.

    void Run();
        //!< The actual event loop.

        //!< Data is read from NI ADC card and stored in a NIDAQdata object. Then the pointer to this object is passed to recorder module for output.
        //!< This function will be called repeatedly in a while-loop as long as STATE is RUN.

    void PostRun();
        //!< Called after the main acquisition loop. It instruct the hardware to stop.

private:

    int next_addr;
        //!< The index (which is also address) of the module to which acquired data is passed to.

    TaskHandle task;
        //!< National Instrument task handle.
    
    string chan;                //!< Channel name specified by the user. From this value the channels enabled are deduced.
    unsigned int nchan;         //!< Number of channels enabled.
    std::vector<int> channels;  //!< Vector of integers representing channels enabled in this run.

    std::vector<float> Vmin;    //!< Lower bound of expected voltage ranges for channels enabled. Proper voltage range needs to be specified for most accurate ADC conversion.
    std::vector<float> Vmax;    //!< Upper bound of expected voltage ranges for channels enabled. Proper voltage range needs to be specified for most accurate ADC conversion.

    float sample_freq;          //!< Sampling frequency, also referred to as clock frequency and sampling rate.

    int buff_depth;             //!< Number of buffers in the polaris circular FIFO. This is the total number of NIDAQdata object used.
    int buff_per_chan;          //!< Buffer size per channel (1K, 1M), which is also number of samples per channel.

    int32 mode;                 //!< Mode of operation for the ADC card (continuous, finite, etc.). Currently only continuous and finite are supported. The actual code representation will be officialized later.

    int32 error;                //!< Error code returned by NI.

    uint32_t start_time;        //!< Time stamp that marks the beginning of data taking.

    unsigned int max_evt;       //!< Maximum number of events to acquire.

    unsigned int evt_counter;   //!< Event counter to keep track of events acquired.

    void ReConfigure( string input );
        //!< Convenience function used to reconfigured each time.
        //!< It turns out that in finite mode without interrupt handler, the channel has to be reconfigured each time one acquisition is finished.

    vector<int> GetChannelsEnabled( string input);
        //!< Finds out the channels to be used from the input string.
        //!< \param[in] input Input string in the format Dev?/ai?:?.
        //!< \return Vector of integers that represents channel indices.

    int32 CreateTask();
        //!< Initializes a NI task.
        //!< \return NI task handle.

    int32 ConfigChannel( string prefix, vector<int> ch, vector<float> vmin, vector<float> vmax);
        //!< Configures NI DAQ device (enable channels and voltage ranges).
        //!< \param[in] prefix  The channel prefix which is common to all channels (e.g. Dev1/ai).
        //!< \param[in] ch      The channel indices.
        //!< \param[in] vmin    Vector containing the lower bound of voltage ranges for each channel.
        //!< \param[in] vmax    Vector containing the upper bound of voltage ranges for each channel.

    int32 ConfigClock( int32 mod, float freq, float buff_per_chan );
        //!< Sets the DAQ mode, ADC sampling rate / clock frequency and buffers.
        //!< \param[in] mod     ADC mode.
        //!< \param[in] freq    ADC sampling frequency in Hz.
        //!< \param[in] buff_per_chan   Buffer per channel. This is the number of samples to be acquired before data is made ready for readout.
};


/// Creator function for loading the module. This is needed for polaris to load module as a dynamic library.
extern "C" NIdaq* create_NIdaq( plrsController* c );


/// Destructor function for releasing the module. This is needed for polaris to load module as a dynamic library.
extern "C" void destroy_NIdaq( NIdaq* p );


#endif
