#include "plrsBaseData.h"
#include "RawRecorder.h"

#include <ctime>
#include <vector>


extern "C" RawRecorder* create_RawRecorder( plrsController* c ){ return new RawRecorder(c);}


extern "C" void destroy_RawRecorder( RawRecorder* p ){ delete p;}


RawRecorder::RawRecorder( plrsController* c) : plrsStateMachine(c){}


RawRecorder::~RawRecorder(){;}


void RawRecorder::Configure(){

    string filename = cparser->GetString( "/cmdl/output" );

    if( filename == "" )
        filename = cparser->GetString( "/cmdl/file" );

    if( filename == "" )
        filename = cparser->GetString( "/module/"+GetModuleName()+"/output" );

    if( filename == "" )
        filename = cparser->GetString( "/module/"+GetModuleName()+"/file" );

    if( filename != "" ){

        output_file.open( filename, ios_base::out | ios::binary);
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


void RawRecorder::Deconfigure(){
    Print( "Closing output file...\n", DETAIL);
    if( output_file.is_open())
        output_file.close();
}


void RawRecorder::PreRun(){
/*
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
*/
}



void RawRecorder::Run(){

    void* rdo = PullFromBuffer();

    while( GetState()==RUN ){
        if( rdo==0 ){
            rdo = PullFromBuffer();
	    if( rdo==0 )
	    	continue;
        }
	    if( output_file ){

            NIDAQdata* data = reinterpret_cast< NIDAQdata* >(rdo);
            if( data->FmtI16() )
                output_file.write( reinterpret_cast<char*>(data->buffer), sizeof(int16)*data->buffsize );
            else
                output_file.write( reinterpret_cast<char*>(data->buffer), sizeof(float64)*data->buffsize );
	    }

	    PushToBuffer( addr_nxt, rdo);
	    rdo = 0;
    }
}



