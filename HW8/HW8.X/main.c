#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include<stdio.h>

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
    
    i2c_master_setup();
    ssd1306_setup();
    
    //char text_message[101] = "The Revolution introduced me to art, and in turn, art introduced me to the Revolution! - A. Einstein";
    //char text_message[100] = "Workers of the work, unite! You have nothing to lose but your chains!";
    char text_message[100] = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    char fps_counter[50];
    float fps;
    int i = 0;
    char purge = 0;
    
    
    
    while (1) {  
        if (!PORTBbits.RB4){
            ssd1306_drawMessage(0,0,text_message);
            purge = 1;
        }
        if (purge == 1 && PORTBbits.RB4){
            ssd1306_clear();
            purge = 0;
        }
        
        i++;
        if (i == 10000){
            i = 0;
        }
        
        LATAbits.LATA4 = 1;
        
        _CP0_SET_COUNT(0);
        ssd1306_update();
        fps = 48000000.0/_CP0_GET_COUNT();
        
        LATAbits.LATA4 = 0; 
        
        sprintf(fps_counter, "FPS = %f", fps);
        ssd1306_drawMessage(0,0,fps_counter);
        sprintf(fps_counter, "%d", i);
        ssd1306_drawMessage(0,8,fps_counter);
    }
}
