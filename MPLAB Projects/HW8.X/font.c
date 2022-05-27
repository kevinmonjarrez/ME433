#include "font.h"

#include <string.h> // for memset
#include <xc.h>
#include "ssd1306.h"

void drawChar(unsigned char x, unsigned char y, unsigned char letter){
    
    for (int i = 0; i < 5; i++){
        char col = ASCII[letter-32][i]; 
        
        for (int j = 0; j < 8; j++){
            char on_or_off = (col>>j)&0b1;
            ssd1306_drawPixel(x+i, y+j, on_or_off); 
            
        }
    }
    
    
    
}
void drawString(char *m, unsigned char x, unsigned char y){
    int k =0;
    while (m[k] != 0){
        drawChar(x,y, m[k]);
        k++; 
        x = x + 5; 
        if (x > 115){
            y = y + 8; 
            x = 0; 
        }
    }
    
    ssd1306_update();
}



