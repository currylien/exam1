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

void print_data() {
    for (int i = 0; i < 128; i++){
        printf("%f\r\n", ADCdata[i]);
        ThisThread::sleep_for(10ms);
    }  
}

void ISR1()
{
    if (f <= 2) {
            f++;
    }
}

void ISR2() {
    if (f >= 1) {
            f--;
    }
}
void wave_generate() {
    int k = 0;
    float gap1, gap2;
    int g = 1 - (2.0f / 3.0f) *frequency[f];
    gap1 = (1.0f  / (sample * 0.333f * frequency[f])) *4.166f;
    gap2 = (1.0f / (sample * 0.333f * frequency[f]))*4.166f;
    for (float j = 0; j <= 1; j = j + gap1) {
            Aout = j;
            if (k < sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
    //for (float w = 0; w <= (sample - sample * (2.0f / 3) * frequency[f]); w+=1) {
            Aout = 1;
            while (k < (sample *  (test[f] / 240))){
                ADCdata[k] = 1;
                k++;
            }
            ThisThread::sleep_for(wait[f] * 1ms);
        //}
    for (float j = 1; j >= 0; j = j - gap2) {
            Aout = j;
            if (k < sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
    queue.call(print_data);
}
int main()
{
    swup.rise(&ISR1);
    swdown.rise(&ISR2);
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    swselect.rise(queue.event(wave_generate));
    uLCD.reset();
    uLCD.cls();
    uLCD.color(GREEN);
    uLCD.printf("* %f\n", frequency[0]);
   while(1) {
       uLCD.printf("%f", frequency[f]);
       uLCD.cls();
   }
}