#include <U8g2lib.h>
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED;

#define MiniShipSize 2
#define MidShipSize 3
#define BigShipSize 4

#define GRID_P_WIDTH 10
#define GRID_P_HEIGHT 10
#define GRID_WIDTH 12
#define GRID_HEIGHT 6

enum PartState{
    Good, Damaged, Empty
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
        return 2 + x*GRID_P_WIDTH;
    }

    int getYFromIndex(const int y){
        return 4 + y*GRID_P_HEIGHT;
    }

    void Draw(){
        for(int i = 0 ; i < GRID_WIDTH ; i++){
            for(int j = 0 ; j < GRID_HEIGHT ; j++){
                OLED.drawFrame(i*GRID_P_WIDTH + 2, j*GRID_P_HEIGHT + 4, GRID_P_WIDTH, GRID_P_HEIGHT);
            }
        }
    }


};GameGrid GRID;



struct Part{
    PartState state = Good;
    unsigned int x;
    unsigned int y;
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
        default:
            break;
        }
    }
};

enum Direction{
    pX, pY, nX, nY
};

class Ship{
    Part parts[];
public:
    Ship(int x, int y, Direction dir, int part_count)
    {
        for(int i = 0; i < part_count; i++){
            switch (dir)
            {
            case pX:
                parts[i].x = x + i;
                parts[i].y = y;
                break;
            case pY:
                parts[i].x = x;
                parts[i].y = y + i;
                break;
            case nX:
                parts[i].x = x - i;
                parts[i].y = y;
                break;
            case nY:
                parts[i].x = x;
                parts[i].y = y - i;
                break;
            default:
                break;
            }
        }
    }
    virtual void Draw() = 0;
    virtual void takeDamage( int x, int y) = 0;
    virtual bool move(Direction dir) = 0;

    bool move(Direction dir, int size){
        // make sure no part is in the edge
        for(int i = 0; i < size ; i++){
            switch (dir)
            {
            case pX:
                if (parts[i].x + 1 == GRID_WIDTH){
                    return false;
                }
                break;
            case pY:
                if (parts[i].y + 1 == GRID_HEIGHT){
                    return false;
                }
                break;
            case nX:
                if (parts[i].x == 0){
                    return false;
                }
                break;
            case nY:
                if (parts[i].y == 0){
                    return false;
                }
                break;
            default:
                break;
            }
        }
        // move the parts
        for(int i = 0; i < size ; i++){
            switch (dir)
            {
            case pX:
                parts[i].x++;
                break;
            case pY:
                parts[i].y++;
                break;
            case nX:
                parts[i].x--;
                break;
            case nY:
                parts[i].y--;
                break;
            default:
                break;
            }
        }
        return true;
    }

    void takeDamage(int x, int y, int size){
        for(int i = 0; i < size; i++){
            if ( parts[i].x == x && parts[i].y == y ){
                parts[i].state = Damaged;
            }
        }
    }

    void Draw(int size){
        for(int i = 0; i < size ; i++){
            parts[i].Draw();
        }
    }
};

class MiniShip : Ship{
    Part parts[MiniShipSize];

public:
    MiniShip(int x, int y, Direction dir) : Ship(x, y, dir, MiniShipSize){}


    void Draw() { ((Ship*)this)->Draw(MiniShipSize); }

    void takeDamage(int x, int y) { ((Ship*)this)->takeDamage(x, y, MiniShipSize); }

    bool move(Direction dir) { return ((Ship*)this)->move(dir, MiniShipSize);}
};

class MidShip : Ship{
    Part parts[MidShipSize];
public:
    MidShip(int x, int y, Direction dir) : Ship(x, y, dir, MidShipSize){}
    void Draw() { ((Ship*)this)->Draw(MidShipSize); }

    void takeDamage(int x, int y) { ((Ship*)this)->takeDamage(x, y, MidShipSize); }

    bool move(Direction dir) { return ((Ship*)this)->move(dir, MidShipSize);}
};

class BigShip : Ship{
    Part parts[BigShipSize];
public:
    BigShip(int x, int y, Direction dir) : Ship(x, y, dir, BigShipSize){}

    void Draw() { ((Ship*)this)->Draw(BigShipSize); }

    void takeDamage(int x, int y) { ((Ship*)this)->takeDamage(x, y, BigShipSize); }

    bool move(Direction dir) { return ((Ship*)this)->move(dir, BigShipSize);}
};