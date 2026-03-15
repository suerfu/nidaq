#!/bin/python3

import numpy as np
import matplotlib.pyplot as plt
import h5py
import sys

plt.figure()

for filename in sys.argv[1:]:

    file = h5py.File( filename, 'r')

    nb_events = file['adc1'].attrs['nb_events']
    nb_samples = file['adc1'].attrs['nb_samples']
    sample_rate = file['adc1'].attrs['sample_rate']
    nb_channels = file['adc1'].attrs['nb_channels']
    
    coeff = file['adc1'].attrs['adc_conversion_factor']
    poly_coeffs = coeff[:,::-1]

    time_per_frame = nb_samples / sample_rate

    fft = None

    for channel in range(0,nb_channels):

        volt = []

        for i in range( 0, nb_events):

            event = np.array( file['adc1/event_{}'.format(i+1)] )
            volt.append( np.polyval( poly_coeffs[channel], event[channel][::1000] ) )
       
        volt = np.concatenate( volt )
        time = np.linspace( 0, nb_events * nb_samples/sample_rate, len(volt) )

        plt.plot( time, volt, label=filename.split("/")[-1]+f"Channel {channel}" )
        
        #output_filename = "fft_" + filename.split("/")[-1].removesuffix('.hdf5') + f"_ch{channel}.csv"
        #combined = np.column_stack( ( fft_freq, fft/nb_events/np.sqrt(df) ) )
        #np.savetxt( output_filename, combined, delimiter=',', header="Frequency (Hz),Amplitude (V/√Hz)", fmt='%.6e')

    file.close()

plt.xlabel('Time [s]')
plt.ylabel('Voltage [V]')

#plt.xscale("log")
#plt.yscale("log")

plt.legend()

plt.show()

