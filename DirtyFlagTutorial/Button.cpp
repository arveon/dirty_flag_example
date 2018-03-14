#include "Button.h"

#pragma region Initialisation
//different initialisation overloads


void Button::init(SDL_Point init_coords, int font_size)
{
	this->image = SDL_Manager::get_texture_from_text("toggle", {20,20,80});
	SDL_QueryTexture(image, NULL, NULL, &draw_rect.w, &draw_rect.h);
	draw_rect.x = init_coords.x;
	draw_rect.y = init_coords.y;
}

#pragma endregion


void Button::update(Mouse* mouse)
{
	//if button is disabled, don't update it
	if (cur_state == state::Disabled)
		return;

	SDL_Point mouse_p = { mouse->x, mouse->y };

	//if mouse is over button
	if (SDL_PointInRect(&mouse_p, &draw_rect))
	{
		//if it just got pressed
		if (mouse->lmb_down && !mouse->prev_lmb_down)
		{
			//if state wasn't changed yet
			if (cur_state != Pressed)
			{
				//if its a text button, change the texture to a different color one
				if (!is_icon)
				{
					SDL_DestroyTexture(this->image);
					this->image = SDL_Manager::get_texture_from_text("toggle", {125,125,0});
				}
			}
			cur_state = state::Pressed;
		}
		else if (!mouse->lmb_down && cur_state == state::Pressed)//if was pressed and mouse button is up now, set to clicked
		{
			cur_state = state::Clicked;
		}
		else if(!mouse->lmb_down)
		{//if mouse not pressed, but over it, it's in hover mode
			if (cur_state != Hovered)
			{
				if (!is_icon)//swap texture if not icon
				{
					SDL_DestroyTexture(this->image);
					this->image = SDL_Manager::get_texture_from_text("toggle", { 80,80,140 });
				}
			}
			cur_state = state::Hovered;
		}
	}
	else
	{//if mouse not over button, reset it to default state and image
		if (cur_state != None)
		{
			if (!is_icon)
			{
				SDL_DestroyTexture(this->image);
				this->image = SDL_Manager::get_texture_from_text("toggle", { 20,20,80 });
			}
		}
		cur_state = state::None;
	}
}

/*
	function used to reset state and image of the button
*/
void Button::reset_button()
{
	cur_state = state::None;
	if (!is_icon)
	{
		SDL_DestroyTexture(this->image);
		this->image = SDL_Manager::get_texture_from_text("toggle", { 20,20,80 });
	}
}

void Button::draw(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, image, NULL, &draw_rect);
}


Button::Button()
{
}


Button::~Button()
{
	SDL_DestroyTexture(image);
	TTF_CloseFont(font);
}
