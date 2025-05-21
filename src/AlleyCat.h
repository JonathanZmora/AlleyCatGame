#pragma once
#include "components.h"
#include "bagel.h"
#include <SDL3/SDL.h>
#include <box2d/box2d.h>

using namespace bagel;

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
        void createCatEntity();
        void createDogEntity();
        void createWalls();
        void createPlatforms();

        void inputSystem();
        void moveSystem();
        void drawSystem();
        void scoreSystem();
        void boxSystem(float deltaTime);

        SDL_Texture* bgTex;
        SDL_Texture* catTex;
        SDL_Texture* dogTex;
        SDL_Texture* tex;
        SDL_Renderer* ren;
        SDL_Window* win;

        b2WorldId boxWorld = b2_nullWorldId;

        ent_type cat_ent;

        static constexpr int    WIN_WIDTH = 1280;
        static constexpr int    WIN_HEIGHT = 800;
        static constexpr int	FPS = 60;
        static constexpr float	GAME_FRAME = 1000.f/FPS;
        static constexpr float  BOX_SCALE = 64.0f;  // 1 m = 64 px
        static constexpr float  DESIRED_PX_SPEED = 200.0f;
        static constexpr float  DESIRED_PX_JUMP  = 300.0f;
        static constexpr SDL_FPoint platformCenters[] = {
            {260.f, 610.f},
            {420.f, 635.f},
            {900.f, 600.f},
            {1070.f, 626.f},
            {1220.f, 610.f},
            {198.f, 350.f},
            {600.f, 350.f},
            {1000.f, 350.f},
            {198.f, 250.f},
            {600.f, 250.f},
            {1000.f, 250.f},
            {198.f, 160.f},
            {600.f, 160.f},
            {1000.f, 160.f}
        };
    };
}