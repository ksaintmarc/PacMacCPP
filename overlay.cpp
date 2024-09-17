#include "overlay.h"

Overlay::Overlay() :
    wWindow_{ 0 },
    scale_{ 4 },
    marginTop_{ 20 * scale_ },
    alphabet_{ {} },
    numbers_{ {} },
    lifeSprite_{ 19,88, 16,16 },
	winSurf_{ nullptr },
	plancheSprites_{ nullptr }
{}

Overlay::Overlay(int wWindow, int scale, int marginTop, SDL_Surface* plancheSprites, SDL_Surface* winSurf) :
    wWindow_{ wWindow },
    scale_{ scale },
    marginTop_{ marginTop },
    alphabet_{ {} },
    numbers_{ {} },
    lifeSprite_{ 19,88, 16,16 },
    winSurf_{ winSurf },
    plancheSprites_{ plancheSprites }
{
    buildAlphabet();
    buildNumbers();
}

Overlay::~Overlay()
{}

void Overlay::buildAlphabet() {
    //A to O
    for (int i = 0; i < 15; ++i) {
        alphabet_.push_back({ (i + 1) * 8 +3, 60,8,8 });
    }
    //P to Z
    for (int i = 0; i < 11; ++i) {
        alphabet_.push_back({ i * 8 + 3, 68,8,8 });
    }
}

void Overlay::buildNumbers() {
    for (int i = 0; i < 10; ++i) {
        numbers_.push_back({ i * 8 + 3, 52,9,9 });
    }
}

void Overlay::write(SDL_Rect position, std::string message) {
    int value = 0;
    int l = (int)message.length();
    SDL_Rect sprite;

    SDL_SetColorKey(plancheSprites_, true, 0);
        for (auto ch : message) {
            if (isalnum(ch)) {
                if (isdigit(ch))
                    sprite = numbers_[(int(ch) - '0') +1 ];
                else
                    sprite = alphabet_[(int(tolower(ch)) - 'a') +1];
                SDL_BlitScaled(plancheSprites_, &sprite, winSurf_, &position);
            }
            else if (ch == '!') {
                sprite = { 100,51,9,9 };
                SDL_BlitScaled(plancheSprites_, &sprite, winSurf_, &position);
            }
            position.x += position.w;
            
        }
    SDL_SetColorKey(plancheSprites_, false, 0);
    return;
}

void Overlay::lives(SDL_Rect position, int n) {
    for (int i = 0; i < n; ++i) {
        SDL_BlitScaled(plancheSprites_, &lifeSprite_, winSurf_, &position);
        position.x += position.w;
    }
}

void Overlay::middleScreenMessage(std::string message) {
    int size = 9 * (scale_-1);
    int middle = (int)(message.length() * size) / 2;
    write({ wWindow_ / 2 - middle, 15*8*scale_ + marginTop_, size , size }, message);
}
