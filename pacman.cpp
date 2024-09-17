#include <SDL.h>

#include <iostream>
#include <vector>


// fonction qui met à jour la surface de la fenetre "win_surf"
void draw()
{
	SDL_SetColorKey(plancheSprites, false, 0);
	SDL_BlitScaled(plancheSprites, &src_bg, win_surf, &bg);

	// On dessine l'entité "Blinky" sur la surface
	blinky.draw(*plancheSprites, *win_surf);
	pinky.draw(*plancheSprites, *win_surf);
	inky.draw(*plancheSprites, *win_surf);
	clyde.draw(*plancheSprites, *win_surf);

	//TO DO : delete this tests at the end
	// 
	//Begin tests
	// 
	//To test the vulnerability system
	Uint32 now = SDL_GetTicks();
	if (now - timeVulnerable_ >= 5000) {
		if (first) {
			inky.setVulnerableState();
			clyde.setVulnerableState();
		}
		blinky.setVulnerableState();
		first = false;
		timeVulnerable_ = now;
	}

	//To test the dead system
	if (now - timeKill_ >= 7000) {
		blinky.setDeadState();
		timeKill_ = now;
	}
	//End test
}

Uint32 next_time;

Uint32 time_left(void) {
	Uint32 now = SDL_GetTicks();
	if (next_time <= now)
		return 0;
	else
		return next_time - now;
}

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "Echec de l'initialisation de la SDL " << SDL_GetError() << std::endl;
		return 1;
	}

	init();
	// BOUCLE PRINCIPALE
	bool quit = false;

	next_time = SDL_GetTicks() + TICK_INTERVAL;
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

		// Gestion du clavier        
		int nbk;
		const Uint8* keys = SDL_GetKeyboardState(&nbk);
		if (keys[SDL_SCANCODE_ESCAPE])
			quit = true;
		if (keys[SDL_SCANCODE_LEFT])
			puts("LEFT");
		if (keys[SDL_SCANCODE_RIGHT])
			puts("RIGHT");

		// AFFICHAGE
		draw();
		SDL_UpdateWindowSurface(pWindow);

		SDL_Delay(time_left());
		next_time += TICK_INTERVAL;
	}
	SDL_Quit(); // ON SORT
	return 0;
}
