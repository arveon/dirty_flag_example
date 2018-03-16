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
void toggle_dirty_flag();
void update_animations();
void update_animations_df();

std::vector<Entity*> objects;//all objects in game space

Entity* status_text;

Mouse mouse;
Button toggle_button;

bool df_enabled = false;

const int TICKS_FOR_UPDATE = 250;//how often the animation changes. CAN BE ADJUSTED IF NEEDED TO SLOW DOWN/SPEED UP ANIMATION



