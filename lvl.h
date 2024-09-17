#ifndef LVL_H
#define LVL_H

#include <iostream>
#include <vector>
#include <SDL.h>
#include "overlay.h"

#define H -1
#define P 0
#define W 1
#define T 2
#define U 3
#define D 4
#define V 5
#define S 6
#define F 7

class Lvl
{
private:
    SDL_Surface* plancheSprites_, * winSurf_;
    //The total number of dot/pellet
    int dot_num;
    SDL_Rect dotSprite_;
    SDL_Rect pelletSprite_;
    SDL_Rect pos_;
    //To display some text on the screen
    Overlay overlay_;

    //Flicker animation
    Uint32 timeElapsedFlicker_;
    int timeCycleFliker_;
public:
    //The level as a grid of 8 pixel by cell
    std::vector<std::vector<int>> lvl1Grid_;
    SDL_Rect srcBg_;

    int eaten_bonuses_, eaten_dots_, eaten_ghosts_, score_, lives_;
    int scale_, marginTop_;

    //High score
    int highScore_;
    
    //If pacman is dead the lvl change this attribute
    bool endgame_;

    //Is true if all the pellet have been eaten
    bool finish_;

    Lvl();
    Lvl(SDL_Rect srcBg_, int scale, int marginTop, int highScore, Overlay overlay, SDL_Surface* plancheSprites, SDL_Surface* winSurf);
    ~Lvl();

    void draw();
    /*Display the score on the screen*/
    void display();
    /*Use to flicker the level sprite at the end of the level*/
    void flickerAnimation();
};

#endif // LVL_H