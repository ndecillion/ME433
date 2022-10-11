#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>

#include "ws2812b.h"

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use fast frc oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = OFF // primary osc disabled
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt value
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz fast rc internal oscillator
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
    TRISBbits.TRISB4 = 1;

    __builtin_enable_interrupts();
    
    ws2812b_setup();
    wsColor worker[8];
    float hues[8] = {0,30,60,90,120,150,180,210};
    int ouvrier = 0;
    
    unsigned int test[8];
       
    while (1) { 
        //worker = HSBtoRGB(90,0.5,0.5);
        //worker = HSBtoRGB(270,0,0);
        
        for (ouvrier = 0; ouvrier <= 7; ouvrier++){
            hues[ouvrier] = hues[ouvrier] + 10; // changing this increment to a lower value results in flickering LEDs
            if (hues[ouvrier] > 360){
                hues[ouvrier] = 0;
            }
            worker[ouvrier] = HSBtoRGB(hues[ouvrier],1,0.1);
        }
        
        ws2812b_setColor(&worker, 8);
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<24000000/2/20){}
        
        /*LATAbits.LATA4 = 1;
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<24000000/2/20){}
        LATAbits.LATA4 = 0;
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<24000000/2/20){}*/
    }
}
