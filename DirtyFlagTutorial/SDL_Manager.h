#pragma once
#include <SDL.h>
#include <assert.h>
#include <string>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "Mouse.h"

//DON'T WORRY ABOUT ANY OF THIS CODE
class SDL_Manager
{
private:
	static SDL_Window * game_window;
	static SDL_Renderer* renderer;
public:
	SDL_Manager();
	~SDL_Manager();

	static void init();
	static void input(Mouse* mouse);
	static SDL_Texture* get_texture_from_spritesheet(SDL_Texture* texture, SDL_Rect src);
	static SDL_Texture* load_texture(const char* path);
	static void clear() { SDL_RenderClear(renderer); }
	static void render() { SDL_RenderPresent(renderer); }
	static void add_to_queue(SDL_Texture* tex, SDL_Rect dest_rect) { SDL_RenderCopy(renderer, tex, nullptr, &dest_rect); }
	static SDL_Texture* get_texture_from_text(const char*  text, SDL_Color color);
	static void destroy_texture(SDL_Texture* tex) { SDL_DestroyTexture(tex); }

	static SDL_Rect get_texture_size(SDL_Texture* tex) { SDL_Rect temp{0,0,0,0}; SDL_QueryTexture(tex, NULL, NULL, &temp.w, &temp.h); return temp; }
};



