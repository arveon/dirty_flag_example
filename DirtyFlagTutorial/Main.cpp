#include "SDL_Manager.h"

#include <SDL.h>
#include <vector>

#include <cstdlib>
#include <ctime>

#include <thread>
#include <mutex>

#include <SDL_thread.h>

std::mutex tex;
std::mutex ss;
std::mutex transf;

SDL_sem* sem_tex;
SDL_sem* sem_ss;
SDL_sem* sem_transf;
SDL_sem* sem_obj;
SDL_sem* sem_obj_df;

////class represents any object inside game
//class Entity
//{
//	SDL_Texture* texture;//texture that is being rendered
//	SDL_Texture* spritesheet;//whole animation spritesheet
//	SDL_Rect transform;//position in world
//public:
//	SDL_Texture* get_texture() { return texture; }
//	void set_texture(SDL_Texture* texture) { SDL_SemWait(sem_tex); this->texture = texture; SDL_SemPost(sem_tex); }
//
//	SDL_Texture* get_spritesheet() {  return spritesheet; }
//	void set_spritesheet(SDL_Texture* spritesheet) { SDL_SemWait(sem_ss); this->spritesheet = spritesheet;  SDL_SemPost(sem_ss); }
//
//	SDL_Rect get_transf() { return transform; }
//	void set_transf(SDL_Rect transform) { SDL_SemWait(sem_transf); this->transform = transform; SDL_SemPost(sem_transf); }
//
//	float speed_x = 3.0f;//movement speed
//
//	int cur_frame = 0;//frame out of spritesheet that is currently displayed
//	bool animation_changed = true;//dirty flag for when texture needs to be updated
//};

//class represents any object inside game
class Entity
{
	SDL_Texture* texture;//texture that is being rendered
	SDL_Texture* spritesheet;//whole animation spritesheet
	SDL_Rect transform;//position in world
public:
	SDL_Texture * get_texture() { return texture; }
	void set_texture(SDL_Texture* texture) { this->texture = texture; }

	SDL_Texture* get_spritesheet() { return spritesheet; }
	void set_spritesheet(SDL_Texture* spritesheet) { this->spritesheet = spritesheet; }

	SDL_Rect get_transf() { return transform; }
	void set_transf(SDL_Rect transform) { this->transform = transform;}

	float speed_x = 3.0f;//movement speed

	int cur_frame = 0;//frame out of spritesheet that is currently displayed
	bool animation_changed = true;//dirty flag for when texture needs to be updated
};

void shared_print(std::string message)
{
	std::lock_guard<std::mutex> print(tex);
	std::cout << message << std::endl;
}

typedef void(*func)(std::vector<Entity*>&);

void init_objects();
void update();
void update_df();
void draw();
void update_animations_df();
void update_animations();

int game_loop(void* data);

int draw_loop(void* data);


std::vector<Entity*> objects;//all objects in game space
std::vector<Entity*> objects_df;//all objects in game space

const int TICKS_FOR_UPDATE = 500;//how often the animation changes. CAN BE ADJUSTED IF NEEDED TO SLOW DOWN/SPEED UP ANIMATION

int main(int argc, char* args[])
{
	SDL_Manager::init();//initialise all required subsystems for rendering and creating window
	sem_tex = SDL_CreateSemaphore(1);
	sem_ss = SDL_CreateSemaphore(1);
	sem_transf = SDL_CreateSemaphore(1);
	sem_obj = SDL_CreateSemaphore(1);
	sem_obj_df = SDL_CreateSemaphore(1);

	init_objects();//initialise all objects in game space

	//game_loop();//to make it run the df version, instead of &update_animations use &update_animations_df
	SDL_Thread* df_loop = SDL_CreateThread(game_loop, "df", new int(1));
	SDL_Thread* nm_loop = SDL_CreateThread(game_loop, "nm", new int(0));
	SDL_Thread* d_loop = SDL_CreateThread(draw_loop, "nam", new int(0));

	
	SDL_WaitThread(d_loop, NULL);
	return 0;
}

//function initialises 15 game objects, their positions, loads spritesheet and crops the texture out of it
void init_objects()
{
	for (int i = 0; i < 5; i++)
	{
		Entity* temp = new Entity();
		objects.push_back(temp);

		temp->set_transf({0, 64*i, 32, 64});
		temp->cur_frame = rand() % 8;//start with a random frame

		temp->set_spritesheet(SDL_Manager::load_texture("walking_cycle.png"));
		temp->set_texture(SDL_Manager::get_texture_from_spritesheet(temp->get_spritesheet(), { temp->cur_frame * 32, 128, 32,64 }));
	}

	for (int i = 5; i < 10; i++)
	{
		Entity* temp = new Entity();
		objects_df.push_back(temp);

		temp->set_transf({ 0, 64 * i, 32, 64 });
		temp->cur_frame = rand() % 8;//start with a random frame

		temp->set_spritesheet(SDL_Manager::load_texture("walking_cycle.png"));
		temp->set_texture(SDL_Manager::get_texture_from_spritesheet(temp->get_spritesheet(), { temp->cur_frame * 32, 192, 32,64 }));
	}
}

//called every tick of the game
void update()
{
	static int counter = 0;//keeps track of how many ticks were passed since the last update
	static int ticks_passed = 0;
	if (counter >= TICKS_FOR_UPDATE)//every n ticks animation is updated
	{//when animation is updated
		//std::cout << ticks_passed << std::endl;
		counter = 0;//reset counter
		ticks_passed++;
		//change frame for all entities
		SDL_SemWait(sem_obj);
		for (int i = 0; i < objects.size(); i++)
		{
			Entity* ent = objects.at(i);
			ent->animation_changed = true;//SET the flag
			
			//update entity position and if end reached, loop around
			SDL_Rect transf = ent->get_transf();
			transf.x += ent->speed_x;
			ent->set_transf(transf);
			if (ent->get_transf().x + ent->get_transf().w > 640)
			{
				transf.x = 0;
				ent->set_transf(transf);
			}

			//update entity frame and if end reached, loop around
			ent->cur_frame++;
			if (ent->cur_frame > 7)
				ent->cur_frame = 0;
		}
		SDL_SemPost(sem_obj);
	}

	update_animations();
	counter++;//increase the counter
}

//called every tick of the game
void update_df()
{
	static int counter = 0;//keeps track of how many ticks were passed since the last update
	static int ticks_passed = 0;
	if (counter >= TICKS_FOR_UPDATE)//every n ticks animation is updated
	{//when animation is updated
	 //std::cout << ticks_passed << std::endl;
		counter = 0;//reset counter
		ticks_passed++;
		SDL_SemWait(sem_obj_df);
		//change frame for all entities
		for (int i = 0; i < objects.size(); i++)
		{
			Entity* ent = objects.at(i);
			ent->animation_changed = true;//SET the flag

										  //update entity position and if end reached, loop around
			SDL_Rect transf = ent->get_transf();
			transf.x += ent->speed_x;
			ent->set_transf(transf);
			if (ent->get_transf().x + ent->get_transf().w > 640)
			{
				transf.x = 0;
				ent->set_transf(transf);
			}

			//update entity frame and if end reached, loop around
			ent->cur_frame++;
			if (ent->cur_frame > 7)
				ent->cur_frame = 0;
		}
		SDL_SemPost(sem_obj_df);
	}

	update_animations_df();
	counter++;//increase the counter
}

//function responsible for updating the textures on entities. CALLED EVERY TICK TO BETTER DISPLAY THE DIFFERENCE WHEN DIRTY FLAG IS AND ISN'T USED
void update_animations()
{	
	SDL_SemWait(sem_obj);
	//update all of the objects animations
	for (int i = 0; i < objects.size(); i++)
	{
		Entity* ent = objects.at(i);
		SDL_Manager::destroy_texture(ent->get_texture());//deallocate the previously saved texture to avoid memory leaks
		ent->set_texture(SDL_Manager::get_texture_from_spritesheet(ent->get_spritesheet(), { ent->cur_frame * 32, 128, 32,64 }));//crop the new frame out of the spritesheet
		shared_print("updated normal animations");
	}
	SDL_SemPost(sem_obj);
}

void update_animations_df()
{
	SDL_SemWait(sem_obj_df);
	//update all of the objects animations
	for (int i = 0; i < objects_df.size(); i++)
	{
		shared_print("updated df animations");
		Entity* ent = objects_df.at(i);
		if (ent->animation_changed)
		{
			Entity* ent = objects.at(i);
			SDL_Manager::destroy_texture(ent->get_texture());//deallocate the previously saved texture to avoid memory leaks
			ent->set_texture(SDL_Manager::get_texture_from_spritesheet(ent->get_spritesheet(), { ent->cur_frame * 32, 128, 32,64 }));//crop the new frame out of the spritesheet
			ent->animation_changed = false;
		}
	}
	SDL_SemPost(sem_obj_df);
}

int game_loop(void* data)
{
	//game loop
	while (true)
	{
		//std::cout << "update" << std::endl;
		//SDL_Manager::input();
		//draw();
		int* a = (int*)data;
		int num = *a;
		shared_print(std::to_string(num));
		if (num == 1)
			update_df();
		else
			update();
	}
	return 0;
}

int draw_loop(void* data)
{
	while (true)
	{
		SDL_Manager::input();
		draw();
	}
	return 0;
}


//simple draw function
void draw()
{
	SDL_Manager::clear();//fill screen with blue color
	//add all objects to batch
	
	SDL_SemWait(sem_obj);
	for (int i = 0; i < objects.size(); i++)
	{
		Entity* ent = objects.at(i);
		SDL_Manager::add_to_queue(ent->get_texture(), ent->get_transf());
	}
	SDL_SemPost(sem_obj);
	
	SDL_SemWait(sem_obj_df);
	for (int i = 0; i < objects_df.size(); i++)
	{
		Entity* ent = objects_df.at(i);
		SDL_Manager::add_to_queue(ent->get_texture(), ent->get_transf());

	}
	SDL_SemPost(sem_obj_df);
	shared_print("drawn");
	//draw batch
	SDL_Manager::render();
}




