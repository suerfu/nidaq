
#include <string>
#include <iostream>

#include <stdio.h>
#include <NIDAQmx.h>

// #define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

using namespace std;

int main(int argc, char* argv[]){

	string chan = "Dev1/ai0";
	float Vmin = -2.;
	float Vmax = 2.;

	float sample_freq = 3000000;
	int32 mode = DAQmx_Val_ContSamps;
	const uInt64 buff_size = 1000000;

	int32       error=0;
	TaskHandle  taskHandle=0;
	char        errBuff[2048]={'\0'};

	int  totalRead = 1000000;
	int32 read=0;
	float64 data[buff_size];

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

	//DAQmxRegisterEveryNSamplesEvent(taskHandle,DAQmx_Val_Acquired_Into_Buffer,1000,0,EveryNCallback,NULL);
	//DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL);

	DAQmxStartTask(taskHandle);
	cout << "DAQ starting..." << endl;
	
	for( int i=0; i<totalRead; i++){
		error = DAQmxReadAnalogF64( taskHandle, buff_size, 0, DAQmx_Val_GroupByChannel, data, buff_size, &read, NULL);
		if( error==-200284 || read>0 ){	
			//cout << read << " samples read " << endl;
			for( int i=0; i<read; i++ )
				cerr << data[i] << "\n";
		}
		else{
			DAQmxGetExtendedErrorInfo(errBuff,2048);
			cout << "Error code " << error << " : " << errBuff << endl;
		}
	}

	DAQmxStopTask(taskHandle);
	DAQmxClearTask(taskHandle);

	return 0;
/*
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
*/
}

/*
int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32       error=0;
	char        errBuff[2048]={'\0'};
	static int  totalRead=0;
	int32       read=0;
	float64     data[1000];

	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByScanNumber,data,1000,&read,NULL));
	if( read>0 ) {
		printf("Acquired %d samples. Total %d\r",(int)read,(int)(totalRead+=read));
		printf("0th sample: %f\n", data[0]);
		fflush(stdout);
	}

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   error=0;
	char    errBuff[2048]={'\0'};

	// Check to see if an error stopped the task.
	DAQmxErrChk (status);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}
*/
