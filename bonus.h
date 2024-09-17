#ifndef BONUS_H
#define BONUS_H

#include <string>
#include <iostream>
#include <memory>

#include <SDL.h>

#include "entity.h"
#include "lvl.h"

#define CHERRY_SPRITE {290,236,16,16}
#define STRAWBERRY_SPRITE {306,236,16,16}
#define ORANGE_SPRITE {322,236,16,16}
#define APPLE_SPRITE {338,236,16,16}
#define MELON_SPRITE {344,236,16,16}
#define SPACE_SPRITE {460,236,16,16}
#define BELL_SPRITE {476,236,16,16}
#define KEY_SPRITE {492,236,16,16}

class Bonus : public Entity
{
private:
	int value_;
	Uint32 timeToBeVisible_;
	//List of bonus to appear in the level
	std::vector<std::pair<SDL_Rect, int>> bonuses_;
	//To avoid the spawn of 2 bonus one after the other
	bool isEaten_;

public:
	Bonus();
	Bonus(std::shared_ptr<Lvl> lvl, SDL_Surface* plancheSprites, SDL_Surface* winSurf);
	~Bonus();

	void draw();
	//Use if pacman eat a bonus and increment the number of eaten bonuses and the score of the level
	void eaten();
	inline void setIsEaten() { isEaten_ = false; };
};
#endif // BONUS_H