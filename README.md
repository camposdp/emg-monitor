# emg-monitor

The main ideia is to measure EMG data using the ADC of a microcontroller (ESP32) and to send chunks of the data to PC via socket. The Python code filter the EMG, calculate the RMS value, plot in real-time, compare the data to a threshold and send a flag if the value surpass a threshold. 

The code still under construction.


Update:

-The first version has an issue with the reconnection which was making the data transfer slow. This issue was corrected.
-New Python code was added to dynamicly plot a bar instead of the amplitude over time.

Last version: 
-PC_getEMG_setFlag.py
-ESP32_sendEMG_getFlag

Some bugs were fixed. Python send the threhold (comparing the EMG signal) and information when button state is changed. 
ESP32 connect directly now and blink led if threhold flag arrives. 

Future devs> 
- Include FES code (pulse modulation) 
- Maybe include some processing fom python into the ESP32
- Paralell processing and/or acquisition in the interruption. 
