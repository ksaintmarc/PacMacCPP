#ifndef GHOST_H
#define GHOST_H

#define TIME_BEFOR_PATROL 20000 //20s
#define TIME_PAUSE_DEAD 1500 //2s

#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <climits>

#include <SDL.h>

#include "entity.h"
#include "lvl.h"

class Ghost : public Entity
{
    struct Node {
        std::pair<int, int> coord; // position ine the lvl grid
        unsigned int f, h, g;
        bool isWall;
        bool isTunnel;
        bool cantGoUp;
        std::vector<std::shared_ptr<Node>> neighbors;
        std::shared_ptr<Node> parent;

        // constructeur
        Node() :
            Node{ 0, 0 }
        {}

        Node(int x, int y) :
            coord{ x, y },
            f{ UINT_MAX },
            h{ UINT_MAX },
            g{ UINT_MAX },
            isWall{ true },
            isTunnel{ false },
            cantGoUp{ false },
            neighbors{}
        {}

        // distance
        unsigned int distance(Node other) {
            //Euclidian
            int dx = coord.first - other.coord.first;
            int dy = coord.second - other.coord.second;
            return  dx * dx + dy * dy;
        }
    };

public:

    Ghost();
    Ghost(std::shared_ptr<Lvl> lvl, int speed, SDL_Surface* plancheSprites, SDL_Surface* winSurf);
    Ghost(EntityType type, State state, SDL_Rect r_sprite, SDL_Rect l_sprite, SDL_Rect u_sprite, SDL_Rect d_sprite, std::pair<int, int> startPoint, std::shared_ptr<Lvl> lvl, std::pair<int, int> patrolPoint, unsigned int waitingTime, int speed, SDL_Surface* plancheSprites, SDL_Surface* winSurf, std::shared_ptr<Ghost> blinkyForInky=nullptr);
    ~Ghost();

    void draw();

    //Vulnerable state

    void setVulnerableState(bool vulnarable = true);

    //Dead state

    void setDeadState(bool isDead = true);

    //Pac-Man

    inline void setPacmanPos(std::shared_ptr <std::pair<int, int>> pos, std::shared_ptr <Direction> dir) { pacmanPos_ = pos; pacmanDir_ = dir; };
    inline bool isPacmanKilled() { return haveKillPacman_; };

    //End

    inline void setStopMoving(bool stopMoving = true) { stopMoving_ = stopMoving; if(stopMoving_)timePauseElapsed_ = now_; };

private:
    int scale_;
    std::pair<int, int> startPoint_;
    std::vector<std::pair<int, int>> directions_;
    std::pair<int, int> currentPosition_;
    std::pair<int, int> previousPosition_;
    //For tunnel travelling
    int inTransition_;

    //Pathfinding

    std::vector<std::pair<int, int>> findAPath(std::pair<int, int> destCoord);
    std::vector<std::pair<int, int>> findAPathBlinky(std::pair<int, int> destCoord);
    std::vector<std::pair<int, int>> findAPathPinky(std::pair<int, int> destCoord);
    std::vector<std::pair<int, int>> findAPathInky(std::pair<int, int> destCoord);
    std::vector<std::pair<int, int>> findAPathClyde(std::pair<int, int> destCoord);

    std::shared_ptr<Ghost> blinkyForInky_;

    std::vector<std::pair<int, int>> moveTo(std::pair<int, int> dest);
    Direction getDirectionFromInt(int d);
    std::vector<std::pair<int, int>> toDirection(Direction destDir);
    void chooseDirection(std::pair<int, int> dest, int x, int y, int incrX, int incrY, double& min, Direction& dir, Direction newDir);

    bool isClose(std::vector<std::shared_ptr<Node>> close, std::shared_ptr<Node> n);
    std::vector<std::pair<int, int>> convertToPath(std::shared_ptr<Node> n);
    std::vector<std::pair<int, int>> aStar(std::pair<int, int> startCoord, std::pair<int, int> destCoord);
    void addToPath(std::vector<std::pair<int, int>>& path, int d1, int d2, int incrX, int incrY, std::shared_ptr<Lvl> lvl, int x, int y, bool second);
    bool pacmanIsNear();

    std::map<std::pair<int, int>, std::shared_ptr<Node>> nodesGrid_;
    void initNodesGrid();
    void reinitNodesGridScores();

    //Time
    Uint32 timePauseElapsed_;
    void timeManager();

    //House
    bool isHome_;
    bool goOutOfHome_;
    unsigned int waitingTime_;

    //Patrol
    bool goOnPatrol_;
    unsigned int timeToBeOnPatrol_;
    std::pair<int, int> patrolPoint_;
    int patrolCounter_;
    Uint32 timeElapsedBeforePatrol_;

    //Vulnerable state
    unsigned int timeToBeVulnerable_;
    unsigned int timeToFlicker_;
    Uint32 timeFlickerAnimation_;
    Uint32 timeElapsedVulnerable_;
    SDL_Rect vulnerableSprite_;
    SDL_Rect flikerSprite_;
    SDL_Rect changeSprite(int spriteId);
    std::vector<std::pair<int, int>> randomDirection(std::pair<int, int> previousCoord);

    //Dead state
    bool isDead;
    SDL_Rect rDeadSprite_;
    SDL_Rect lDeadSprite_;
    SDL_Rect uDeadSprite_;
    SDL_Rect dDeadSprite_;
    void collisionDetectorWithPacman();

    //Animation
    void movingAnimation();

    //Pac-Man
    std::shared_ptr<std::pair<int, int>> pacmanPos_;
    std::shared_ptr<Direction> pacmanDir_;
    bool haveKillPacman_;

    //End
    bool stopMoving_;
};

#endif // GHOST_H
