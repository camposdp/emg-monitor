import socket
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
import json
from scipy import signal



######################################
#Initial parameters

#HOST = socket.gethostbyname(socket.gethostname())
HOST = '192.168.1.26'  # Manual host enter
PORT = 80  # Port

fs = 1000 #Sampling frequency
f0 = 60  #Notch filter frequencu
Q = 5 #Quality factor (Notch Filter)
fL = 10 #Low frequency (band-pass filter)
fH = 450 #High frequency (band-pass filter)

rms = 0 #RMS feature
F = 2 #number of sub-windows to divide the buffer
th = 0.03 #initial threshold
time_range = 4 #time range to plot
buffer_size = 64 #size of the received buffer

#define the plot size
plot_size = int(time_range*fs*F/buffer_size)

######################################
#Create filters
bf, af = signal.iirnotch(f0, Q, fs)
b1h, a1h = signal.iirnotch(2*f0, Q, fs)
b2h, a2h = signal.iirnotch(3*f0, Q, fs)
b, a = signal.iirfilter(4, [2*fL/fs, 2*fH/fs],
                        btype='band', analog=False, ftype='butter')
######################################

######################################
# Create plot area
amp = 0.65 #plot limit (y-axis)
plt.ion()
data_array = np.zeros(plot_size)+0.1
fig, = plt.plot(data_array)
th_line = plt.axhline(th, color='r', linestyle='-')
ax = plt.ylim((-amp/100,amp))
######################################

######################################
# Create Slider to choose threshold
v0 = 0
axb = plt.axes([0.1, 0.9, 0.8, 0.03])
sb = Slider(axb, 'b', 0.01, 1.0, valinit=v0)
######################################

#plt.pause(1)

######################################
# Functions
######################################

#Function to update the threshold
def update(val):
    global th
    th = val
    th_line.set_ydata(th)
    

#Function to plot the data (and update)
def plotdata(data):
    global data_array
    
    sb.on_changed(update)    
    fig.set_ydata(np.append(fig.get_ydata()[len(data):], data))
    plt.draw()    
    plt.pause(0.01)

#Function to calculate rms value
    # data: EMG buffer
    # N: number of sub-windows
def rmsVector(data,N):
    rms = np.zeros(N)
    ndata = np.array_split(data,N)
    for i in range(len(ndata)):
        y = ndata[i]
        rms[i] = np.sqrt(np.mean(y**2))
    return rms
    

#Main function
def connected(c):
    
    global data, msg, rms
    


    while True:
        
        #ack = sock.recv(1).decode()
        #print(ack)
        
        #print('bip')
        msg = c.recv(4096) #receive data...
        #print('bop')
        #if not msg: break
        if msg is not None:
            #print('zip')
                    
            #Convert the data arriving
            data_msg = json.loads(msg)
            
            #Filter the data
            y = signal.filtfilt(bf, af, data_msg)
            y = signal.filtfilt(b1h, a1h, y)
            y = signal.filtfilt(b2h, a2h, y)
            y = signal.filtfilt(b, a, y)
           
            #Calculate the RMS
            rms = rmsVector(y,F)
            #print(rms)
            #Plot data
            plotdata(rms)
                    
            #compare data to the threshold...
            #and encode a mensage to send.
            if np.mean(rms)>th: 
                c.send(("a \n").encode()) 
            else:
                c.send(("b \n").encode())
        
    #_thread.exit()


######################################
# Main Code
######################################
                
#Create Socket                
sock = socket.socket()
sock.connect((HOST, PORT))
#tcp.bind(orig)
#tcp.listen(1)

while True:
    try:
        #con, cliente = tcp.accept()
        #_thread.start_new_thread(connected, tuple([con, cliente]))
        
        #do things
        connected(sock)
    except Exception as e: print(e)

con.close()
tcp.close()
