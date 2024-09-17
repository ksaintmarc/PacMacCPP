#include "bonus.h"

Bonus::Bonus() :
Entity{},
value_{ 0 },
timeToBeVisible_{ 9000 },
isEaten_{false},
bonuses_{ {CHERRY_SPRITE, 100}, {STRAWBERRY_SPRITE, 300}}
{};

Bonus::Bonus(std::shared_ptr<Lvl> lvl, SDL_Surface* plancheSprites, SDL_Surface* winSurf) :
Entity{ Fruit, Invisible, CHERRY_SPRITE, CHERRY_SPRITE, CHERRY_SPRITE, CHERRY_SPRITE, {10 , 15, SPRITE_SCALE, SPRITE_SCALE}, lvl, 0, plancheSprites, winSurf },
value_{ 0 },
timeToBeVisible_{ 9000 },
isEaten_{false},
bonuses_{ {CHERRY_SPRITE, 100}, { STRAWBERRY_SPRITE, 300 } }
{};

Bonus::~Bonus()
{}

void Bonus::draw() {
	now_ = SDL_GetTicks();

	if (state_ == Invisible && (lvl_->eaten_dots_ == 70 || lvl_->eaten_dots_ == 170) && !isEaten_) {
			state_ = Normal;
			timeElapsed_ = now_;
			return;
	}
	if (state_ == Normal) {
		if (now_ - timeElapsed_ >= timeToBeVisible_) {
			state_ = Invisible;
			timeElapsed_ = now_;
			return;
		}
		if (lvl_->eaten_bonuses_ >= bonuses_.size()) return;
		sprite_ = bonuses_[lvl_->eaten_bonuses_].first;
		value_ = bonuses_[lvl_->eaten_bonuses_].second;
		Entity::draw();
	}
}

void Bonus::eaten() {
	if (state_ != Invisible) {
		lvl_->score_ += value_;
		++lvl_->eaten_bonuses_;
		isEaten_ = true;
		state_ = Invisible;
	}
}
