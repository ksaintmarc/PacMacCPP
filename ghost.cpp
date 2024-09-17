#include "ghost.h"

#include <iostream>
#include <random>
#include <algorithm>

Ghost::Ghost() :
Entity{},
blinkyForInky_{ nullptr },
startPoint_{10,10},
directions_{},
nodesGrid_{},
currentPosition_{ 10, 10 },
previousPosition_{ 11, 10 },
inTransition_{ 0 },
isHome_{ false },
goOutOfHome_{ false },
waitingTime_{ 0 },
goOnPatrol_{true},
patrolCounter_{0},
timeToBeOnPatrol_{7000},
patrolPoint_{ 19, -2 },
timeElapsedBeforePatrol_{ 0 },
timeToBeVulnerable_{ 6000 },
timeToFlicker_{ 3000 },
timeFlickerAnimation_{ 0 },
timeElapsedVulnerable_{ 0 },
vulnerableSprite_{3, 195, 16, 16},
flikerSprite_{37, 195, 16, 16},
isDead{false},
rDeadSprite_{ 71, 198, 16, 16 },
lDeadSprite_{ 89, 198, 16, 16 },
uDeadSprite_{ 105, 198, 16, 16 },
dDeadSprite_{ 122, 198, 16, 16 },
scale_{ 1 },
pacmanPos_{},
pacmanDir_{},
haveKillPacman_{false},
stopMoving_{false},
timePauseElapsed_{ 0 }
{
    scale_ = SPRITE_SCALE * lvl_->scale_;
    sprite_ = r_sprite_;
}

Ghost::Ghost(std::shared_ptr<Lvl> lvl, int speed, SDL_Surface* plancheSprites, SDL_Surface* winSurf):
Entity{ lvl, speed, plancheSprites, winSurf },
blinkyForInky_{ nullptr },
startPoint_{ 10,10 },
directions_{},
nodesGrid_{},
currentPosition_{ 10, 10 },
previousPosition_{ 11, 10 },
inTransition_{ 0 },
isHome_{ false },
goOutOfHome_{ false },
waitingTime_{ 0 },
goOnPatrol_{ true },
patrolCounter_{ 0 },
timeToBeOnPatrol_{ 7000 },
patrolPoint_{ 19, -2},
timeElapsedBeforePatrol_{ 0 },
timeToBeVulnerable_{ 6000 },
timeToFlicker_{ 3000 },
timeFlickerAnimation_{ 0 },
timeElapsedVulnerable_{ 0 },
vulnerableSprite_{ 3, 195, 16, 16 },
flikerSprite_{ 37, 195, 16, 16 },
isDead{ false },
rDeadSprite_{ 71, 198, 16, 16 },
lDeadSprite_{ 89, 198, 16, 16 },
uDeadSprite_{ 105, 198, 16, 16 },
dDeadSprite_{ 122, 198, 16, 16 },
scale_{ 1 },
pacmanPos_{},
pacmanDir_{},
haveKillPacman_{ false },
stopMoving_{ false },
timePauseElapsed_{ 0 }
{
    scale_ = SPRITE_SCALE * lvl_->scale_;
    sprite_ = r_sprite_;
    initNodesGrid();
}

Ghost::Ghost(EntityType type, State state, SDL_Rect r_sprite, SDL_Rect l_sprite, SDL_Rect u_sprite, SDL_Rect d_sprite, std::pair<int, int> startPoint, std::shared_ptr<Lvl> lvl, std::pair<int, int> patrolPoint, unsigned int waitingTime, int speed, SDL_Surface* plancheSprites, SDL_Surface* winSurf, std::shared_ptr<Ghost> blinkyForInky):
Entity{ type, state, r_sprite, l_sprite, u_sprite, d_sprite, {startPoint.first, startPoint.second, SPRITE_SCALE, SPRITE_SCALE}, lvl, speed, plancheSprites, winSurf },
blinkyForInky_{ blinkyForInky },
startPoint_{ startPoint },
directions_{},
nodesGrid_{},
currentPosition_{0,0},
previousPosition_{ 11, 10 },
inTransition_{0},
isHome_{ true },
goOutOfHome_{false},
waitingTime_{ waitingTime },
goOnPatrol_{ false },
patrolCounter_{ 0 },
timeToBeOnPatrol_{ 7000 },
patrolPoint_{ patrolPoint },
timeElapsedBeforePatrol_{ 0 },
timeToBeVulnerable_{ 6000 },
timeToFlicker_{ 3000 },
timeFlickerAnimation_{ 0 },
timeElapsedVulnerable_{0},
vulnerableSprite_{ 3, 195, 16, 16 },
flikerSprite_{ 37, 195, 16, 16 },
isDead{ false },
rDeadSprite_{ 71, 198, 16, 16 },
lDeadSprite_{ 89, 198, 16, 16 },
uDeadSprite_{ 105, 198, 16, 16 },
dDeadSprite_{ 122, 198, 16, 16 },
scale_{1},
pacmanPos_{},
pacmanDir_{},
haveKillPacman_{ false },
stopMoving_{ false },
timePauseElapsed_{0}
{
    scale_ = SPRITE_SCALE * lvl_->scale_;
    previousPosition_ = startPoint;
    currentPosition_ = startPoint;
    initNodesGrid();
}

Ghost::~Ghost()
{}

void Ghost::draw() {
    now_ = SDL_GetTicks();
    if ((int)(now_ - time_) < msBetweenFrame_) {

        //To make a smooth transition in the tunnel
        position_.w = scale_;
        position_.h = scale_;
        if (inTransition_ > 0)
            position_.x = -1 * scale_ + inTransition_;
        else if (inTransition_ < 0)
            position_.x = 22 * scale_ + inTransition_;

        Entity::draw();
        return;
    }
    else if (stopMoving_ && now_ - timePauseElapsed_ < TIME_PAUSE_DEAD) {
        sprite_ = previousSprite_;
        movingAnimation();
        Entity::draw();
        return;
    }
    else if ((stopMoving_ && now_ - timePauseElapsed_ >= TIME_PAUSE_DEAD))
        return;

    time_ = now_;

    int y = (position_.y - lvl_->marginTop_) / scale_;
    int x = position_.x / scale_;

    //Find a target if no target left and not in the home
    if (directions_.empty() && !isHome_) {
        if (state_ == Scared) {
            directions_ = randomDirection(previousPosition_);
        }
        else {
            if (goOnPatrol_)
                directions_ = moveTo(patrolPoint_);
            else
                directions_ = findAPath({ pacmanPos_->first / scale_, (pacmanPos_->second - lvl_->marginTop_) / scale_ });
        }
        if (!directions_.empty()) {
            std::reverse(directions_.begin(), directions_.end());
            previousPosition_ = currentPosition_;
            currentPosition_ = directions_.back();
        }
    }

    //Move the ghost throught all the target in the list directions_
    std::pair<int, int> next;
    if (!directions_.empty()) {
        next = directions_.back();
        //Usefull for the transition from one tunnel to another
        //The current and previous can't be the same tunnel
        std::shared_ptr<Node> n = nodesGrid_[next];
        std::shared_ptr<Node> previousN = nodesGrid_[currentPosition_];
        if (n->isTunnel && previousN->isTunnel) {
            //This bloc is for the transition animation
            //Reset the scale by default each time (because SDL_BlitScaled change it when the sprite goes out)

            position_.w = scale_;
            position_.h = scale_;

            if (inTransition_ != 0) {
                if (n->coord.first < previousN->coord.first) {
                    position_.x = (next.first - 1) * scale_ + inTransition_;
                    inTransition_++;
                }
                else {
                    position_.x = (next.first + 1) * scale_ + inTransition_;
                    inTransition_--;
                }
                if (position_.x == next.first * scale_)
                    inTransition_ = 0;
            }
            else if (n->coord.first < previousN->coord.first) {
                next.first = currentPosition_.first + 1;
                if (next.first == x) {
                    position_.x = (directions_.back().first - 1) * scale_;
                    inTransition_ = 1;
                }
            }
            else {
                //For the other direction;
                next.first = currentPosition_.first - 1;
                //Use to know if are in a transition animation when the root of sprite has to be outside
                inTransition_--;
                position_.x = currentPosition_.first * scale_ + inTransition_;
                if (next.first * scale_ == position_.x) {
                    position_.x = (directions_.back().first + 1) * scale_;
                    inTransition_ = 1;
                }
            }
        }
    }
    else if (isHome_ && directions_.empty()){
        next.first = x;
        if (startPoint_.second == y)
            next.second = y - 1;
        else
            next = startPoint_;
        directions_.push_back(next);
    }
    else {
        next.first = x;
        next.second = y;
    }

    //Move
    std::pair<int, int> nextScaled = { next.first * scale_, next.second * scale_ };

    if (nextScaled.first > position_.x) {
        sprite_ = changeSprite(Right);
        position_.x++;
    }
    else if (nextScaled.second > position_.y - lvl_->marginTop_) {
        sprite_ = changeSprite(Down);
        position_.y++;
    }
    else if (nextScaled.first < position_.x) {
        sprite_ = changeSprite(Left);
        position_.x--;
    }
    else if (nextScaled.second < position_.y - lvl_->marginTop_) {
        sprite_ = changeSprite(Up);
        position_.y--;
    }
    else {
        positionOnGrid_.x = x;
        positionOnGrid_.y = y;
        size_t size = directions_.size();
        if (size > 0) {
            directions_.pop_back();
        }

        if (goOnPatrol_ && now_ - timeElapsed_ >= timeToBeOnPatrol_) {
            goOnPatrol_ = false;
            timeElapsed_ = now_;
            timeElapsedBeforePatrol_ = now_;
        }

        if (getState() == Dead && directions_.empty()) {
            setDeadState(false);
            //Path to the enter of the home. All this have for goal to avoid Scared mode random direction when going outside
            directions_ = aStar({ positionOnGrid_.x, positionOnGrid_.y }, { 10,10 });
            if (directions_.size() != 0)
                std::reverse(directions_.begin(), directions_.end());
            goOutOfHome_ = true;
            sprite_ = u_sprite_;
        }
        else
            sprite_ = previousSprite_;

        if (goOutOfHome_ && directions_.empty()) {
            isHome_ = false;
            timeElapsed_ = now_;
            goOnPatrol_ = true;
            goOutOfHome_ = false;
            //To avoid turning right at the begining
            previousPosition_ = { 11,10 };
            currentPosition_ = { 10,10 };
        }
        else
        {
            previousPosition_ = currentPosition_;
            currentPosition_ = next;
        }
    }

    collisionDetectorWithPacman();

    previousSprite_ = sprite_;

    movingAnimation();

    Entity::draw();
    timeManager();
}

void Ghost::timeManager() {
    //Time manager for action with duration
    if (goOnPatrol_ || isHome_)
        timeElapsedBeforePatrol_ = now_;

    //After the timer and if we are not in patrol and if we didn't have the 3 waves already
    if (!goOnPatrol_ && patrolCounter_ < 3 && now_ - timeElapsedBeforePatrol_ >= TIME_BEFOR_PATROL && inTransition_ == 0) {
        goOnPatrol_ = true;
        directions_.clear();
        timeElapsed_ = now_;
        patrolCounter_++;
        //After the seconde time in patrol mode, the time is reduce
        if (patrolCounter_ > 1)
            timeToBeOnPatrol_ = 5000;
    }

    if (isHome_ && waitingTime_ <= now_ - timeElapsed_ && directions_.empty()) {
        //Path  to the enter of the home. All this have for goal to avoid Scared mode random direction when going outside
        directions_ = aStar({ positionOnGrid_.x, positionOnGrid_.y }, { 10,10 });
        goOutOfHome_ = true;
        if (directions_.size() != 0)
            std::reverse(directions_.begin(), directions_.end());
    }

    if (getState() == Scared) {
        if (now_ - timeElapsedVulnerable_ >= (timeToBeVulnerable_ + timeToFlicker_))
            setVulnerableState(false);
    }
}

void Ghost::movingAnimation() {
    //We change between 2 sprites to do a move animation
    //There is no animation when a ghost is dead
    //Each 15 frames we change the sprite for 15 frames for a speed of 60
    unsigned int timing = (actualSpeed_ / 4) * msBetweenFrame_;
    if (getState() != Dead && (int)(now_ - timeMoveAnimation_) >= timing) {
        sprite_.x += SPRITE_SCALE * 2 + 1;
        if (now_ - timeMoveAnimation_ >= timing * 2)
            timeMoveAnimation_ = now_;
    }
}

//Pathfinding

std::vector<std::pair<int, int>> Ghost::findAPath(std::pair<int, int> destCoord) {
    std::vector<std::pair<int, int>> path{};
    switch (getName())
    {
    case Blinky:
        path = findAPathBlinky(destCoord);
        break;
    case Pinky:
        path = findAPathPinky(destCoord);
        break;
    case Inky:
        path = findAPathInky(destCoord);
        break;
    case Clyde:
        path = findAPathClyde(destCoord);
        break;
    }
    return path;
}

std::vector<std::pair<int, int>> Ghost::moveTo(std::pair<int, int> dest) {
    int x = positionOnGrid_.x, y = positionOnGrid_.y;
    double min = FLT_MAX;
    Direction dir = Left;
    if (previousPosition_.first != x+1)
        chooseDirection(dest, x, y, 1, 0, min, dir, Right);
    if (previousPosition_.second != y + 1)
        chooseDirection(dest, x, y, 0, 1, min, dir, Down);
    if (previousPosition_.first != x - 1)
        chooseDirection(dest, x, y, -1, 0, min, dir, Left);
    if (previousPosition_.second != y - 1 && (lvl_->lvl1Grid_[y][x] != U && lvl_->lvl1Grid_[y][x] != U))
        chooseDirection(dest, x, y, 0, -1, min, dir, Up);
    std::vector<std::pair<int, int>> path = toDirection(dir);

    return path;
}

void Ghost::chooseDirection(std::pair<int, int> dest, int x, int y, int incrX, int incrY, double &min, Direction &dir, Direction newDir) {
    if (lvl_->lvl1Grid_[y + incrY][x + incrX] != W && lvl_->lvl1Grid_[y + incrY][x + incrX] != H) {
        int dx = dest.first - (x + incrX);
        int dy = dest.second - (y + incrY);

        if (dx == dy && dx == 0) {
            min = 0;
            dir = newDir;
        }

        double score = sqrt(dx * dx + dy * dy);
        if (score <= min) {
            min = score;
            dir = newDir;
        }
    }
}

Direction Ghost::getDirectionFromInt(int d) {
    switch (d)
    {
    case 0:
        return Up;
    case 1:
        return Right;
    case 2:
        return Down;
    case 3:
        return Left;
    default:
        return Up;
    }
}

//List all the cell to travel in a direction until the next intersection
std::vector<std::pair<int, int>> Ghost::toDirection(Direction dir) {
    std::pair<int, int> pos{ positionOnGrid_.x, positionOnGrid_.y };
    std::shared_ptr<Node> n = nodesGrid_[pos];

    std::vector<std::pair<int, int>>directions{};
    std::vector<std::shared_ptr<Node>> neighbors{ n->neighbors };

    Direction sourceDir = getDirectionFromInt((dir + 2)%4);

    int size = 1;

    while (size == 1)
    {
        bool goodDir = false;
        Direction tmpDir = dir;
        for (int i = 0; i < 4; i++) {
            if (neighbors[i] != nullptr && i != sourceDir && (!n->cantGoUp || i != Up)) {
                std::pair<int, int> coord = neighbors[i]->coord;
                if (lvl_->lvl1Grid_[coord.second][coord.first] == H)
                    continue;
                if (i == dir)
                    goodDir = true;
                else
                    tmpDir = getDirectionFromInt(i);
            }
        }
        if (!goodDir)
            dir = tmpDir;
        sourceDir = getDirectionFromInt((dir + 2) % 4);
        n = neighbors[dir];
        directions.push_back(n->coord);
        neighbors = n->neighbors;

        size = 0;
        for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
            if (*it != nullptr && *it != neighbors[sourceDir] && (!n->cantGoUp || *it != neighbors[Up])) {
                std::pair<int, int> coord = (*it)->coord;
                if (lvl_->lvl1Grid_[coord.second][coord.first] == H)
                    continue;
                size++;
            }
        }
    }
    return directions;
}

void Ghost::addToPath(std::vector<std::pair<int, int>>& path, int d1, int d2, int incrX, int incrY, std::shared_ptr<Lvl> lvl, int x, int y, bool second = false) {
    if (d1 < 0 && lvl->lvl1Grid_[y - incrY][x - incrX] != W && lvl->lvl1Grid_[y - incrY][x - incrX] != H && !(incrY == 1 && (lvl->lvl1Grid_[y][x] == U || lvl->lvl1Grid_[y][x] == V))) {
        std::pair<int, int> p{ x - incrX, y - incrY};
        path.push_back(p);
    }
    else if (d1 > 0 && lvl->lvl1Grid_[y + incrY][x + incrX] != W && lvl->lvl1Grid_[y + incrY][x + incrX] != H) {
        std::pair<int, int> p{ x + incrX, y + incrY };
        path.push_back(p);
    }
    else if (!second)
        addToPath(path, d2, d1, abs(incrX - 1), abs(incrY - 1), lvl, x, y, true);
}

std::vector<std::pair<int, int>> Ghost::findAPathInky(std::pair<int, int> destCoord) {
    std::pair<int, int> dest{};
    switch (*pacmanDir_)
    {
    case Up:
        destCoord.second -= 2;
        //No break to recreate the bug with pacman facing Up
    case Left:
        destCoord.first -= 2;
        break;
    case Right:
        destCoord.first += 2;
        break;
    case Down:
        destCoord.second += 2;
        break;
    }
    dest.first = 2* destCoord.first - blinkyForInky_->positionOnGrid_.x;
    dest.second = 2 * destCoord.second - blinkyForInky_->positionOnGrid_.y;
    return moveTo(dest);
}

std::vector<std::pair<int, int>> Ghost::findAPathClyde(std::pair<int, int> destCoord) {
    if (!pacmanIsNear())
        return moveTo(destCoord);
    return moveTo(patrolPoint_);
}

bool Ghost::pacmanIsNear() {
    std::pair<int, int> dest = { pacmanPos_->first / scale_, (pacmanPos_->second - lvl_->marginTop_) / scale_ };
    return abs(dest.first - positionOnGrid_.x) + abs(dest.second - positionOnGrid_.y) <= 8;
}

std::vector<std::pair<int, int>> Ghost::findAPathPinky(std::pair<int, int> destCoord) {
    switch (*pacmanDir_)
    {
    case Up:
        destCoord.second -= 4;
        //No break to recreate the bug with pacman facing Up
    case Left:
        destCoord.first -= 4;
        break;
    case Right:
        destCoord.first += 4;
        break;
    case Down:
        destCoord.second += 4;
        break;
    }
    return moveTo(destCoord);
}

std::vector<std::pair<int, int>> Ghost::findAPathBlinky(std::pair<int, int> destCoord) {
    return moveTo(destCoord);
}

//Use for A*

bool Ghost::isClose(std::vector<std::shared_ptr<Node>> close, std::shared_ptr<Node> n) {
    for (auto it = close.begin(); it != close.end(); it++)
        if (n == *it)
            return true;
    return false;
}

std::vector<std::pair<int, int>> Ghost::convertToPath(std::shared_ptr<Node> n) {
    std::vector<std::pair<int, int>> path{};
    while (n->parent != n)
    {
        path.push_back(n->coord);
        n = n->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<std::pair<int, int>> Ghost::aStar(std::pair<int, int> startCoord, std::pair<int, int> destCoord) {
    std::vector<std::shared_ptr<Node>> open{};
    std::vector<std::shared_ptr<Node>> close{};

    //Initialisation of the first step in the path (start)
    std::shared_ptr<Node> current{ nodesGrid_[startCoord] };
    current->f = 0;
    current->h = 0;
    current->g = 0;
    current->parent = current;
    open.push_back(current);

    //start of the algo
    while (!open.empty()) {
        unsigned int tmp = UINT_MAX;
        std::vector<std::shared_ptr<Node>>::iterator itNode;
        for (auto it = open.begin(); it != open.end(); it++) {
            std::shared_ptr<Node> n = *it;
            if (n->f < tmp) {
                tmp = n->f;
                itNode = it;
            }
        }
        current = *itNode;
        open.erase(itNode);
        close.push_back(current);

        for (auto it = current->neighbors.begin(); it != current->neighbors.end(); it++) {
            //If we can't go up and the neighbour is the up case, we ignore it
            if ((current->cantGoUp && (*it) == current->neighbors[Up]) || *it == nullptr)
                continue;
            unsigned int gNew, hNew, fNew;
            std::shared_ptr<Node> n{ *it };
            std::pair<int, int> coord{ n->coord };
            if (destCoord == coord) {
                nodesGrid_[coord]->parent = current;
                auto path = convertToPath(nodesGrid_[coord]);
                reinitNodesGridScores();
                return path;
            }
            else if (!isClose(close, n)) {
                gNew = current->g + 1;
                hNew = n->distance(*nodesGrid_[destCoord]);
                fNew = gNew + hNew;
                if (n->f > fNew) {
                    n->f = fNew;
                    n->g = gNew;
                    n->h = hNew;
                    n->parent = current;
                    open.push_back(n);
                }
            }
        }
    }
    std::cout << "No solution" << std::endl;
    reinitNodesGridScores();
    return std::vector<std::pair<int, int>>{};
}

void Ghost::initNodesGrid() {

    std::vector<std::vector<int>>::iterator itRow = lvl_->lvl1Grid_.begin();

    int x = 0, y = 0;
    for (itRow; itRow < lvl_->lvl1Grid_.end(); itRow++)
    {
        std::vector<int>::iterator itCol = (*itRow).begin();
        for (itCol; itCol < (*itRow).end(); itCol++)
        {
            Node n{ x,y };
            n.isWall = (*itCol == W);
            n.isTunnel = (*itCol == T);
            n.neighbors = { nullptr, nullptr, nullptr, nullptr };
            nodesGrid_[{x, y}] = std::make_shared<Node>(n);
            x++;
        }
        x = 0;
        y++;
    }

    std::map<std::pair<int, int>, std::shared_ptr<Node>>::iterator it = nodesGrid_.begin();

    int xMax = (int)lvl_->lvl1Grid_.front().size();
    int yMax = (int)lvl_->lvl1Grid_.size();

    while (it != nodesGrid_.end())
    {
        std::shared_ptr<Node> n = it->second;
        //Ignore the wall
        if (n->isWall) {
            it = nodesGrid_.erase(it);
            continue;
        }

        int x = it->first.first, y = it->first.second;

        if (n->isTunnel && x + 1 > xMax - 1) {
            n->neighbors[Right] = nodesGrid_[{0, y}];
            nodesGrid_[{0, y}]->neighbors[Left] = n;
        }
        else if (x + 1 <= xMax - 1 && !nodesGrid_[{x + 1, y}]->isWall) {
            n->neighbors[Right] = nodesGrid_[{x + 1, y}];
            nodesGrid_[{x + 1, y}]->neighbors[Left] = n;
        }
        if (y + 1 <= yMax - 1 && !nodesGrid_[{x, y + 1}]->isWall) {
            if (lvl_->lvl1Grid_[y + 1][x] == U || lvl_->lvl1Grid_[y + 1][x] == V)
                nodesGrid_[{x, y + 1}]->cantGoUp = true;
            n->neighbors[Down] = nodesGrid_[{x, y + 1}];
            nodesGrid_[{x, y + 1}]->neighbors[Up] = n;
        }
        it++;
    }
}

void Ghost::reinitNodesGridScores() {
    std::map<std::pair<int, int>, std::shared_ptr<Node>>::iterator it = nodesGrid_.begin();

    while (it != nodesGrid_.end())
    {
        std::shared_ptr<Node> n = it->second;
        n->f = UINT_MAX;
        n->h = UINT_MAX;
        n->g = UINT_MAX;
        n->parent = nullptr;
        it++;
    }
}

//Set

SDL_Rect Ghost::changeSprite(int spriteId) {
    if (getState() == Scared && now_ - timeElapsedVulnerable_ < (timeToBeVulnerable_ + timeToFlicker_)) {
        //Each 15 frames we change the sprite for 15 frames for a speed of 60 (To do the white flicker of the ghost)
        unsigned int timing = (actualSpeed_ / 4) * msBetweenFrame_;
        if (now_ - timeElapsedVulnerable_ >= timeToBeVulnerable_ && now_ - timeFlickerAnimation_ >= timing) {
            if(now_ - timeFlickerAnimation_ >= timing * 2)
                timeFlickerAnimation_ = now_;
            return flikerSprite_;
        }
        else
            return vulnerableSprite_;
    }
    switch (spriteId){
        case Up:
            if (getState() == Dead)
                return uDeadSprite_;
            return u_sprite_;
        case Right:
            if (getState() == Dead)
                return rDeadSprite_;
            return r_sprite_;
        case Down:
            if (getState() == Dead)
                return dDeadSprite_;
            return d_sprite_;
        case Left:
            if (getState() == Dead)
                return lDeadSprite_;
            return l_sprite_;
        default:
            if (getState() == Dead)
                return rDeadSprite_;
            return r_sprite_;
    }
}

void Ghost::setVulnerableState(bool vulnerable) {

    //Can't be vulnerable if he's dead
    if (getState() == Dead || inTransition_ || (lvl_->lvl1Grid_[positionOnGrid_.y][positionOnGrid_.x] == H && !isHome_))
        return;

    bool wasVulnerable = getState() == Scared;

    if (vulnerable) {
        setState(Scared);
        changeSpeed((int)(defaultSpeed_ / 1.5f));
    }
    else {
        setState(Normal);
        resetSpeed();
    }

    //Desactivation of all timing actions (without housing and vulnerable)
    timeElapsedVulnerable_ = now_;
    timeElapsedBeforePatrol_ = now_;
    if ((vulnerable && !wasVulnerable) || !vulnerable) {
        goOnPatrol_ = false;
        lvl_->display();
        if (!isHome_ && vulnerable && !wasVulnerable) {
            if (directions_.size() != 0)
                previousPosition_ = directions_.back();
            directions_.clear();
        }
    }
}

std::vector<std::pair<int, int>> Ghost::randomDirection(std::pair<int, int> previousCoord) {
    std::pair<int, int> pos{ positionOnGrid_.x, positionOnGrid_.y };
    std::shared_ptr<Node> n = nodesGrid_[pos];

    std::vector<std::pair<int, int>>directions{};

    std::random_device rd;
    std::mt19937 gen{ rd() };
    bool first = true;
    std::vector<std::shared_ptr<Node>> neighbors{ n->neighbors };

    int size = 0;
    for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
        if (*it != nullptr)
            size++;
    }

    while (first || size < 3)
    {
        auto it = neighbors.begin();
        while (it != neighbors.end())
        {
            if (*it != nullptr) {
                std::pair<int, int> coord{ (*it)->coord };
                if (coord == previousCoord || lvl_->lvl1Grid_[coord.second][coord.first] == H) {
                    it = neighbors.erase(it);
                    continue;
                }
            }
            else {
                it = neighbors.erase(it);
                continue;
            }
            it++;
        }
        std::uniform_int_distribution<> dis{ 0, (int)neighbors.size() - 1 };
        int index = dis(gen);
        previousCoord = n->coord;
        n = neighbors[index];
        directions.push_back(n->coord);
        neighbors = n->neighbors;
        first = false;
        size = 0;
        for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
            if (*it != nullptr) {
                std::pair<int, int> coord{ (*it)->coord };
                if(coord != previousCoord && lvl_->lvl1Grid_[coord.second][coord.first] != H)
                    size++;
            }
        }
    }
    return directions;
}

//Dead state

void Ghost::setDeadState(bool isDead) {
    //Can be kill only when scared
    if (isDead && getState() == Scared) {
        setState(Dead);
        changeSpeed(500);
    }
    else if (!isDead) {
        setState(Normal);
        resetSpeed();
    }
    else
        return;

    //Desactivation of all timing actions (without housing and vulnerable)
    timeElapsed_ = now_;
    timeElapsedBeforePatrol_ = now_;
    goOnPatrol_ = false;
    directions_.clear();

    if (isDead) {
        std::pair<int, int> dest = startPoint_;
        if (getName() == Blinky)
            dest.second += 2;
        //Use the A* algo for all when the ghost is dead
        directions_ = aStar({ positionOnGrid_.x, positionOnGrid_.y }, dest);

        if (!directions_.empty()) {
            std::reverse(directions_.begin(), directions_.end());
            previousPosition_ = currentPosition_;
            currentPosition_ = directions_.back();
        }
    }
}

void Ghost::collisionDetectorWithPacman() {
    //No collision with a dead ghost
    if (getState() == Dead)
        return;

    int spriteScaleHalf = SPRITE_SCALE / 2 * lvl_->scale_;
    std::pair<int, int> destPlus = {
        pacmanPos_->first + spriteScaleHalf,
        pacmanPos_->second + spriteScaleHalf
    };
    std::pair<int, int> destMinus = {
        pacmanPos_->first - spriteScaleHalf,
        pacmanPos_->second - spriteScaleHalf
    };
    bool inXRange = position_.x <= destPlus.first && position_.x >= destMinus.first;
    bool inYRange = position_.y <= destPlus.second && position_.y >= destMinus.second;
    if (inXRange && inYRange) {
        //Kill the ghost
        if (getState() == Scared) {
            setDeadState();
            ++lvl_->eaten_ghosts_;
            lvl_->score_ += int(pow(2, lvl_->eaten_ghosts_))*100;
        }
            
        //Kill pacman
        else
            haveKillPacman_ = true;
    }
}