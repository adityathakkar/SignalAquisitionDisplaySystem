
//;*********************************************************************                  
//;*                       McMaster University                         *
//;*                      2DP4 Microcontrollers                        *
//;*                          Lab Section 06                           *
//;*                  Aditya Thakkar thakkaap 001429465                *
//;*********************************************************************
//;*********************************************************************
//;*                       Final Project                               *
//;*  This code implements an A/C conversion channel at a sampling     *
//;*  rate of 320 Hz, with a clock speed of 4 MHz, and a 10 bit ADC    *
//:*          resolution. The input channel is channel 4.              *
//;*********************************************************************


#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "SCI.h"

void setClk(void);
void delayneg4s(int delayBy);
void OutCRLF(void);


int onOff = 0;       // Counter to check if button is on 
int adcInput = 0;    // ADC input value 
int temp;


void main(void) {
  
	ATDCTL1 = 0x2F;		// set for 10-bit resolution
	ATDCTL3 = 0x88;		// right justified, one sample per sequence
	ATDCTL4 = 0x02;		// prescaler = 2; ATD clock = 4MHz / (2 * (2 + 1)) == 0.667MHz
	ATDCTL5 = 0x24;		// continuous conversion on channel 4
  
  EnableInterrupts;
  
  //Set Ports
  DDRJ = 0xFF;      //set all port J as output
  
           
  TSCR1 = 0x90;    //Timer System Control Register 1
                  // TSCR1[7] = TEN:  Timer Enable (0-disable, 1-enable)
                  // TSCR1[6] = TSWAI:  Timer runs during WAI (0-enable, 1-disable)
                  // TSCR1[5] = TSFRZ:  Timer runs during WAI (0-enable, 1-disable)
                  // TSCR1[4] = TFFCA:  Timer Fast Flag Clear All (0-normal 1-read/write clears interrupt flags)
                  // TSCR1[3] = PRT:  Precision Timer (0-legacy, 1-precision)
                  // TSCR1[2:0] not used

  TSCR2 = 0x00;    //Timer System Control Register 2
                   // TSCR2[7] = TOI: Timer Overflow Interrupt Enable (0-inhibited, 1-hardware irq when TOF=1)
                   // TSCR2[6:3] not used
                     // TSCR2[2:0] = Timer Prescaler Select: See Table22-12 of MC9S12G Family Reference Manual r1.25 (set for bus/1)
  
                     
  TIOS = 0xFE;     //Timer Input Capture or Output capture
                     //set TIC[0] and input (similar to DDR)
  PERT = 0x01;     //Enable Pull-Up resistor on TIC[0]

  TCTL3 = 0x00;    //TCTL3 & TCTL4 configure which edge(s) to capture
  TCTL4 = 0x02;    //Configured for falling edge on TIC[0]

           
   
  TIE = 0x01;      //Timer Interrupt Enable
   
   
  setClk();        // Set Clock 
  SCI_Init(19200);
  DDRJ |= 0x01;


  for(;;) {
     
    if (onOff%2 == 0) {  // Check if we should be transmitting based on the number of times the button has been pressed
   
      adcInput = ATDDR0;      // Take in analog value
      
      if (adcInput < 1000) {   // If adcInput is < 1000 (ie adcInput has less than 4 digits) pad the value with an extra 0 at the start
                          // This is to ensure that the transmission time is even
        SCI_OutUDec(0);   // Send out 0 pad 
        SCI_OutUDec(adcInput); // Send out value
        OutCRLF();        // New line
        delayneg4s(10);   // Delay to get 320 Hz sampling time
        PTJ ^= 1;       // Alternate the state of the on board LED
      } 
      
      else {
                                                            
      SCI_OutUDec(adcInput); // Send out the value read into the ADC
      OutCRLF();        // New line
      delayneg4s(10);  // Delay to get 320 Hz sampling time  
      PTJ ^= 1;        // Change state of the on board LED
      
      }
      
   }
    
   else {       // If the button is off
   
       delayneg4s(10);    // Delay 
   
   }
   
  } // Loop forever
  
    
}



// Function to set the clock speed to 4 MHz
void setClk(void) {
  
     // Project spec tells to use a 4 MHz Bus Speed 
     
     TIE = 0x01;
    
     CPMUCLKS = 0x80; // Set PLLSEL = 1
     CPMUOSC = 0x00;  // Use the 1 MHz Internal Reference Clock
     
     
     CPMUSYNR = 0x0F; // Set VCOFRQ = 0 and SYNDIV = 15 for the project spec
     
     // fREF = 1 MHz and fVCO = 2*fREF*(15+1) = 32 MHz for the project spec
     
     CPMUFLG = 0x00;
     CPMUPOSTDIV = 0x03;
     
     // PLLCLK = fVCO/4 = 8 MHz for this project spec
     // Bus Clock = 4 MHz for this project spec 
     
     while(!(CPMUFLG & 0x08));
     
}

// Function that delays by a multiple of 1e-4 s
void delayneg4s(int delayBy) {
    
    int a;
    TSCR1 = 0x90; // Enable time and clear fast timer flag
    TSCR2 = 0x00; // Disable timer interrupt, set prescaler to 1        
    
    TIOS |= 0x02;
    TC1 = TCNT + 400; // Delay by 1e-4 
    
    for (a = 0; a < delayBy; a++) {
      
      while(!(TFLG1_C1F)); 
      
      TC1 += 400;  
    
    }
    
}


// Taken from the Lab 4 
// Output a CR,LF to SCI to go to a new line
void OutCRLF(void){

  SCI_OutChar(CR);
  SCI_OutChar(LF);
  
}

// Taken from Lab 4 
interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void)
{

  unsigned int temp;
  onOff = onOff + 1;    // Increment the On/Off counter 
  temp = TC0;       //Refer back to TFFCA, we enabled FastFlagClear, thus by reading the Timer Capture input we automatically clear the flag, allowing another TIC interrupt

}





