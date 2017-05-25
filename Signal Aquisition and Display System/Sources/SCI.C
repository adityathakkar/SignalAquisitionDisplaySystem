// filename  ***************  SCI.C ****************************** 
// Simple I/O routines to 9S12C32 serial port   
// Jonathan W. Valvano 2/14/06

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Microcomputer Systems: 
//    Motorola 6811 and 6812 Simulation", Brooks-Cole, copyright (c) 2002

// Copyright 2006 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 
// Modified by EE345L students Charlie Gough && Matt Hawk
// Modified by EE345M students Agustinus Darmawan + Mingjie Qiu

//*********************************************************************
// Adapted by Carl Barnes @ Technological Arts for S12G - November 2014
//*********************************************************************
 
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"  /* derivative information */
#include "SCI.h"

#define RDRF 0x20   // Receive Data Register Full Bit
#define TDRE 0x80   // Transmit Data Register Empty Bit

//-------------------------SCI_Init------------------------
// Initialize Serial port SCI0
// Input: baudRate is tha baud rate in bits/sec
// Output: none
// Important! After reset, MCU clock is in PEI mode (refer to 10.1.2 in S12G Manual),
// so Bus Clock (MCLK) is 6.25 MHz
// baud divisor = MCLK/(16*baudrate)
// baudRate =  2400 bits/sec   SCI0BDL=163
// baudRate =  4800 bits/sec   SCI0BDL=81
// baudRate =  9600 bits/sec   SCI0BDL=41
// baudRate =  19200 bits/sec  SCI0BDL=20
// baudRate =  384000 bits/sec SCI0BDL=10
// sets baudRate to 96000 bits/sec if doesn't match one of the above
void SCI_Init(unsigned short baudRate){
  SCI0BDH = 0;   // br=MCLK/(16*baudRate) 
  
  switch(baudRate){
    case 2400:  SCI0BDL=163; break;
    case 4800:  SCI0BDL=81;  break;
    case 9600:  SCI0BDL=41;  break;
    case 19200: SCI0BDL=20;  break;
    case 38400: SCI0BDL=10;  break;
    default:    SCI0BDL = 41 ;  // 9600 
  }
  SCI0CR1 = 0;
/* bit value meaning
    7   0    LOOPS, no looping, normal
    6   0    SCISWAI, SCI stop in Wait Mode
    5   0    RSRC, not appliable with LOOPS=0
    4   0    M, 1 start, 8 data, 1 stop
    3   0    WAKE, wake by idle (not applicable)
    2   0    ILT, short idle time (not applicable)
    1   0    PE, no parity
    0   0    PT, parity type (not applicable with PE=0) */ 
  SCI0CR2 = 0x0C; 
/* bit value meaning
    7   0    TIE, no transmit interrupts on TDRE
    6   0    TCIE, no transmit interrupts on TC
    5   0    RIE, no receive interrupts on RDRF
    4   0    ILIE, no interrupts on idle
    3   1    TE, enable transmitter
    2   1    RE, enable receiver
    1   0    RWU, no receiver wakeup
    0   0    SBK, no send break */ 
}
    
//-------------------------SCI_InChar------------------------
// Wait for new serial port input, busy-waiting synchronization
// The input is not echoed
// Input: none
// Output: ASCII code for key typed
char SCI_InChar(void){
  while((SCI0SR1 & RDRF) == 0){};
  return(SCI0DRL);
}
        
//-------------------------SCI_OutChar------------------------
// Wait for buffer to be empty, output 8-bit to serial port
// busy-waiting synchronization
// Input: 8-bit data to be transferred
// Output: none
void SCI_OutChar(char data){
  while((SCI0SR1 & TDRE) == 0){};
  SCI0DRL = data;
}

   
//-------------------------SCI_InStatus--------------------------
// Checks if new input is ready, TRUE if new input is ready
// Input: none
// Output: TRUE if a call to InChar will return right away with data
//         FALSE if a call to InChar will wait for input
char SCI_InStatus(void){
  return(SCI0SR1 & RDRF);
}

//-----------------------SCI_OutStatus----------------------------
// Checks if output data buffer is empty, TRUE if empty
// Input: none
// Output: TRUE if a call to OutChar will output and return right away
//         FALSE if a call to OutChar will wait for output to be ready
char SCI_OutStatus(void){
  return(SCI0SR1 & TDRE);
}


//-------------------------SCI_OutString------------------------
// Output String (NULL termination), busy-waiting synchronization
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void SCI_OutString(char *pt){
  while(*pt){
    SCI_OutChar(*pt);
    pt++;
  }
}

//----------------------SCI_InUDec-------------------------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 16 bit unsigned number
//     valid range is 0 to 65535
// Input: none
// Output: 16-bit unsigned number
// If you enter a number above 65535, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCI_InUDec(void){	
unsigned short number=0, length=0;
char character;
  character = SCI_InChar();	
  while(character != CR){ // accepts until <enter> is typed
// The next line checks that the input is a digit, 0-9.
// If the character is not 0-9, it is ignored and not echoed
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 65535
      length++;
      SCI_OutChar(character);
    } 
// If the input is a backspace, then the return number is
// changed and a backspace is outputted to the screen
    else if((character==BS) && length){
      number /= 10;
      length--;
      SCI_OutChar(character);
    }
    character = SCI_InChar();	
  }
  return number;
}


//-----------------------SCI_OutUDec-----------------------
// Output a 16-bit number in unsigned decimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1-5 digits with no space before or after
void SCI_OutUDec(unsigned short n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string 
  if(n >= 10){
    SCI_OutUDec(n/10);
    n = n%10;
  }
  SCI_OutChar(n+'0'); /* n is between 0 and 9 */
}



//---------------------SCI_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 16-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 4 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFF
// If you enter a number above FFFF, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCI_InUHex(void){	
unsigned short number=0, digit, length=0;
char character;
  character = SCI_InChar();
  while(character != CR){	
    digit = 0x10; // assume bad
    if((character>='0') && (character<='9')){
      digit = character-'0';
    }
    else if((character>='A') && (character<='F')){ 
      digit = (character-'A')+0xA;
    }
    else if((character>='a') && (character<='f')){ 
      digit = (character-'a')+0xA;
    }
// If the character is not 0-9 or A-F, it is ignored and not echoed
    if(digit <= 0xF){	
      number = number*0x10+digit;
      length++;
      SCI_OutChar(character);
    }
// Backspace outputted and return value changed if a backspace is inputted
    else if((character==BS) && length){
      number /= 0x10;
      length--;
      SCI_OutChar(character);
    }
    character = SCI_InChar();
  }
  return number;
}

//--------------------------SCI_OutUHex----------------------------
// Output a 16 bit number in unsigned hexadecimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1 to 4 digits with no space before or after
void SCI_OutUHex(unsigned short number){
// This function uses recursion to convert the number of 
//   unspecified length as an ASCII string
  if(number >= 0x10){
    SCI_OutUHex(number/0x10);
    SCI_OutUHex(number%0x10);
  }
  else{
    if(number < 0xA){
      SCI_OutChar(number+'0');
     }
    else{
      SCI_OutChar((number-0x0A)+'A');
    }
  }
}
  

//------------------------SCI_InString------------------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed 
//    or until max length of the string is reached.  
// It echoes each character as it is entered.  
// If a backspace is entered, the string is modified 
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void SCI_InString(char *bufPt, unsigned short max) {	
int length=0;
char character;
  character = SCI_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        SCI_OutChar(BS);
      }
    }
    else if(length < max){
      *bufPt = character;
      bufPt++;
      length++; 
      SCI_OutChar(character);
    }
    character = SCI_InChar();
  }
  *bufPt = 0;
}



