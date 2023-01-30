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

    int next_addr;
    
	int prev_addr;

    int GetNextModuleID();
        //!< Retrieves the ID/address of next module.
        //!< If not specified, it returns the module's own address.
    
	int GetPreviousModuleID();
};

extern "C" NIFilter* create_NIFilter( plrsController* c);

extern "C" void destroy_NIFilter( NIFilter* p );


#endif
