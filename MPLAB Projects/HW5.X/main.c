/* 
 * File:   main.c
 * Author: kevmo
 *
 * Created on April 15, 2022, 7:49 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "spi.h"
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "NU32.h"
#include<math.h>

#define PI 3.141592

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
unsigned short make16(char c, unsigned char v){
    unsigned short s; 
    s = 0; 
    s = s|(c<<15);
    s = s|(0b111<<12);
    s = s| (v << 4); 
    return s; 
}

int makeV(float Vout){
    int v; 
    v = (int) ((Vout*(256))/3.3);
    return v; 
}

int main() {
    NU32_Startup();
    
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
    
    initSPI();
    __builtin_enable_interrupts();
    
    float sines[360] = {0}; 
    float tris[360] = {0}; 
    
    for (int i = 0; i < 360; i++)
    {
        sines[i] = (1.65*sin(i*(PI/180)))+1.65; 
    }
    
    for (int j = 0; j < 360; j++){
        tris[j] = j*(17/24); 
    }
    
    int x = 0; 
    int parity = 0; 
    char c;
    unsigned char v;
    unsigned int s; 
    while(1){
        
        LATAbits.LATA0 = 0; //cs off
        // logic triangle
        if (parity == 0){
            v = makeV(tris[x]);
        }
        else{
            v = makeV(tris[360-x]);
        }
        c = 1;
        s = make16(c,v);
        spi_io(s>>8); 
        spi_io(s);
        LATAbits.LATA0 = 1;
        
        
        //second channel TRIANGLE
        LATAbits.LATA0 = 0;
        v = makeV(sines[x]); //change later
        c = 0; 
        s = make16(c,v);
        spi_io(s>>8); 
        spi_io(s);
        LATAbits.LATA0 = 1;

        
        x++; 
        if (x == 361){
            x = 0;
            if (parity % 2 == 0){
                parity = 1;
            }
            else{
                parity = 0;
            }
        }
        
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 48000000/(4*360)){
        }
    }
}

