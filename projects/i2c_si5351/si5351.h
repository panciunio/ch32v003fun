/*
 * Single-File-Header for using I2C SI5351
 */
/*****************************************************************/
/*             RF generator with Si5153                          */
/*  ************************************************************ */
/*  Mikrocontroller:  CH32V003                                   */
/*                                                               */
/*  Author:           Peter Rachow (DK7IH) for AVR ATmega8       */
/*  Last Change:      2017-FEB-23                                */
/*                                                               */
/*  Adapted for CH32V003: pancio@pancio.net, 2023-11-23          */ 
/*****************************************************************/

#ifndef _SI5351_H
#define _SI5351_H

#include <stdint.h>

/////////////////////
//Defines for Si5351
/////////////////////
#ifndef SI5351_ADDRESS
  #define SI5351_ADDRESS 0x60 
#endif

//Set of Si5351A register addresses
#define CLK_ENABLE_CONTROL       3
#define PLLX_SRC				15
#define CLK0_CONTROL            16 
#define CLK1_CONTROL            17
#define CLK2_CONTROL            18
#define SYNTH_PLL_A             26
#define SYNTH_PLL_B             34
#define SYNTH_MS_0              42
#define SYNTH_MS_1              50
#define SYNTH_MS_2              58
#define PLL_RESET              177
#define XTAL_LOAD_CAP          183

/*
 * send register/data pair to SI5351
 */
uint8_t si5351_write(uint8_t reg_addr, uint8_t reg_value)
{
   i2c_send_datareg(SI5351_ADDRESS,reg_addr,reg_value);
   return 0;
} 

/*
 * Init clock chip
 */
uint8_t si5351_start()
{
    unsigned long a, b, c; 
    unsigned long p1, p2; 

    si5351_write(XTAL_LOAD_CAP, 0xD2);      // Set crystal load capacitor to 10pF (default),  
                                            // for bits 5:0 see also AN619 p. 60 
    si5351_write(CLK_ENABLE_CONTROL, 0x00); // Enable all outputs 
    si5351_write(CLK0_CONTROL, 0x0F);       // Set PLLA to CLK0, 8 mA output 
    si5351_write(CLK1_CONTROL, 0x2F);       // Set PLLB to CLK1, 8 mA output 
    si5351_write(CLK2_CONTROL, 0x2F);       // Set PLLB to CLK2, 8 mA output 
    si5351_write(PLL_RESET, 0xA0);          // Reset PLLA and PLLB 
    
    // Set VCOs of PLLA and PLLB to 900 MHz 
    a = 36;           // Division factor 900/25 MHz 
    b = 0;            // Numerator, sets b/c=0 
    c = 1048575;      //Max. resolution, but irrelevant in this case (b=0) 
    
    //Formula for splitting up the numbers to register data, see AN619 
    p1 = 128 * a + (unsigned long) (128 * b / c) - 512; 
    p2 = 128 * b - c * (unsigned long) (128 * b / c); 
 
    //Write data to registers PLLA and PLLB so that both VCOs are set to 900MHz intermal freq 
    si5351_write(SYNTH_PLL_A, 0xFF); 
    si5351_write(SYNTH_PLL_A + 1, 0xFF); 
    si5351_write(SYNTH_PLL_A + 2, (p1 & 0x00030000) >> 16); 
    si5351_write(SYNTH_PLL_A + 3, (p1 & 0x0000FF00) >> 8); 
    si5351_write(SYNTH_PLL_A + 4, (p1 & 0x000000FF)); 
    si5351_write(SYNTH_PLL_A + 5, 0xF0 | ((p2 & 0x000F0000) >> 16)); 
    si5351_write(SYNTH_PLL_A + 6, (p2 & 0x0000FF00) >> 8); 
    si5351_write(SYNTH_PLL_A + 7, (p2 & 0x000000FF)); 
 
    si5351_write(SYNTH_PLL_B, 0xFF); 
    si5351_write(SYNTH_PLL_B + 1, 0xFF); 
    si5351_write(SYNTH_PLL_B + 2, (p1 & 0x00030000) >> 16); 
    si5351_write(SYNTH_PLL_B + 3, (p1 & 0x0000FF00) >> 8); 
    si5351_write(SYNTH_PLL_B + 4, (p1 & 0x000000FF)); 
    si5351_write(SYNTH_PLL_B + 5, 0xF0 | ((p2 & 0x000F0000) >> 16)); 
    si5351_write(SYNTH_PLL_B + 6, (p2 & 0x0000FF00) >> 8); 
    si5351_write(SYNTH_PLL_B + 7, (p2 & 0x000000FF)); 
    
    return 0;
}

 void si5351_set_freq(int synth, unsigned long freq) 
{ 
   
  unsigned long a, b, c = 1048575; 
  unsigned long f_xtal = 25000000; 
  double fdiv = (double) (f_xtal * 36) / freq; //division factor fvco/freq (will be integer part of a+b/c) 
  double rm; //remainder 
  unsigned long p1, p2; 
   
  a = (unsigned long) fdiv; 
  rm = fdiv - a;  //(equiv. b/c) 
  b = rm * c; 
  p1 = 128 * a + (unsigned long) (128 * b / c) - 512; 
  p2 = 128 * b - c * (unsigned long) (128 * b / c); 
   
  //Write data to multisynth registers of synth n 
  si5351_write(synth,   0xFF);      //1048757 MSB 
  si5351_write(synth + 1, 0xFF);  //1048757 LSB 
  si5351_write(synth + 2, (p1 & 0x00030000) >> 16); 
  si5351_write(synth + 3, (p1 & 0x0000FF00) >> 8); 
  si5351_write(synth + 4, (p1 & 0x000000FF)); 
  si5351_write(synth + 5, 0xF0 | ((p2 & 0x000F0000) >> 16)); 
  si5351_write(synth + 6, (p2 & 0x0000FF00) >> 8); 
  si5351_write(synth + 7, (p2 & 0x000000FF)); 
} 

#endif
