#include <U8g2lib.h>
#include "grid.h"
#define PART
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED;

struct Part{
    PartState state = Good;
    uint8_t x;
    uint8_t y;
    void Draw(){
        switch (state)
        {
        case Good:
            OLED.drawBox(GRID.getXFromIndex(x), GRID.getYFromIndex(y), GRID_P_WIDTH, GRID_P_HEIGHT);
            break;
        case Damaged:
            OLED.drawLine(GRID.getXFromIndex(x), GRID.getYFromIndex(y), GRID.getXFromIndex(x) + GRID_P_WIDTH, GRID.getYFromIndex(y) + GRID_P_HEIGHT);
            OLED.drawLine(GRID.getXFromIndex(x), GRID.getYFromIndex(y) + GRID_P_HEIGHT, GRID.getXFromIndex(x) + GRID_P_WIDTH, GRID.getYFromIndex(y));
            break;
        case Empty:
            OLED.drawDisc(GRID.getXFromIndex(x) + GRID_P_WIDTH/2, GRID.getYFromIndex(y) + GRID_P_HEIGHT/2, GRID_P_HEIGHT/4);
            break;
        case Sunk:
            break;
        }
    }
};