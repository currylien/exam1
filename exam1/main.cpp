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
int f = 0;
float frequency_use = 0.125;
Thread t;
int sample = 128;
float ADCdata[256];

void ISR1()
{
    if (f <= 2) {
            f++;
            //uLCD.locate(0,(f-1));
            //uLCD.printf("  %f Hz", frequency[f - 1]);
            //uLCD.locate(0,f);
            //uLCD.printf("* %f Hz", frequency[f]);
            //frequency_use = frequency[f];
    }
}

void ISR2() {
    if (f >= 1) {
            f--;
            //uLCD.locate(0, (f + 1));
            //uLCD.printf("  %f Hz", frequency[f + 1]);
            //uLCD.locate(0, f);
            //uLCD.printf("* %f Hz", frequency[f]);
            //frequency_use = frequency[f];
    }
}
void wave_generate() {
    while(1){
    int k = 0;
    float gap1, gap2;
    gap1 = (1.0f  / (sample * 0.333f * frequency[f])) ;//4.166f;
    gap2 = (1.0f / (sample * 0.333f * frequency[f]));//4.166f;
    for (float j = 0; j <= 1; j = j + gap1) {
            Aout = j;
            if (k < sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
    for (float w = 0; w <= (sample - sample * (2.0f / 3) * frequency[f]); w+=1) {
            Aout = 1;
            if (k < sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
    for (float j = 1; j >= 0; j = j - gap2) {
            Aout = j;
            if (k < sample) {
                ADCdata[k] = Ain;
                k++;
            }
            ThisThread::sleep_for(1000ms / sample);
        }
    }
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