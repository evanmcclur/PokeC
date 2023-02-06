#ifndef SHARED_HPP
#define SHARED_HPP

#define MAP_X              80
#define MAP_Y              21
#define WORLD_CENTER_X     200
#define WORLD_CENTER_Y     200
#define WORLD_DIM          401
#define MIN_TRAINERS       7   
#define ADD_TRAINER_PROB   50

struct Coordinate
{
    int x;
    int y;
};

enum Terrain
{
	EMPTY,
	FOREST,
	MOUNTAIN,
	TREE,
	CLEARING,
	LONG_GRASS,
	BORDER,
	ROAD,
	EXIT,
	POKEMART,
	POKECENTER,
	NUM_TERRAIN
};

enum Direction
{
	NONE = -1,
	UPPER_LEFT,
	UP,
	UPPER_RIGHT,
	LEFT,
	RIGHT,
	BOTTOM_LEFT,
	BOTTOM,
	BOTTOM_RIGHT,
	NUM_DIRECTIONS
};

enum MovementType
{
	MOVE_PLAYER,
	MOVE_HIKER,
	MOVE_RIVAL,
	MOVE_PACER,
    MOVE_WANDERER,
    MOVE_SENTRY,
    MOVE_EXPLORER,
	NUM_MOVE_TYPES
};

enum CharacterType
{
    PLAYER,
	HIKER,
	RIVAL,
	PACER,
	SENTRY,
	WANDERER,
	EXPLORER,
	NUM_CHARACTERS
};

#endif