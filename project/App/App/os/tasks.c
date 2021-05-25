#include "OS.h"
#include "tasks.h"
#include "ac780x_gpio.h"
#include "sdk_system.h"
#include "board_config.h"

#define SPI_TRANSFER_NUM                3

uint16_t g_CapturePC11Value;
uint16_t g_CapturePD2Value;
uint8_t g_spiTxbuffer[SPI_TRANSFER_NUM];
uint8_t g_spiRxbuffer[SPI_TRANSFER_NUM];
void LED_crl(void) 
{
	LED2_TOGGLE;
}

uint32_t g_1msCnt ;
uint32_t g_10msCnt;
uint32_t g_100msCnt;
uint32_t g_2000msCnt;

void task0_1ms(void) 
{
  g_1msCnt++; //for debug
   
}
 


void task1_10ms(void) 
{
    
   g_10msCnt++;    //for debug
//   LED_crl();
}



void task2_100ms(void) 
{
    g_100msCnt++;   //for debug

    LED_crl();

    System_UpdateAPP();
    
}

void task3_2000ms(void) 
{
    g_2000msCnt++;
    printf("tick cnt:%d\r\n", g_2000msCnt);
}

void task(void)
{
   
  
   SCH_Add_Task(task0_1ms, 0,1);
    
   SCH_Add_Task(task1_10ms, 1,10);
    
   SCH_Add_Task(task2_100ms, 4,100);
   SCH_Add_Task(task3_2000ms, 4,2000);
   
 
 // Start the scheduler
   SCH_Start();
   //任务调度在SCH_Update()中进行管理，在1ms中断中调用
   while(1)
   {
      SCH_Dispatch_Tasks();
   }
}

