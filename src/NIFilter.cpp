
#include "plrsBaseData.h"

#include "NIFilter.h"


extern "C" NIFilter* create_NIFilter( plrsController* c ){ return new NIFilter(c);}

extern "C" void destroy_NIFilter( NIFilter* p ){ delete p;}


NIFilter::NIFilter( plrsController* c) : plrsStateMachine(c){ }
	// Not too much to be done in the constructor



NIFilter::~NIFilter(){}



void NIFilter::Configure(){

    Print("configuring...\n", DEBUG);
    
    // =========================================================
    // Obtain ID/address of the module to which to send data to.
    // =========================================================

    next_addr = GetNextModuleID();
    prev_addr = GetPreviousModuleID();
    Print("Data path set.\n", DEBUG);


	// =================================================================
	// Get relevant parameters such as mode of operation, threshold, etc. from ConfigParser
	// =================================================================
	
	mode = GetConfigParser()->GetString("/module/"+GetModuleName()+"/mode", "");
		// mode of operation. Continuous or triggered.
		// different functions should be added here as functionality expands
	if( mode=="" ){
		Print("Mode not specified. Filter module will act as FIFO buffer.\n", INFO);
	}
	else if( mode=="scattering-hw-trigger" ){
		trig_channel_indice = GetConfigParser()->GetIntArray("/module/"+GetModuleName()+"/filter-channels", "");
		trig_threshold_float = GetConfigParser()->GetFloatArray("/module/"+GetModuleName()+"/filter-thresholds", "");
	}
	

    // =========================================================
    // During config phase, DAQ will pass empty data for 
	// recorder module to  configure metadata
	// Filter module should pass the data directly to filter
    // =========================================================

    void* rdo = 0;
    while( rdo==0 && GetState()==CONFIG ){
        rdo = PullFromBuffer();
    }

    // If state is no longer CONFIG due to error, return the memory and exit
    if( GetState()!=CONFIG ){
        if( rdo!=0 )
    	    PushToBuffer( next_addr, rdo);
        return;
    }
	
	// At this point, it is safe to assume no error ocurred during config phase.
	// The calibration coefficient in the dummy data is used to back-compute threshold in ADC counts

	

	// Done with the dummy data. Pass it on to the next module.

	PushToBuffer( next_addr, rdo);
    
    Print( this->GetModuleName()+" configured.\n", DEBUG);
}



void NIFilter::Deconfigure(){}


void NIFilter::PreRun(){}


void NIFilter::PostRun(){
}


void NIFilter::Run(){

    // Obtain a valid/ready pointer to data object from circular FIFO buffer.
    void* rdo = PullFromBuffer();
    if( rdo==0 ){
        return;
	}
    
    // =======================================================================
    // Check if data passes
	// Detailed event data/waveform can be accessed via the data pointer below
    // =======================================================================

    // Since the readout pointer is of type void*, first cast it to NIDAQdata type.
    NIDAQdata* data = reinterpret_cast< NIDAQdata* >(rdo);

    dim[0] = data->GetNChannels();
    dim[1] = data->GetBufferPerChannel();

	if( /*if data passes filter*/ ){
		PushToBuffer( next_addr, rdo);
	}
	else if( /*did not pass, but no error*/ ){
		PushToBuffer( prev_addr, rdo);
	}
	else{
        Print( "Encountered error.\n", ERR);
	    PushToBuffer( next_addr, rdo);
        SetStatus(ERROR);
        return;
    }
}


int NIFilter::GetNextModuleID(){

    int next_addr;

    string next_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/next", "");
        // if not found, returns default value of ""
    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );   // nonnegative if valid
    }
    if( next_module=="" || next_addr<0 ){
        Print("Next module not specified. Setting up loop-back.\n", INFO);
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }

    return next_addr;
}


int NIFilter::GetPreviousModuleID(){

    int prev_addr;

    string prev_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/prev", "");
        // if not found, returns default value of ""
    if( prev_module!="" ){
        prev_addr = ctrl->GetIDByName( prev_module );   // nonnegative if valid
    }
    if( prev_module=="" || prev_addr<0 ){
        Print("Next module not specified. Setting up loop-back.\n", INFO);
        prev_addr = ctrl->GetIDByName( this->GetModuleName() );
    }

    return prev_addr;
}


