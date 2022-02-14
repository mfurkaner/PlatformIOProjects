#include <U8g2lib.h>
#include "part.h"
#define SHIP

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED;

#define MiniShipSize 2
#define MidShipSize 3
#define BigShipSize 4

enum Direction{
    pX, pY, nX, nY
};

class Ship{
protected:
    Part* parts = NULL;
    PartState state = Good;
public:
    Ship(){}
    Ship(int x, int y, Direction dir, int part_count)
    {
        parts = new Part[part_count];
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
    virtual ~Ship(){delete[] parts;}
    
    virtual uint8_t getSize() = 0;
    virtual void Draw() = 0;
    virtual bool takeDamage( int x, int y) = 0;
    virtual bool move(Direction dir) = 0;
    virtual bool isHere(uint8_t x, uint8_t y) = 0;

    bool doCollide(Ship& other){
        for(int i = 0; i < getSize(); i++){
            for(int j = 0; j < other.getSize(); j++){
                if( parts[i].x == other.parts[j].x && parts[i].y == other.parts[j].y ){
                    return true;
                }
            }
        }
        return false;
    }

    bool isAtEdge(){
        
    }

    PartState getState(){return state;}
    
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
    bool takeDamage(uint8_t x, uint8_t y, int size){
        uint8_t damagedCount = 0;
        for(int i = 0; i < size; i++){
            if (parts[i].state == Damaged){
                damagedCount++;
            }
            else if ( parts[i].x == x && parts[i].y == y ){
                parts[i].state = Damaged;
                if(damagedCount == size - 1){
                    state = Sunk;
                    for(int i = 0; i < size ; i++){
                        parts[i].state = Damaged;
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool isHere(uint8_t x, uint8_t y, uint8_t size){
        for(uint8_t i = 0; i < size ; i++){
            if( parts[i].x == x && parts[i].y == y ){
                return true;
            }
        }
        return false;
    }

    void Draw(uint8_t size){
        for(uint8_t i = 0; i < size ; i++){
            parts[i].Draw();
        }
    }
};