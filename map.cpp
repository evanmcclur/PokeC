#include<ncurses.h>
#include "util.hpp"
#include "map.hpp"
#include "util.hpp"

/****************************************/
/*********    Extra Functions    ********/
/****************************************/

static int turn_compare(const void *turn1, const void *turn2)
{
	return (((Character *) turn1)->getTime() - ((Character *) turn2)->getTime());
}

/****************************************/
/*********   Class Definitions   ********/
/****************************************/

Map::Map()
{
    // row, col
	int i, j;
	for (i = 0; i < MAP_Y; i++) {
		for (j = 0; j < MAP_X; j++) {
			if ((i == 0 || j == 0 || i == MAP_Y - 1 || j == MAP_X - 1)) {
				terrain[i][j] = BORDER;
			} else {
				terrain[i][j] = EMPTY;
			}
			// init trainer map to null
			tmap[i][j] = NULL;
		}
	}
	turn = new MinHeap(turn_compare, NULL);
}

Map::~Map()
{
    // row, col
	int i, j;
	for (i = 0; i < MAP_Y; i++) {
		for (j = 0; j < MAP_X; j++) {
			if (tmap[i][j] != NULL) 
			{
				delete[] tmap[i][j];
				tmap[i][j] = NULL;
			}
		}
	}
	delete turn;
}

void Map::growRegion(int x, int y, int size, int placed, int terrain_type)
{
    // get value at position
	int map_value = terrain[y][x];
	if (map_value != EMPTY)
	{
		if (placed == 0)
		{
			// display_message("Placed nothing | Func: grow_region");
		}
		return;
	}
	else if (placed < size && (x > 0 && x < MAP_X - 1) && (y > 0 && y < MAP_Y - 1))
	{
	    terrain[y][x] = terrain_type; // given position (middle)
		placed++;
		growRegion(x - 1, y + 1, size, placed, terrain_type); // top left
		growRegion(x, y + 1, size, placed, terrain_type); // top middle
		growRegion(x + 1, y + 1, size, placed, terrain_type); // top right
		growRegion(x - 1, y, size, placed, terrain_type); // middle left
		growRegion(x + 1, y, size, placed, terrain_type); // middle right
		growRegion(x - 1, y - 1, size, placed, terrain_type); // bottom left
		growRegion(x, y - 1, size, placed, terrain_type); // bottom middle
		growRegion(x + 1, y - 1, size, placed, terrain_type); // bottom right
	}
}

void Map::generateMapTerrain(int initial, Coordinate pos, Coordinate ns_exit, Coordinate ew_exit)
{
    int regions[5] = { LONG_GRASS, LONG_GRASS, LONG_GRASS, FOREST, MOUNTAIN };
	Util::shuffle(regions, 5);

	Coordinate top_left =  { .x = Util::randrng(5, 1), .y = Util::randrng(3, 1) };
	Coordinate bottom_left = { .x = Util::randrng(10, 5), .y = Util::randrng(19, 14) };
	Coordinate middle_top =  { .x = Util::randrng(39, 34), .y = Util::randrng(3, 1) };
	Coordinate top_right = { .x = Util::randrng(MAP_X - 2, MAP_X - 5), .y = Util::randrng(3, 1) };
	Coordinate bottom_right = { .x = Util::randrng(MAP_X - 2, MAP_X - 6), .y = Util::randrng(19, 14) };

	Coordinate region_cds[5] = {top_left, top_right, bottom_left, bottom_right, middle_top};

	for (int i = 0; i < 5; i++)
	{
		int region = regions[i];
		Coordinate rc = region_cds[i];
		int size = Util::randrng(15, 10);
		growRegion(rc.x, rc.y, size, 0, region);
	}

	for (int i = 1; i < MAP_Y - 1; i++)
	{
		for (int j = 1; j < MAP_X - 1; j++)
		{
			if (terrain[i][j] == EMPTY)
			{
				int p = rand() % 100;
				if (p < 90)
				{
					terrain[i][j] = CLEARING;
				}
				else
				{
					terrain[i][j] = TREE;
				}
			}
		}
	}

    // Place exits
    terrain[0][ns_exit.x] = EXIT;
	terrain[MAP_Y - 1][ns_exit.x] = EXIT;
	terrain[ew_exit.y][0] = EXIT;
	terrain[ew_exit.y][MAP_X - 1] = EXIT;

	// We do MAP_Y - 2 here to avoid replacing the exits
	Coordinate ns_road_end_coords = { ns_exit.x, MAP_Y - 2 };
	Coordinate ew_road_end_coords = { MAP_X - 2, ew_exit.y };

	connectRoad(ns_exit, ns_road_end_coords);
	connectRoad(ew_exit, ew_road_end_coords);

	int distance = abs(200 - pos.x) + abs(200 - pos.y);
	int chance = (((-45 * distance) / 200) + 50);

	int rnd_chance = Util::randrng(100, 0);

	if (initial || rnd_chance < chance)
	{
		placeBuildings(ns_exit, ew_exit);
	}
}

void Map::connectRoad(Coordinate start, Coordinate end)
{
    if (start.x < 0 || start.x > MAP_X - 1 || start.y < 0 || start.y > MAP_Y - 1)
		return;

	if (start.x == end.x && start.y == end.y) {
		// we've reached the end of the road
		return;
	} else if (start.x == end.x) {
		// this is for north-south roads
		terrain[start.y + 1][start.x] = ROAD;
		start.y += 1;
		connectRoad(start, end);
	} else if (start.y == end.y) {
		// this is for east-west roads
		terrain[start.y][start.x + 1] = ROAD;
		start.x += 1;
		connectRoad(start, end);
	} else {
		// error ?
		// display_message("ERROR: Unhandled case | Func: connect_road");
	}
}

void Map::placeBuildings(Coordinate ns_exit, Coordinate ew_exit)
{
    // pick random seed position for pokemart and connect to nearest road
	int x1 = (MAP_X - ns_exit.x);
	int y1 = (MAP_Y - ew_exit.y);
	int pmpc_x, pmpc_y;
	
	if (x1 > ns_exit.x)
	{
		pmpc_x = Util::randrng(MAP_X - 5, ns_exit.x + 5);
	}
	else
	{
		pmpc_x = Util::randrng(ns_exit.x - 2, 1);
	}

	if (y1 > ew_exit.y)
	{
		pmpc_y = Util::randrng(MAP_Y - 5, ew_exit.y + 5);
	}
	else
	{
		pmpc_y = Util::randrng(ew_exit.y - 5, 1);
	}

	// poke mart
	terrain[pmpc_y][pmpc_x] = POKEMART;
	terrain[pmpc_y][pmpc_x + 1] = POKEMART;
	// road in between
	terrain[pmpc_y + 1][pmpc_x] = ROAD;
	terrain[pmpc_y + 1][pmpc_x + 1] = ROAD;

	// Coordinates pmpc_start = { .x = pmpc_x, .y = pmpc_y + 1 };
	// Coordinates ns_exit_connect = { .x = ns_exit.x, .y = pmpc_y + 1 };

	int increment;
	int current_x = pmpc_x;
	int current_y = pmpc_y + 1;
	if (ns_exit.x > pmpc_x)
	{
		current_x += 2;
		increment = 1;
	}
	else
	{
		current_x -= 1;
		increment = -1;
	}
	
	while (terrain[current_y][current_x] != ROAD)
	{
		terrain[current_y][current_x] = ROAD;
		current_x += increment;
	}

	// poke center
	terrain[pmpc_y + 2][pmpc_x] = POKECENTER;
	terrain[pmpc_y + 2][pmpc_x + 1] = POKECENTER;
}

Terrain Map::getTerrain(Coordinate pos) const
{
    return (Terrain)terrain[pos.y][pos.x];
}

void Map::addTrainer(Character * trainer, Coordinate pos)
{
	tmap[pos.y][pos.x] = trainer;
}

void Map::removeTrainer(Coordinate pos)
{
	tmap[pos.y][pos.x] = NULL;
}

Character * Map::getTrainer(Coordinate pos) const
{
    return tmap[pos.y][pos.x];
}

bool Map::checkForTrainer(Coordinate pos) const
{
    if (tmap[pos.y][pos.x] != NULL)
    {
        return true;
    }
    return false;
}

bool Map::checkForTrainer(Coordinate pos, CharacterType ctype) const
{
	Character *c = tmap[pos.y][pos.x];
    if (c != NULL)
    {
        return c->getType() == ctype;
    }
    return false;
}

MinHeap * Map::getHeap()
{
	return turn;
}