#include "SDL_Manager.h"

#include <SDL.h>
#include <vector>

#include <cstdlib>
#include <ctime>

#include <thread>
#include <mutex>

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
SDL_Texture* tex;
SDL_Texture* tex2;
//SDL_Texture* spritesheet;
SDL_Rect draw_rect;
SDL_Rect draw_rect2;
int cur_1;
int cur_2;

bool a_changed;

std::mutex rect_lock;
std::mutex rect2_lock;
std::mutex tex_lock;
std::mutex tex2_lock;


//typedef void(*func)(std::vector<Entity*>&);

void init_objects();
void update();
void update2();
void draw();
void update_animations();
void update_animations_df();

void update_loop();
void update_loop2();

void game_loop();

std::vector<Entity*> objects;//all objects in game space
std::vector<Entity*> objects_df;//all objects in game space

const int TICKS_FOR_UPDATE = 1000;//how often the animation changes. CAN BE ADJUSTED IF NEEDED TO SLOW DOWN/SPEED UP ANIMATION

int main(int argc, char* args[])
{
	SDL_Manager::init();//initialise all required subsystems for rendering and creating window
	init_objects();
	

	std::thread u2(update_loop2);
	std::thread u(update_loop);
	game_loop();
	
	return 0;
}

void update_loop2()
{
	while (true)
		update2();
}

void update_loop()
{
	while(true)
		update();
}

void game_loop()
{
	while (true)
	{
		SDL_Manager::input();
		draw();
	}


}

std::mutex crop;
void shared_crop(SDL_Texture* tex, SDL_Rect src)
{
	std::lock_guard<std::mutex> tl(tex_lock);
	std::lock_guard<std::mutex> tl2(tex2_lock);
	std::lock_guard<std::mutex> cr(crop);
	SDL_Texture* temp = tex;
	temp = SDL_Manager::get_texture_from_spritesheet(tex, src);
	SDL_Manager::destroy_texture(tex);
	tex = temp;
}

//function initialises 15 game objects, their positions, loads spritesheet and crops the texture out of it
void init_objects()
{
#pragma region not-used
	/*
	for (int i = 0; i < 5; i++)
	{
		Entity* temp = new Entity();
		objects.push_back(temp);

		temp->transform = { 0, 64 * i, 32, 64 };
		temp->cur_frame = rand() % 8;//start with a random frame

		temp->spritesheet = SDL_Manager::load_texture("walking_cycle.png");
		temp->texture = SDL_Manager::get_texture_from_spritesheet(temp->spritesheet, { temp->cur_frame * 32, 128, 32,64 });
	}

	for (int i = 5; i < 10; i++)
	{
		Entity* temp = new Entity();
		objects_df.push_back(temp);

		temp->transform = { 0, 64 * i, 32, 64 };
		temp->cur_frame = rand() % 8;//start with a random frame

		temp->spritesheet = SDL_Manager::load_texture("walking_cycle.png");
		temp->texture = SDL_Manager::get_texture_from_spritesheet(temp->spritesheet, { temp->cur_frame * 32, 192, 32,64 });
	}
	*/
#pragma endregion

	tex = SDL_Manager::load_texture("walking_cycle.png");
	tex = SDL_Manager::get_texture_from_spritesheet(tex, { 0, 128, 32,64 });
	draw_rect = {0,0,32,64};

	tex2 = SDL_Manager::load_texture("walking_cycle.png");
	tex2 = SDL_Manager::get_texture_from_spritesheet(tex2, { 0, 128, 32,64 });
	draw_rect2 = { 0,64,32,64 };
}

//called every tick of the game
void update()
{
	static int counter = 0;//keeps track of how many ticks were passed since the last update
	static int ticks_passed = 0;
	if (counter >= TICKS_FOR_UPDATE)//every n ticks animation is updated
	{//when animation is updated
		//std::cout << counter << std::endl;
		counter = 0;//reset counter
		std::lock_guard<std::mutex> r_l(rect_lock);
		draw_rect.x += 1;
		if (draw_rect.x >= 640)
			draw_rect.x = 0;
		cur_1++;

		if (cur_1 > 7)
			cur_1 = 0;
	}

	update_animations();
	counter++;//increase the counter
	SDL_Delay(100);
}

void update2()
{
	static int counter2 = 0;//keeps track of how many ticks were passed since the last update
	static int ticks_passed2 = 0;
	if (counter2 >= TICKS_FOR_UPDATE)//every n ticks animation is updated
	{//when animation is updated
		//std::cout << counter2 << std::endl;
		counter2 = 0;//reset counter
		std::lock_guard<std::mutex> r_l(rect2_lock);
		draw_rect2.x += 1;
		if (draw_rect2.x >= 640)
			draw_rect2.x = 0;
		a_changed = true;
	}

	//update_animations_df();
	counter2++;//increase the counter
	SDL_Delay(100);
}


//function responsible for updating the textures on entities. CALLED EVERY TICK TO BETTER DISPLAY THE DIFFERENCE WHEN DIRTY FLAG IS AND ISN'T USED
void update_animations()
{	
	shared_crop(tex, { cur_1 * 32, 128, 32, 64 });
	
	//std::cout << "updated animation" << std::endl;

	//for (int i = 0; i < objects.size(); i++)
	//{
	//	Entity* ent = objects.at(i);
	//	if (ent->animation_changed)
	//	{
	//		SDL_Manager::destroy_texture(ent->texture);//deallocate the previously saved texture to avoid memory leaks
	//		ent->texture = SDL_Manager::get_texture_from_spritesheet(ent->spritesheet, { ent->cur_frame * 32, 128, 32,64 });//crop the new frame out of the spritesheet
	//		ent->animation_changed = false;//clears the flag when animation change is done
	//	}
	//}

}

void update_animations_df()
{
	if (a_changed)
	{
		std::lock_guard<std::mutex> r_l(rect_lock);
		SDL_Manager::destroy_texture(tex);
		tex = SDL_Manager::get_texture_from_spritesheet(tex, { cur_1 * 32, 128, 32, 64 });
		a_changed = false;
	}
}


//simple draw function
void draw()
{
	SDL_Manager::clear();//fill screen with blue color
						 //add all objects to batch
	//for (int i = 0; i < objects.size(); i++)
	//{
	//	Entity* ent = objects.at(i);
	//	SDL_Manager::add_to_queue(ent->texture, ent->transform);
	//}

	////add all df objects to batch
	//for (int i = 0; i < objects_df.size(); i++)
	//{
	//	Entity* ent = objects_df.at(i);
	//	SDL_Manager::add_to_queue(ent->texture, ent->transform);
	//}
	////draw batch
	
	std::lock_guard<std::mutex> tl(tex_lock);
	std::lock_guard<std::mutex> tl2(tex2_lock);
	std::lock_guard<std::mutex> rl(rect_lock);
	std::lock_guard<std::mutex> rl2(rect2_lock);
	SDL_Manager::add_to_queue(tex, draw_rect);
	SDL_Manager::add_to_queue(tex2, draw_rect2);
	SDL_Manager::render();
}




