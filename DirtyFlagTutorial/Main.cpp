#include "SDL_Manager.h"

#include <SDL.h>
#include <vector>

#include <cstdlib>
#include <ctime>

#include "Mouse.h"
#include "Button.h"

//class represents any object inside game
class Entity
{
public:
	SDL_Texture * texture;//texture that is being rendered
	SDL_Texture* spritesheet;//whole animation spritesheet
	SDL_Rect transform;//position in world

	float speed_x = 3.0f;//movement speed

	int cur_frame = 0;//frame out of spritesheet that is currently displayed
	bool animation_changed = true;//dirty flag for when texture needs to be updated
};

void init_objects();
void update();
void draw();
void update_animations();
void update_animations_df();

std::vector<Entity*> objects;//all objects in game space
std::vector<Entity*> objects_df;//all objects in game space

Entity* status_text;

Mouse mouse;
Button toggle_button;

bool df_enabled = false;

const int TICKS_FOR_UPDATE = 500;//how often the animation changes. CAN BE ADJUSTED IF NEEDED TO SLOW DOWN/SPEED UP ANIMATION

int main(int argc, char* args[])
{
	SDL_Manager::init();//initialise all required subsystems for rendering and creating window

	init_objects();//initialise all objects in game space

	while (true)
	{
		SDL_Manager::input(&mouse);
		update();
		draw();
	}

	return 0;
}

//function initialises 15 game objects, their positions, loads spritesheet and crops the texture out of it
void init_objects()
{
	for (int i = 1; i < 11; i++)
	{
		Entity* temp = new Entity();
		objects.push_back(temp);

		temp->transform = { 0, 64 * i, 32, 64 };
		temp->cur_frame = rand() % 8;//start with a random frame

		temp->spritesheet = SDL_Manager::load_texture("walking_cycle.png");
		temp->texture = SDL_Manager::get_texture_from_spritesheet(temp->spritesheet, { temp->cur_frame * 32, 128, 32,64 });
	}

	toggle_button.init({ 0,0 }, 24);
	mouse.init(SDL_Manager::load_texture("mouse.png"));

	status_text = new Entity();
	status_text->texture = SDL_Manager::get_texture_from_text("Dirty flag is Off", {255,255,255});
	status_text->transform = SDL_Manager::get_texture_size(status_text->texture);
	status_text->transform.x = 640 - status_text->transform.w;
}

//called every tick of the game
void update()
{
	static int counter = 0;//keeps track of how many ticks were passed since the last update
	static int ticks_passed = 0;
	if (counter >= TICKS_FOR_UPDATE)//every n ticks animation is updated
	{//when animation is updated
		counter = 0;//reset counter
		ticks_passed++;
		//change frame for all entities
		for (int i = 0; i < objects.size(); i++)
		{
			Entity* ent = objects.at(i);
			ent->animation_changed = true;//SET the flag
											//update entity position and if end reached, loop around
			ent->transform.x += ent->speed_x;
			if (ent->transform.x + ent->transform.w > 640)
				ent->transform.x = 0;

			//update entity frame and if end reached, loop around
			ent->cur_frame++;
			if (ent->cur_frame > 7)
				ent->cur_frame = 0;
		}
	}

	if (!df_enabled)
		update_animations();
	else
		update_animations_df();
	counter++;//increase the counter
	toggle_button.update(&mouse);
	if (toggle_button.is_clicked())
	{
		std::string text;
		df_enabled = !df_enabled;
		if (df_enabled)
			text = "Dirty flag is On";
		else
			text = "Dirty flag is Off";
		SDL_Manager::destroy_texture(status_text->texture);
		status_text->texture = SDL_Manager::get_texture_from_text(text.c_str(), { 255,255,255 });
		status_text->transform = SDL_Manager::get_texture_size(status_text->texture);
		status_text->transform.x = 640 - status_text->transform.w;
	}
	mouse.update();
}

//function responsible for updating the textures on entities. CALLED EVERY TICK TO BETTER DISPLAY THE DIFFERENCE WHEN DIRTY FLAG IS AND ISN'T USED
void update_animations()
{
	//std::cout << "b" << std::endl;
	//update all of the objects animations
	for (int i = 0; i < objects.size(); i++)
	{
		Entity* ent = objects.at(i);
		SDL_Manager::destroy_texture(ent->texture);//deallocate the previously saved texture to avoid memory leaks
		ent->texture = SDL_Manager::get_texture_from_spritesheet(ent->spritesheet, { ent->cur_frame * 32, 128, 32,64 });//crop the new frame out of the spritesheet
	}
}

//function responsible for updating the textures on entities. CALLED EVERY TICK TO BETTER DISPLAY THE DIFFERENCE WHEN DIRTY FLAG IS AND ISN'T USED
void update_animations_df()
{
	//std::cout << "b" << std::endl;
	//update all of the objects animations
	for (int i = 0; i < objects.size(); i++)
	{
		Entity* ent = objects.at(i);
		if (ent->animation_changed)
		{
			SDL_Manager::destroy_texture(ent->texture);//deallocate the previously saved texture to avoid memory leaks
			ent->texture = SDL_Manager::get_texture_from_spritesheet(ent->spritesheet, { ent->cur_frame * 32, 128, 32,64 });//crop the new frame out of the spritesheet
			ent->animation_changed = false;
		}
	}
}

//simple draw function
void draw()
{
	SDL_Manager::clear();//fill screen with blue color
						 //add all objects to batch
	for (int i = 0; i < objects.size(); i++)
	{
		Entity* ent = objects.at(i);
		SDL_Manager::add_to_queue(ent->texture, ent->transform);
	}
	SDL_Manager::add_to_queue(toggle_button.image, toggle_button.draw_rect);
	SDL_Manager::add_to_queue(mouse.texture, mouse.draw_rect);
	SDL_Manager::add_to_queue(status_text->texture, status_text->transform);
	//draw batch
	SDL_Manager::render();
}




