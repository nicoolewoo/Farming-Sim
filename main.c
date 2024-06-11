#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gba.h"
#include "images/basket.h"
#include "images/garbage.h"
#include "images/title.h"
#include "images/single.h"
#include "images/fire.h"
#include "images/family.h"


//Contains state machine to call all other functionality in program

#define BACKGROUND_COLOR (RED)
#define PLAY_COLOR (WHITE)
#define MAX_STRAW (20) //collect 10 strawberry
#define MAX_BOMB (3)
  
int strawberryCount = 0; //amount of strawberries on screen
int bombCount = 0;
int filledScreen = 0;
int points = 0;
char charPts = 0;
int reset = 0;

struct strawberry strawberry[MAX_STRAW];
struct strawberry bomb[MAX_BOMB];
struct state cState;
struct state pState;

enum gba_state {
  START,
  START_TITLE,
  PLAY,
  WIN,
  WIN2,
  LOSE,
  LOSE2,
};

int main(void) {

  REG_DISPCNT = MODE3 + BG2_ENABLE;


  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  enum gba_state state = START;

  void startScreen(void);
  void startScreen2(void);
  int collision(struct strawberry class[], int row, int col, int size);
  void makeStrawberry(void);
  void makeBasket(void);
  void playScreen(void);
  struct strawberry strawberryErase(int row, int col);
  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons
    //char *oldPts = charPts;

    switch (state) {
      case START:
        startScreen();
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)){
          state = PLAY;
        }
        state = START_TITLE;

        // state = ?
        break;
      case START_TITLE:
        startScreen2();
        
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)){
          if (filledScreen == 0){
            fillScreenDMA(PLAY_COLOR);
            //drawFullScreenImageDMA(title);
            filledScreen = 1; 
          }
          makeStrawberry();
          makeBasket();
          state = PLAY;
        }
        break;
      case PLAY:
        
        if(KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)){
            reset = 1;
        } 
        if(reset){
          reset = 0;
          bombCount = 0;
          strawberryCount = 0;
          charPts = 0;
          points = 0;
          state = START;
        }
        if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
          cState.mainBasket.row += 2; //move basket
          if(!collision(bomb, cState.mainBasket.row + 2, cState.mainBasket.col, bombCount)){
            state = LOSE;
          }
          if (!collision(strawberry, cState.mainBasket.row + 2, cState.mainBasket.col, strawberryCount)){ //if strawberry collides with basket
            struct strawberry blank = strawberryErase(cState.mainBasket.row + 2, cState.mainBasket.col);
            drawRectDMA(blank.row, blank.col, blank.width, blank.height, WHITE); //make strawberry disappear
            points++;
            charPts++;
            drawRectDMA(150, 10, 10, 10, WHITE);
            drawChar(150, 10, charPts + 48, BLACK); 
          } 
        }
        if (KEY_DOWN(BUTTON_UP, currentButtons)) {
          cState.mainBasket.row -= 2; //move basket
          if(!collision(bomb, cState.mainBasket.row - 2, cState.mainBasket.col, bombCount)){
            state = LOSE;
          } 
          if (!collision(strawberry, cState.mainBasket.row - 2, cState.mainBasket.col, strawberryCount)){ //if strawberry collides with basket
            struct strawberry blank = strawberryErase(cState.mainBasket.row - 2, cState.mainBasket.col);
            drawRectDMA(blank.row, blank.col, blank.width, blank.height, WHITE); //make strawberry disappear
            points++;
            charPts++;
            drawRectDMA(150, 10, 10, 10, WHITE);
            drawChar(150, 10, charPts + 48, BLACK); 
          }
        }
        if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
          cState.mainBasket.col -= 2; //move basket
          if(!collision(bomb, cState.mainBasket.row, cState.mainBasket.col - 2, bombCount)){
            state = LOSE;
          }
          if (!collision(strawberry, cState.mainBasket.row, cState.mainBasket.col - 2, strawberryCount)){ //if strawberry collides with basket
            struct strawberry blank = strawberryErase(cState.mainBasket.row, cState.mainBasket.col - 2);
            drawRectDMA(blank.row, blank.col, blank.width, blank.height, WHITE); //make strawberry disappear
            points++;
            charPts++;
            drawRectDMA(150, 10, 10, 10, WHITE);
            drawChar(150, 10, charPts + 48, BLACK); 
          }
        }
        if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
          cState.mainBasket.col += 2; //move basket
          if(!collision(bomb, cState.mainBasket.row, cState.mainBasket.col + 2, bombCount)){
            state = LOSE;
          } 
          if (!collision(strawberry, cState.mainBasket.row, cState.mainBasket.col + 2, strawberryCount)){ //if strawberry collides with basket
            struct strawberry blank = strawberryErase(cState.mainBasket.row, cState.mainBasket.col + 2);
            drawRectDMA(blank.row, blank.col, blank.width, blank.height, WHITE); //make strawberry disappear
            points++;
            charPts++;
            drawRectDMA(150, 10, 10, 10, WHITE);
            drawChar(150, 10, charPts + 48, BLACK); 
          }
        }
        if (points == 10) {
          state = WIN;
        }
        playScreen();
        // state = ?
        break;
      case WIN:
        waitForVBlank();
        drawFullScreenImageDMA(family);
        state = WIN2;

        // state = ?
        break;
      case WIN2:
        waitForVBlank();
        drawCenteredString(HEIGHT / 2 - 9,WIDTH / 2 , 2 , 2 , "Congrats!! You got 10 berries!", BLACK);
        if(KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)){
            reset = 1;
        } 
        if(reset){
          reset = 0;
          bombCount = 0;
          strawberryCount = 0;
          points = 0;
          charPts = 0;
          state = START;
        } 
        break;

      case LOSE:
        waitForVBlank();
        drawFullScreenImageDMA(fire);
        state = LOSE2;

        // state = ?
        break;
      case LOSE2:
        waitForVBlank();
        drawCenteredString(HEIGHT / 2 - 9,WIDTH / 2 , 2 , 2 , "Oh no! You blew up the strawberry farm!", WHITE);
        if(KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)){
            reset = 1;
        } 
        if(reset){
          reset = 0;
          bombCount = 0;
          strawberryCount = 0;
          points = 0;
          charPts = 0;
          state = START;
        }
        break;
        }
    previousButtons = currentButtons; // Store the current state of the buttons
  }
  UNUSED(previousButtons); // You can remove this once previousButtons is used
  return 0;
}

void startScreen(void) {
  waitForVBlank();
  drawFullScreenImageDMA(title);
}
void startScreen2(void) {
  waitForVBlank();
  drawCenteredString(HEIGHT / 2 - 9,WIDTH / 2 , 2 , 2 , "Strawberry Fields", WHITE);
  drawCenteredString(HEIGHT / 2, WIDTH / 2 , 0 , 0 , "Start", WHITE);
}
int collision(struct strawberry class[], int row, int col, int size){
    int notOverlap = 1;
    
    for(int i = 0; i < size; i++){
        if(((row >= class[i].row && row <= class[i].row + 10)
                || (row + 10 >= class[i].row && row + 10 <= class[i].row + 10))
                && ((col >= class[i].col && col <= class[i].col + 10)
                || (col + 10 >= class[i].col && col + 10 <= class[i].col + 10))){

            notOverlap = 0;
            break;
        }
    }
    return notOverlap;
}

//makes a strawberry
void makeStrawberry(void) {
while(bombCount < MAX_BOMB) {
    int row = randint(10, HEIGHT - 10);
    int col = randint(20, WIDTH - 10);
    struct strawberry bomb1; 
    bomb1.row = row;
    bomb1.col = col;
    bomb1.height = 10;
    bomb1.width = 10;
    bomb1.color = GRAY;
    waitForVBlank();
    drawImageDMA(bomb1.row, bomb1.col, bomb1.width, bomb1.height, single);
    bomb[bombCount] = bomb1;
    bombCount++;
  }

while(strawberryCount < MAX_STRAW){
        int row = randint(2, HEIGHT - 10);
        int col = randint(0, WIDTH - 10);
        struct strawberry new_berry;
        new_berry.row = row;
        new_berry.col = col;
        new_berry.height = 10;
        new_berry.width = 10;
        //new_berry.color = RED;
        waitForVBlank();
        drawImageDMA(new_berry.row, new_berry.col, new_berry.width, new_berry.height, single);
        strawberry[strawberryCount] = new_berry;
        strawberryCount++;   
    }
}

//make basket using an image
void makeBasket(void) {
  cState.mainBasket.row = 130;
  cState.mainBasket.col = 100;
  cState.mainBasket.size = 10;
  waitForVBlank();
  drawImageDMA(cState.mainBasket.row, cState.mainBasket.col, cState.mainBasket.size, cState.mainBasket.size, basket);
}

//strawberry eraser
struct strawberry strawberryErase(int row, int col){
    struct strawberry eStraw;
    int found = 0;
    for(int i = 0; i < strawberryCount; i++){
        if(((row >= strawberry[i].row && row <= strawberry[i].row + 10)
                || (row + 10 >= strawberry[i].row && row + 10 <= strawberry[i].row + 10))
                && ((col >= strawberry[i].col && col <= strawberry[i].col + 10)
                || (col + 10 >= strawberry[i].col && col + 10 <= strawberry[i].col + 10))){
            
            eStraw = strawberry[i];
            found = 1;
        }
        if(found){
         strawberry[i] = strawberry[i + 1];
        }
    }
    strawberryCount--;
    return eStraw;
}

void playScreen(void) {
  waitForVBlank();  
//GAME MOVEMENT
  drawRectDMA(pState.mainBasket.row, pState.mainBasket.col, pState.mainBasket.size, pState.mainBasket.size, WHITE);
  drawImageDMA(cState.mainBasket.row, cState.mainBasket.col, cState.mainBasket.size, cState.mainBasket.size, basket);
  pState = cState;
  }

