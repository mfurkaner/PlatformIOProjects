#include "ship_base.h"

class MiniShip : Ship{
public:
    MiniShip(){}
    MiniShip(int x, int y, Direction dir) : Ship(x, y, dir, MiniShipSize){}
    ~MiniShip(){delete[] parts;}


    void Draw() { ((Ship*)this)->Draw(MiniShipSize); }

    uint8_t getSize(){return MiniShipSize;}

    bool takeDamage(int x, int y) { return ((Ship*)this)->takeDamage(x, y, MiniShipSize); }

    bool move(Direction dir) { return ((Ship*)this)->move(dir, MiniShipSize);}

    bool isHere(uint8_t x, uint8_t y) { return ((Ship*)this)->isHere(x, y, MiniShipSize);}
};

class MidShip : Ship{
public:
    MidShip(){}
    MidShip(int x, int y, Direction dir) : Ship(x, y, dir, MidShipSize){}
    ~MidShip(){delete[] parts;}

    void Draw() { ((Ship*)this)->Draw(MidShipSize); }

    uint8_t getSize(){return MidShipSize;}

    bool takeDamage(int x, int y) { return ((Ship*)this)->takeDamage(x, y, MidShipSize); }

    bool move(Direction dir) { return ((Ship*)this)->move(dir, MidShipSize);}

    bool isHere(uint8_t x, uint8_t y) { return ((Ship*)this)->isHere(x, y, MidShipSize);}
};

class BigShip : Ship{
public:
    BigShip(){}
    BigShip(int x, int y, Direction dir) : Ship(x, y, dir, BigShipSize){}
    ~BigShip(){delete[] parts;}

    void Draw() { ((Ship*)this)->Draw(BigShipSize); }

    uint8_t getSize(){return BigShipSize;}

    bool takeDamage(int x, int y) { return ((Ship*)this)->takeDamage(x, y, BigShipSize); }

    bool move(Direction dir) { return ((Ship*)this)->move(dir, BigShipSize);}

    bool isHere(uint8_t x, uint8_t y) { return ((Ship*)this)->isHere(x, y, BigShipSize);}
};