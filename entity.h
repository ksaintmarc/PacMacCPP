#ifndef ENTITY_H
#define ENTITY_H

#define SPRITE_SCALE 8
#define DEFAULT_SPEED 90

#include <string>
#include <iostream>
#include <vector>
#include <memory>

#include <SDL.h>

#include "lvl.h"

enum EntityType { Blinky, Pinky, Inky, Clyde, Pacman, Fruit };
enum State { Normal, Scared, Invisible, Dead };
enum Direction { Up, Right, Down, Left, None };

class Entity
{
protected:
    EntityType type_;
    State state_;
    SDL_Rect r_sprite_; // First frame for right animation
    SDL_Rect l_sprite_; // First frame for left animation
    SDL_Rect u_sprite_; // First frame for up animation
    SDL_Rect d_sprite_; // First frame for down animation
    SDL_Rect positionOnGrid_;   // Position of the entity int the lvl grid
    SDL_Rect position_;   // Position of the entity depending of the lvl scale and margin

    SDL_Surface *plancheSprites_, *winSurf_;

    std::shared_ptr<Lvl> lvl_;   // Lvl

    //Speed
    int defaultSpeed_; //Highter value = faster speed
    int actualSpeed_; //Highter value = faster speed
    int msBetweenFrame_;
    SDL_Rect sprite_;
    SDL_Rect previousSprite_;

    //Time management
    Uint32 time_;
    Uint32 now_;
    Uint32 timeElapsed_;
    Uint32 timeMoveAnimation_;
public:

    Entity();
    Entity(std::shared_ptr<Lvl> lvl, int speed, SDL_Surface* plancheSprites, SDL_Surface* winSurf);
    Entity(EntityType type, State state, SDL_Rect r_sprite, SDL_Rect l_sprite, SDL_Rect u_sprite, SDL_Rect d_sprite, 
        SDL_Rect position, std::shared_ptr<Lvl> lvl, int speed, SDL_Surface* plancheSprites, SDL_Surface* winSurf);
    ~Entity();

    inline const EntityType getName() { return type_; };
    inline const int getState() { return state_; };
    inline const SDL_Rect getRSprite() { return r_sprite_; };
    inline const SDL_Rect getLSprite() { return l_sprite_; };
    inline const SDL_Rect getUSprite() { return u_sprite_; };
    inline const SDL_Rect getDSprite() { return d_sprite_; };
    inline const SDL_Rect getPosition() { return positionOnGrid_; };
    inline const std::shared_ptr<Lvl> getLvl() { return lvl_; };

    inline void setName(EntityType type) { type_ = type; };
    inline void setState(State state) { state_ = state; };
    inline void setRSprite(SDL_Rect sprite) { r_sprite_ = sprite; };
    inline void setLSprite(SDL_Rect sprite) { l_sprite_ = sprite; };
    inline void setUSprite(SDL_Rect sprite) { u_sprite_ = sprite; };
    inline void setDSprite(SDL_Rect sprite) { d_sprite_ = sprite; };
    inline void setPositions(SDL_Rect position) { 
        positionOnGrid_ = position; 
        position_ = { positionOnGrid_.x * lvl_->scale_ * SPRITE_SCALE, positionOnGrid_.y * lvl_->scale_ * SPRITE_SCALE + lvl_->marginTop_, positionOnGrid_.w * lvl_->scale_, positionOnGrid_.h * lvl_->scale_ };
    };

    void draw();
    inline void changeSpeed(int newSpeed) { actualSpeed_ = newSpeed; msBetweenFrame_ = (int)floor(1000.f / actualSpeed_ + 0.5f); }
    inline void resetSpeed() { actualSpeed_ = defaultSpeed_; msBetweenFrame_ = (int)floor(1000.f / actualSpeed_ + 0.5f); }
};

#endif // ENTITY_H
