#include "entity.h"

#include <iostream>

Entity::Entity():
state_{Normal},
type_{Blinky},
r_sprite_{ 3,123, 16,16 },
l_sprite_{ 37,123, 16,16 },
u_sprite_{ 71,123, 16,16 },
d_sprite_{ 105,123, 16,16 },
position_{ 320, 320, 32, 32 },
plancheSprites_{ nullptr },
winSurf_{ nullptr },
positionOnGrid_{ 10, 10, 8, 8 },
lvl_{ std::make_shared<Lvl>()},
defaultSpeed_{ DEFAULT_SPEED },
actualSpeed_{ DEFAULT_SPEED },
time_{ 0 },
now_{ 0 },
timeElapsed_{ 0 },
timeMoveAnimation_{0},
sprite_{ 0,0,0,0 },
previousSprite_{ 0,0,0,0 },
msBetweenFrame_{ 17 }
{}

Entity::Entity(std::shared_ptr<Lvl> lvl, int speed, SDL_Surface* plancheSprites, SDL_Surface* winSurf) :
state_{ Normal },
type_{ Blinky },
r_sprite_{ 3,123, 16,16 },
l_sprite_{ 37,123, 16,16 },
u_sprite_{ 71,123, 16,16 },
d_sprite_{ 105,123, 16,16 },
position_{ 320, 320, 32, 32 },
plancheSprites_{ plancheSprites },
winSurf_{ winSurf },
positionOnGrid_{ 10, 10, 8, 8 },
lvl_{ lvl },
defaultSpeed_{ speed },
actualSpeed_{ speed },
time_{ 0 },
now_{ 0 },
timeElapsed_{ 0 },
timeMoveAnimation_{ 0 },
sprite_{ 0,0,0,0 },
previousSprite_{ 0,0,0,0 },
msBetweenFrame_{ 17 }
{
    //Update position_ depending of positionOnGrid_ and lvl_->scale
    setPositions(positionOnGrid_);
    time_ = SDL_GetTicks();
    msBetweenFrame_ = (int)floor(1000.f / actualSpeed_ + 0.5f);
}

Entity::Entity(EntityType type, State state, SDL_Rect r_sprite, SDL_Rect l_sprite, SDL_Rect u_sprite, SDL_Rect d_sprite,
    SDL_Rect position, std::shared_ptr<Lvl> lvl, int speed, SDL_Surface* plancheSprites, SDL_Surface* winSurf):
state_{ state },
type_{ type },
r_sprite_{ r_sprite },
l_sprite_{ l_sprite },
u_sprite_{ u_sprite },
d_sprite_{ d_sprite },
position_{ 320, 320, 32, 32 },
plancheSprites_{ plancheSprites },
winSurf_{ winSurf },
positionOnGrid_{ position },
lvl_{ lvl },
defaultSpeed_{ speed },
actualSpeed_{ speed },
time_{ 0 },
now_{ 0 },
timeElapsed_{ 0 },
timeMoveAnimation_{ 0 },
sprite_{ r_sprite },
previousSprite_{ r_sprite },
msBetweenFrame_{17}
{
    //Update position_ depending of positionOnGrid_ and lvl_->scale
    setPositions(positionOnGrid_);
    time_ = SDL_GetTicks();
    msBetweenFrame_ = (int)floor(1000.f / actualSpeed_ + 0.5f);
}

Entity::~Entity()
{}

void Entity::draw() {
        SDL_BlitScaled(plancheSprites_, &sprite_, winSurf_, &position_);
}