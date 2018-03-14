#pragma once
#include <iostream>

#include <SDL.h>
#include "Mouse.h"
#include "SDL_Manager.h"

///Class serves as a representation of the UI button object
///Contains basic button functionality and works for both, image buttons and text buttons
class Button
{
private:
	TTF_Font* font;

	//determines if the button is text or an image
	bool is_icon;
	enum state
	{
		None,
		Hovered,
		Pressed,
		Clicked,
		Disabled
	};
	state cur_state;
	
	std::string caption;
public:
	//determines whether scale button on resolution change or not
	bool scaling = true;
	SDL_Texture* image;
	SDL_Rect draw_rect;

	enum type
	{
		exit,
		start,
		load,
		options,
		ok,
		cancel,
		UI
	};
	void init(SDL_Point coords, int font_size);
	

	void update(Mouse* mouse);
	void draw(SDL_Renderer*);

	bool is_clicked() { return (cur_state == state::Clicked); }
	bool is_pressed(){ return (cur_state == state::Pressed); }
	void reset_button();
	void disable() { cur_state = state::Disabled; }
	void enable() { cur_state = state::None; }

	int get_height() { return draw_rect.h; }
	int get_width() { return draw_rect.w; }

	Button();
	~Button();

	type b_type;
};

