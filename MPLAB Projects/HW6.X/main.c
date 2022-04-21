#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "NU32.h"
#include "i2c_master_noint.h"

#define ADRESS 0b01000000
#define IODIR 0x00//find in data sheet
#define GPIO 0x09
#define OLAT 0x0A

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
#pragma config USERID = 0xabcd // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
void mcp_write(unsigned char Adress, unsigned char reg, unsigned char val){
    i2c_master_start();
    i2c_master_send(Adress);
    i2c_master_send(reg);
    i2c_master_send(val);
    i2c_master_stop();
    
}

unsigned char mcp_read(unsigned char Adress, unsigned char reg){
    i2c_master_start();
    i2c_master_send(Adress);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(Adress|0b1);
    unsigned char r = i2c_master_recv();
    i2c_master_ack(1); 
    i2c_master_stop();  
    return r; 
}

int main() {
    //NU32_Startup();
    
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
    
    U1RXRbits.U1RXR = 0b0000; // Set A2 to U1RX
    RPB3Rbits.RPB3R = 0b0001; // Set B3 to U1TX
    
    
    // turn on i2c
    i2c_master_setup();
    //init the mcp23008
    //send start bit
    // send the address with write bit
    // send the name of the reg to change IODIR
    // send the 8 bits for IO
    // send stop bit
    
    __builtin_enable_interrupts();

    TRISBbits.TRISB4 = 1; 
    TRISAbits.TRISA4 = 0;
    mcp_write(ADRESS, IODIR, 0b01111111);
    
    
    
    //read what pins are on
    //
    /*
    unsigned char r = mcp_read(ADRESS, OLAT); 
    
    
     */
    unsigned char r; 
    while (1) {
        
        r = mcp_read(ADRESS, GPIO);
        if (r&0b1 == 0b1){ 
            mcp_write(ADRESS, OLAT, 0b10000000);
        }
        else{ 
            mcp_write(ADRESS, OLAT, 0b00000000);
        } 
        //heart
        if (_CP0_GET_COUNT() > 12000000){
            LATAbits.LATA4 = 1;
        }
        else{
            LATAbits.LATA4 = 0;
        }
        
        if (_CP0_GET_COUNT() > 24000000){
            _CP0_SET_COUNT(0); 
        }
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk
            
    }
}

