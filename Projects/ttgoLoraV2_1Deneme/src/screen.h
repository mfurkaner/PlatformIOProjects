#include <Arduino.h>
#include "ship.h"

class Screen{
public:
    virtual void Draw() = 0;
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
        else {select = Join; }
    }

    void Draw(){
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
    }

};

class JoinScreen : Screen{
    String select = "Select a network";

    void Draw();
};

class CreateScreen : Screen{
    String created = "Created a game";
    String waiting = "Waiting for a player";
    String joined = "A player has joined";


    void Draw();
};

class GameScreen : Screen{
    void Draw();
};