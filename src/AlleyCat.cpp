#include "AlleyCat.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <box2d/box2d.h>
#include <iostream>

using namespace std;

namespace AlleyCatGame
{
	bool AlleyCat::prepareWindowAndTexture()
	{
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			cout << SDL_GetError() << endl;
			return false;
		}

		if (!SDL_CreateWindowAndRenderer("Alley Cat", WIN_WIDTH, WIN_HEIGHT, 0, &win, &ren))
		{
			cout << SDL_GetError() << endl;
			return false;
		}

		/*SDL_Surface *surf = IMG_Load("resources/alleycat.png");
		if (surf == nullptr)
		{
			cout << SDL_GetError() << endl;
			return false;
		}

		tex = SDL_CreateTextureFromSurface(ren, surf);
		if (tex == nullptr)
		{
			cout << SDL_GetError() << endl;
			return false;
		}

		SDL_DestroySurface(surf);*/

		return true;
	}

	void AlleyCat::prepareBoxWorld()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = {0,0};
		boxWorld = b2CreateWorld(&worldDef);
	}

	bool AlleyCat::valid() const
	{
		return ren != nullptr;
	}

    AlleyCat::AlleyCat()
	{
		if (!prepareWindowAndTexture()) return;
    	SDL_srand(time(nullptr));

		prepareBoxWorld();
	}

	AlleyCat::~AlleyCat()
    {
    	if (b2World_IsValid(boxWorld))
    		b2DestroyWorld(boxWorld);
    	if (tex != nullptr)
    		SDL_DestroyTexture(tex);
    	if (ren != nullptr)
    		SDL_DestroyRenderer(ren);
    	if (win != nullptr)
    		SDL_DestroyWindow(win);

    	SDL_Quit();
    }

	void AlleyCat::run()
	{
		SDL_SetRenderDrawColor(ren, 0,0,0,255);
		auto start = SDL_GetTicks();
		bool quit = false;

		while (!quit)
		{
			SDL_Event e;
			while (SDL_PollEvent(&e))
			{
				if ((e.type == SDL_EVENT_QUIT) ||
				   ((e.type == SDL_EVENT_KEY_DOWN) && (e.key.scancode == SDL_SCANCODE_ESCAPE)))
					quit = true;
			}

			SDL_RenderClear(ren);
			SDL_RenderPresent(ren);

			auto end = SDL_GetTicks();
			if (end - start < GAME_FRAME)
				SDL_Delay(GAME_FRAME - (end - start));
		}
	}
}