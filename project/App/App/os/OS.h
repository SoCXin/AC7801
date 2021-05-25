
#define SCH_REPORT_ERRORS

// Main.H (v1.00) 
#ifndef _OS_H
#define _OS_H

//------------------------------------------------------------------
// WILL NEED TO EDIT THIS SECTION FOR EVERY PROJECT
//------------------------------------------------------------------

// Must include the appropriate microcontroller header file here
//#include <AT89x52.h>

// Include oscillator / chip details here 
// (essential if generic delays / timeouts are used)
//  -
// Oscillator / resonator frequency (in Hz) e.g. (11059200UL)
//#define OSC_FREQ (12000000UL)

// Number of oscillations per instruction (4, 6 or 12)
// 12 - Original 8051 / 8052 and numerous modern versions
//  6 - Various Infineon and Philips devices, etc.
//  4 - Dallas, etc.
//
// Take care with Dallas devices 
// - Timers default to *12* osc ticks unless CKCON is modified 
// - If using generic code on a Dallas device, use 12 here
//#define OSC_PER_INST (12)

//------------------------------------------------------------------
// SHOULD NOT NEED TO EDIT THE SECTIONS BELOW
//------------------------------------------------------------------
typedef unsigned char tByte;
typedef unsigned int  tWord;
typedef unsigned long tLong;
//by lidan 为的是与编译器兼容的定义
typedef unsigned char bit;
// Misc #defines
//#ifndef TRUE
//#define FALSE 0
//#define TRUE (!FALSE)
//#endif

#define RETURN_NORMAL (bit) 0
#define RETURN_ERROR (bit) 1


//------------------------------------------------------------------
// Interrupts
// - see Chapter 13.  
//------------------------------------------------------------------
/*
// Generic 8051/52 timer interrupts (used in most schedulers)
#define INTERRUPT_Timer_0_Overflow 1
#define INTERRUPT_Timer_1_Overflow 3
#define INTERRUPT_Timer_2_Overflow 5

// Additional interrupts (used in shared-clock schedulers)
#define INTERRUPT_EXTERNAL_0 0
#define INTERRUPT_EXTERNAL_1 2
#define INTERRUPT_UART_Rx_Tx 4
#define INTERRUPT_CAN_c515c 17
*/
//------------------------------------------------------------------
// Error codes 
// - see Chapter 14. 
//------------------------------------------------------------------

#define ERROR_SCH_TOO_MANY_TASKS (1)
#define ERROR_SCH_CANNOT_DELETE_TASK (2)

#define ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK (3)
#define ERROR_SCH_WAITING_FOR_START_COMMAND_FROM_MASTER (3)

#define ERROR_SCH_ONE_OR_MORE_SLAVES_DID_NOT_START (4)
#define ERROR_SCH_LOST_SLAVE (5)

#define ERROR_SCH_CAN_BUS_ERROR (6)

#define ERROR_I2C_WRITE_BYTE (10)
#define ERROR_I2C_READ_BYTE (11)
#define ERROR_I2C_WRITE_BYTE_AT24C64 (12)
#define ERROR_I2C_READ_BYTE_AT24C64 (13)
#define ERROR_I2C_DS1621 (14)

#define ERROR_USART_TI (21)
#define ERROR_USART_WRITE_CHAR (22)

#define ERROR_SPI_EXCHANGE_BYTES_TIMEOUT (31)
#define ERROR_SPI_X25_TIMEOUT (32)
#define ERROR_SPI_MAX1110_TIMEOUT (33)

#define ERROR_ADC_MAX150_TIMEOUT (44)

#endif

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/


//    LED_flas.H (v1.00)
// ------ Public function prototypes -------------------------------

void LED_Flash_Init(void);
void LED_Flash_Update(void);

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/


//   2_05_12g.h (v1.00) 

// ------ Public function prototypes -------------------------------

void SCH_Init_OS_timer(void) ;
void SCH_Start(void);
void SCH_Update(void);                              
/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/

  
//   SCH51a.h (v1.00)

// ------ Public data type declarations ----------------------------

// Store in DATA area, if possible, for rapid access  
// Total memory per task is 5 bytes

//typedef data struct 

typedef struct data  
   {
   // Pointer to the task (must be a 'void (void)' function)
 //  void (code * pTask)(void);
 //by lidan  
   void (* pTask)(void); 
 //by lidan
   // Delay (ticks) until the function will (next) be run
   // - see SCH_Add_Task() for further details
   //tByte Delay;       
   tWord Delay;
   // Interval (ticks) between subsequent runs.
   // - see SCH_Add_Task() for further details
   //tByte Period;       
   tWord Period;
   // Incremented (by scheduler) when task is due to execute
   tByte RunMe;       
   } sTask; 

// ------ Public function prototypes -------------------------------

// Core scheduler functions
void  SCH_Dispatch_Tasks(void);
//by lidan
//tByte SCH_Add_Task(void (code*) (void), const tByte, const tByte); 
tByte SCH_Add_Task(void (* pFunction)(), const tWord, const tWord); 
//by lidan

bit   SCH_Delete_Task(const tByte);
void  SCH_Report_Status(void);

// ------ Public constants -----------------------------------------

// The maximum number of tasks required at any one time
// during the execution of the program
//
// MUST BE ADJUSTED FOR EACH NEW PROJECT
#define SCH_MAX_TASKS   8   


                           
/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/




