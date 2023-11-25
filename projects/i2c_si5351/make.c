/*
 * Example for using I2C with SI5351
 * 21-11-2023 PanciO
 */


#define SI5351_ADDRESS 0x60

#include "ch32v003fun.h"
#include <stdio.h>
#include "i2c.h"
#include "si5351.h"
#include <string.h>

#define WRITE   0
#define READ    1
int main()
{
	SystemInit();                       // 48MHz internal clock
    SetupUART(UART_BRR);                // start serial @ default 115200bps
    USART1->CTLR1 |= USART_Mode_Rx;     // we need to additional turn on RX mode
    Delay_Ms(100);                      // startup delay

    if(!i2c_init())                     // I2C init
    {
        unsigned long freq = 14187570;  //Atari XTAL clock
        unsigned long freq2 = 1000000;  //1MHz

        i2c_init();
        si5351_start();                    
        si5351_set_freq(SYNTH_MS_0, freq);
        si5351_set_freq(SYNTH_MS_1, freq2);
     }
    
    
    while(1)                            // main loop
    {
          //
    }
    
    
}
        





