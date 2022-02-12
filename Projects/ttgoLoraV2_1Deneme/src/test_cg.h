#include <Arduino.h>
#include <U8g2lib.h>
#ifndef GAME
    #include "game.h"
#endif
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED;

TCPConnection connection;


void TEST_SET(){
    OLED.begin();
    OLED.clear();
    OLED.setFont(u8g2_font_profont15_tf);
    OLED.setCursor(25, 2*OLED.getFontAscent());
    pinMode(LED_BUILTIN, OUTPUT);
}

void TEST_CLIENT(){
  connection.setConnectionType(_Client);
  while( !connection.printNearbyNetworks() ){
    delay(1000);
  }
  bool status;
  do{
    status = connection.setUpConnection();
    digitalWrite(LED_BUILTIN, status);
    delay(2000);
  } while(!status);

  OLED.clearBuffer();
  OLED.setFont(u8g2_font_profont15_tf);
  OLED.setCursor(25, 2*OLED.getFontAscent());
  OLED.print("I am now");
  OLED.setCursor(25, 4*OLED.getFontAscent());
  OLED.print("connected");
  OLED.setCursor(20, 6*OLED.getFontAscent());
  OLED.print("to the server!");
}

void TEST_CLIENT_LOOP(){
  static int i = 0;
  if( connection.handleIncoming() == R_Check ) {
    OLED.clearBuffer();
    OLED.setFont(u8g2_font_profont12_tf);
    OLED.setCursor(20, 3*OLED.getFontAscent());
    OLED.print("Incoming shot");
    if( i%3 == 0 ){
      OLED.setCursor(30, 5*OLED.getFontAscent());
      OLED.print("HIT!");
    }
    connection.notifyFireRespose( i%3 == 0 );
    i++;
    OLED.sendBuffer();
  }

  delay(50);
}

void TEST_GATEWAY(){
  connection.setConnectionType(_Gateway);
  while( !connection.setUpConnection() ){ delay(50);}
  delay(1000);
  OLED.clearBuffer();
  OLED.setFont(u8g2_font_profont10_tf);
  OLED.setCursor(15, 5*OLED.getFontAscent());
  digitalWrite(LED_BUILTIN, HIGH);
  OLED.print("A client is among us!");
}

void TEST_GATEWAY_LOOP(){
  static uint8_t x = 0;
  static uint8_t y = 10;
  delay(1000);
  connection.notifyFiring(x,y);
  OLED.clearBuffer();
  OLED.setFont(u8g2_font_profont15_tf);
  OLED.setCursor(25, 2*OLED.getFontAscent());
  OLED.print(connection.handleIncoming() == R_Hit ? "Hit!" : "Miss");
  OLED.setCursor(25, 4*OLED.getFontAscent());
  OLED.print("Shooting...");
  OLED.setCursor(35, 6*OLED.getFontAscent());
  OLED.print("(" + String(x) + " " + String(y) + ")");
  OLED.sendBuffer();
  x += 1U;
  y -= 1U;
}


void Game::testSetGateway(){
    TEST_SET();
    Run();
    delay(1000);

    for(int i = 0; i < 3 ; i++){    // go to create game
        move = true;
        Run();     
        delay(1000);
    }
    click = true;
    Run();                          // click
    delay(1000);

    for(int i = 0; i < 6 ; i++){    // create 6 ships
        click = true;
        Run();     
        delay(1000);
    }

}

void Game::testLoopGateway(){
    if( state == PlayerTurn){
        setCursor( random(0, 12), random(0,6) );
        click = true;
    }
    Run();     
    delay(2000);
}

void Game::testSetClient(){
    TEST_SET();
    Run();
    delay(1000);

    for(int i = 0; i < 2 ; i++){    // go to join game
        move = true;
        Run();     
        delay(1000);
    }
    click = true;
    Run();                          // click
    delay(1000);

    for(int i = 0; i < 6 ; i++){    // create 6 ships
        click = true;
        Run();     
        delay(1000);
    }

}

void Game::testLoopClient(){
    if( state == PlayerTurn ){
        setCursor( random(0, 12), random(0,6) );
        click = true;
    }
    Run();     
    delay(2000);
}