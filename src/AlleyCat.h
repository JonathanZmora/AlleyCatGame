#pragma once
#include "bagel.h"
#include <SDL3/SDL.h>
#include <box2d/box2d.h>

namespace AlleyCatGame {
    class AlleyCat
    {
    public:
        AlleyCat();
        ~AlleyCat();

        bool valid() const;
        void run();

    private:
        bool prepareWindowAndTexture();
        void prepareBoxWorld();

        SDL_Texture* tex;
        SDL_Renderer* ren;
        SDL_Window* win;

        b2WorldId boxWorld = b2_nullWorldId;

        static constexpr int    WIN_WIDTH = 1280;
        static constexpr int    WIN_HEIGHT = 800;
        static constexpr int	FPS = 60;
        static constexpr float	GAME_FRAME = 1000.f/FPS;
    };
}