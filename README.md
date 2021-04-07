# exam1
```c
#include "mbed.h"
#include "uLCD_4DGL.h"

InterruptIn swup(D3);
InterruptIn swdown(D5);
InterruptIn swselect(D6);
InterruptIn sw3(USER_BUTTON);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
uLCD_4DGL uLCD(D1, D0, D2);
AnalogOut Aout(D7);
AnalogIn Ain(A0);

float frequency[4] = {0.125, 0.25, 0.5, 1};
int wait[4] = {220, 200, 160,80};
int test[4] = {230, 220, 200, 160};
int f = 0;
float frequency_use = 0.125;
Thread t;

int sample = 128;
float ADCdata[256];

void print_data() {                         // printdata
    for (int i = 0; i < 128; i++){
        printf("%f\r\n", ADCdata[i]);
        ThisThread::sleep_for(10ms);
    }  
}

void ISR1()                                // up button
{
    if (f <= 2) {
            f++;
    }
}

void ISR2() {                             // down button
    if (f >= 1) {
            f--;
    }
}
void wave_generate() {                    // generate wave
    int k = 0;
    float gap1, gap2;
    int g = 1 - (2.0f / 3.0f) *frequency[f];
    gap1 = (1.0f  / (sample * 0.333f * frequency[f])) *4.166f;  
    gap2 = (1.0f / (sample * 0.333f * frequency[f]))*4.166f;    
    for (float j = 0; j <= 1; j = j + gap1) {             // wave for slope up
            Aout = j;
            //if (k < sample) {
                ADCdata[k] = Aout;
                k++;
            //}
            ThisThread::sleep_for(1000ms / sample);
        }
    //for (float w = 0; w <= (sample - sample * (2.0f / 3) * frequency[f]); w+=1) { 
            Aout = 1;
            while (k < (sample *  (test[f] / 240.0f))){
                ADCdata[k] = 1;
                k++;
            }
            ThisThread::sleep_for(wait[f] * 1ms);
        //}
    for (float j = 1; j >= 0; j = j - gap2) {    // wave for slope down
            Aout = j;
            //if (k < sample) {
                ADCdata[k] = Aout;
                k++;
            //}
            ThisThread::sleep_for(1000ms / sample);
        }
        //printf("%d", k);
    queue.call(print_data);
}
int main()                                  // main function
{
    swup.rise(&ISR1);                       // interrupt for button
    swdown.rise(&ISR2);
    t.start(callback(&queue, &EventQueue::dispatch_forever));   // eventqueue for generate wave
    swselect.rise(queue.event(wave_generate));
    uLCD.reset();
    uLCD.cls();
    uLCD.color(GREEN);
    uLCD.printf("* %f\n", frequency[0]);
   while(1) {                                        // show current frequency
       uLCD.printf("%f", frequency[f]);
       uLCD.cls();
   }
}
```
```python
import matplotlib.pyplot as plt
import numpy as np
import serial
import time

Fs = 128.0;  # sampling rate
Ts = 1.0/Fs; # sampling interval
t = np.arange(0,1,Ts) # time vector; create Fs samples between 0 and 1.0 sec.
y = np.arange(0,1,Ts) # signal vector; create Fs samples

n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # a vector of frequencies; two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range

serdev = '/dev/ttyACM3'
s = serial.Serial(serdev)
for x in range(0, int(Fs)):
    line=s.readline() # Read an echo string from B_L4S5I_IOT01A terminated with '\n'
    # print line
    y[x] = float(line)

Y = np.fft.fft(y)/n*2 # fft computing and normalization
Y = Y[range(int(n/2))] # remove the conjugate frequency parts

fig, ax = plt.subplots(2, 1)
ax[0].plot(t,y)
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Amplitude')
ax[1].plot(frq,abs(Y),'r') # plotting the spectrum
ax[1].set_xlabel('Freq (Hz)')
ax[1].set_ylabel('|Y(freq)|')
plt.show()
s.close()
```
