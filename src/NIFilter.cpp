
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
	
	mode = GetConfigParser()->GetString("/module/"+GetModuleName()+"/mode", "all-pass");
		// mode of operation. Continuous or triggered.
		// different functions should be added here as functionality expands
	if( mode=="all-pass" ){
		Print( "Filter module will act as an all-pass filter...\n", INFO );
	}
	else if( mode=="scattering-hw-trigger" ){
		
		trig_channel_indices = GetConfigParser()->GetIntArray("/module/"+GetModuleName()+"/filter-channels");
		trig_threshold_v = GetConfigParser()->GetFloatArray("/module/"+GetModuleName()+"/filter-thresholds");
		trig_polarity = GetConfigParser()->GetIntArray("/module/"+GetModuleName()+"/filter-polarity");
		
		// One should check validity of the specified parameters here!

		trig_threshold_adc = vector<int16>( trig_threshold_v.size() );
			// last statement is a simple initialization.
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

	// Suerfu on 1/30/2023
	// To get ADC counts from volt/float, one has to invert a 3-rd order polynomial since volt = a0+a1*x+a2*x^2+a3*x^3
	// where a0-3 are calibration coefficients obtained from the hardware.
	// At this moment we will first use only first two terms (linear approximation) of the calibration coefficients.
	// The error introduced is on the order of 100 mV at maximum based on previous tests.

    NIDAQdata* header = reinterpret_cast<NIDAQdata*>(rdo);
		// rdo is of type void*. First cast it to the correct type.

	for( unsigned int i=0; i<trig_channel_indices.size(); i++){
		
		int pos = header->GetChannelPosition( i );

		if( pos<0 ){
			Print("Online trigger channel is not enabled.", ERR);
			SetStatus(ERROR);
			return;
		}

		trig_threshold_adc[i] = VoltToADC( trig_threshold_v[i], header->GetCalCoeff(), pos );
	}
	

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

    //dim[0] = data->GetNChannels();
    //dim[1] = data->GetBufferPerChannel();

	bool pass = Filter( data, mode );

	if( pass==true ){
		PushToBuffer( next_addr, rdo);
	}
	else{
		PushToBuffer( prev_addr, rdo);
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


int16 NIFilter::VoltToADC( float threshold_v, float64** cal, int pos ){
	
	float64 a0 = cal[pos][0];
	float64 a1 = cal[pos][1];
	//float64 a2 = cal[pos][2];
	//float64 a3 = cal[pos][3];
	
	return (threshold_v-a0)/a1;
		// using linear approximation
}


bool NIFilter::Filter( NIDAQdata* data, string mode ){

    if( mode=="all-pass" ){
        return true;
    }

	for( unsigned int chan = 0; chan < trig_channel_indices.size(); chan++){
		
		int pos = data->GetChannelPosition( trig_channel_indices[chan] );
		
		// loop over the given data to check for over or under threshold behavior
		//

		unsigned int buffer_size = data->GetBufferPerChannel();
			// buffer per channel

		int16** buffer =  data->GetBuffer();
			// actual memory holding the ADC values

		// If positive polarity, look for over-threshold
		if( trig_polarity[chan]>0 ){
			for( unsigned int j=0; j<buffer_size; j++){
				if( buffer[pos][j] > trig_threshold_adc[chan] )
					return true;
			}
		}
        // Otherwise, do the opposite
        //
		else{
			for( unsigned int j=0; j<buffer_size; j++){
				if( buffer[pos][j] < trig_threshold_adc[chan] )
					return true;
			}
		}
	}
    
    // The function will return true if trigger condition is met at any point.
    // Reaching this line means no trigger was found.

    return false;

}
