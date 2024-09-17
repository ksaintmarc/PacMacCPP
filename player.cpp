#include "player.h"

#include <iostream>

Player::Player() :
Entity{},
direction_{},
blinky_{},
pinky_{},
inky_{},
clyde_{},
positionForGhosts_{},
bonus_{},
inTransition_{ false },
hasBeenKilled_{ false },
beginDyingAnimation_{ false },
endDyingAnimation_{false}
{
    direction_ = std::make_shared<Direction>(Right);
    nextDirection_ = Right;
    positionForGhosts_ = std::make_shared<std::pair<int, int>>();
}

Player::Player(std::shared_ptr<Lvl> lvl, int speed, std::shared_ptr<Ghost> blinky, std::shared_ptr<Ghost> pinky, std::shared_ptr<Ghost> inky, std::shared_ptr<Ghost> clyde, 
    std::shared_ptr<Bonus> bonus, SDL_Surface* plancheSprites, SDL_Surface* winSurf):
Entity{ Pacman, Normal, PACMAN_SPRITE, PACMAN_SPRITE, PACMAN_SPRITE, PACMAN_SPRITE, {10 , 20, SPRITE_SCALE, SPRITE_SCALE}, lvl, speed, plancheSprites, winSurf },
direction_{},
blinky_{blinky},
pinky_{pinky},
inky_{inky},
clyde_{clyde},
positionForGhosts_{},
bonus_{bonus},
inTransition_{false},
hasBeenKilled_{false},
beginDyingAnimation_{false},
endDyingAnimation_{ false }
{
    direction_ = std::make_shared<Direction>(Right);
    nextDirection_ = Right;
    positionForGhosts_ = std::make_shared<std::pair<int, int>>(position_.x, position_.y);

    //Link Pac-Man datas to each ghost
    blinky_->setPacmanPos(positionForGhosts_, direction_);
    pinky_->setPacmanPos(positionForGhosts_, direction_);
    inky_->setPacmanPos(positionForGhosts_, direction_);
    clyde_->setPacmanPos(positionForGhosts_, direction_);
}

Player::~Player()
{}

void Player::draw() {
    now_ = SDL_GetTicks();
    if ((int)(now_ - time_) < msBetweenFrame_ && !endDyingAnimation_) {
        Entity::draw();
        return;
    }

    time_ = now_;

    //Check if kill in the previous call
    if (!beginDyingAnimation_  && checkIsKill()) {
        //Notif for the ghost to stop moving
        blinky_->setStopMoving();
        pinky_->setStopMoving();
        inky_->setStopMoving();
        clyde_->setStopMoving();
        beginDyingAnimation_ = true;
        sprite_ = PACMAN_SPRITE;
        previousSprite_ = sprite_;
        timeElapsed_ = now_;
    }
    if (beginDyingAnimation_ && !endDyingAnimation_){
        if (now_ - timeElapsed_ < TIME_PAUSE_DEAD) {
            timeMoveAnimation_ = now_;
            Entity::draw();
        }
        else
            dyingAnimation();
        return;
    }

    if (isValidDirection())
        *direction_ = nextDirection_;

    onEatable();

    int scale = 8 * lvl_->scale_;
    int yScaled = position_.y - lvl_->marginTop_;
    int y = yScaled / scale;
    int x = position_.x / scale;
    bool notMoving = false;

    //Tunnel handling

    if (inTransition_ || lvl_->lvl1Grid_[y][x+1] == T) {
        if (*direction_ == Right) {
            inTransition_ = true;
            if (position_.x+1 >= lvl_->srcBg_.w *lvl_->scale_-1) {
                position_.x = 0;
                inTransition_ = false;
            }
            position_.x++;
        }
        else if (position_.x - 1 <= lvl_->srcBg_.w * lvl_->scale_ - scale) {
            inTransition_ = false;
            position_.x--;
        }
        else
            position_.x--;
    }
    if (lvl_->lvl1Grid_[y][x] == T) {
        if (*direction_ == Left) {
            if (x == 0) {
                position_.x = lvl_->srcBg_.w * lvl_->scale_;
                inTransition_ = true;
            }
            position_.x--;
        }
        else position_.x++;
    }
    else if (*direction_ == Right && lvl_->lvl1Grid_[y][x+1] != W && lvl_->lvl1Grid_[y][x + 1] != H) {
        sprite_ = r_sprite_;
        position_.x++;
    }
    else if (*direction_ == Down && lvl_->lvl1Grid_[y + 1][x] != W && lvl_->lvl1Grid_[y + 1][x] != H) {
        sprite_ = d_sprite_;
        position_.y++;
    }
    else if (*direction_ == Left && (position_.x % scale || lvl_->lvl1Grid_[y][x - 1] != W && lvl_->lvl1Grid_[y][x - 1] != H)) {
        sprite_ = l_sprite_;
        position_.x--;
    }
    else if (*direction_ == Up && (yScaled % scale || lvl_->lvl1Grid_[y - 1][x] != W && lvl_->lvl1Grid_[y - 1][x] != H)) {
        sprite_ = u_sprite_;
        position_.y--;
    }
    else {
        sprite_ = previousSprite_;
        notMoving = true;
    }

    previousSprite_ = sprite_;

    movingAnimation(notMoving);

    position_.w = scale;
    position_.h = scale;
    Entity::draw();

    //Update the position for the ghosts
    *positionForGhosts_ = { position_.x, position_.y };
}

void Player::movingAnimation(bool notMoving) {
    if (notMoving)
        return;
    unsigned int timing = (unsigned int)(actualSpeed_ / 8.f) * msBetweenFrame_;
    unsigned int elapsed = now_ - timeMoveAnimation_;
    if (elapsed >= timing * 3) {
        switch (*direction_) {
        case Right:
            sprite_.x += 17;
            break;
        case Left:
            sprite_.x += 46;
            break;
        case Up:
            sprite_.x += 79;
            break;
        case Down:
            sprite_.x += 113;
            sprite_.y += 1;
            break;
        }
        if (elapsed >= timing * 4)
            timeMoveAnimation_ = now_;
    }
    else if (elapsed >= timing * 2) {
        switch (*direction_) {
        case Right:
            sprite_.x += 32;
            break;
        case Left:
            sprite_.x += 62;
            break;
        case Up:
            sprite_.x += 96;
            break;
        case Down:
            sprite_.x += 130;
            sprite_.y += 4;
            break;
        }
    }
    else if (elapsed >= timing) {
        switch (*direction_){
        case Right:
            sprite_.x += 17;
            break;
        case Left:
            sprite_.x += 46;
            break;
        case Up:
            sprite_.x += 79;
            break;
        case Down:
            sprite_.x += 113;
            sprite_.y += 1;
            break;
        }
    }
}

void Player::dyingAnimation() {
    unsigned int timing = (unsigned int)(actualSpeed_ / 8.f) * msBetweenFrame_;
    unsigned int elapsed = now_ - timeMoveAnimation_;
    sprite_ = PACMAN_SPRITE;
    if (elapsed >= timing * 12) {
        sprite_ = { 150, 106, 16 ,16 };
        if (elapsed >= timing * 13) {
            timeMoveAnimation_ = now_;
            endDyingAnimation_ = true;
        }
    }
    else if (elapsed >= timing * 11) {
        sprite_ = { 137, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 10) {
        sprite_ = { 126, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 9) {
        sprite_ = { 113, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 8) {
        sprite_ = { 98, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 7) {
        sprite_ = { 80, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 6) {
        sprite_ = { 61, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 5) {
        sprite_ = { 42, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 4) {
        sprite_ = { 23, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 3) {
        sprite_ = { 4, 104, 16 ,16 };
    }
    else if (elapsed >= timing * 2) {
        sprite_.x += 96;
    }
    else if (elapsed >= timing) {
        sprite_.x += 79;
    }

    Entity::draw();
}

void Player::onEatable() {
    int scale = 8 * lvl_->scale_;

    int y = position_.y - lvl_->marginTop_;

    if (position_.x % scale == 0 && y % scale == 0) {
        int spot = lvl_->lvl1Grid_[y / scale][position_.x / scale];
        if (spot == D || spot == S) {
            lvl_->lvl1Grid_[y / scale][position_.x / scale] = P;
            if (spot == S) {
                lvl_->eaten_ghosts_ = 0;
                blinky_->setVulnerableState();
                pinky_->setVulnerableState();
                inky_->setVulnerableState();
                clyde_->setVulnerableState();
                lvl_->score_ += 50;
                ++lvl_->eaten_dots_;
                bonus_->setIsEaten();
            }
            else if (spot == D) {
                lvl_->score_ += 10;
                ++lvl_->eaten_dots_;
                bonus_->setIsEaten();
            }
        }
        else if (spot == F) {
            bonus_->eaten();
        }
        else if (spot == V) {
            lvl_->lvl1Grid_[y / scale][position_.x / scale] = U;
            ++lvl_->eaten_dots_;
        }
    }
}

bool Player::isValidDirection() {
    int scale = 8 * lvl_->scale_;

    int y = position_.y - lvl_->marginTop_;

    if (position_.x % scale != 0 || y % scale != 0 || nextDirection_ == *direction_)
        return false;

    switch (nextDirection_)
    {
    case Up:
        return *direction_ == Down || lvl_->lvl1Grid_[(y / scale) - 1][(position_.x) / scale] != W;
    case Down:
        return *direction_ == Up || (lvl_->lvl1Grid_[(y / scale) + 1][(position_.x) / scale] != W);
    case Left:
        return *direction_ == Right || (lvl_->lvl1Grid_[y / scale][position_.x / scale - 1] != W);
    case Right:
        return *direction_ == Left || (lvl_->lvl1Grid_[y / scale][position_.x / scale + 1] != W);
    default:
        return false;
    }
}

bool Player::checkIsKill() {
    if(!hasBeenKilled_)
        hasBeenKilled_ = blinky_->isPacmanKilled() || pinky_->isPacmanKilled() || inky_->isPacmanKilled() || clyde_->isPacmanKilled();
    return hasBeenKilled_;
}