#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h> 
#include <avr/pgmspace.h>

#include "graphics/rzl.h"

#define DELAY_MS 0

//Row on/off defines to toggle Bit Pins
// Port D:  Pin0    Pin1  Pin2  Pin3  Pin4
//          Row_EN  N/A   Row0  Row1  Row2
#define ROWS_ON   PORTD &= ~(1<<PD3);
#define ROWS_OFF  PORTD  |= (1<<PD3);
#define ROW0_ON   PORTD  |= (1<<PD7);
#define ROW0_OFF  PORTD &= ~(1<<PD7);
#define ROW1_ON   PORTD  |= (1<<PD6);
#define ROW1_OFF  PORTD &= ~(1<<PD6);
#define ROW2_ON   PORTD  |= (1<<PD5);
#define ROW2_OFF  PORTD &= ~(1<<PD5);

// definitions for the Row selection
#define ROW2 ROW0_OFF ROW1_OFF ROW2_OFF
#define ROW3 ROW0_OFF ROW1_OFF ROW2_ON
#define ROW4 ROW0_OFF ROW1_ON ROW2_OFF
#define ROW5 ROW0_OFF ROW1_ON ROW2_ON
#define ROW6 ROW0_ON ROW1_OFF ROW2_OFF
#define ROW7 ROW0_ON ROW1_OFF ROW2_ON
#define ROW8 ROW0_ON ROW1_ON ROW2_OFF
#define ROW1 ROW0_ON ROW1_ON ROW2_ON

// definitions for the shift register controls
#define MR_HI   PORTA |= (1<<PA0);
#define MR_LO   PORTA &= ~(1<<PA0);
#define DATA_HI PORTA |= (1<<PA2);
#define DATA_LO PORTA &= ~(1<<PA2);
#define OE_HI   PORTA |= (1<<PA4);
#define OE_LO   PORTA &= ~(1<<PA4);

#define SHCP_HI  PORTA |=  (1<<PA3);
#define SHCP_LO  PORTA &= ~(1<<PA3);

#define STCP_HI  PORTA |=  (1<<PA1);
#define STCP_LO  PORTA &= ~(1<<PA1);
            
//volatile char framebuff[1024];

void showframe (char *frame);

int main (void){
  DDRA = 0xFF;  /* Set our output register to output */
  DDRD = 0xFF;
  MR_LO
  MR_HI
  OE_LO
  ROWS_ON
  SHCP_LO
	STCP_LO
	int paintcount=0;
  	
	while(42) {
	  if(paintcount < 125){
//	    memcpy_P(&framebuff[0], &frame1, 1024);
      showframe(&frame1);
    } else {
//      memcpy_P(&framebuff[0], &frame2, 1024);
      showframe(&frame2);
    }
//    showframe(&framebuff[0]);
    if(paintcount++ == 250) {
      paintcount = 0;
    }
  }
return 0;
}

/* showframe function displays a single full frame (1024 pixels)
 *
 * this iterates 8 times through filling the 128 bit shift register
 * Each 32 pixel row is taken from a 1024Byte array that is provided as parameter to the function
 * because of the 128bit shift register, care has to be taken to have the pixels that are first in the array
 * shifted last into the register. After 4 rows of 32 have been read and put in the register, the ROW is chosen via
 * an 3-to-8bit multiplexer. Finally the STCP line is toggled once, so the buffered bits in the register are output
 * on the output pins and thus to the LEDs.
 * This procedure is repeated 8 times to get to the full 1024 pixels, but fast enough for the eye to think it is a
 * single picture
 */
void showframe (char *frame){
  int rowoffset=0;
  int i,j,k,l,m;

  for(i=0; i<8; i++){ //we have 8*128 Pixel
    switch(i){
      case 0:
			  ROW1
			  break;
		  case 1:
			  ROW2
 			  rowoffset=32;
			  break;
		  case 2:
			  ROW3
			  rowoffset=64;
			  break;
		  case 3:
			  ROW4
			  rowoffset=96;
			  break;
		  case 4:
			  ROW5
			  rowoffset=128;
			  break;
		  case 5:
			  ROW6
			  rowoffset=160;
			  break;
		  case 6:
			  ROW7
			  rowoffset=192;
			  break;
		  case 7:
			  ROW8
			  rowoffset=224;
			  break;
		}
    for(j=31; j>=0; j--){ //row number 1,9,17,
      SHCP_LO
      if(*(frame+(rowoffset+768+j)) == 0){
        DATA_LO
      } else {
        DATA_HI
      }
      SHCP_HI
    }
    for(k=31; k>=0; k--){
      SHCP_LO
      if(*(frame+(rowoffset+512+k)) == 0){
        DATA_LO
      } else {
        DATA_HI
      }
      SHCP_HI
    }
    for(l=31; l>=0; l--){
      SHCP_LO
      if(*(frame+(rowoffset+256+l)) == 0){
        DATA_LO
      } else {
        DATA_HI
      }
      SHCP_HI
    }
    for(m=31; m>=0; m--){
      SHCP_LO
      if(*(frame+(rowoffset+0+m)) == 0){
        DATA_LO
      } else {
        DATA_HI
      }
      SHCP_HI
    }
    STCP_HI
    STCP_LO
  }
}

