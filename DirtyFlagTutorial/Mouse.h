#pragma once
#include <SDL.h>

///
///Class serves as a representation of mouse states. It contains x, y, button current and previous positions
///as well as methods to draw and update it
class Mouse
{

public:
	SDL_Texture * texture;
	SDL_Rect draw_rect;
	bool is_drawn = true;

	int x, y;
	bool lmb_down;
	bool rmb_down;
	bool prev_lmb_down;
	bool prev_rmb_down;
	Mouse(){}
	~Mouse(){}

	void init(SDL_Texture* tex)
	{
		is_drawn = true;
		texture = tex;
		draw_rect.x = 0;
		draw_rect.y = 0;
		draw_rect.w = 20;
		draw_rect.h = 20;
		//SDL_QueryTexture(texture, NULL, NULL, &draw_rect.w, &draw_rect.h);
	}

	void update()
	{
		draw_rect.x = x;
		draw_rect.y = y;

		prev_lmb_down = lmb_down;
	}
	
	void draw(SDL_Renderer* renderer)
	{
		if (is_drawn)
			SDL_RenderCopy(renderer, texture, NULL, &draw_rect);
	}
};

