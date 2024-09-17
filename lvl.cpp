#include "lvl.h"

Lvl::Lvl():
	srcBg_{ 0,0,0,0 },
    scale_{ 4 },
	marginTop_{ 20 * scale_ },
	dotSprite_{1, 78, 8, 8},
	pelletSprite_{ 8,78,8,8 },
	pos_{ 0, 20 * scale_, 0, 0 },
	lives_{2},
	eaten_dots_{ 0 },
	eaten_ghosts_{ 0 },
	eaten_bonuses_{ 0 },
	score_{0},
	lvl1Grid_{ {
		{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
		{W,D,D,D,D,D,D,D,D,D,W,D,D,D,D,D,D,D,D,D,W},
		{W,D,W,W,W,D,W,W,W,D,W,D,W,W,W,D,W,W,W,D,W},
		{W,S,W,W,W,D,W,W,W,D,W,D,W,W,W,D,W,W,W,S,W},
		{W,D,W,W,W,D,W,W,W,D,W,D,W,W,W,D,W,W,W,D,W},
		{W,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,W},
		{W,D,W,W,W,D,W,D,W,W,W,W,W,D,W,D,W,W,W,D,W},
		{W,D,W,W,W,D,W,D,W,W,W,W,W,D,W,D,W,W,W,D,W},
		{W,D,D,D,D,D,W,D,D,D,W,D,D,D,W,D,D,D,D,D,W},
		{W,W,W,W,W,D,W,W,W,P,W,P,W,W,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,P,U,P,U,P,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,W,W,H,W,W,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,W,H,H,H,W,P,W,D,W,W,W,W,W},
		{T,P,P,P,P,D,P,P,W,H,H,H,W,P,P,D,P,P,P,P,T},
		{W,W,W,W,W,D,W,P,W,W,W,W,W,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,P,P,F,P,P,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,W,W,W,W,W,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,W,W,W,W,W,P,W,D,W,W,W,W,W},
		{W,D,D,D,D,D,D,D,D,D,W,D,D,D,D,D,D,D,D,D,W},
		{W,D,W,W,W,D,W,W,W,D,W,D,W,W,W,D,W,W,W,D,W},
		{W,S,D,D,W,D,D,D,D,V,P,V,D,D,D,D,W,D,D,S,W},
		{W,W,W,D,W,D,W,D,W,W,W,W,W,D,W,D,W,D,W,W,W},
		{W,W,W,D,W,D,W,D,W,W,W,W,W,D,W,D,W,D,W,W,W},
		{W,D,D,D,D,D,W,D,D,D,W,D,D,D,W,D,D,D,D,D,W},
		{W,D,W,W,W,W,W,W,W,D,W,D,W,W,W,W,W,W,W,D,W},
		{W,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,W},
		{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}
	} },
	dot_num{192},
	finish_{ false },
	timeCycleFliker_{ 0 },
	timeElapsedFlicker_{ 250 },
	highScore_{0},
	winSurf_{ nullptr },
	plancheSprites_{ nullptr },
	endgame_{false}
{
}

Lvl::Lvl(SDL_Rect srcBg, int scale, int marginTop, int highScore, Overlay overlay, SDL_Surface* plancheSprites, SDL_Surface* winSurf) :
	srcBg_{srcBg},
	scale_{ scale },
	marginTop_{ marginTop },
	pos_{ 0, marginTop, 0, 0 },
	dotSprite_{ 1, 78, 8, 8 },
	pelletSprite_{ 8,78,8,8 },
	overlay_{overlay},
	lives_{2},
	score_{0},
	eaten_dots_{ 0 },
	eaten_ghosts_{ 0 },
	eaten_bonuses_{0},
	lvl1Grid_ {
		{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
		{W,D,D,D,D,D,D,D,D,D,W,D,D,D,D,D,D,D,D,D,W},
		{W,D,W,W,W,D,W,W,W,D,W,D,W,W,W,D,W,W,W,D,W},
		{W,S,W,W,W,D,W,W,W,D,W,D,W,W,W,D,W,W,W,S,W},
		{W,D,W,W,W,D,W,W,W,D,W,D,W,W,W,D,W,W,W,D,W},
		{W,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,W},
		{W,D,W,W,W,D,W,D,W,W,W,W,W,D,W,D,W,W,W,D,W},
		{W,D,W,W,W,D,W,D,W,W,W,W,W,D,W,D,W,W,W,D,W},
		{W,D,D,D,D,D,W,D,D,D,W,D,D,D,W,D,D,D,D,D,W},
		{W,W,W,W,W,D,W,W,W,P,W,P,W,W,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,P,U,P,U,P,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,W,W,H,W,W,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,W,H,H,H,W,P,W,D,W,W,W,W,W},
		{T,P,P,P,P,D,P,P,W,H,H,H,W,P,P,D,P,P,P,P,T},
		{W,W,W,W,W,D,W,P,W,W,W,W,W,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,P,P,F,P,P,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,W,W,W,W,W,P,W,D,W,W,W,W,W},
		{W,W,W,W,W,D,W,P,W,W,W,W,W,P,W,D,W,W,W,W,W},
		{W,D,D,D,D,D,D,D,D,D,W,D,D,D,D,D,D,D,D,D,W},
		{W,D,W,W,W,D,W,W,W,D,W,D,W,W,W,D,W,W,W,D,W},
		{W,S,D,D,W,D,D,D,D,V,P,V,D,D,D,D,W,D,D,S,W},
		{W,W,W,D,W,D,W,D,W,W,W,W,W,D,W,D,W,D,W,W,W},
		{W,W,W,D,W,D,W,D,W,W,W,W,W,D,W,D,W,D,W,W,W},
		{W,D,D,D,D,D,W,D,D,D,W,D,D,D,W,D,D,D,D,D,W},
		{W,D,W,W,W,W,W,W,W,D,W,D,W,W,W,W,W,W,W,D,W},
		{W,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,W},
		{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}
	},
	dot_num{ 192 },
	finish_{ false },
	timeElapsedFlicker_{0},
	timeCycleFliker_{250}, //0.25s
	highScore_{highScore},
	winSurf_{ winSurf },
	plancheSprites_{ plancheSprites },
	endgame_ {false}
{
}

Lvl::~Lvl()
{
}

void Lvl::display() {
	int pos_scale = 8 * scale_;

	int size = 9 * (scale_ - 1);
	std::string msg{ "High Score" };
	int middle = (int)(msg.length() * size) / 2;
	std::string highScore = std::to_string(highScore_);
	int middleHighScore = (int)(highScore.length() * size) / 2;

	overlay_.write({ 5 * scale_,5 * scale_,8 * scale_,8 * scale_ }, std::to_string(score_));
	overlay_.write({ srcBg_.w * scale_ / 2 - middle, 3 * scale_, size , size}, msg);
	overlay_.write({ srcBg_.w * scale_ / 2 - middleHighScore, 12 * scale_, size , size }, highScore);
	overlay_.lives({ 0,27 * pos_scale + marginTop_,pos_scale,pos_scale }, lives_);
}

void Lvl::flickerAnimation() {
	Uint32 now = SDL_GetTicks();
	SDL_Rect bg_ = { 0, marginTop_, srcBg_.w * scale_, srcBg_.h * scale_ };
	SDL_Rect srcBg{ srcBg_ };
	SDL_SetColorKey(plancheSprites_, false, 0);

	if ((int)(now - timeElapsedFlicker_) < timeCycleFliker_)
		SDL_BlitScaled(plancheSprites_, &srcBg, winSurf_, &bg_);
	else if ((int)(now - timeElapsedFlicker_) >= timeCycleFliker_ * 2)
		timeElapsedFlicker_ = now;
	else {
		srcBg.x = 540;
		SDL_BlitScaled(plancheSprites_, &srcBg, winSurf_, &bg_);
	}
}

void Lvl::draw() {
	int pos_scale = 8 * scale_;

	pos_.w = pos_scale;
	pos_.h = pos_scale;

	display();

	for (int i = 0; i != 27; ++i) {
		for (int j = 0; j != 21; ++j) {
			//TO DO : refacto into a function what is inside
			if (lvl1Grid_.data()[i].data()[j] == D || lvl1Grid_.data()[i].data()[j] == V) {
				pos_.x = j * pos_scale;
				pos_.y = i * pos_scale + marginTop_;
				SDL_SetColorKey(plancheSprites_, true, 0);
				SDL_BlitScaled(plancheSprites_, &dotSprite_, winSurf_, &pos_);
			}
			if (lvl1Grid_.data()[i].data()[j] == S) {
				pos_.x = j * pos_scale;
				pos_.y = i * pos_scale + marginTop_;
				SDL_SetColorKey(plancheSprites_, true, 0);
				SDL_BlitScaled(plancheSprites_, &pelletSprite_, winSurf_, &pos_);
			}
		}
	}

	finish_ = dot_num == eaten_dots_;
}