#ifndef WORLD_HPP
#define WORLD_HPP

#include "map.hpp"

class World
{
    private:
        int trainers;
        bool wquit;
        Coordinate world_pos;
        Coordinate ns_exit;
        Coordinate ew_exit;
        Character *pc;
        Map *curmap;
        Map *mmap[WORLD_DIM][WORLD_DIM];
        void newHiker();
        void newRival();
        void newNPCOther();
    public:
        World(int numTrainers);
        ~World();
        /* Data */
        int hiker_dist[MAP_Y][MAP_X];
        int rival_dist[MAP_Y][MAP_X];
        /* Methods */
        Coordinate getPos() const;
        /**
         * @brief Gets the terrain at the given position on the current map
         * 
         * @param pos The position
         * @return Terrain - at position
         */
        Terrain getTerrain(Coordinate pos);
        /**
         * @brief Gets the pc
         * 
         * @return Character* 
         */
        Character * getPlayer() const;
        /**
         * @brief Prints the current map to the console
         * 
         */
        void printMap() const;
        /**
         * @brief Gets the current map
         * 
         * @return Map* 
         */
        Map * currentMap();
        /**
         * @brief "Visits" the new map at the given pos and updates the current map to the visited one.
         * 
         * @param m map
         * @param pos position
         * @return true if success
         * @return false if failed
         */
        bool visitMap(Coordinate pos);
        /**
         * @brief Places a given number of trainers on the map
         * 
         * @param numTrainers the number of trainers to place on map
         */
        void placeNPCS(int numTrainers);
        /**
         * @brief Gets the current quit value
         * 
         * @return true if game has ended
         * @return false otherwise
         */
        bool quit() const;
        /**
         * @brief Quits the world, ending the game
         * 
         */
        void quitWorld();
};

#endif