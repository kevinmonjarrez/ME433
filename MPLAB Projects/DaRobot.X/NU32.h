#ifndef NU32__H__
#define NU32__H__

#include <xc.h>                     // processor SFR definitions
#include <sys/attribs.h>            // __ISR macro

#define NU32_USER PORTDbits.RD7     // USER button on the NU32 board
#define NU32_SYS_FREQ 48000000ul    // 80 million Hz

void NU32_Startup(void);
void NU32_ReadUART1(char * string, int maxLength);
void NU32_WriteUART1(const char * string);

#endif // NU32__H__