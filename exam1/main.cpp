#include "mbed.h"
#include "uLCD_4DGL.h"

InterruptIn swup(D3);
InterruptIn swdown(D5);
InterruptIn swselect(D6);
InterruptIn sw3(USER_BUTTON);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
uLCD_4DGL uLCD(D1, D0, D2);

float frequency[4] = {0.125, 0.25, 0.5, 1};
int f = 0;
float frequency_use = 0.125;
Thread t;

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
    
}
int main()
{
    swup.rise(&ISR1);
    swdown.rise(&ISR2);
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    sw3select.rise(queue.event(wave_generate));
    uLCD.reset();
    uLCD.cls();
    uLCD.color(GREEN);
    uLCD.printf("* %f\n", frequency[0]);
   while(1) {
       uLCD.printf("%f", frequency[f]);
       uLCD.cls();
   }
}