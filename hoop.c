#include <LPC17xx.H>
#include "KBD.h"
#include "GLCD.h"
#include "stdlib.h"
#include "backboard.c"
#include "basketball.c"
#include "white.c"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define ARROW_HEIGHT 30
#define ARROW_WIDTH 150
#define POWER_BAR_WIDTH 20
#define POWER_BAR_HEIGHT 100
#define BALL_SIZE 30
#define BALL_SPEED 5
#define MAX_CONSECUTIVE_MISSES 3

extern void delay(unsigned long ticks);
extern int main_menu(void);
extern int mode;

int consecutiveMisses=0;
int hoopX, hoopY;
int final_score=0,score=0;
int powerBarSpeed, arrowBarSpeed;
int backdropSizeX,backdropSizeY;
char strscore[10];
char strpos[10];

void setDifficultySettings(int difficulty) {
    // Adjust settings based on the selected difficulty level
    switch (difficulty) {
			case 1: // Easy
					// Use default parameters for easy difficulty
					backdropSizeX = GIMP_IMAGE5_WIDTH;
					backdropSizeY = GIMP_IMAGE5_HEIGHT;
					powerBarSpeed = 1;
					arrowBarSpeed = 1;
					break;
			case 2: // Medium
					// Modify parameters for medium difficulty
					backdropSizeX = GIMP_IMAGE5_WIDTH;
					backdropSizeY = GIMP_IMAGE5_HEIGHT;
					powerBarSpeed = 2; // Increase the power bar graph speed for medium difficulty
					arrowBarSpeed = 2;
					break;
			case 3: // Hard
					// Modify parameters for hard difficulty
					backdropSizeX = GIMP_IMAGE5_WIDTH ;
					backdropSizeY = GIMP_IMAGE5_HEIGHT;
					powerBarSpeed = 3; // Further increase the power bar graph speed for hard difficulty
					arrowBarSpeed = 3;
					break;
    }

    // Calculate hoopX and hoopY based on the bottom 40% and 10% left/right
    hoopX = backdropSizeX/3;
    hoopY = backdropSizeY - (backdropSizeY * 0.4);
}

void drawBackboard(int difficuly){
	int r1 = rand() % 2;
	int r2 = rand() % 3;
	GLCD_Clear(White);
	switch (difficuly){
		case 1:
			GLCD_Bitmap((SCREEN_WIDTH-backdropSizeX)/2, (SCREEN_HEIGHT-backdropSizeY)/2, backdropSizeX, backdropSizeY, (unsigned char *) GIMP_IMAGE5_PIXEL_DATA); // Load backboard image	
			break;
		case 2:
			if (r1==0){
				GLCD_Bitmap(0, (SCREEN_HEIGHT-backdropSizeY)/2, backdropSizeX, backdropSizeY, (unsigned char *) GIMP_IMAGE5_PIXEL_DATA); // Load backboard image		
			}
			else{
				GLCD_Bitmap(30, (SCREEN_HEIGHT-backdropSizeY)/2, backdropSizeX, backdropSizeY, (unsigned char *) GIMP_IMAGE5_PIXEL_DATA); // Load backboard image	
			}
			break;
			case 3:
			if (r2==0){
				GLCD_Bitmap(0, (SCREEN_HEIGHT-backdropSizeY)/2, backdropSizeX, backdropSizeY, (unsigned char *) GIMP_IMAGE5_PIXEL_DATA); // Load backboard image		
			}
			else if (r2==1){
				GLCD_Bitmap(20, (SCREEN_HEIGHT-backdropSizeY)/2, backdropSizeX, backdropSizeY, (unsigned char *) GIMP_IMAGE5_PIXEL_DATA); // Load backboard image		
			}
			else{
				GLCD_Bitmap(40, (SCREEN_HEIGHT-backdropSizeY)/2, backdropSizeX, backdropSizeY, (unsigned char *) GIMP_IMAGE5_PIXEL_DATA); // Load backboard image	
			}
			break;
		}
}

int drawArrow(int arrowBarSpeed) {
	int direction=0;
	int arrowPosition = SCREEN_WIDTH / 2 - ARROW_WIDTH / 2;	
	while (get_button() != KBD_SELECT) {
		
		if (arrowBarSpeed<0){
			GLCD_Bitmap(45,220, arrowPosition, ARROW_HEIGHT, (unsigned char *)WHITE_PIXEL);
		}
		
		arrowPosition += arrowBarSpeed;
		GLCD_Bargraph(45,220, arrowPosition, ARROW_HEIGHT, Red); //SCREEN_WIDTH / 2-ARROW_WIDTH/2, SCREEN_HEIGHT - ARROW_HEIGHT
		
		//sprintf(strpos, "%d", arrowPosition);
		//GLCD_DisplayString(3,1, 1, (unsigned char *)strpos);		
		
		if (arrowPosition <= 45 || arrowPosition >= 45 + ARROW_WIDTH ) { 
			arrowBarSpeed = -arrowBarSpeed;
		}
		
	}
	// Save the direction when the user selects
	direction = arrowPosition-(45+(ARROW_WIDTH/2));  // Save the distance from the center
	return direction;
}

int drawPowerBar(int powerBarSpeed) {// Continue updating power bar graph until user selects
	int power=0;
	int powerBarPosition = SCREEN_HEIGHT / 2 - POWER_BAR_HEIGHT / 2;
	while (get_button() != KBD_SELECT) {
		// Draw power bar graph based on the powerBarPosition		
		if (powerBarSpeed<0){
			GLCD_Bitmap(5, 45, POWER_BAR_WIDTH, powerBarPosition, (unsigned char *)WHITE_PIXEL);
		}
		// Update power bar position for the next frame
		powerBarPosition += powerBarSpeed;
		GLCD_Bargraph(5, 45, POWER_BAR_WIDTH, powerBarPosition, Green);
		
		// Check for power bar reaching the top or bottom, reverse direction if needed
		if (powerBarPosition <= 45 || powerBarPosition >= 45 + POWER_BAR_HEIGHT ) { // 45, 195
			powerBarSpeed = -powerBarSpeed;
		}      
	}
	power=powerBarPosition;
	return power;
}

void drawBall(int x, int y) {
    delay(1000000);
    GLCD_Bitmap(x, y, BALL_SIZE, BALL_SIZE, (unsigned char *)GIMP_IMAGE4_PIXEL_DATA); // Load basketball image
}

void gameOverPrompt(int final_score) {
	GLCD_Clear(White);
	delay(1000000);	
	while (get_button() != KBD_SELECT){
    GLCD_DisplayString(5, 2, 1, (unsigned char*)"Game Over!");
		GLCD_DisplayString(6, 2, 1, (unsigned char*)"You Score is:");
		sprintf(strscore, "%d", final_score);
		GLCD_DisplayString(6,16, 1, (unsigned char *)strscore);
    GLCD_DisplayString(7, 0, 1, (unsigned char*)"Select for main menu");
	}
    main_menu();
		return;
}

void scoredPrompt() {
    GLCD_Clear(White);
    delay(1000000);
    GLCD_DisplayString(6, 2, 1, (unsigned char*)"You Scored!");
    GLCD_DisplayString(8, 0, 1, (unsigned char*)"New round");
    delay(5000000);
}
void missedPrompt() {
    GLCD_Clear(White);
    delay(1000000);
    GLCD_DisplayString(6, 2, 1, (unsigned char*)"You Missed!");
    GLCD_DisplayString(8, 0, 1, (unsigned char*)"New round");
    delay(5000000);
}

int getDifficulty() {
    int difficulty = 1; // Default difficulty level

    GLCD_Clear(White);
    GLCD_DisplayString(2, 2, 1, (unsigned char*)"Select Difficulty:");
    GLCD_DisplayString(4, 2, 1, (unsigned char*)"1. Easy");
    GLCD_DisplayString(5, 2, 1, (unsigned char*)"2. Medium");
    GLCD_DisplayString(6, 2, 1, (unsigned char*)"3. Hard");

    while (1) {
        uint32_t button = get_button();
				delay(1000000);
        if (button & KBD_SELECT) {
            return difficulty;
        } else if (button & KBD_UP) {
            difficulty = (difficulty % 3) + 1; // Cycle through 1, 2, 3
						delay(1000000);
            GLCD_DisplayString(8, 2, 1, (unsigned char*)"                   "); // Clear previous selection
            GLCD_DisplayString(8, 2, 1, (unsigned char*)"Selected: ");
            GLCD_DisplayChar(8, 12, 1, difficulty + '0');
						delay(1000000);
        } else if (button & KBD_DOWN) {
            difficulty = (difficulty == 1) ? 3 : (difficulty - 1); // Cycle through 3, 2, 1
						delay(1000000);
            GLCD_DisplayString(8, 2, 1, (unsigned char*)"                   "); // Clear previous selection
            GLCD_DisplayString(8, 2, 1, (unsigned char*)"Selected: ");
            GLCD_DisplayChar(8, 12, 1, difficulty + '0');
						delay(1000000);
        }
    }
}

int throwBall(int power, int direction) {
	int ballX = SCREEN_WIDTH / 2 - BALL_SIZE/2;
	int ballY = SCREEN_HEIGHT - ARROW_HEIGHT;
	int gravity;

	for (int t = 0; t < 100; t++) {
		delay(1000000);
		// Clear the previous ball position
		GLCD_Bitmap(ballX, ballY , BALL_SIZE, BALL_SIZE, (unsigned char *)WHITE_PIXEL);
	
		// Update ball position based on projectile motion equations
		gravity = 0.5 * 9.8 * (t / 10) * (t / 10);
		//ballX = (SCREEN_WIDTH / 2)-(direction/20) * power * (t / 10) ;
		ballX += direction/35 * power/10; 
		ballY += gravity-(power/10);
		
		// Draw the ball at the new position
		drawBall(ballX, ballY);

		// Check if the ball is within the hoop
		if (ballX > hoopX && ballX < hoopX + backdropSizeX/3 && ballY > hoopY && ballY < hoopY + (0.1*backdropSizeY) && gravity > (power/10) ) {
				score += 10;
				GLCD_DisplayString(6, 1, 1, (unsigned char*)"Score: ");
				GLCD_DisplayChar(6, 6, 1, score);
				scoredPrompt();
				return 1; // Exit the function if the ball is within the hoop
		}
		else if (ballX > SCREEN_WIDTH || ballX < 0 || ballY > SCREEN_HEIGHT || ballY < 0){
				consecutiveMisses++;
				missedPrompt();
				return 0;
		}
	}
		return -1;
}

enum GameState {
    STATE_DIFFICULTY,
    STATE_BASKETBALL_GAME,
    STATE_GAME_OVER,
};

extern void basketballGame(void) { 
	enum GameState currentState = STATE_DIFFICULTY;  // Initialize to main menu
	int power; int direction, difficulty;
	while (1) {
		switch (currentState) {
			case STATE_DIFFICULTY:
				difficulty = getDifficulty();
				setDifficultySettings(difficulty);
				currentState = STATE_BASKETBALL_GAME;  // Move to the basketball game state
				break;
			case STATE_BASKETBALL_GAME:
				while (consecutiveMisses < MAX_CONSECUTIVE_MISSES) {		
					drawBackboard(difficulty);
					power= drawPowerBar(powerBarSpeed);
					delay(10000000);
					direction= drawArrow(arrowBarSpeed);
					throwBall(power, direction);
				}
				currentState = STATE_GAME_OVER;  // Move to the game over state
				break;
			case STATE_GAME_OVER:
				do {
					final_score= score;
					gameOverPrompt(final_score);
					score = 0;
					consecutiveMisses=0;
				} while (get_button() != KBD_SELECT);			
				//break;
				mode=0;
				main_menu();
				return;
		}
	}	
}
