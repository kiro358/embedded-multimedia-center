#include <stdio.h>
#include <math.h>
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "GLCD.h"
#include "LPC17xx.h"
#include "LED.h"

#define __FI        1                      /* Font index 16x24      */
/*----------------------------------------------------------------------------
 *      Sample threads
 *---------------------------------------------------------------------------*/
double nlin2=0,nquad=0,nlin=0;
int eq=0,i=1,j=1,k=1;
 
  
void Thread1 (void const *argument); // thread function
void Thread2 (void const *argument); // thread function
void Thread3 (void const *argument); // thread function


osThreadId tid_Thread; // thread id
osThreadDef (Thread1, osPriorityNormal, 1, 0);                   // thread object

osThreadId tid2_Thread; // thread id
osThreadDef (Thread2, osPriorityNormal, 1, 0);                   // thread object

osThreadId tid3_Thread; // thread id
osThreadDef (Thread3, osPriorityNormal, 1, 0);                   // thread object

void delay_func(unsigned long ticks){
	while (ticks >0){
		__asm__ volatile ("nop");
		ticks--;
	}
}

int Init_Thread (void) {

  tid_Thread = osThreadCreate (osThread(Thread1), NULL);
	tid2_Thread = osThreadCreate (osThread(Thread2), NULL);
	tid3_Thread = osThreadCreate (osThread(Thread3), NULL);
  
	if(!tid_Thread) return(-1);
  
  return(0);
}


void Thread1 (void const *argument) {
	for(;;) {
			LPC_GPIO1->FIOSET = (1<<28);
			LPC_GPIO2->FIOCLR = (1<<2);
			LPC_GPIO2->FIOCLR = (1<<3);
//			GLCD_DisplayString(5, 0, __FI, "                                ");
//			GLCD_DisplayString(5, 0, __FI, "Thread 1 2(n)");
			nlin = 2*k;
			k++;
			delay_func(10000);
			if (k>=0x7A120) break;		
  }
}

void Thread2 (void const *argument) {
	for(;;) {
		LPC_GPIO1->FIOCLR = (1<<28);
		LPC_GPIO2->FIOSET = (1<<2);
		LPC_GPIO2->FIOCLR = (1<<3);
//		GLCD_DisplayString(5, 0, __FI, "                                ");
//		GLCD_DisplayString(5, 0, __FI, "Thread 2 4(n)");
		nlin2 = 4*j;	
    j++;
		delay_func(10000);
		if (j>=0x7A120) break;
  }                                          
}

void Thread3 (void const *argument) {
	  for(;;) {
			LPC_GPIO1->FIOCLR = (1<<28);
			LPC_GPIO2->FIOCLR = (1<<2);
			LPC_GPIO2->FIOSET = (1<<3);	
//			GLCD_DisplayString(5, 0, __FI, "                                ");
//			GLCD_DisplayString(5, 0, __FI, "Thread 3 (n)^2");
		  nquad = i*i; 
			i++;
			delay_func(10000);
			if (i>=0x7A120) break;
  }
}

