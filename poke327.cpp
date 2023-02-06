#include<iostream>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<limits.h>
#include<ncurses.h>

#include "world.hpp"
#include "character.hpp"
#include "interface.hpp"
#include "heap.h"

struct PathNode
{
	heap_node_t *heap_node;
	Coordinate pos;
};

typedef Coordinate move_char(Character *t);

static int directions[NUM_DIRECTIONS][2] = {
	{ -1, -1 }, // upper left
	{ 0, -1 }, // up
	{ 1, -1 }, // upper right
	{ -1, 0 }, // left
	{ 1, 0 }, // right
	{ -1, 1 }, // bottom left
	{ 0, 1 }, // bottom
	{ 1, 1 } // bottom right
};

// Global world variable
World *world;

static int terrain_cost(int terrain, MovementType mt)
{
	// 4 player types, terrain types
	int terrain_cost_map[NUM_MOVE_TYPES][NUM_TERRAIN] = 
	{
		// EMPTY,  FOREST,  MOUNTAIN, TREE,    CLEARING, LONG_GRASS, BORDER,  ROAD,   EXIT,      POKEMART, POKECENTER
		{ INT_MAX, INT_MAX, INT_MAX,  INT_MAX, 10,       20,         INT_MAX, 10,     10,        10,       10 },     // PC
		{ INT_MAX, 15,      15,       INT_MAX, 10,       15,         INT_MAX, 10,     INT_MAX,   50,       50 },     // Hiker
		{ INT_MAX, INT_MAX, INT_MAX,  INT_MAX, 10,       20,         INT_MAX, 10,     INT_MAX,   50,       50 },     // Rival
		{ INT_MAX, INT_MAX, INT_MAX,  INT_MAX, 10,       20,         INT_MAX, 10,     INT_MAX,   50,       50 },     // Pacer
		{ INT_MAX, INT_MAX, INT_MAX,  INT_MAX, 10,       20,         INT_MAX, 10,     INT_MAX,   50,       50 },     // Wanderer
		{ INT_MAX, INT_MAX, INT_MAX,  INT_MAX, 10,       20,         INT_MAX, 10,     INT_MAX,   50,       50 },     // Sentry
		{ INT_MAX, INT_MAX, INT_MAX,  INT_MAX, 10,       20,         INT_MAX, 10,     INT_MAX,   50,       50 }      // Explorer
	};
	return terrain_cost_map[mt][terrain];
}

Direction randDir() 
{     
  int d = rand() & 0x7;    
  return (Direction)d;
}

static int hiker_compare(const void *pn1, const void *pn2)
{
  return (world->hiker_dist[((PathNode *)pn1)->pos.y][((PathNode *)pn1)->pos.x] -
		  world->hiker_dist[((PathNode *)pn2)->pos.y][((PathNode *)pn2)->pos.x]);
}

static int rival_compare(const void *pn1, const void *pn2)
{
  return (world->rival_dist[((PathNode *)pn1)->pos.y][((PathNode *)pn1)->pos.x] -
		  world->rival_dist[((PathNode *)pn2)->pos.y][((PathNode *)pn2)->pos.x]);
}

void paths_to_player(World * world)
{
	Map *map = world->currentMap();
	heap_t heap;
	int x, y;
	PathNode path_node[MAP_Y][MAP_X], *current_node;

	// init arrays
	for (y = 0; y < MAP_Y; y++)
	{
		for (x = 0; x < MAP_X; x++)
		{
			path_node[y][x].pos.x = x;
			path_node[y][x].pos.y = y;
			path_node[y][x].heap_node = NULL;
			// set all distances to INT_MAX (infinity)
			world->hiker_dist[y][x] = world->rival_dist[y][x] = INT_MAX;
		}
	}

	// set player distance to 0
	Character *pc = world->getPlayer();
	world->hiker_dist[pc->getPos().y][pc->getPos().x] = world->rival_dist[pc->getPos().y][pc->getPos().x] = 0;

	heap_init(&heap, hiker_compare, NULL);

	for (y = 1; y < MAP_Y - 1; y++)
	{
		for (x = 1; x < MAP_X - 1; x++)
		{
			Coordinate p = { .x = x, .y = y };
			if (terrain_cost(map->getTerrain(p), MOVE_HIKER) != INT_MAX)
			{
				path_node[y][x].heap_node = heap_insert(&heap, &path_node[y][x]);
			}
		}
	}

	while ((current_node = static_cast<PathNode *>(heap_remove_min(&heap))))
	{
		// get terrain
		Terrain ter = map->getTerrain(current_node->pos);
		current_node->heap_node = NULL;
		// check upper left
		if ((path_node[current_node->pos.y - 1][current_node->pos.x - 1].heap_node) &&
			(world->hiker_dist[current_node->pos.y - 1][current_node->pos.x - 1] >
			world->hiker_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_HIKER)))
		{
			world->hiker_dist[current_node->pos.y - 1][current_node->pos.x - 1] =
				world->hiker_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_HIKER);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y - 1][current_node->pos.x - 1].heap_node);
		}
		// check up
		if ((path_node[current_node->pos.y - 1][current_node->pos.x].heap_node) &&
			(world->hiker_dist[current_node->pos.y - 1][current_node->pos.x] >
			world->hiker_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_HIKER)))
		{
			world->hiker_dist[current_node->pos.y - 1][current_node->pos.x] =
				world->hiker_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_HIKER);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y - 1][current_node->pos.x].heap_node);
		}
		// check upper right
		if ((path_node[current_node->pos.y - 1][current_node->pos.x + 1].heap_node) &&
			(world->hiker_dist[current_node->pos.y - 1][current_node->pos.x + 1] >
			world->hiker_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_HIKER)))
		{
			world->hiker_dist[current_node->pos.y - 1][current_node->pos.x + 1] =
				world->hiker_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_HIKER);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y - 1][current_node->pos.x + 1].heap_node);
		}
		// check left
		if ((path_node[current_node->pos.y][current_node->pos.x - 1].heap_node) &&
			(world->hiker_dist[current_node->pos.y][current_node->pos.x - 1] >
			world->hiker_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_HIKER)))
		{
			world->hiker_dist[current_node->pos.y][current_node->pos.x - 1] =
				world->hiker_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_HIKER);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y][current_node->pos.x - 1].heap_node);
		}
		// check right
		if ((path_node[current_node->pos.y][current_node->pos.x + 1].heap_node) &&
			(world->hiker_dist[current_node->pos.y][current_node->pos.x + 1] >
			world->hiker_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_HIKER)))
		{
			world->hiker_dist[current_node->pos.y][current_node->pos.x + 1] =
				world->hiker_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_HIKER);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y][current_node->pos.x + 1].heap_node);
		}
		// check bottom left
		if ((path_node[current_node->pos.y + 1][current_node->pos.x - 1].heap_node) &&
			(world->hiker_dist[current_node->pos.y + 1][current_node->pos.x - 1] >
			world->hiker_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_HIKER)))
		{
			world->hiker_dist[current_node->pos.y + 1][current_node->pos.x - 1] =
				world->hiker_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_HIKER);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y + 1][current_node->pos.x - 1].heap_node);
		}
		// check bottom
		if ((path_node[current_node->pos.y + 1][current_node->pos.x].heap_node) &&
			(world->hiker_dist[current_node->pos.y + 1][current_node->pos.x] >
			world->hiker_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_HIKER)))
		{
			world->hiker_dist[current_node->pos.y + 1][current_node->pos.x] =
				world->hiker_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_HIKER);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y + 1][current_node->pos.x].heap_node);
		}
		// check bottom right
		if ((path_node[current_node->pos.y + 1][current_node->pos.x + 1].heap_node) &&
			(world->hiker_dist[current_node->pos.y + 1][current_node->pos.x + 1] >
			world->hiker_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_HIKER)))
		{
			world->hiker_dist[current_node->pos.y + 1][current_node->pos.x + 1] =
				world->hiker_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_HIKER);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y + 1][current_node->pos.x + 1].heap_node);
		}
	}
	heap_delete(&heap);

	// now we do the same for the rival

	heap_init(&heap, rival_compare, NULL);

	for (y = 1; y < MAP_Y - 1; y++)
	{
		for (x = 1; x < MAP_X - 1; x++)
		{
			Coordinate p = { .x = x, .y = y };
			if (terrain_cost(map->getTerrain(p), MOVE_RIVAL) != INT_MAX)
			{
				path_node[y][x].heap_node = heap_insert(&heap, &path_node[y][x]);
			}
			else
			{
				path_node[y][x].heap_node = NULL;
			}
		}
	}

	while ((current_node = static_cast<PathNode *>(heap_remove_min(&heap))))
	{
		// get terrain 
		Terrain ter = map->getTerrain(current_node->pos);
		current_node->heap_node = NULL;
		// check upper left
		if ((path_node[current_node->pos.y - 1][current_node->pos.x - 1].heap_node) &&
			(world->rival_dist[current_node->pos.y - 1][current_node->pos.x - 1] >
			world->rival_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_RIVAL)))
		{
			world->rival_dist[current_node->pos.y - 1][current_node->pos.x - 1] =
				world->rival_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_RIVAL);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y - 1][current_node->pos.x - 1].heap_node);
		}
		// check upper middle
		if ((path_node[current_node->pos.y - 1][current_node->pos.x].heap_node) &&
			(world->rival_dist[current_node->pos.y - 1][current_node->pos.x] >
			world->rival_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_RIVAL)))
		{
			world->rival_dist[current_node->pos.y - 1][current_node->pos.x] =
				world->rival_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_RIVAL);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y - 1][current_node->pos.x].heap_node);
		}
		// check upper right
		if ((path_node[current_node->pos.y - 1][current_node->pos.x + 1].heap_node) &&
			(world->rival_dist[current_node->pos.y - 1][current_node->pos.x + 1] >
			world->rival_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_RIVAL)))
		{
			world->rival_dist[current_node->pos.y - 1][current_node->pos.x + 1] =
				world->rival_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_RIVAL);

			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y - 1][current_node->pos.x + 1].heap_node);
		}
		// check left
		if ((path_node[current_node->pos.y][current_node->pos.x - 1].heap_node) &&
			(world->rival_dist[current_node->pos.y][current_node->pos.x - 1] >
			world->rival_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_RIVAL)))
		{
			world->rival_dist[current_node->pos.y][current_node->pos.x - 1] =
				world->rival_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_RIVAL);

			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y][current_node->pos.x - 1].heap_node);
		}
		// check right
		if ((path_node[current_node->pos.y][current_node->pos.x + 1].heap_node) &&
			(world->rival_dist[current_node->pos.y][current_node->pos.x + 1] >
			world->rival_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_RIVAL)))
		{
			world->rival_dist[current_node->pos.y][current_node->pos.x + 1] =
				world->rival_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_RIVAL);
			
			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y][current_node->pos.x + 1].heap_node);
		}
		// check bottom left
		if ((path_node[current_node->pos.y + 1][current_node->pos.x - 1].heap_node) &&
			(world->rival_dist[current_node->pos.y + 1][current_node->pos.x - 1] >
			world->rival_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_RIVAL)))
		{
			world->rival_dist[current_node->pos.y + 1][current_node->pos.x - 1] =
				world->rival_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_RIVAL);

			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y + 1][current_node->pos.x - 1].heap_node);
		}
		// check bottom
		if ((path_node[current_node->pos.y + 1][current_node->pos.x].heap_node) &&
			(world->rival_dist[current_node->pos.y + 1][current_node->pos.x] >
			world->rival_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_RIVAL)))
		{
			world->rival_dist[current_node->pos.y + 1][current_node->pos.x] =
				world->rival_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_RIVAL);

			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y + 1][current_node->pos.x].heap_node);
		}
		// check bottom right
		if ((path_node[current_node->pos.y + 1][current_node->pos.x + 1].heap_node) &&
			(world->rival_dist[current_node->pos.y + 1][current_node->pos.x + 1] >
			world->rival_dist[current_node->pos.y][current_node->pos.x] + terrain_cost(ter, MOVE_RIVAL)))
		{
			world->rival_dist[current_node->pos.y + 1][current_node->pos.x + 1] =
				world->rival_dist[current_node->pos.y][current_node->pos.x] +
				terrain_cost(ter, MOVE_RIVAL);

			heap_decrease_key_no_replace(&heap, path_node[current_node->pos.y + 1][current_node->pos.x + 1].heap_node);
		}
	}
	heap_delete(&heap);
}

void battle(NPC *npc)
{
	// Clear the screen
	// clear();
	// int finished = 0;
	// while (!finished)
	// {
	// 	mvprintw(0, 0, "BATTLE!");
	// 	// Get user input 
	// 	int input = getch();
	// 	// If the escape key is pressed, finish battle 
	// 	if (input == 27)
	// 	{
	// 		npc->setDefeated(true);
	// 		npc->setMovementType(MOVE_WANDERER);
	// 		// Clear battle message
	// 		clear();
	// 		// End battle
	// 		finished = 1;
	// 	}
	// }
}

void enter_building()
{
	Coordinate pos = world->getPlayer()->getPos();
	// if player is not on a building, exit the function
	if ((world->currentMap()->getTerrain(pos) != POKEMART) && (world->currentMap()->getTerrain(pos) != POKECENTER))
	{
		return;
	}
	// Clear the screen
	clear();
	int exit = 0;
	while (!exit)
	{
		if (world->currentMap()->getTerrain(pos) == POKECENTER)
		{
			mvprintw(0, 0, "Welcome to the PokeCenter! *healing noises*");
		}
		else
		{
			mvprintw(0, 0, "Welcome to the PokeMart! *shopping cart sounds*");
		}
		// Get user input 
		int input = getch();
		// If the left arrow is pressed, leave building 
		if (input == '<')
		{
			// Clear building greeting message
			clear();
			// Leave building
			exit = 1;
		}
	}
}

static Coordinate move_player_dir(Character *c, Direction dir)
{
	Coordinate cur = c->getPos();
	Coordinate dest = { 
		.x = cur.x + directions[dir][0],
		.y = cur.y + directions[dir][1]
	};

	if (world->currentMap()->getTerrain(dest) == EXIT)
	{
		Coordinate new_map = world->getPos();
		world->currentMap()->removeTrainer(cur);
		if (dest.x == 0)
		{
			// left
			new_map.x--;
			world->visitMap(new_map);
			dest.x = MAP_X - 2;
		}
		else if (dest.x == (MAP_X - 1))
		{
			// right
			new_map.x++;
			world->visitMap(new_map);
			dest.x = 1;
		}
		else if (dest.y == 0)
		{
			// up
			new_map.y--;
			world->visitMap(new_map);
			dest.y = MAP_Y - 2;
		}
		else if (dest.y == (MAP_Y - 1))
		{
			// down
			new_map.y++;
			world->visitMap(new_map);
			dest.y = 1;
		}
		return dest;
	}
	// impassable terrain check
	if (terrain_cost(world->currentMap()->getTerrain(dest), MOVE_PLAYER) == INT_MAX)
	{
		ErrorLogger::log(IMPASSABLE_TERRAIN);
		// return unsuccessful move
		return cur;
	}
	// check if current cell is occupied by a trainer and if they've been defeated already or not 
	if (world->currentMap()->checkForTrainer(dest) && !(dynamic_cast<NPC *>(world->currentMap()->getTrainer(dest)))->isDefeated())
	{
		// battle between pc and npc
		battle((dynamic_cast<NPC *>(world->currentMap()->getTrainer(dest))));
		// return unsuccessful move
		return cur;
	}
	// move was successful
	return dest;
}

static Coordinate move_player(Character *c)
{
	world->printMap();

	Coordinate dest;

	int input = getch();
	switch (input)
	{
		case '7':
		case 'y':
		{
			dest = move_player_dir(c, UPPER_LEFT);
			break;
		}
		case '8':
		case 'k':
		{
			dest = move_player_dir(c, UP);
			break;
		}
		case '9':
		case 'u':
		{
			dest = move_player_dir(c, UPPER_RIGHT);					
			break;
		}
		case '6':
		case 'l':
		{
			dest = move_player_dir(c, RIGHT);					
			break;
		}
		case '3':
		case 'n':
		{
			dest = move_player_dir(c, BOTTOM_RIGHT);					
			break;
		}
		case '2':
		case 'j':
		{
			dest = move_player_dir(c, BOTTOM);					
			break;
		}
		case '1':
		case 'b':
		{
			dest = move_player_dir(c, BOTTOM_LEFT);					
			break;
		}
		case '4':
		case 'h':
		{
			dest = move_player_dir(c, LEFT);				
			break;
		}
		case '5':
		case ' ':
		case '.':
		{
			clear();
			world->printMap();
			break;
		}
		case '>':
		{
			dest = c->getPos();
			enter_building();
			break;
		}
		case 't':
		{			
			clear();
			// print_trainer_list_control(win, num_trainers);
			break;
		}
		case 'Q':
		{
			world->quitWorld();
			break;
		}
		case 'f': 
		{
			Coordinate visit;
			scanf(" %d %d", &(visit.x), &(visit.y));
			//mvprintw(0, 0, "Visit x: %d, y: %d", visit.x, visit.y);
			visit.x += 200;
			if (visit.y > 0 )
			{
				visit.y = 200 - visit.y;
			} else
			{
				visit.y = 200 + abs(visit.y);
			}
			if(!world->visitMap(visit))
			{
				ErrorLogger::log(DEFAULT);
			}
			dest = c->getPos();
			break;
		}
		default:
		{
			ErrorLogger::log(INVALID_KEY);
			dest = c->getPos();
			break;
		}
	}

	refresh();

	return dest;
}

static Coordinate move_hiker(Character *c)
{
  int min;
  int base;
  int i;

  NPC *n = dynamic_cast<NPC *>(c);

  base = rand() & 0x7;

  Coordinate dest = c->getPos();
  Coordinate pos = c->getPos();
  min = INT_MAX;
  
  for (i = base; i < 8 + base; i++) 
  {
	Coordinate dir = { 
		.x = pos.x + directions[i & 0x7][0], 
		.y = pos.y + directions[i & 0x7][1] 
	};
    if ((world->hiker_dist[pos.y + directions[i & 0x7][1]]
                         [pos.x + directions[i & 0x7][0]] <=
         min) && !world->currentMap()->checkForTrainer(dir)) 
	{
		dest = dir;
      	min = world->hiker_dist[dest.y][dest.x];
    }
	// player
    if (world->hiker_dist[dest.y][dest.x] == 0) 
	{
      battle(n);
      break;
    }
  }
  return dest;
}

static Coordinate move_rival(Character *c)
{
	int min;
	int base;
	int i;

	NPC *n = dynamic_cast<NPC *>(c);

	base = rand() & 0x7;

	Coordinate dest = c->getPos();
	Coordinate pos = c->getPos();
	min = INT_MAX;

	for (i = base; i < 8 + base; i++) 
	{
		Coordinate dir = { 
			.x = pos.x + directions[i & 0x7][0], 
			.y = pos.y + directions[i & 0x7][1] 
		};
		if ((world->rival_dist[pos.y + directions[i & 0x7][1]]
								[pos.x + directions[i & 0x7][0]] <=
				min) && !world->currentMap()->checkForTrainer(dir)) 
		{
			dest = dir;
			min = world->rival_dist[dest.y][dest.x];
		}
		// player
		if (world->rival_dist[dest.y][dest.x] == 0) 
		{
			battle(n);
			break;
		}
	}
	return dest;
}

static Coordinate move_pacer(Character *c)
{
	NPC *n = dynamic_cast<NPC *>(c);
	Coordinate dest = c->getPos();
	Coordinate dir = { 
		.x = dest.x + directions[n->getDirection()][0],
		.y = dest.y + directions[n->getDirection()][1]
	};

	if (!n->isDefeated() && world->currentMap()->checkForTrainer(dir, PLAYER)) {
		battle(n);
		return dest;
	}

	if ((world->currentMap()->getTerrain(dir) != world->currentMap()->getTerrain(c->getPos())) ||
	     world->currentMap()->checkForTrainer(dir)) 
	{
		Direction d = (Direction)((NUM_DIRECTIONS - 1) - n->getDirection());
		n->setDirection(d);
	}

	if (world->currentMap()->getTerrain(dir) == world->currentMap()->getTerrain(c->getPos()) &&
		!world->currentMap()->checkForTrainer(dir))
	{
		dest = dir;
	}

	return dest;
}

static Coordinate move_wanderer(Character *c)
{
	NPC *n = dynamic_cast<NPC *>(c);
	Coordinate dest = c->getPos();
	Coordinate dir = { 
		.x = dest.x + directions[n->getDirection()][0],
		.y = dest.y + directions[n->getDirection()][1]
	};

	if (!n->isDefeated() && world->currentMap()->checkForTrainer(dir, PLAYER)) {
		battle(n);
		return dest;
	}

    if ((world->currentMap()->getTerrain(dir) != world->currentMap()->getTerrain(c->getPos())) ||
  	   world->currentMap()->checkForTrainer(dir))
	{
		// go in random direction
		Direction d = randDir();
		dir.x = dest.x + directions[d][0];
		dir.y = dest.y + directions[d][1];
	}

	if (world->currentMap()->getTerrain(dir) == world->currentMap()->getTerrain(c->getPos()) &&
		!world->currentMap()->checkForTrainer(dir))
	{
		dest = dir;
	}

	return dest;
}

static Coordinate move_sentry(Character *c)
{
	// Sentries do not move
	return c->getPos();
}

static Coordinate move_explorer(Character *c)
{
	NPC *n = dynamic_cast<NPC *>(c);
	Coordinate dest = c->getPos();
	Coordinate dir = { 
		.x = dest.x + directions[n->getDirection()][0],
		.y = dest.y + directions[n->getDirection()][1]
	};

	if (!n->isDefeated() && world->currentMap()->checkForTrainer(dir, PLAYER)) {
		battle(n);
		return dest;
	}

    if ((terrain_cost(world->currentMap()->getTerrain(dir), c->getMovementType()) == INT_MAX) ||
  	   world->currentMap()->checkForTrainer(dir))
	{
		// go in random direction
		Direction d = randDir();
		dir.x = dest.x + directions[d][0];
		dir.y = dest.y + directions[d][1];
	}

	if ((terrain_cost(world->currentMap()->getTerrain(dir), c->getMovementType()) != INT_MAX) &&
		!world->currentMap()->checkForTrainer(dir))
	{
		dest = dir;
	}

	return dest;
}

static move_char *move_table[NUM_MOVE_TYPES] = {
	move_player,
	move_hiker,
	move_rival,
	move_pacer,
	move_wanderer,
	move_sentry,
	move_explorer
};

static void start_map_color()
{
	start_color();
	init_pair(CLEARING_PAIR, COLOR_YELLOW, COLOR_GREEN);
	init_pair(GRASS_PAIR, COLOR_YELLOW, COLOR_GREEN);
	init_pair(MOUNTAIN_PAIR, COLOR_BLACK, COLOR_WHITE);
	init_pair(TREE_PAIR, COLOR_RED, COLOR_GREEN);
	init_pair(ROAD_PAIR, COLOR_BLACK, COLOR_GREEN);
	init_pair(POKECENTER_PAIR, COLOR_WHITE, COLOR_RED);
	init_pair(POKEMART_PAIR, COLOR_WHITE, COLOR_BLUE);
	init_pair(EXIT_PAIR, COLOR_RED, COLOR_WHITE);
}

int main(int argc, char *argv[])
{
	// seed random gen - IMPORTANT: MUST BE BEFORE WORLD INIT
	srand(time(NULL));

	// default number of players to 10
	int num_trainers = MIN_TRAINERS;

	// parse command line arguments
	if (argc > 1)
	{
		if(!(strcmp(argv[1], "--numtrainers")))
		{
			num_trainers = atoi(argv[2]);
		}
	}

	world = new World(num_trainers);

	// WINDOW *win;
	/* Determine terminal type and it's features like size */
	initscr();
	/* Do not echo things such as user key presses back to the screen */
	noecho();
	/* Disable line buffering and instead make characters instantly available to the program */
	raw();
	/* Enable reading of keys like F1, F2 and arrow keys */
	keypad(stdscr, TRUE);
	/* Enable colors for map */
	start_map_color();

	Character *c;
	Coordinate dest;

	while (!world->quit())
	{
		c = static_cast<Character *>(world->currentMap()->getHeap()->extractMin());
		// mvprintw(0, 0, "Character: %c", c->getIcon());
		dest = move_table[c->getMovementType()](c);

		world->currentMap()->removeTrainer(c->getPos());
		world->currentMap()->addTrainer(c, dest);

		if (c->getType() == PLAYER) {
			paths_to_player(world);
		}

		c->increaseTime(terrain_cost(world->currentMap()->getTerrain(dest), c->getMovementType()));

		c->setPos(dest);

		world->currentMap()->getHeap()->insert(c);
	}

	endwin();

	delete world;

    return 0;
}