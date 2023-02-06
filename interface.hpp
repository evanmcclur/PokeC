#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "ncurses.h"

#define CLEARING_PAIR     1
#define GRASS_PAIR        2
#define TREE_PAIR         3
#define MOUNTAIN_PAIR     4
#define PLAYER_PAIR       5
#define ROAD_PAIR         6
#define POKECENTER_PAIR   7
#define POKEMART_PAIR     8
#define EXIT_PAIR         9

enum ErrorMessage
{
    DEFAULT,
    IMPASSABLE_TERRAIN,
    OCCUPIED_CELL,
    INVALID_KEY,
    MEMORY_ALLOC_FAIL,
    NUM_ERROR_MESSAGES
};

class ErrorLogger
{
    private:
        static const char * errors[NUM_ERROR_MESSAGES][100];
    public:
        static void log(ErrorMessage msg);
};

#endif