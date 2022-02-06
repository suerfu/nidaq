# nidaq

*nidaq* is a Linux library of [polaris](https://github.com/suerfu/polaris) modules. These modules are loaded by the polaris executable at runtime and are designed to provide DAQ functionalities related to configuring and reading data from National Instruments analog-to-digital converter (ADC) cards and storing them in HDF5 format. *nidaq* runs only on Linux operating system. Currently, it is tested only on CentOS 8 with NI PCIe-6376. However, the library and its dependant libraries use general Linux libraries and can potentially run on other Linux distributions as well.

Currently *nidaq* version is 1.0.0.

## Requirements
*nidaq* library depends on three external libraries and executables:
+ [polaris](https://github.com/suerfu/polaris)
+ [NI-DAQmx](https://www.ni.com/en-us/support/documentation/supplemental/18/daqmx-for-desktop-linux-tools.html)
+ [H5CPP](http://h5cpp.org/)

The polaris executable provides the main DAQ framework within which *nidaq* modules are invoked. NI-DAQmx libraries are required for configuring and reading data out from the ADC hardware. NI-DAQmx library can be downloaded [here](https://www.ni.com/en-us/support/downloads/drivers/download.ni-daqmx.html#428058). After download, the repo can be installed with `yum install foo.rpm` and then ni-daqmx library can be installed by running `yum install ni-daqmx`. A list of packages in the repo can be found [here](https://www.ni.com/pdf/manuals/378353f.html).

H5CPP libraries are needed to write data in HDF5 formats. Future versions might include modules that write data in other formats than HDF5 and thus HDF5 might not be mandatory. For installation of these programs, please see their webpages.


## Download, Compilation and Installation
To download *nidaq* library, in the terminal type
```
git clone https://github.com/suerfu/nidaq.git
```
Next change directory to *nidaq* and compile and install the library by
```
cd nidaq
make
```
This will compile the library under `nidaq/lib`. Note that this step requires the polaris library, NI-DAQmx library and H5CPP library to be installed in a location ld linker can find (such as /usr/lib). One can check the ld search path by
```
ldconfig -v 2>/dev/null | grep -v ^$'\t'
```
If these libraries are installed in an unreachable location, one can set the LD_LIBRARY_PATH environmental variable to include additional locations.


## Usage

The main usage is
```
polaris --cfg foo.cfg [--time T] [--file file.name] [--prefix file.prefix] [--event N] [--comment "This is a comment"]
```
+ foo.cfg is name of the configuration file for your task. This configuration file includes information such as what *nidaq* modules to load, how to configure the hardware/output, and how data should be passed across different modules. One can easily implement other custom modules and include it in the application. For more details on the config file and required fields, see the polaris page.
+ --file will set the output file.
+ --prefix will set the prefix of output file. The actual output file will include addiitonal information such as the timestamp.
+ --event will set the maximum number of events to record.
+ --comment will append the user comment in the output file. One can use it to record additional auxiliary information.

Above are commonly used commandline arguments. Additional arguments (general for polaris) includes:
+ -q and --quiet optional arguments will suppress all output except error messages.
+ --log bar.log optional argument will keep another version of output in bar.log log file in addition to the standard output.
+ --help libxxx.so will print the usage information from libxxx.so. This works only if the target library is a polaris module.

## Example Config File

Below shows an example *nidaq* config file to be used with polaris. Comments start with # and will not be read by the program. Note that there are spaces on either sides of the colon and a comma at the end of each line.
```
module {
    daq { 
        lib : ./lib/libnidaq.so,
        fcn : NIdaq,
        enable : true,
        next : recorder,
    }   

    recorder { 
        lib : ./lib/libnidaq.so,
        fcn : HDF5Recorder,
        enable : true,
        next : daq,
    }   
}

max_MB_per_file : 5000,

prefix : /path/to/prefix,

comment : "This is a comment",

adc1 {
    data_mode : cont,       # choose from cont trig-ext trig-int threshold
    sample_rate : 1250000,  # in Hz
    nb_samples  : 12500,

    channel : Dev1/ai1,   # can also be specified as a range such as Dev1/ai0:7
    Vmin : -10,           # in Volt
    Vmax :  10, 
    
    trig_channel : Dev1/ai1,  # Dev1/pfi0, etc. for trig-ext
    trig_polarity : true,     # true for positive edge.
    trig_threshold : 0.2,     # measured in Volt.
    pre_trig_sample : 8000,
 
    channel0 : raw,
    channel1 : 10kHz-filtered,
}

detconfig {
    key1 : value1,
    ...
}
```

+ module keyword
  - module keyword is mandatory. It tells polaris which module(s) to load and where to find them (lib)
    + Currently NIdaq and HDF5Recorder are the only two modules implemented.
  - The enable keyword is used to turn the feature on and off.
  - The next keyword specifies which module to give data to after each module acquires/processes the data.
    + For efficiency, data are handled by address, not value. Therefore the data path must be implemented and closed for the program to function properly.
    + The name of next module must be one of the module names already defined (in this example daq and recorder).
+ max_MB_per_file
  - sets the maximum number of MB in a single output file before the HDF5Recorder module closes current file and opens a new file with an incremented dump number.
+ prefix
  - prefix of output file names. The prefix specified in config file will be overriden by commandline if specified.
+ comment
  - User can specify custom comment to be appended to the output file. This comment will be joined by the comment from commandline.
+ adc1 includes keywords relevant to the first ADC card used in the application. Currently the library supports only one ADC card. Future versions might include multiple ADC cards.
  - data_mode can be cont (continuous), trig-ext (triggered by external trigger input), trig-int (periodic trigger by software) and threshold (internal threshold trigger).
  - sample_rate is the ADC sampling frequency in Hertz.
  - nb_samples sets the number of data samples per dataset. Each dataset is considered as one event even in continuous mode.
  - channel specifies the channels to be enabled. Enabled channels can be enumerated or specified as range such as Dev1/ai0:7.
  - Vmin/Vmax sets the min/max voltages (in volt) expected in each corresponding channels.
  - trig_channel specifies which port to use as trigger input.
    + This is only meaningful in trig-ext and threshold mode.
    + In trig-ext mode, trig_channel should be set as one of the pfi ports (e.g. Dev1/pfi0).
    + In threshold mode, this channel is one of the ai0:7 channels.
    + trig_polarity is true for trigger upon rising edge, and false for falling edge, for both external and threshold trigger.
    + trig_threshold sets the threshold (in unit of measurement, not ADC) for analog trigger threshold mode. The unit for NI PCIe-6736 is volt.
    + pre_trig_sample sets the number of data samples to be recorded when trigger condition is met in either external or threshold trigger mode.
  - channel0, channel1, etc. records connection information. This field is not mandatory and it's metadata is not used by DAQ, though subsequent pipeline might make use of connection details.
+ detconfig
  - Other detector-specific informations can be included as metadata in the output. The HDF5Recorder will record whatever directory name and key-value pairs specified. The only requirement is not to conflict with keywords (module and adcx).
