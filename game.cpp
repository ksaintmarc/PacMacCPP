#include "game.h"

Game::Game():
pWindow_{ nullptr },
winSurf_{ nullptr },
plancheSprites_{ nullptr },
bgSprite_{ nullptr },
wWindow_{0},
hWindow_{0},
marginTop_{0},
srcBg_{0, 0, 0, 0},
bg_{0 ,0 , 0, 0},
blinky_{},
pinky_{},
inky_{},
clyde_{},
player_{},
bonus_{},
lvl_{},
reinit_{true},
timeBeforeBegin_{2000},
timeElapsed_{0},
finish_{false},
timeElapsedFinishAnimation_{0},
timeFinishAnimation_{4000}, //4s
highScore_{0},
begin_{true}
{}

Game::~Game()
{}

void Game::init() {
	//Level coordinates and dimensions on the spriteSheet
	int xBg = 369, yBg = 3, wBg = 168, hBg = 216;
	//The window size
	wWindow_ = wBg * SCALE;
	hWindow_ = hBg * SCALE;
	//To let a space at the top of the windows to do some text display
	marginTop_ = 20 * SCALE;

	//Initialize the game windows
	pWindow_ = SDL_CreateWindow("PacMan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wWindow_, hWindow_ + marginTop_ * 2, SDL_WINDOW_SHOWN);
	winSurf_ = SDL_GetWindowSurface(pWindow_);
	plancheSprites_ = SDL_LoadBMP("./pacman_sprites.bmp");

	//Background source position on spriteSheet
	srcBg_ = { xBg, yBg, wBg, hBg };
	//Background position on the window
	bg_ = { 0, marginTop_, wWindow_, hWindow_ };

	// The background is always the same, so there is no need to rescale in all the loop. (to reduce the time need)
	// bgSprite_ allow us to store the bg with the matching scale for the destination surface with SDL_BlitScaled
	bgSprite_ = SDL_CreateRGBSurfaceWithFormat(0, winSurf_->w, winSurf_->h, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_SetColorKey(plancheSprites_, false, 0);
	SDL_BlitScaled(plancheSprites_, &srcBg_, bgSprite_, &bg_); // resize the surface to match the destination surface
	SDL_SetColorKey(plancheSprites_, true, 0);

	//Initialize the needed classes in the game
	overlay_ = Overlay{ wWindow_,SCALE,marginTop_,plancheSprites_,winSurf_};
	lvl_ = std::make_shared <Lvl>(srcBg_, SCALE, marginTop_, highScore_, overlay_, plancheSprites_, winSurf_);
	Bonus tmp_bonus{ lvl_, plancheSprites_, winSurf_ };
	bonus_ = std::make_shared<Bonus>(tmp_bonus);

	commonReinit();
}

bool Game::reinitLvl() {

	//Decrease pacman lives and end the game if inferior to 0
	--lvl_->lives_;
	if (lvl_->lives_ < 0) {
		lvl_->endgame_ = true;
		return false;
	}

	commonReinit();

	return true;
}

void Game::restart() {
	//Update the high score
	highScore_ = std::max(lvl_->score_, highScore_);
	//Reinitialize the needed classes for the game
	lvl_ = std::make_shared <Lvl>(srcBg_, SCALE, marginTop_, highScore_, overlay_, plancheSprites_, winSurf_);
	Bonus tmp_bonus{ lvl_, plancheSprites_, winSurf_ };
	bonus_ = std::make_shared<Bonus>(tmp_bonus);

	commonReinit();

	//To allow the reinitilisation after a death
	reinit_ = true;
	//To know if the game is finish
	finish_ = false;
}

void Game::commonReinit() {
	blinky_ = std::make_shared<Ghost>(lvl_, SPEED, plancheSprites_, winSurf_);

	//I don't know why, but I had to create a tmp value, because make_shared do not match the argument
	Ghost tmp{ Pinky, Normal, { 3,141, 16,16 }, { 37,141, 16,16 }, { 71,141, 16,16 }, { 105,141, 16,16 }, { 10 , 13 }, lvl_, {2,-2}, 500, SPEED, plancheSprites_, winSurf_ };
	pinky_ = std::make_shared<Ghost>(tmp);

	tmp = Ghost{ Inky, Normal, {3,159, 16,16}, {37,159, 16,16}, {71,159, 16,16}, {105,159, 16,16}, {9, 13}, lvl_, {21,29}, 7000, SPEED, plancheSprites_, winSurf_, blinky_ };
	inky_ = std::make_shared<Ghost>(tmp);

	tmp = Ghost{ Clyde, Normal, {3,177, 16,16}, {37,177, 16,16}, {71,177, 16,16}, {105,177, 16,16}, {11, 13}, lvl_, {0,29}, 13000, SPEED, plancheSprites_, winSurf_ };
	clyde_ = std::make_shared<Ghost>(tmp);

	player_ = Player{ lvl_, SPEED, blinky_, pinky_, inky_, clyde_ , bonus_, plancheSprites_, winSurf_ };

	begin_ = true;
	timeElapsed_ = SDL_GetTicks();
}

bool Game::eventOnScreen() {
	Uint32 now = SDL_GetTicks();
	
	//Manage the animation when we finish the level
	if (lvl_->finish_ && (int)(now - timeElapsedFinishAnimation_) < timeFinishAnimation_) {
		if (!finish_) {
			blinky_->setStopMoving();
			pinky_->setStopMoving();
			inky_->setStopMoving();
			clyde_->setStopMoving();
			finish_ = true;
			timeElapsed_ = now;
		}
		//Draw the ghost for some time before the level fliker
		if ((int)(now - timeElapsed_) < timeFinishAnimation_/2) {
			blinky_->draw();
			pinky_->draw();
			inky_->draw();
			clyde_->draw();
		}
		else
			lvl_->flickerAnimation();

		//Continue to display the score
		lvl_->display();
		player_.Entity::draw();
		return false;
	}
	//When the animation is finish we restart the level
	else if (lvl_->finish_) {
		restart();
		return false;
	}
	timeElapsedFinishAnimation_ = now;

	//Animation at the begining (delay when we display "Ready")
	if (begin_ && now - timeElapsed_ <= timeBeforeBegin_) {
		lvl_->display();
		overlay_.middleScreenMessage("Ready!");
		return false;
	}
	begin_ = false;

	//Reinitialize the level if pacman is dead
	if (reinit_ && player_.getDead())
		reinit_ = reinitLvl();
	if (!reinit_) {
		highScore_ = std::max(lvl_->score_, highScore_);
		lvl_->highScore_ = highScore_;

		//Display messages
		lvl_->display();
		overlay_.middleScreenMessage("Game Over");
		int size = 9 * (SCALE - 1);
		std::string restartMessage{"Press R to restart !"};
		int middle = (int)(restartMessage.length() * size) / 2;
		overlay_.write({ wWindow_ / 2 - middle, hWindow_ + (int)(marginTop_ * 1.25f), size , size }, restartMessage);
		return false;
	}
	return true;
}

void Game::draw() {
	//Display the level background
	SDL_FillRect(winSurf_, NULL, NULL);
	SDL_BlitScaled(bgSprite_, NULL, winSurf_, NULL);

	if (!eventOnScreen())
		return;

	//Display/draw all the entities of the game
	lvl_->draw();
	player_.draw();
	
	//Draw each ghosts
	blinky_->draw();
	pinky_->draw();
	inky_->draw();
	clyde_->draw();

	bonus_->draw();
}

int Game::gameLoop() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "Echec de l'initialisation de la SDL " << SDL_GetError() << std::endl;
		return 1;
	}

	init();
	bool quit = false;
	while (!quit)
	{
		SDL_Event event;
		while (!quit && SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			default: break;
			}
		}

		//Keyboard management       
		int nbk;
		const Uint8* keys = SDL_GetKeyboardState(&nbk);
		if (keys[SDL_SCANCODE_ESCAPE])
			quit = true;
		if (keys[SDL_SCANCODE_LEFT])
			player_.setDirection(Left);
		if (keys[SDL_SCANCODE_RIGHT])
			player_.setDirection(Right);
		if (keys[SDL_SCANCODE_UP])
			player_.setDirection(Up);
		if (keys[SDL_SCANCODE_DOWN])
			player_.setDirection(Down);
		if (keys[SDL_SCANCODE_R])
			restart();

		draw();
		SDL_UpdateWindowSurface(pWindow_);
	}
	SDL_Quit();
	return 0;
}