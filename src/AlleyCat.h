#pragma once
#include "bagel.h"
#include <SDL3/SDL.h>
#include <box2d/box2d.h>

namespace AlleyCatGame {

    using Transform = struct { SDL_FPoint p; float angle; };
    using Drawable = struct { SDL_FRect part; SDL_FPoint size; };
    using Intent = struct { bool left, right, up; };
    using Collider = struct { b2BodyId body; };

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
        void createBackgroundEntity();
        void createWalls();

        void inputSystem();
        void moveSystem();
        void drawSystem();
        void boxSystem(float deltaTime);

        SDL_Texture* bgTex;
        SDL_Texture* catTex;
        SDL_Texture* tex;
        SDL_Renderer* ren;
        SDL_Window* win;


        b2WorldId boxWorld = b2_nullWorldId;

        static constexpr int    WIN_WIDTH = 1280;
        static constexpr int    WIN_HEIGHT = 800;
        static constexpr int	FPS = 60;
        static constexpr float	GAME_FRAME = 1000.f/FPS;
        static constexpr float BOX_SCALE = 64.0f;  // 1 m = 64 px
        static constexpr float DESIRED_PX_SPEED = 200.0f;
        static constexpr float DESIRED_PX_JUMP  = 150.0f;
    };
}