#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <iostream>
#include <memory>

#include <SDL.h>

#include "entity.h"
#include "lvl.h"
#include "ghost.h"
#include "bonus.h"

#define PACMAN_SPRITE {3,88, 16,16}

class Player : public Entity
{
private:
    std::shared_ptr <Direction> direction_;
    Direction nextDirection_;
    std::shared_ptr<Ghost> blinky_;
    std::shared_ptr<Ghost> pinky_;
    std::shared_ptr<Ghost> inky_;
    std::shared_ptr<Ghost> clyde_;
    std::shared_ptr<std::pair<int, int>> positionForGhosts_;
    std::shared_ptr<Bonus> bonus_;
    bool hasBeenKilled_;
    bool beginDyingAnimation_;
    bool endDyingAnimation_;

    bool inTransition_;

    /*Check from the ghost if one has killed Pac-man*/
    bool checkIsKill();
    /*Animation of Pac-man when dying*/
    void dyingAnimation();
    /*The animation to do when Pac-man is moving*/
    void movingAnimation(bool notMoving);

public:

    Player();
    Player(std::shared_ptr<Lvl> lvl, int speed, std::shared_ptr<Ghost> blinky, std::shared_ptr<Ghost> pinky, std::shared_ptr<Ghost> inky, std::shared_ptr<Ghost> clyde, 
        std::shared_ptr<Bonus> bonus_, SDL_Surface* plancheSprites, SDL_Surface* winSurf);
    ~Player();

    void draw();
    inline void setDirection(Direction direction) { nextDirection_ = direction; };
    void onEatable();
    bool isValidDirection();

    inline bool getDead() { return endDyingAnimation_; };
    inline bool getIsKill() { return hasBeenKilled_; };

};

#endif // PLAYER_H