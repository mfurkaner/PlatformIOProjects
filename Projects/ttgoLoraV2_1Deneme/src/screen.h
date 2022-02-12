#include <Arduino.h>
#ifndef SHIP
    #include "ship.h"
#endif
#ifndef CONNECTION
    #include "connection.h"
#endif

#define SCREEN

class Screen{
public:
    virtual bool Draw() = 0;
    Screen(){}
    virtual ~Screen(){}
};


enum HomeSelect{
    Join, Create
};
class HomeScreen : Screen{
    String title = "AMIRAL BATTI";
    String join = "join game";
    String create = "create game";
    String pointer = "->";
    HomeSelect select = Join;
    const uint8_t* title_font = u8g2_font_profont15_tf;
    const uint8_t* other_font = u8g2_font_profont12_tf;

public:
    HomeSelect getState(){return select;}
    void ChangeState(){
        if (select == Join){ select = Create; }
        else if(select == Create) {select = Join; }
    }

    bool Draw(){
        OLED.clearBuffer();
        OLED.drawFrame(0,0,OLED.getWidth(),OLED.getHeight());
        OLED.setFont(title_font);
        OLED.setCursor(20, 2*OLED.getFontAscent());
        OLED.print(title);
        OLED.setFont(other_font);
        OLED.setCursor(30, 4.5*OLED.getFontAscent());
        OLED.print((select == Join ? pointer : "") + join);
        OLED.setCursor(30, 6.5*OLED.getFontAscent());
        OLED.print((select == Create ? pointer : "") + create);
        OLED.sendBuffer();
        return false;
    }

};

class JoinScreen : Screen{
    public:
    bool Draw(){return false;}
};

class CreateScreen : Screen{
    public:
    bool Draw(){
        OLED.clearBuffer();
        OLED.drawFrame(0,0,OLED.getWidth(),OLED.getHeight());
        OLED.setFont(u8g2_font_profont12_tf);
        OLED.setCursor(20, 3*OLED.getFontAscent());
        OLED.print("Creating the");
        OLED.setCursor(15, 5*OLED.getFontAscent());
        OLED.print("game network...");
        OLED.sendBuffer();
        delay(1000);
        return false;
    }
};

class GameScreen : Screen{
    public:
    bool Draw(){
        OLED.clearBuffer();
        OLED.drawFrame(0,0,OLED.getWidth(),OLED.getHeight());
        OLED.setFont(u8g2_font_profont12_tf);
        GRID.Draw();
        return true;
    }
};