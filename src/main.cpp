#include "AlleyCat.h"

int main()
{
    AlleyCatGame::AlleyCat game;
    if (!game.valid())
        return 1;

    game.run();
    return 0;
}