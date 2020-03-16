#ifndef NIDAQ_H
    #define NIDAQ_H 1

// system libraries
#include <fstream>
#include <chrono>

// driver
#include <NIDAQmx.h>

// polaris-specific
#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"


/// NIdaq is a module for reading data from serial port.
/// It uses a simple protocol for ADC:
///     /adc/{freq N, on, off}
/// freq parameter will change the sampling frequency N, which is given as interval in ms between sampling.
/// on and off will turn the adc on and off
/// By default the hardware device has echo enabled, so it is necessary to mark messages sent to the device by delimiters ( #, /, (space)) so that echoed messages can be identified.

class NIdaq : public plrsModuleDAQ {

public:

    NIdaq( plrsController* ctrl);
        //!< Constructor

    ~NIdaq();
        //!< Destructor

protected:

    void Configure();
        //!< Configure method should open the serial port and configure the device for data-taking.

    void Deconfigure();
        //!< Close serial port.

    void PreRun();
        //!< Instruct hardware to start data taking. 

    void Run();

    void PostRun();
        //!< Instruct hardware to stop.

private:

    TaskHandle task;
    
    string chan;
    unsigned int nchan;

    float Vmin;
    float Vmax;

    float sample_freq;

    int buff_depth;
        // number of buffers in the FIFO
    int buff_size;
        // size of one buffer (1K, 1M)

    int32 mode;
    int32 error;

    uint32_t start_time;
};


/// creator function for loading the module.
extern "C" NIdaq* create_NIdaq( plrsController* c );


/// destructor function for releasing the module.
extern "C" void destroy_NIdaq( NIdaq* p );


#endif
