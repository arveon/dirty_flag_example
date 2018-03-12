#include "SDL_Manager.h"

#include <SDL.h>
#include <vector>

#include <cstdlib>
#include <ctime>

//class represents any object inside game
class Entity
{
public:
	SDL_Texture* texture;//texture that is being rendered
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

std::vector<Entity*> objects;//all objects in game space

const int TICKS_FOR_UPDATE = 500;//how often the animation changes. CAN BE ADJUSTED IF NEEDED TO SLOW DOWN/SPEED UP ANIMATION

int main(int argc, char* args[])
{
	SDL_Manager::init();//initialise all required subsystems for rendering and creating window

	init_objects();//initialise all objects in game space

	//game loop
	while (true)
	{
		SDL_Manager::input();
		
		update();
		draw();
	}
}

//function initialises 15 game objects, their positions, loads spritesheet and crops the texture out of it
void init_objects()
{
	for (int i = 0; i < 15; i++)
	{
		Entity* temp = new Entity();
		objects.push_back(temp);

		temp->transform = {0, 64*i, 32, 64};
		temp->cur_frame = rand() % 8;//start with a random frame

		temp->spritesheet = SDL_Manager::load_texture("walking_cycle.png");
		temp->texture = SDL_Manager::get_texture_from_spritesheet(temp->spritesheet, { temp->cur_frame * 32, 128, 32,64 });
	}
}

//called every tick of the game
void update()
{
	static int counter = 0;//keeps track of how many ticks were passed since the last update
	if (counter >= TICKS_FOR_UPDATE)//every n ticks animation is updated
	{//when animation is updated
		counter = 0;//reset counter
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

	update_animations();
	counter++;//increase the counter
}

//function responsible for updating the textures on entities. CALLED EVERY TICK TO BETTER DISPLAY THE DIFFERENCE WHEN DIRTY FLAG IS AND ISN'T USED
void update_animations()
{
	//update all of the objects animations
	for (int i = 0; i < objects.size(); i++)
	{
		Entity* ent = objects.at(i);
		if (ent->animation_changed)
		{
			SDL_Manager::destroy_texture(ent->texture);//deallocate the previously saved texture to avoid memory leaks
			ent->texture = SDL_Manager::get_texture_from_spritesheet(ent->spritesheet, { ent->cur_frame * 32, 128, 32,64 });//crop the new frame out of the spritesheet
			//LINE USED TO TOGGLE DIRTY FLAG \/
			ent->animation_changed = false;//clears the flag when animation change is done (if commented out, flag will not be cleared and will update frame every tick)
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
	//draw batch
	SDL_Manager::render();
}


