/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include <stdio.h>
#include "GLCD.h"
#include "LPC17xx.h"
#include "LED.h"
#include "KBD.h"

#define __FI        1                      /* Font index 16x24      */
#define __USE_LCD   0										/* Uncomment to use the LCD */


#include "feathers.c"
#include "fur.c"
#include "scales.c"

extern int main_menu(void);
extern void mp3_player (void);
extern void basketballGame(void);
extern void tictactoe (void);
extern int Init_Thread (void);
int x,mode = 0,pre_mode = 1,flag=1; 	
char text[10];
uint32_t joystick;

void delay(unsigned long ticks){
	while (ticks >0){
		__asm__ volatile ("nop");
		ticks--;
	}
}

void photo_gallery (void){
	int img_choice = 1;
	GLCD_SetBackColor(Blue);
	GLCD_DisplayString(0, 0, __FI, "   COE718 Project    ");
	GLCD_DisplayString(1, 0, __FI, "   Media Center       ");
	GLCD_DisplayString(2, 0, __FI, "   Kirolos Youssef    ");	
	GLCD_SetBackColor(Green);
	GLCD_DisplayString(3, 0, __FI, "     Photo Gallery       ");

	while (1){			
		joystick = get_button();
		delay(1000000);
		if (joystick==KBD_SELECT){
			mode=0;
			main_menu();
			break;
		}
		
		if (joystick==KBD_RIGHT){
			img_choice+=1;
			if (img_choice>3){
				img_choice=1;
			}
		}
		
		if (joystick==KBD_LEFT){ 
			img_choice-=1;
			if (img_choice<1){
				img_choice=3;
			}
		}
		
		switch(img_choice){
			case(1): 
				delay(1000000);
				GLCD_Bitmap(0, 100, 320, 140, GIMP_IMAGE1_PIXEL_DATA);
				break;
			case(2): 
				delay(1000000);
				GLCD_Bitmap(0, 100, 320, 140, GIMP_IMAGE2_PIXEL_DATA);
				break;
			case(3):
				delay(1000000);
				GLCD_Bitmap(0, 100, 320, 140, GIMP_IMAGE3_PIXEL_DATA);
				break;
		}
	}	
}

extern int main_menu (void){
	//shawarma:
	delay(1000000);
	GLCD_Clear(White);                         /* Clear graphical LCD display   */
	GLCD_SetBackColor(White);	
	GLCD_SetBackColor(Blue);
	GLCD_SetTextColor(Red);
	GLCD_DisplayString(0, 0, __FI, "   COE718 Project     ");
	GLCD_DisplayString(1, 0, __FI, "   Media Center       ");
	GLCD_DisplayString(2, 0, __FI, "   Kirolos Youssef    ");	

	GLCD_SetBackColor(Green);
	GLCD_DisplayString(3, 0, __FI, "     Main Menu       ");

	while (1){	
		joystick = get_button();
		delay(1000000);
		if (joystick==KBD_SELECT){
			mode=pre_mode;
		}
		
		if (joystick==KBD_DOWN){
			pre_mode+=1;
			if (pre_mode>3){
				pre_mode=1;
			}
		}
		
		if (joystick==KBD_UP){
			pre_mode-=1;
			if (pre_mode<1){
				pre_mode=3;
			}
		}
		delay(1000000);
	
		if (pre_mode!=mode){
			switch(pre_mode){
				case 1:
					delay(100000);	
					GLCD_SetBackColor(LightGrey);
					GLCD_DisplayString(5, 0, __FI, "-   Photo Gallery       ");
					GLCD_SetBackColor(White);	
					GLCD_DisplayString(6, 0, __FI, "   MP3 Player           ");	
					GLCD_DisplayString(7, 0, __FI, "   Game Center          ");
					delay(100000);
					break;
				case 2:
					delay(100000);				
					GLCD_SetBackColor(White);	
					GLCD_DisplayString(5, 0, __FI, "   Photo Gallery       ");
					GLCD_SetBackColor(LightGrey);
					GLCD_DisplayString(6, 0, __FI, "-   MP3 Player         ");	
					GLCD_SetBackColor(White);	
					GLCD_DisplayString(7, 0, __FI, "   Game Center         ");
					delay(100000);
					break;
				case 3:
					delay(100000);	
					GLCD_SetBackColor(White);
					GLCD_DisplayString(5, 0, __FI, "   Photo Gallery       ");
					GLCD_SetBackColor(White);	
					GLCD_DisplayString(6, 0, __FI, "   MP3 Player          ");	
					GLCD_SetBackColor(LightGrey);
					GLCD_DisplayString(7, 0, __FI, "-   Game Center        ");
					delay(100000);
					break;
			}
		}
		switch(mode){
			case(1):
				photo_gallery();				
				break;
			case(2):				
				GLCD_Clear(White); 
				delay(100000);
				GLCD_DisplayString(3, 0, __FI, "     MP3 Player       ");
				GLCD_DisplayString(5, 0, __FI, "   USB Connected      ");
				delay(100000);
				mp3_player();
				break;
			case(3):
				basketballGame();
				break;
			}
	}
}



/*
 * main: initialize and start the system
 */
int main (void) {

	LED_Init();
	KBD_Init();
	#ifdef __USE_LCD 
		GLCD_Init();                               /* Initialize graphical LCD (if enabled */
	#endif	
	main_menu();

}
//	for (x=0;x<8;x++){
//		LED_Off(x);
//	}
//  osKernelInitialize ();                    // initialize CMSIS-RTOS
// 	Init_Thread ();
//  osKernelStart ();                         // start thread execution 
//	osDelay(osWaitForever);
