#include <U8g2lib.h>
#define GRID_

#define GRID_P_WIDTH 10
#define GRID_P_HEIGHT 10
#define GRID_WIDTH 12
#define GRID_HEIGHT 6
#define GRID_WIDTH_OFFSET 4
#define GRID_HEIGHT_OFFSET 2

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED;

enum PartState{
    Good, Damaged, Empty, Sunk
};

class GameGrid{
    PartState grid[GRID_WIDTH][GRID_HEIGHT];
public:
    GameGrid(){
        for(int i = 0; i < GRID_WIDTH; i++){
            for(int j = 0 ; j < GRID_HEIGHT; j++){
                grid[i][j] = Empty;
            }
        }
    }
    int getXFromIndex(const int x){
        return GRID_WIDTH_OFFSET + x*GRID_P_WIDTH;
    }

    int getYFromIndex(const int y){
        return GRID_HEIGHT_OFFSET + y*GRID_P_HEIGHT;
    }

    void Draw(){
        for(int i = 0 ; i < GRID_WIDTH ; i++){
            for(int j = 0 ; j < GRID_HEIGHT ; j++){
                OLED.drawFrame(i*GRID_P_WIDTH + GRID_WIDTH_OFFSET, j*GRID_P_HEIGHT + GRID_HEIGHT_OFFSET, GRID_P_WIDTH, GRID_P_HEIGHT);
            }
        }
    }


};GameGrid GRID;