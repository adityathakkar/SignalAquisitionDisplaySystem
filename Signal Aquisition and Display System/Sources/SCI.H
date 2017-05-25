// filename  ******************* SCI.H **************************
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
 
// standard ASCII symbols 
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20       
#define DEL  0x7F
 
//-------------------------SCI_Init------------------------
// Initialize Serial port SCI
// Input: baudRate is tha baud rate in bits/sec
// Output: none
// SCIBDL=250000/baudRate, these 5 cases only
// baudRate =  2400 bits/sec   SCIBDL=104
// baudRate =  4800 bits/sec   SCIBDL=52
// baudRate =  9600 bits/sec   SCIBDL=26
// baudRate =  19200 bits/sec  SCIBDL=13
// baudRate =  250000 bits/sec SCIBDL=1
// assumes a module clock frequency of 4 MHz
// sets baudRate to 250000 bits/sec if doesn't match one of the above
void SCI_Init(unsigned short baudRate);
 
//-------------------------SCI_InStatus--------------------------
// Checks if new input is ready, TRUE if new input is ready
// Input: none
// Output: TRUE if a call to InChar will return right away with data
//         FALSE if a call to InChar will wait for input
char SCI_InStatus(void);  

//-------------------------SCI_InChar------------------------
// Wait for new serial port input, busy-waiting synchronization
// The input is not echoed
// Input: none
// Output: ASCII code for key typed
char SCI_InChar(void);

//------------------------SCI_InString------------------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed 
//    or until max length of the string is reached.  
// It echoes each character as it is inputted.  
// If a backspace is inputted, the string is modified 
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
void SCI_InString(char *bufPt, unsigned short max);
 

//----------------------SCI_InUDec-------------------------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 16 bit unsigned number
//     valid range is 0 to 65535
// Input: none
// Output: 16-bit unsigned number
// If you enter a number above 65535, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCI_InUDec(void);   

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
unsigned short SCI_InUHex(void);  

//-----------------------SCI_OutStatus----------------------------
// Checks if output data buffer is empty, TRUE if empty
// Input: none
// Output: TRUE if a call to OutChar will output and return right away
//         FALSE if a call to OutChar will wait for output to be ready
char SCI_OutStatus(void);   

//-------------------------SCI_OutChar------------------------
// Wait for buffer to be empty, output 8-bit to serial port
// busy-waiting synchronization
// Input: 8-bit data to be transferred
// Output: none
void SCI_OutChar(char);  
 
//-----------------------SCI_OutUDec-----------------------
// Output a 16-bit number in unsigned decimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1-5 digits with no space before or after
void SCI_OutUDec(unsigned short);    

//-------------------------SCI_OutString------------------------
// Output String (NULL termination), busy-waiting synchronization
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void SCI_OutString(char *pt); 
 
//--------------------------SCI_OutUHex----------------------------
// Output a 16 bit number in unsigned hexadecimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1 to 4 digits with no space before or after
void SCI_OutUHex(unsigned short); 

