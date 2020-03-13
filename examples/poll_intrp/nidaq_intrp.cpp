
#include <string>
#include <iostream>

#include <stdio.h>
#include <NIDAQmx.h>

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

using namespace std;

struct IOStat{
	float64* data;
	int total_read;
	int read;
};

const int bs = 10000;

float64 glb_data[bs];

int main(int argc, char* argv[]){

	string chan = "Dev1/ai0";
	float Vmin = -5.;
	float Vmax = 5.;

	//float sample_freq = 3.e5;
	float sample_freq = 1000000.;
	int32 mode = DAQmx_Val_ContSamps;
	const uInt64 buff_size = bs;

	int32       error=0;
	TaskHandle  taskHandle=0;

	//int  totalRead = 10;
	//int32 read=0;
	//float64 data[buff_size];
	IOStat stat;
	stat.data = new float64[buff_size];
	stat.total_read = 0;
	stat.read = 0;
	
	error = DAQmxCreateTask("",&taskHandle);
	if( error<0 ){
		cout << "Error creating task handle...\n";
		return -1;
	}
	else{
		cout << "Task created." << endl;
	}

	DAQmxCreateAIVoltageChan( taskHandle, chan.c_str(), "", DAQmx_Val_Cfg_Default, Vmin, Vmax, DAQmx_Val_Volts, NULL);
	DAQmxCfgSampClkTiming( taskHandle, "", sample_freq, DAQmx_Val_Rising, mode, buff_size);
	cout << "Task configured." << endl;

	DAQmxRegisterEveryNSamplesEvent( taskHandle, DAQmx_Val_Acquired_Into_Buffer, buff_size, 0, EveryNCallback, &stat);
	DAQmxRegisterDoneEvent( taskHandle, 0, DoneCallback, NULL);

	DAQmxStartTask(taskHandle);
	cout << "DAQ starting..." << endl;
	while( getchar()!='q'){}
	
	DAQmxStopTask(taskHandle);
	DAQmxClearTask(taskHandle);

	return 0;
}


int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData){
	IOStat* stat = reinterpret_cast<IOStat*>(callbackData);
	int error = DAQmxReadAnalogF64( taskHandle, bs/2, 0, DAQmx_Val_GroupByChannel, glb_data/*stat->data*/, bs, &(stat->read),NULL);
	if( stat->read>0 ){
	//	cout << "call-back " << stat->read << endl;
	/*	for( int i=0; i<(stat->read); i++){
			cerr << (stat->data)[i] << endl;
		}*/
	}
	else{
		char errBuff[2048]={'\0'};
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		cout << "Error " << errBuff << endl;
	}
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData){
	IOStat* stat = reinterpret_cast<IOStat*>(callbackData);
	DAQmxReadAnalogF64( taskHandle, 10000, .001, DAQmx_Val_GroupByChannel, stat->data, 10000, &(stat->read),NULL);
	cout << "Cleaning up...\n";
	cout << stat->read << "samples remaining in the buffer...\n";
	for( int i=0; i<(stat->read); i++){
		cerr << (stat->data)[i] << endl;
	}
	return 0;
}
