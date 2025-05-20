#include "AlleyCat.h"

int main(int argc, char** argv)
{
    AlleyCatGame::AlleyCat game;
    if (!game.valid())
        return 1;

    game.run();
    return 0;
}