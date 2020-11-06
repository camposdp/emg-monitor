# emg-monitor

The main ideia is to measure EMG data using the ADC of a microcontroller (ESP32) and to send chunks of the data to PC via socket. The Python code filter the EMG, calculate the RMS value, plot in real-time, compare the data to a threshold and send a flag if the value surpass a threshold. 

The code still in construction.
