#include <LPC17xx.H>
#include "KBD.h"
#include "GLCD.h"

#define GRID_SIZE 3
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define CURRENT_CELL_COLOR LightGrey

#define CELL_SIZE_X (SCREEN_WIDTH / GRID_SIZE)
#define CELL_SIZE_Y (SCREEN_HEIGHT / GRID_SIZE)

#define CELL_SIZE ((CELL_SIZE_X < CELL_SIZE_Y) ? CELL_SIZE_X : CELL_SIZE_Y)

extern void delay(unsigned long ticks);
extern int main_menu(void);
extern int mode;

// Define new states for each cell
#define CELL_EMPTY ' '
#define CELL_X 'X'
#define CELL_O 'O'
#define CELL_BLOCKED 'B'

char board[GRID_SIZE][GRID_SIZE] = {
    {CELL_EMPTY, CELL_EMPTY, CELL_EMPTY},
    {CELL_EMPTY, CELL_EMPTY, CELL_EMPTY},
    {CELL_EMPTY, CELL_EMPTY, CELL_EMPTY}
};

char currentPlayer = 'X';

void initialize() {
    // Initialize GLCD and joystick
    GLCD_Init();
    KBD_Init();
}

void drawX(int x, int y) {
    GLCD_SetTextColor(Red);
    delay(1000000);
    GLCD_DrawChar(x, y, 40, 40, (unsigned char*)"X");
    delay(1000000);
}

void drawO(int x, int y) {
    GLCD_SetTextColor(Blue);
    delay(1000000);
    GLCD_DrawChar(x, y, 40, 40, (unsigned char*)"O");
    delay(1000000);
}

void drawGrid(int currentRow, int currentCol) {
    GLCD_Clear(White);

    // Draw the Tic Tac Toe grid
    for (int i = 1; i < GRID_SIZE; i++) {
        delay(1000000);
        // Draw horizontal lines
        GLCD_Bargraph(0, i * CELL_SIZE - 1, SCREEN_WIDTH, 3, Black);

        // Draw vertical lines
        GLCD_Bargraph(i * CELL_SIZE - 1, 0, 3, SCREEN_HEIGHT, Black);
        delay(1000000);
    }

    // Draw X and O based on the board state
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int x = j * CELL_SIZE + 5;
            int y = i * CELL_SIZE + 5;

            // Highlight the current cell with a different color
            if (i == currentRow && j == currentCol) {
                GLCD_Bargraph(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, CURRENT_CELL_COLOR);
            }

            // Draw based on the state of the cell
            if (board[i][j] == CELL_X) {
                drawX(x, y);
            } else if (board[i][j] == CELL_O) {
                drawO(x, y);
            }
        }
    }
}

int isValidMove(int row, int col, char board[GRID_SIZE][GRID_SIZE]) {
    // Check if the specified cell is empty
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE || board[row][col] != CELL_EMPTY) {
        return 0;  // Invalid move
    }

    return 1;  // Valid move
}

void updateBoard(int row, int col, char player, char board[GRID_SIZE][GRID_SIZE]) {
    board[row][col] = player;
}

void getPlayerMove() {
    int currentRow = 1;  // Starting position
    int currentCol = 1;  // Starting position
    uint32_t button;

    do {
        delay(1000000);
        drawGrid(currentRow, currentCol);
        delay(1000000);
        button = get_button();  // Get button press from joystick

        // Update currentRow and currentCol based on joystick input
        if (button & KBD_UP && currentRow > 0) {
            currentRow--;
        } else if (button & KBD_DOWN && currentRow < GRID_SIZE - 1) {
            currentRow++;
        }

        if (button & KBD_LEFT && currentCol > 0) {
            currentCol--;
        } else if (button & KBD_RIGHT && currentCol < GRID_SIZE - 1) {
            currentCol++;
        }

        // Highlight the current cell with a different color
        drawGrid(currentRow, currentCol);

        if (button == KBD_SELECT) {
            // Place 'X' or 'O' only if the cell is empty (not already selected)
            if (isValidMove(currentRow, currentCol, board)) {
                updateBoard(currentRow, currentCol, currentPlayer, board);
                drawGrid(-1, -1);  // Don't highlight any cell

                // Toggle player turn
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }

    } while (1);
}

int checkWinner(char player, char board[GRID_SIZE][GRID_SIZE]) {
    // Check rows, columns, and diagonals for a win
    for (int i = 0; i < GRID_SIZE; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            return 1;  // Win in a row
        }
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
            return 1;  // Win in a column
        }
    }

    // Check diagonals
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return 1;  // Win in the main diagonal
    }
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return 1;  // Win in the other diagonal
    }

    return 0;  // No winner yet
}

int isBoardFull(char board[GRID_SIZE][GRID_SIZE]) {
    // Check if the board is full (a tie)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == CELL_EMPTY) {
                return 0;  // The board is not full
            }
        }
    }

    return 1;  // The board is full (a tie)
}

void resetGame() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            board[i][j] = CELL_EMPTY;
        }
    }
    currentPlayer = 'X';
}

extern void tictactoe(void) {
    initialize();

    while (1) {
        getPlayerMove();

        // Check for a winner
        if (checkWinner(currentPlayer, board)) {
            drawGrid(-1, -1);  // Don't highlight any cell
            GLCD_DisplayString(7, 0, 1, (unsigned char*)"Player ");
            GLCD_DisplayChar(7, 7, 1, currentPlayer);
            GLCD_DisplayString(7, 8, 1, (unsigned char*)" wins!");

            // Wait for a button press before resetting the game
            while (get_button() == 0);

            resetGame();
        }

        // Check for a tie
        if (isBoardFull(board)) {
            drawGrid(-1, -1);  // Don't highlight any cell
            GLCD_DisplayString(7, 2, 1, (unsigned char*)"It's a tie!");

            // Wait for a button press before resetting the game
            while (get_button() == 0);

            resetGame();
        }
    }
}
