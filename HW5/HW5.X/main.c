#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include<stdio.h>

#include "spi.h"

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
    
    initSPI();

    __builtin_enable_interrupts();
    
    unsigned short i = 0;
    unsigned int a_start = 0b0111;
    unsigned int b_start = 0b1111;
    unsigned int a;
    unsigned int b;
    int worker0;
    double worker1;
    char rising = 1;
    
    while (1) {
        LATAbits.LATA0 = 0;
        
        if (rising == 1){
            a = (a_start<<12)|(i<<4);
        }
        else {
            a = (a_start<<12)|((255 - i)<<4);
        }
        spi_io(a>>8);
        spi_io(a);
        
        LATAbits.LATA0 = 1;
        LATAbits.LATA0 = 0;
        
        worker1 = (double)i / 255*360;
        worker1 = 127*(sin(worker1 * (M_PI/180))) + 128;
        worker0 = worker1;
        b = (b_start<<12)|(worker0<<4);
        spi_io(b>>8);
        spi_io(b);

        LATAbits.LATA0 = 1;  
                
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() <= 24000000/512){
            }
        i++;
        if (i > 254){
            i = 0;
            rising = -1*rising;
        }
    }
}

