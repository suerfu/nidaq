#!/bin/python3

import numpy as np
import matplotlib.pyplot as plt
import h5py
import sys

plt.figure( figsize=(8,6) )

for index,f in enumerate( sys.argv[1:] ):

    print("Processing", f)

    if ':' in f:
        filename = f.split(':')[0]
        
        label_prefix = filename.split("/")[-1]

        channel_str = f.split(':')[1]
        channel_list = [ int(x) for x in channel_str.split(",") ]

        if len(f.split(':')) > 2:
            label_prefix = f.split(':')[-1]
    else:
        filename = f
        channel_list = None

    file = h5py.File( filename, 'r')

    nb_events = file['adc1'].attrs['nb_events']
    nb_samples = file['adc1'].attrs['nb_samples']
    sample_rate = file['adc1'].attrs['sample_rate']
    nb_channels = file['adc1'].attrs['nb_channels']
    
    if channel_list is None:
        channel_list = range( 0, nb_channels)

    coeff = file['adc1'].attrs['adc_conversion_factor']
    poly_coeffs = coeff[:,::-1]

    time_per_frame = nb_samples / sample_rate

    fft = None

    for channel in channel_list:

        for i in range( 0, nb_events):

            time = np.linspace( 0, nb_samples/sample_rate, nb_samples)
            event = np.array( file['adc1/event_{}'.format(i+1)] )
            volts = np.polyval( poly_coeffs[channel], event[channel,:] )
            
            if i==0:
                fft = np.abs( np.fft.rfft( volts ) ) / nb_samples
                fft_freq = np.fft.rfftfreq( len(time), 1 / sample_rate )
            else:
                fft += np.abs( np.fft.rfft( volts) ) / nb_samples

        df = fft_freq[1]-fft_freq[0]
        

        label = label_prefix + f", Channel {channel}"
        
        plt.plot( fft_freq, fft/nb_events/np.sqrt(df), label=label )
        
        #output_filename = "fft_" + filename.split("/")[-1].removesuffix('.hdf5') + f"_ch{channel}.csv"
        #combined = np.column_stack( ( fft_freq, fft/nb_events/np.sqrt(df) ) )
        #np.savetxt( output_filename, combined, delimiter=',', header="Frequency (Hz),Amplitude (V/√Hz)", fmt='%.6e')

    file.close()

plt.xlabel( r'Frequency [Hz]')
plt.ylabel( r'PSD [V/$\sqrt{Hz}$]')

plt.xscale("log")
plt.yscale("log")

plt.legend()

plt.show()

