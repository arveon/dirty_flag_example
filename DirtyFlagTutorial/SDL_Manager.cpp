#include "SDL_Manager.h"

SDL_Window * SDL_Manager::game_window;
SDL_Renderer* SDL_Manager::renderer;

//DON'T WORRY ABOUT ANY OF THIS CODE
void SDL_Manager::init()
{
	assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);

	game_window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 920, SDL_WINDOW_SHOWN/* || SDL_WINDOW_FULLSCREEN*/);


	//initialise window and renderer
	assert(game_window);
	renderer = SDL_CreateRenderer(game_window, -1, SDL_RENDERER_ACCELERATED);
	assert(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_ADD);

	//initialise img and mix libraries
	int img_flags = IMG_INIT_PNG;

	assert(IMG_Init(img_flags) & img_flags);
	assert(TTF_Init() != -1);
	assert(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) >= 0);

	SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);

	//hide mouse cursor so the custom one can be drawn instead
	SDL_ShowCursor(0);
}

SDL_Texture* SDL_Manager::get_texture_from_spritesheet(SDL_Texture* tilesheet, SDL_Rect src)
{
	SDL_Texture* result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, src.w, src.h);
	//set result texture as a target of the renderer
	SDL_SetRenderTarget(renderer, result);
	//set texture mode transparent
	SDL_SetTextureBlendMode(result, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	//copy a rectangle from the texturemap tex to tile tex
	SDL_RenderCopy(renderer, tilesheet, &src, NULL);
	//set renderer target back to screen
	SDL_SetRenderTarget(renderer, NULL);
	//std::cout << " loaded " << IMG_GetError() << std::endl;
	return result;
}

SDL_Texture* SDL_Manager::load_texture(const char* path)
{
	SDL_Texture* temp = IMG_LoadTexture(renderer, path);
	assert(temp);
	return temp;
}

void SDL_Manager::input(Mouse* mouse)
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
		{
			SDL_Quit();
			exit(0);
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			mouse->lmb_down = true;
			mouse->prev_lmb_down = false;
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			mouse->lmb_down = false;
			mouse->prev_lmb_down = true;
		}
	}
	SDL_GetMouseState(&mouse->x, &mouse->y);

}

SDL_Texture * SDL_Manager::get_texture_from_text(const char*  text, SDL_Color color)
{
	TTF_Font* font = TTF_OpenFont("Optimus_Princeps.ttf", 24);
	assert(font);
	
	SDL_Surface* temp_s = TTF_RenderText_Solid(font, text, { color.r, color.g, color.b, 255 });
	assert(temp_s);
	SDL_Texture* temp_t = SDL_CreateTextureFromSurface(renderer, temp_s);

	SDL_FreeSurface(temp_s);
	return temp_t;
}

SDL_Manager::SDL_Manager()
{
}


SDL_Manager::~SDL_Manager()
{
}
