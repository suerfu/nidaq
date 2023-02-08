#ifndef NIFILTER_HHDF5RECORDER_H
    #define NIFILTER_H 1

#include <string>
#include <sstream>

#include "plrsController.h"

#include "NIDAQdata.h"

using namespace std;


/// This module filters data stream passed by DAQ module

/// DAQ and Recorder module should be specified with /module/filter/prev and /module/filter/next
/// so that events failing the filter will be returned back to the DAQ module


class NIFilter : public plrsStateMachine{

public:

    NIFilter( plrsController* c);    //!< Constructor.

    virtual ~NIFilter();  //!< Destructor

private:

    //----------- Mandatory polaris virtual functions ----//

    void Configure();

    void Deconfigure();

    void PreRun();

    void Run();
    
    void PostRun();


private:

    string mode;
        // mode of trigger
        // CONTINUOUS, OR, AND, etc.

    int next_addr;
        // address to send data to
        // usually this is the recorder
    
	int prev_addr;
        // address to return data to
        // usually this is the DAQ module

    unsigned int nb_post_filter_events;
        // Nb of events to record after an event passes the filter
        // For completeness of the pulse.

    unsigned int post_filter_event_counter;
        // Used to record how many extra post-filter events to record.

    int GetNextModuleID();
        //!< Retrieves the ID/address of next module.
        //!< If not specified, it returns the module's own address.
    
	int GetPreviousModuleID();
        //!< Retrieves the ID/address of the previous module.
        //!< This is necessary when the data fails the filter and needs to be returned to the DAQ module for reuse.

    int16** buffer;

    vector<int> trig_channel_indices;
        // channels to consider in making online trigger decisions.

    vector<float> trig_threshold_v;
        // trigger threshold in actual voltage

    vector<int16> trig_threshold_adc;
        // trigger threshold in ADC counts
    
    vector<int> trig_polarity;
        // true if trigger over threshold
        // false if trigger under threshold

    int16 VoltToADC( float threshold, float64** cal_coeff, int position) ;

    bool Filter( NIDAQdata* data, string mode);

};

extern "C" NIFilter* create_NIFilter( plrsController* c);

extern "C" void destroy_NIFilter( NIFilter* p );


#endif
