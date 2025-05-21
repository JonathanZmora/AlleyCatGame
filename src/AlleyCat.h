#pragma once
#include "bagel.h"
#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <vector>
#include <array>
//#include <box2d/b2_contact_listener.h>

namespace AlleyCatGame {

    using Transform = struct { SDL_FPoint p; float angle; };
    using Drawable = struct { SDL_FRect part; SDL_FPoint size; };
    using Intent = struct { bool left, right, up; };
    using Collider = struct { b2BodyId body; };
    using Platform = struct { float width, height; }; //in pixels

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
        void createPlatforms ();
        void inputSystem();
        void moveSystem();
        void drawSystem();
        void boxSystem(float deltaTime);
        void platformsSystem();

        static constexpr struct { float x, y, w, h; } platformDefs[] = {
            //cans
            {260.f, 610.f, 100.f , 5.f},
                {420.f, 635.f, 100.f , 5.f},
                {900.f, 600.f, 100.f , 5.f},
                {1070.f, 626.f, 100.f , 5.f},
                {1220.f, 610.f, 100.f , 5.f},
            //windows
            {198.f, 400.f, 300.f, 5.f},
               {540.f, 400.f, 300.f, 5.f},
               {1000.f, 400.f, 300.f, 5.f},
               {198.f, 305.f,300.f, 5.f},
               {540.f, 305.f, 300.f, 5.f},
               {1000.f, 305.f, 300.f, 5.f},
               {198.f, 160.f, 300.f, 5.f},
               {540.f, 160.f, 300.f, 5.f},
               {1000.f, 160.f, 300.f, 5.f}

        };
        static constexpr size_t NumDefs = std::size(platformDefs);
        std::vector<b2ShapeId> platformShapes;
        using ShowPlat = struct {
            b2BodyId   body  = {0};
            bagel::ent_type ent  = {0};
        };
        std::array<ShowPlat, NumDefs> spawnedPlatforms{};
       // std::array<bagel::ent_type, NumDefs> showPlatforms;

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
        static constexpr float BOX_SCALE = 64.0f;
        static constexpr float DESIRED_PX_SPEED = 200.0f;
        static constexpr float DESIRED_PX_JUMP  = 200.0f;
        static constexpr SDL_FPoint platformCenters[] = {
            {260.f, 610.f},
            {420.f, 635.f},
            {900.f, 600.f},
            {1070.f, 626.f},
            {1220.f, 610.f},
            {198.f, 350.f},
            {540.f, 350.f},
            {1000.f, 350.f},
            {198.f, 250.f},
            {540.f, 250.f},
            {1000.f, 250.f},
            {198.f, 160.f},
            {540.f, 160.f},
            {1000.f, 160.f}
        };

    };




}