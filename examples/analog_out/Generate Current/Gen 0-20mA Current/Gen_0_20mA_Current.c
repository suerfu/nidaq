/*********************************************************************
*
* ANSI C Example program:
*    Gen_0_20mA_Current.c
*
* Example Category:
*    AO
*
* Description:
*    This example demonstrates how to generate a single current value
*    on one or more current output channels.
*
* Instructions for Running:
*    1. Select the physical channel or channels which correspond to
*       where your signal is to be generated.
*    2. Enter the minimum and maximum current ranges, in amps (not
*       milliamps).
*    3. Enter a current value to generate in the data array. There
*       should be one array value for each channel specified in the
*       Physical Channels control. The values will be generated in
*       the order that they appear in the Physical Channels control.
*
*    Note: Just like the minimum and maximum current ranges, the data
*          values to generate are in units of amps, not milliamps.
*
* Steps:
*    1. Create a task.
*    2. Create an Analog Output Current Channel.
*    3. Use the Write function to Output 1 Sample to 1 Channel.
*    4. Call the Clear Task function to clear the Task.
*    5. Display an error if any.
*
* I/O Connections Overview:
*    The SCXI-1124 can operate on either an external or internal
*    current source. The only difference is in the signal
*    connections. When using the internal current source, connect a
*    load between the SUPPLY and ISINK terminals. When using an
*    external current source, connect the source and load to the
*    ISINK and GND terminals. In either case, be sure that the
*    channel numbers of the terminals used match the channel numbers
*    specified in the Physical Channels control.
*
*    Note: When using an external current source, be careful to avoid
*          creating an uncontrolled current loop. See the SCXI-1124
*          User's Manual for more information.
*
*    The NI 6238 and NI 6239 devices require external current
*    sources. See the M Series User Manual for more information about
*    signal connections to these devices.
*
*    The output current can be measured by connecting an ammeter in
*    series with the current loop. Alternatively, the current can be
*    measured by replacing the load with a resistor of known value.
*    By measuring the voltage across the resistor and dividing by the
*    resistance, the current through the resistor can be calculated
*    (Ohm's law).
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int         error=0;
	TaskHandle	taskHandle=0;
	char        errBuff[2048]={'\0'};
	float64     data[1] = {0.01};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAOCurrentChan(taskHandle,"Dev1/ao0","",0.0,0.2,DAQmx_Val_Amps,""));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Write Code
	/*********************************************/
	DAQmxErrChk (DAQmxWriteAnalogF64(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,data,NULL,NULL));

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error %s\n",errBuff);
	else
		printf("Current Generated\n");
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}
