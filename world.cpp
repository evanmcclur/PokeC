#include<ncurses.h>
#include<limits.h>
#include "util.hpp"
#include "world.hpp"
#include "interface.hpp"
#include "character.hpp"
#include "util.hpp"

/****************************************/
/*********   Class Definitions   ********/
/****************************************/

// Private Method Definitions

void World::newHiker()
{
	Coordinate pos;
	NPC *c;

	do {
		pos = Util::randpos();
	} while (hiker_dist[pos.y][pos.x] == INT_MAX ||
			 curmap->checkForTrainer(pos)        ||
			 pos.x < 3 || pos.x > MAP_X - 4      ||
			 pos.y < 3 || pos.y > MAP_Y - 4);

	c = new NPC('h', pos, NONE, HIKER, MOVE_HIKER);
	curmap->getHeap()->insert(c);
	// heap_insert(curmap->getHeap(), c);
	curmap->addTrainer(c, pos);
}

void World::newRival()
{
	Coordinate pos;
	NPC *c;

	do {
		pos = Util::randpos();
	} while (rival_dist[pos.y][pos.x] == INT_MAX ||
			 rival_dist[pos.y][pos.x] < 0        ||
			 curmap->checkForTrainer(pos)        ||
			 pos.x < 3 || pos.x > MAP_X - 4      ||
			 pos.y < 3 || pos.y > MAP_Y - 4);

	// world.cur_map->cmap[pos.y][pos.x] = c = malloc(sizeof (*c));
	// c->npc = malloc(sizeof (*c->npc));
	c = new NPC('r', pos, NONE, RIVAL, MOVE_RIVAL);
	curmap->getHeap()->insert(c);
	// heap_insert(curmap->getHeap(), c);
	curmap->addTrainer(c, pos);
}

void World::newNPCOther()
{
	Coordinate pos;
	Direction dir;
	NPC *c;

	do {
		pos = Util::randpos();
	} while (rival_dist[pos.y][pos.x] == INT_MAX  ||
			 rival_dist[pos.y][pos.x] < 0         ||
			 curmap->checkForTrainer(pos)         ||
			 pos.x < 3 || pos.x > MAP_X - 4       ||
			 pos.y < 3 || pos.y > MAP_Y - 4);

	dir = Util::randdir();
	switch (rand() % 4) 
	{
	case 0:
	c = new NPC('p', pos, dir, PACER, MOVE_PACER);
	break;
	case 1:
	c = new NPC('w', pos, dir, WANDERER, MOVE_WANDERER);
	break;
	case 2:
	c = new NPC('s', pos, dir, SENTRY, MOVE_SENTRY);
	break;
	case 3:
	c = new NPC('e', pos, dir, EXPLORER, MOVE_EXPLORER);
	break;
	}
	// c->npc->defeated = 0;
	// c->pc = NULL;
	// c->next_turn = 0;
	curmap->getHeap()->insert(c);
	// heap_insert(curmap->getHeap(), c);
	curmap->addTrainer(c, pos);
	// world.cur_map->cmap[pos.y][pos.x] = c;
}

void World::placeNPCS(int numTrainers)
{
	if (numTrainers < MIN_TRAINERS) 
		numTrainers = MIN_TRAINERS;

	int count = 0;

	//Always place a hiker and a rival, then place a random number of others
	newHiker();
	newRival();
	do {
		//higher probability of non- hikers and rivals
		switch(rand() % 10) {
		case 0:
		newHiker();
		break;
		case 1:
		newRival();
		break;
		default:
		newNPCOther();
		break;
		}
		/* Game attempts to continue to place trainers until the probability *
		* roll fails, but if the map is full (or almost full), it's         *
		* impossible (or very difficult) to continue to add, so we abort if *
		* we've tried MAX_TRAINER_TRIES times.                              */
	} while (++count < numTrainers ||
			((rand() % 100) < ADD_TRAINER_PROB));
}

// Public Method Definitions

World::World(int numTrainers)
{
    int i, j;
    for (i = 0; i < 401; i++)
	{
		for (j = 0; j < 401; j++)
		{
            mmap[i][j] = NULL;
		}
	}

    Coordinate origin = { .x = 200, .y = 200 };
	// generate random exit spots
	int ns_road_start = Util::randrng(MAP_X - 5, 5);
	int ew_road_start = Util::randrng(MAP_Y - 5, 5);
	// create coordinates
	Coordinate ns_road_start_coords = { .x = ns_road_start, .y = 0 };
	Coordinate ew_road_start_coords = { .x = 0, .y = ew_road_start };
    // set exit positions
    ns_exit = ns_road_start_coords;
    ew_exit = ew_road_start_coords;
	// "visit" origin map - this will set the curmap
	if(!visitMap(origin)) 
	{
		ErrorLogger::log(DEFAULT);
	}

	Coordinate pc_loc;
	do
	{
		pc_loc.x = Util::randrng(MAP_X - 2, 2);
		pc_loc.y = Util::randrng(MAP_Y - 2, 2);
	} while (curmap->getTerrain(pc_loc) != ROAD);

    // create player and place at origin
    pc = new Player('@', pc_loc, PLAYER, MOVE_PLAYER);
	wquit = false;
	trainers = numTrainers;

	// add pc to origin map
	curmap->addTrainer(pc, pc_loc);
	// add pc to current map turn heap
	curmap->getHeap()->insert(pc);
}

World::~World()
{
    int i, j;
	for (i = 0; i < 401; i++)
	{
		for (j = 0; j < 401; j++)
		{
			Map *m = mmap[i][j];
			if (m != NULL)
			{
				delete[] m;
				mmap[i][j] = NULL;
			}
		}
	}

	if (curmap != NULL)
	{
		delete[] curmap;
	}
	
	if (pc != NULL)
	{
		delete[] pc;
	}
}

Coordinate World::getPos() const
{
	return world_pos;
}

Character * World::getPlayer() const
{
	return pc;
}

Map * World::currentMap()
{
    return curmap;
}
 
bool World::visitMap(Coordinate coords)
{
	if (!(coords.x >= 0 && coords.x < 401 && coords.y >= 0 && coords.y < 401))
	{
		// display_message("Error: attempted to go out of map bounds | Func: visit_map");
		return false;
	}

	if (mmap[coords.x][coords.y] == NULL)
	{
		Map *map = new Map;
		int origin = 0;

		if (map == NULL)
		{
			ErrorLogger::log(MEMORY_ALLOC_FAIL);
			return false;
		}

		if (coords.x == WORLD_CENTER_X && coords.y == WORLD_CENTER_Y)
		{
			origin = 1;
		}

		map->generateMapTerrain(origin, coords, ns_exit, ew_exit);

        // update world map
		mmap[coords.x][coords.y] = map;
		curmap = map;
		world_pos = coords;

		placeNPCS(trainers);
	}
	else
	{
		curmap = mmap[coords.x][coords.y];
	}
	return true;
}

void World::printMap() const
{
    int x, y;
    // Create a local coordinate for current position which will be deleted when we leave scope
    Coordinate pos;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
            // update pos
            pos.x = x, pos.y = y;
			if (curmap->checkForTrainer(pos))
			{
				mvaddch(y + 1, x, curmap->getTrainer(pos)->getIcon());
			}
			else 
			{
				switch (curmap->getTerrain(pos))
				{
				case POKEMART:
					attron(COLOR_PAIR(POKEMART_PAIR));
					mvaddch(y + 1, x, 'M');
					attroff(COLOR_PAIR(POKEMART_PAIR));
					break;
				case POKECENTER:
					attron(COLOR_PAIR(POKECENTER_PAIR));
					mvaddch(y + 1, x, 'C');
					attroff(COLOR_PAIR(POKECENTER_PAIR));
					break;
				case MOUNTAIN:
				case BORDER:
					attron(COLOR_PAIR(MOUNTAIN_PAIR));
					mvaddch(y + 1, x, '%');
					attroff(COLOR_PAIR(MOUNTAIN_PAIR));
					break;
				case ROAD:
					attron(COLOR_PAIR(ROAD_PAIR));
					mvaddch(y + 1, x, '#');
					attroff(COLOR_PAIR(ROAD_PAIR));
					break;
				case EXIT:
					attron(COLOR_PAIR(EXIT_PAIR));
					mvaddch(y + 1, x, '#');
					attroff(COLOR_PAIR(EXIT_PAIR));
					break;
				case CLEARING:
					attron(COLOR_PAIR(CLEARING_PAIR));
					mvaddch(y + 1, x, '.');
					attroff(COLOR_PAIR(CLEARING_PAIR));
					break;
				case FOREST:
				case TREE:
					attron(COLOR_PAIR(TREE_PAIR));
					mvaddch(y + 1, x, '^');
					attroff(COLOR_PAIR(TREE_PAIR));
					break;
				case LONG_GRASS:
					attron(COLOR_PAIR(CLEARING_PAIR));
					mvaddch(y + 1, x, ':');
					attroff(COLOR_PAIR(CLEARING_PAIR));
					break;
				default:
					mvaddch(y + 1, x, '?');
					break;
				}
			}
		}
	}
}

bool World::quit() const 
{
	return wquit;
}

void World::quitWorld()
{
	wquit = true;
}