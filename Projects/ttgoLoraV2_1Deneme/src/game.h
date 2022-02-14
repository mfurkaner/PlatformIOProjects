#include <Arduino.h>
#include <vector>

#ifndef SCREEN
    #include "screen.h"
#endif

#ifndef CONNECTION
    #include "connection.h"
#endif


#define GAME

#define MAX_SHIP_COUNT 6

enum GameState: uint8_t{
    Idle, Joining, Creating, 
    Setting, PlayerTurn,
    EnemyTurn, Connection_Lost, Win, Lost
};

struct GameCursor{
    int x = 0;
    int y = 0;
    void reset(){
        x = 0;
        y = 0;
    }
    void move(Direction dir){
        switch (dir)
        {
        case pX:
            if (x < GRID_WIDTH){
                x++;
            }
            break;
        case nX:
            if (x >= 0){
                x--;
            }
            break;
        case pY:
            if (y < GRID_HEIGHT){
                y++;
            }
            break;
        case nY:
            if (y >= 0){
                y--;
            }
            break;
        default:
            break;
        }
    }
};

class Game{
    GameState state = Idle;
    //GameGrid Map;
    GameCursor cursor;
    std::vector<Part> damagedEnemyParts;
    std::vector<Part> emptyTiles;
    std::vector<Ship*> playerShips;
    TCPConnection connection;
    Response response;
    bool shoot = false;

    Screen* screen = (Screen*) new HomeScreen();

    void reset(){}

    void setCursor(uint8_t x, uint8_t y){cursor.x = x; cursor.y = y;}

    bool isHit(uint8_t x, uint8_t y){
        for(uint8_t i = 0; i < playerShips.size(); i++){
            if( playerShips[i]->takeDamage(x, y) ) return true;
        }
        return false;
    }

    bool isShot(uint8_t x, uint8_t y){
        uint8_t size = damagedEnemyParts.size() + emptyTiles.size();
        for(uint8_t i = 0; i < size ; i++ ){
            if( i < damagedEnemyParts.size() && damagedEnemyParts[i].x == x && damagedEnemyParts[i].y == y){
                return true;
            }
            else if ( i >= damagedEnemyParts.size() && emptyTiles[i - damagedEnemyParts.size()].x == x && emptyTiles[i - damagedEnemyParts.size()].y == y ){
                return true;
            }
        }
        return false;
    }

    bool didLose(){
        for(uint8_t i = 0; i < playerShips.size(); i++){
            if( playerShips[i]->getState() == Good ) return false;
        }
        return true;
    }

    void drawPlayerShips(){
        for(uint8_t i = 0; i < playerShips.size(); i++){
            playerShips[i]->Draw();
        }
    }
    void drawEnemyShips(){
        for(uint8_t i = 0; i < damagedEnemyParts.size(); i++){
            damagedEnemyParts[i].Draw();
        }
        for(uint8_t i = 0; i < emptyTiles.size(); i++){
            emptyTiles[i].Draw();
        }
    }

    void findNetwork(){
        while( !connection.printNearbyNetworks() ){
            delay(200);
        }
        delay(2000);
    }

    void Draw(){
        if ( screen->Draw() ){
            if( state == Setting ){
                // draw set your ship position
                drawPlayerShips();
            }
            else if (state == EnemyTurn ){
                // draw enemy turn
                drawPlayerShips();
            }
            else if(state == PlayerTurn){
                // draw your turn
                drawEnemyShips();
            }
            OLED.sendBuffer();
        }
    }

    bool detectCollision(Ship& newShip){
        for(int i = 0; i < playerShips.size() ; i++ ){
            if ( newShip.doCollide(*playerShips[i]) ){
                return true;
            }
        }
        return false;
    }

    void createShip(){
        if (playerShips.size() == MAX_SHIP_COUNT){
            return;
        }
        uint8_t x;
        uint8_t y;
        Ship* newShip = NULL;
        if (playerShips.size() < MAX_SHIP_COUNT/3){
            do{
                delete(newShip);
                x = random(0, 12);
                y = random(0, 3);
                newShip = (Ship*)new BigShip(x, y, pY);
            }while ( detectCollision(*newShip) );
            playerShips.push_back(newShip);
        }
        else if (playerShips.size() < MAX_SHIP_COUNT*2/3){
            do{
                delete(newShip);
                x = random(0, 10);
                y = random(0, 6);
                newShip = (Ship*)new MidShip(x, y, pX);
            }while ( detectCollision(*newShip) );
            
        }
        else if (playerShips.size() < MAX_SHIP_COUNT){
            do{ 
                delete(newShip);
                x = random(0, 11);
                y = random(0, 6);
                newShip = (Ship*)new MiniShip(x, y, pX);
            }while ( detectCollision(*newShip) );
        }
        playerShips.push_back(newShip);
    }

    void handleInput(){
        switch (state)
        {
        case Idle:
            /* home screen - join, create click implement */
            // move cursor
            if(move){
                ((HomeScreen*)screen)->ChangeState();
                move = false;
            }
            // click
            if (click){
                state = ( ((HomeScreen*)screen)->getState() == Create) ? Creating : Joining;
                delete(screen);
                if ( state == Creating ){
                    screen = (Screen*) new CreateScreen();
                    connection.setConnectionType(_Gateway);
                } 
                else{
                    screen = (Screen*) new JoinScreen();
                    connection.setConnectionType(_Client);
                    //findNetwork();
                } 
            }
            break;
        case Setting:
            // move
            if (move){
                playerShips.back()->move(direction);
                move = false;
            }
            // click                                                                // TODO : ship positions and directions
            if (click){
                createShip();
                click = false;
            }
            break;
        case PlayerTurn:
            // move
            if (move){
                cursor.move(direction);
                move = false;
            }
            // click
            if (click){ shoot = true; click = false; }
            break;
        default:
            break;
        }
    }

    void update(){
        switch (state)
        {
            case Joining:
                digitalWrite(LED_BUILTIN, HIGH);
                delete(screen);
                screen = (Screen*) new GameScreen();
                state = Setting;
                break;
            case Creating:
                digitalWrite(LED_BUILTIN, HIGH);
                delete(screen);
                screen = (Screen*) new GameScreen();
                state = Setting;
                break;
            case Setting:
                if( playerShips.size() == MAX_SHIP_COUNT ){
                    state = ( connection.getConnectionType() == _Gateway ) ? PlayerTurn : EnemyTurn;
                    delete(screen);
                    screen = (Screen*) new GameScreen();
                }
                break;
            case PlayerTurn:
                if(response == R_Lost ){ state = Win;}
                else if(shoot){
                    Part shotTile;
                    shotTile.x = cursor.x;
                    shotTile.y = cursor.y;
                    if (response == R_Hit){
                        shotTile.state = Damaged;
                        damagedEnemyParts.push_back(shotTile);
                    }
                    else if(response == R_Miss){
                        shotTile.state = Empty;
                        emptyTiles.push_back(shotTile);
                        state = EnemyTurn;
                    }
                    else{
                        Serial.print("Firing Response is not valid : ");
                        Serial.println(response);
                    }
                    shoot = false;
                }
                break;
            case EnemyTurn:
                if (response == R_Check){
                    bool hit = isHit(connection.hitX, connection.hitY);
                    connection.notifyFireRespose(hit);
                    if( didLose() ){
                        state = Lost;
                    }
                    else if (!hit){
                        state = PlayerTurn;
                    }
                }
                else if (response == R_Lost){
                    state = Win;
                }
                break;
            case Lost:
                reset();
                break;
            case Win:
                reset();
                break;
            case Connection_Lost:
                reset();
                break;
            default:
                break;
        }

    }
    void handleCommunication(){
        if ( !(state != Idle || state != Joining || state != Creating) && !connection.checkConnection() ){
            state = Connection_Lost;
            OLED.clearBuffer();
            OLED.drawFrame(0,0,OLED.getWidth(),OLED.getHeight());
            OLED.setFont(u8g2_font_profont10_tf);
            OLED.setCursor(15, 5*OLED.getFontAscent());
            OLED.print("OOPSSYYYY");
            OLED.sendBuffer();
            delay(2000);
        }
        switch (state)
        {
            case Joining:
                while( !connection.setUpConnection() ){delay(100);}
                break;
            case Creating:
                while( !connection.setUpConnection() ) {delay(100);}
                break;
            case Setting:
                if ( playerShips.size() == MAX_SHIP_COUNT ){
                    connection.notifySettingReady();
                    connection.waitForEnemySettings();
                }
                break;
            case PlayerTurn:
                if(connection.available() > 1 ) response = connection.handleIncoming();
                else if (shoot){
                    connection.notifyFiring(cursor.x, cursor.y);
                    response = connection.handleIncoming();
                }
                break;
            case EnemyTurn:
                response = connection.handleIncoming();
                break;
            case Lost:
                connection.notifyLosing();
                break;
            default:
                break;
        }
    }

public:
    Game(){
        pinMode(LED_BUILTIN, OUTPUT);
    }

    void Run(){
        Draw();
        handleInput();
        handleCommunication();
        update();
        Draw();
    }


    bool move = false;
    bool click = false;
    Direction direction;

    void testSetClient();
    void testLoopClient();

    void testSetGateway();
    void testLoopGateway();

    void setState(GameState state){this->state = state;}
    GameState getState(){return state;}
};