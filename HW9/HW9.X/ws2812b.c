// WS2812B library

#include "ws2812b.h"
// other includes if necessary for debugging

// Timer2 delay times, you can tune these if necessary
#define LOWTIME 15 // number of 48MHz cycles to be low for 0.35uS
#define HIGHTIME 65 // number of 48MHz cycles to be high for 1.65uS

#define HIGH_0 15 //47 per 10 scaled up
#define LOW_0 65
#define HIGH_1 65
#define LOW_1 15

/*#define HIGH_0 15 //47 per 10 scaled up
#define LOW_0 73
#define HIGH_1 33
#define LOW_1 28*/

/*#define HIGH_0 14 //47 per 10 scaled up
#define LOW_0 42
#define HIGH_1 28
#define LOW_1 28*/

// setup Timer2 for 48MHz, and setup the output pin
void ws2812b_setup() {
    T2CONbits.TCKPS = 0; // Timer2 prescaler N=1 (1:1)
    PR2 = 65535; // maximum period
    TMR2 = 0; // initialize Timer2 to 0
    T2CONbits.ON = 1; // turn on Timer2

    // initialize output pin as off
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 0;
}

// build an array of high/low times from the color input array, then output the high/low bits
void ws2812b_setColor(wsColor * c, int numLEDs) {
    int i = 0; int j = 0; // for loops
    int numBits = 2 * 4 * 8 * numLEDs; // the number of high/low bits to store, 2 per color bit
    volatile unsigned int delay_times[2*4*8 * 8]; // I only gave you 5 WS2812B, adjust this if you get more somewhere

    int test[8] = {1,1,1,1,1,1,1,1};
    int test0[8] = {0,0,0,0,0,0,0,0};
    int test5[8] = {0,0,0,0,0,0,0,1};
    
    // start at time at 0
    delay_times[0] = 0;
    
    int nB = 1; // which high/low bit you are storing, 2 per color bit, 24 color bits per WS2812B
	
    // loop through each WS2812B
    for (i = 0; i < numLEDs; i++) {
        // loop through each color bit, MSB first
        for (j = 7; j >= 0; j--) {
            // if the bit is a 1
            if (((c[i].g << j) & 128) == 128) { //c[j].g == 1test0[j] == 1
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGH_1;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOW_1;
                nB++;
            } 
            // if the bit is a 0
            else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + HIGH_0;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOW_0;
                nB++;
            }
        }
        // do it again for green
        for (j = 7; j >= 0; j--) {
            if (((c[i].r << j) & 128) == 128) { //c[j].r == 1
                delay_times[nB] = delay_times[nB - 1] + HIGH_1;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOW_1;
                nB++;
            } 
            else {
                delay_times[nB] = delay_times[nB - 1] + HIGH_0;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOW_0;
                nB++;
            }
        }
		// do it again for blue
        for (j = 7; j >= 0; j--) {
            if (((c[i].b << j) & 128) == 128) { //c[j].b == 1
                delay_times[nB] = delay_times[nB - 1] + HIGH_1;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOW_1;
                nB++;
            } 
            else {
                delay_times[nB] = delay_times[nB - 1] + HIGH_0;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOW_0;
                nB++;
            }
        }
        for (j = 7; j >= 0; j--) {
            delay_times[nB] = delay_times[nB - 1] + HIGH_0;
            nB++;
            delay_times[nB] = delay_times[nB - 1] + LOW_0;
            nB++;
        }
    }

    // turn on the pin for the first high/low
    
    /*int numBits = 4;
    int delay_times[4];
    delay_times[0] = 0;
    delay_times[1] = LOWTIME;
    delay_times[2] = HIGHTIME+LOWTIME;
    delay_times[3] = LOWTIME+HIGHTIME+LOWTIME;
    int i;*/
    
    LATAbits.LATA3 = 1;
    TMR2 = 0; // start the timer
    for (i = 1; i < numBits; i++) {
        while (TMR2 < delay_times[i]) {
        }
        LATAINV = 0b1000; // invert A3
    }
    LATAbits.LATA3 = 0;
    TMR2 = 0;
    while(TMR2 < 2353){} // wait 50uS, reset condition
}

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}