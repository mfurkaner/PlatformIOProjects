#include "test_cg.h"

#define GATEWAY 0

U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED(U8G2_R2, OLED_RST, OLED_SCL, OLED_SDA);

Game game;


void setup() {

  #if GATEWAY
    game.testSetGateway();
    //TEST_GATEWAY();
  #else
    game.testSetClient();
    //TEST_CLIENT();
  #endif
} 

void loop() {

  #if GATEWAY
    game.testLoopGateway();
    //TEST_GATEWAY_LOOP();
  #else
    game.testLoopClient();
     //TEST_CLIENT_LOOP();
  #endif

}