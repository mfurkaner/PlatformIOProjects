#include <Arduino.h>
#include <vector>
#include "screen.h"
#include "connection.h"

#define MAX_SHIP_COUNT 6

enum GameState{
    Idle, Joining, Creating, 
    Connected, Setting, PlayerTurn,
    EnemyTurn, Connection_Lost, Quit
};

struct GameCursor{
    int x = 0;
    int y = 0;
    void reset(){
        x = 0;
        y = 0;
    }
    bool move(Direction dir);
};

class Game{
    GameState state = Idle;
    GameGrid playerMap;
    GameGrid enemyMap;
    GameCursor cursor;
    std::vector<Part> damagedEnemyParts;
    std::vector<Part> emptyTiles;
    std::vector<Ship*> playerShips;
    TCPConnection connection;


    Screen* screen = (Screen*) new HomeScreen();

    void handleInput(){
        switch (state)
        {
        case Idle:
            /* home screen - join, create click implement */
            // move cursor
            ((HomeScreen*)screen)->ChangeState();
            // click
            state = ( ((HomeScreen*)screen)->getState() == Create) ? Creating : Joining;
            delete(screen);
            if ( state == Creating ){
                screen = (Screen*) new CreateScreen();
                connection.setConnectionType(_Gateway);
            } 
            else{
                screen = (Screen*) new JoinScreen();
                connection.setConnectionType(_Client);
            } 

            break;
        case Joining:
            break;/* join screen - no input just waiting */
        case Creating:
            break;/* create screen - no input just waiting */
        case Setting:
            // move
            Direction dir;
            playerShips.back()->move(dir);
            // click                                                                // TODO : ship positions and directions
            if ( playerShips.size() == MAX_SHIP_COUNT){
                state = ( connection.getConnectionType() == _Gateway ) ? PlayerTurn : EnemyTurn;
                delete(screen);
                screen = (Screen*) new GameScreen();
            }
            else if (playerShips.size() < MAX_SHIP_COUNT/3){
                playerShips.push_back((Ship*)new BigShip(0,0,dir));
            }
            else if (playerShips.size() < MAX_SHIP_COUNT*2/3){
                playerShips.push_back((Ship*)new MidShip(0,0,dir));
            }
            else{
                playerShips.push_back((Ship*)new MiniShip(0,0,dir));
            }
            break;
        case PlayerTurn:
            // move
            Direction dir;
            cursor.move(dir);
            // click

            break;
        case EnemyTurn:
            break;/* enemy turn - no input just waiting */
        case Connection_Lost:

            break;
        case Quit:
            break;/* quit - no input just waiting */
        default:
            break;
        }
    }

    void update(){

    }
    void handleCommunication(){
        
    }
public:

    void Run(){
        screen->Draw();
        handleInput();
        handleCommunication();
        update();
    }
};