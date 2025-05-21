#pragma once

#include <SDL3/SDL.h>
#include <box2d/box2d.h>

namespace AlleyCatGame {

    using Transform = struct { SDL_FPoint p; float angle; };
    using Drawable  = struct { SDL_FRect part; SDL_FPoint size; };
    using Intent    = struct { bool left, right, up; };
    using Collider  = struct { b2BodyId body; };
    using Enemy     = struct {};

}
