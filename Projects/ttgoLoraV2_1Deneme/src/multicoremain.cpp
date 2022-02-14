/*
#include <Arduino.h>
#include <U8g2lib.h>
#include "screen.h"
#include "connection.h"*/
#include "test_cg.h"

#define GATEWAY 0

U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED(U8G2_R2, OLED_RST, OLED_SCL, OLED_SDA);

Game game;

bool start = false;

void GameTaskRun(void* gamePtr);
bool startScreen();

void setup() {
} 

void loop() {
    if( start ){
      xTaskCreatePinnedToCore(GameTaskRun, 
      "Amiral Batti", 14000, &game, 3,
      NULL, 1);
      start = false;
    }
    else{
      start = startScreen();
    }
}


void GameTaskRun(void* gamePtr){
  Game *game = (Game*)gamePtr;
  #if GATEWAY
    game->testSetGateway();
    //TEST_GATEWAY();
  #else
    game->testSetClient();
    //TEST_CLIENT();
  #endif

  do{
    #if GATEWAY
      game->testLoopGateway();
      //TEST_GATEWAY_LOOP();
    #else
      game->testLoopClient();
      //TEST_CLIENT_LOOP();
    #endif
  }while( game->getState() != Win && game->getState() != Lost );
}