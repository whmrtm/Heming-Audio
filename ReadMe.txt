Work done for Undergraduate Research Assistant:

September:

Work on UBS-driven devices

Realtime display of audio amplitude and audio freqency:
  owen_recorder.py, RealtimeAmplitude.py, RealtimeFrequency.py
  Implemented by matplot and Qt, give intuitive picture of sound received by recorder.

Python program that play and record wav file: play_and_record.py
 
October:

Demonstation programs of using PyAudio library:
  Folder of Pyaudio-example:
    Play wav file     Play wav file with callback function
    Record wav file    Wire        Wire and callback

Begin to work on myDAQ device using python:
  DAQTestTone.py - based on the library PyDAQmx
  Connect ao0 and ai0 channel of myDAQ. Read data from a wav file, then write it to analog output channel,
  Read data from analog input channel, and plot wave  graphs for both channels. Compare their differences.

November, December:

Begin to use C program to manipulate myDAQ:
  Devcpp as the develope environment, use the library and header file provided by National Instrument.
  Generate an exe program that can test tone between analog channels of myDAQ.
Successfully apply the program to test tone in Octave and Matlab.
    