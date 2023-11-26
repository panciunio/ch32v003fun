/*
 * Example for using I2C with 128x32/128x64 graphic OLED
 * 03-29-2023 E. Brombaugh /2023-10-20 B. Biernat
 */


//what type of OLED - uncomment just one
//#define SSD1306_64X32
//#define SSD1306_128X32
#define SSD1306_128X64

#define BUFSIZE     128
#define MAXx        128
#define MAXy        64      

#define NOPer       0x00
#define POSzero     0xe0
#define POSx        0xe1
#define POSy        0xe2
#define POSabs      0xe3
#define WINDOW      0xe5
#define COLOR       0xe6
#define CHAR2LCD    0xe7
#define CHAR2BUFF   0xe8

#define PLOT        0xea
#define LINE        0xeb
#define CIRCLE      0xec
#define FCIRCLE     0xed
#define RECTANGLE   0xee
#define FRECTANGLE  0xef

#define CLRSC       0xf0
#define SHOW        0xf1
#define SHOWTBUFF   0xf2
#define INVERT      0xf3
#define NORMAL      0xf4
#define NOP         0xf5


#include "ch32v003fun.h"
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "ssd1306.h"
#include "bomb.h" 
#include <unistd.h>
#include <string.h>
#include <stdbool.h>


int _read(int fd, char *ptr, int len) 
{
  if (fd == STDIN_FILENO)
  {
      //wait for a character to be received
      while (!(USART1->STATR & USART_STATR_RXNE)) { }
      uint8_t rxData = USART1->DATAR;
      *ptr = rxData;  //always only return 1 byte
      
      return 1;
  }
  return -1;
}

// user input buffer and position in buffer
static char buf[BUFSIZE];

static uint8_t pos = 0; 
static uint8_t posx = 0;
static uint8_t posy = 0;
static uint8_t posx2 = 0;
static uint8_t posy2 = 0;
static uint8_t color = 1;
static uint8_t radius = 1;
static uint8_t width = 0;
static uint8_t high = 0;

static uint8_t value = 0;     //last value (debug)
static uint8_t window = 0;    //last window (debug)
static uint8_t command = 0;   //last command (debug)


int8_t get_byte()
{
    char c;
    
    _read(STDIN_FILENO, &c, 1);
    return c;
}

    
void show_text_buffer()
{    
                  
        ssd1306_drawstr_sz(0,0,  buf,    color, fontsize_8x8);
        ssd1306_drawstr_sz(0,8,  buf+16, color, fontsize_8x8);
        ssd1306_drawstr_sz(0,16, buf+32, color, fontsize_8x8);
        ssd1306_drawstr_sz(0,24, buf+48, color, fontsize_8x8);
        ssd1306_drawstr_sz(0,32, buf+64, color, fontsize_8x8);
        ssd1306_drawstr_sz(0,40, buf+80, color, fontsize_8x8);
        ssd1306_drawstr_sz(0,48, buf+96, color, fontsize_8x8);
        ssd1306_drawstr_sz(0,56, buf+112,color, fontsize_8x8);
        
        ssd1306_refresh(); 
}

int get_data()
{
    char cmd;   //command
    
    cmd = get_byte();
    
    switch(cmd)
				{
					case SHOW:      { ssd1306_refresh(); };
                    break;	
                    
					case SHOWTBUFF: { show_text_buffer(); };
                    break;	
                    
					case INVERT:    { ssd1306_cmd(SSD1306_INVERTDISPLAY); };
                    break;	
                    
					case NORMAL:    { ssd1306_cmd(SSD1306_NORMALDISPLAY); };
                    break;
                    
					case NOP:       { ssd1306_cmd(SSD1306_NOP); };
                    break;	
                    					
                    case POSzero:   { pos = 0x00; }; 
                    break;
                        
					case POSx:      { posx = get_byte(); if(posx>MAXx-1) {posx=MAXx-1;} };
                    break;

					case POSy:  	{ posy = get_byte(); if(posy>MAXy-1) {posx=MAXy-1;} };
                    break;					

					case POSabs:	{ pos = get_byte(); if(pos>BUFSIZE-1) {pos=BUFSIZE-1;} };     //0xF3
                    break;	
                        				
					case COLOR:     { color = get_byte(); };
                    break;	
                        				
					case CHAR2BUFF:   
                    { 
                        buf[pos] = get_byte(); 
                        value=buf[pos]; pos++; 
                    };  
                    break;
                    
					case CHAR2LCD:   
                    { 
                        posx = get_byte(); 
                        posy = get_byte();
                        char c = get_byte();
                        color = get_byte(); 
                        ssd1306_drawchar(posx, posy, c, color);
                    }; 
                    break;
                        					
					case CLRSC:     { pos = 0; window = 0; color = 1; memset(buf, 0x20, sizeof(buf)); show_text_buffer();  };
                    break;	
                        				
					case WINDOW:    
                    { 
                        window = get_byte(); 
                        for(uint8_t znak=0; znak<window; znak++)
                        {   
                            uint8_t c = get_byte(); 
                            if((pos+window)<=sizeof(buf))  // avoid memory leak
                                { buf[pos++] = c; };   
                        }
                    }
                    break;
                     
                    case PLOT:  
                    { 
                        posx = get_byte(); 
                        posy = get_byte(); 
                        color = get_byte(); 
                        ssd1306_drawPixel(posx, posy, color); };
                    break;
                        				
                    case LINE:  
                    { 
                        posx = get_byte(); 
                        posy = get_byte(); 
                        posx2 = get_byte(); 
                        posy2 = get_byte();
                        color = get_byte();
						ssd1306_drawLine(posx, posy, posx2, posy2, color);
                    }
                    break;
                        				
                    case CIRCLE:  
                    { 
                        posx = get_byte(); 
                        posy = get_byte(); 
                        radius = get_byte(); 
                        color = get_byte();         
                        ssd1306_drawCircle(posx, posy, radius, color);
                    }
                    break;
        
                    case FCIRCLE:  
                    { 
                        posx = get_byte(); 
                        posy = get_byte(); 
                        radius = get_byte(); 
                        color = get_byte();         
                        ssd1306_fillCircle(posx, posy, radius, color);
                    }
                    break;
        
                    case RECTANGLE:  
                    { 
                        posx = get_byte(); 
                        posy = get_byte(); 
                        width = get_byte(); 
                        high = get_byte(); 
                        color = get_byte();         
                        ssd1306_drawRect(posx, posy, width, high, color);
                    }
                    break;
        
                    case FRECTANGLE:    //0xfe
                    { 
                        posx = get_byte(); 
                        posy = get_byte(); 
                        width = get_byte(); 
                        high = get_byte(); 
                        color = get_byte();         
                        ssd1306_fillRect(posx, posy, width, high, color);
                    }
                    break;          
                        				
                } //switch
              
    command = cmd;
    return command;
}

void debug()
{
        printf("Pos: \"%x\", ", pos);
        printf(" Window: \"%x\", ", window);
        printf(" Value: \"%c\", ", value);
        printf(" Command: \"%x\", ", command);
        printf(" Color: \"%x\"\n\r", color);
        
        for(uint8_t znak=0;znak<sizeof(buf);znak++) 
        {
            printf("%x[%c], ", znak, buf[znak]);
        }
        printf("\n\r");   
}

int main()
{
	SystemInit();                   // 48MHz internal clock
    SetupUART(UART_BRR);            // start serial @ default 115200bps
    USART1->CTLR1 |= USART_Mode_Rx; // we need to additional turn on RX mode
    Delay_Ms(100);                  // startup delay

    if(!ssd1306_i2c_init())         // LCD init
    {
        ssd1306_init();
        ssd1306_setbuf(0);
    }
    
    memset(buf, 0, sizeof(buf));    // Clean LCD buffer 
    
    while(1)                        // main loop
    {   
        get_data();                 // get data from serial (blocking))         
        //debug();                    // debug (not needed in production)
    }
}
             





