#include "plrsBaseData.h"
#include "AsciiRecorder.h"

#include <ctime>
#include <vector>


extern "C" AsciiRecorder* create_AsciiRecorder( plrsController* c ){ return new AsciiRecorder(c);}


extern "C" void destroy_AsciiRecorder( AsciiRecorder* p ){ delete p;}


AsciiRecorder::AsciiRecorder( plrsController* c) : plrsStateMachine(c){}


AsciiRecorder::~AsciiRecorder(){;}


void AsciiRecorder::Configure(){

    string filename = cparser->GetString( "/cmdl/output" );

    if( filename == "" )
        filename = cparser->GetString( "/cmdl/file" );

    if( filename == "" )
        filename = cparser->GetString( "/module/"+GetModuleName()+"/output" );

    if( filename == "" )
        filename = cparser->GetString( "/module/"+GetModuleName()+"/file" );

    if( filename != "" ){

        output_file.open( filename, ios_base::out );
        if( !output_file.is_open() || !output_file.good() ){
            Print( "Error: cannot open "+filename+" for writing\n", ERR);
            SetStatus(ERROR);
            return;
        }
        else{
            Print( "opened file "+filename+" for writing output\n", INFO);
        }
    }

    else{
        Print( "Warning: no output file specified\n", INFO);
    }
}


void AsciiRecorder::Deconfigure(){
    Print( "Closing output file...\n", DETAIL);
    if( output_file.is_open())
        output_file.close();
}


void AsciiRecorder::PreRun(){

    if( output_file ){
        if( cparser->GetString("/cmdl/no-header")=="" && cparser->GetString("/module/"+GetModuleName()+"/no-header")=="" ){
            cparser->Print( output_file, "# " );

            time_t rawtime;
            struct tm* loctime;
            time( &rawtime );
            loctime = localtime( &rawtime);

            output_file << "# " << asctime( loctime) << endl;
        }
    }
}



void AsciiRecorder::Run(){

    void* rdo = PullFromBuffer();

    while( GetState()==RUN ){
        if( rdo==0 ){
            rdo = PullFromBuffer();
	    if( rdo==0 )
	    	continue;
        }
	if( output_file ){

        NIDAQdata* data = reinterpret_cast< NIDAQdata* >(rdo);
        
        int incr_chan, incr_time;
            // Incremental size across channels and times.
            // The values will be changed based on how data is grouped.

        if ( data->group == DAQmx_Val_GroupByScanNumber ){
            incr_chan = 1;  // next channel is 1 sample away
            incr_time = data->nchan;
        }
        else{
            incr_chan = data->buff_per_chan;
            incr_time = 1;  // within each channel, next sample follows immediately.
        }
        
        for( int i=0; i<data->read; i++ ){
            for( int j=0; j<data->nchan; j++){
                if( data->FmtI16() ){
                    int16* buffer = data->GetBufferI16();
                    output_file << buffer[incr_time*i+incr_chan*j] << ' ';
                }
                else{
                    float64* buffer = data->GetBufferF64();
                    output_file << buffer[incr_time*i+incr_chan*j] << ' ';                
                }
            }
            output_file << endl;
        }
	}
	PushToBuffer( addr_nxt, rdo);
	rdo = 0;
    }
}



