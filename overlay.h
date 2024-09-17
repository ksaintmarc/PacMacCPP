#ifndef OVERLAY_H
#define OVERLAY_H

#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>

class Overlay{
	private:
		SDL_Surface* plancheSprites_;
		SDL_Surface* winSurf_;

		int wWindow_, scale_, marginTop_;
		//All the letters sprites
		std::vector<SDL_Rect> alphabet_;
		//All the numbers sprites
		std::vector<SDL_Rect> numbers_;
		//Pacman icon sprite
		SDL_Rect lifeSprite_;

		/*Add the sprites in alphabet_*/
		void buildAlphabet();
		/*Add the sprites in numbers_*/
		void buildNumbers();
	public:
		Overlay();
		Overlay(int wWindow_, int scale, int marginTop, SDL_Surface* plancheSprites, SDL_Surface* winSurf);
		~Overlay();

		/*Write a message at a given position and scale*/
		void write(SDL_Rect position, std::string message);
		/*Draw the pacman lives that are left*/
		void lives(SDL_Rect position, int n);
		/*Show a message at the center of the game level*/
		void middleScreenMessage(std::string message);
};

#endif // OVERLAY_H