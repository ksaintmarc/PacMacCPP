#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <SDL.h>
#include <memory>

#include "ghost.h"
#include "player.h"
#include "lvl.h"
#include "overlay.h"
#include "bonus.h"

#define SCALE 4
#define SPEED 120

class Game
{
private:
    //Windows
    SDL_Window* pWindow_;
    SDL_Surface* plancheSprites_, *winSurf_, *bgSprite_;
    int wWindow_, hWindow_, marginTop_;

    //Background
    SDL_Rect srcBg_;
    SDL_Rect bg_;

    //Entities
    std::shared_ptr<Ghost> blinky_;
    std::shared_ptr<Ghost> pinky_;
    std::shared_ptr<Ghost> inky_;
    std::shared_ptr<Ghost> clyde_;
    Player player_;
    std::shared_ptr<Bonus> bonus_;
    std::shared_ptr<Lvl> lvl_;

    /*To display message on screen*/
    Overlay overlay_;

    /*To know if we have to reinitialize the level*/
    bool reinit_;

    /*To know if we have finish and can do the animation of the end*/
    bool finish_;

    //Begin animation
    unsigned int timeBeforeBegin_;
    Uint32 timeElapsed_;

    //End animation
    Uint32 timeElapsedFinishAnimation_;
    int timeFinishAnimation_;

    //HighScore
    int highScore_;

    /*To know if we have to do the begining animation*/
    bool begin_;

    /*Do animation for begin and ending and display on screen*/
    bool eventOnScreen();
    
    /*This function use lvl_ and bonus_, so do not forget to create it before use*/
    void commonReinit();

    /*Game initilization*/
    void init();

    /*Restart the level with minus one life*/
    bool reinitLvl();

    /*Restart the game after Game Over or when the player want to*/
    void restart();

    /*Display the game*/
    void draw();

public :
    Game();
    ~Game();

    /*To launch the game*/
    int gameLoop();
};

#endif // GAME_H
