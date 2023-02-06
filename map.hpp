#ifndef MAP_HPP
#define MAP_HPP

#include "shared.hpp"
#include "character.hpp"
#include "minheap.hpp"

class Map
{
    private:
        int terrain[MAP_Y][MAP_X];
        Character *tmap[MAP_Y][MAP_X];
        MinHeap *turn;
    public:
        Map();
        ~Map();
        /**
         * @brief Used to generate terrain. Grows a region on this map according to given parameters.
         * 
         * @param x The x position of the region (starting x)
         * @param y The y position of the region (starting y)
         * @param size The size of the region
         * @param placed How many terrain have been placed on map
         * @param terrain The type of terrain
         */
        void growRegion(int x, int y, int size, int placed, int terrain);
        /**
         * @brief Generates the map terrain for this map
         * 
         * @param initial the inital generation (e.g. the starting point of the world)
         * @param coords the coordinates of the map in the world
         */
        void generateMapTerrain(int initial, Coordinate pos, Coordinate ns_exit, Coordinate ew_exit);
        /**
         * @brief Connects two coordinates with a road
         * 
         * @param start starting position
         * @param end ending position
         */
        void connectRoad(Coordinate start, Coordinate end);
        /**
         * @brief Places the PokeMart and PokeCenter buildings on the map
         *
         */
        void placeBuildings(Coordinate ns_exit, Coordinate ew_exit);
        /**
         * @brief Get the Terrain at position
         * 
         * @param pos position
         * @return Terrain 
         */
        Terrain getTerrain(Coordinate pos) const;
        /**
         * @brief Adds a trainer to this maps trainer map
         * 
         * @param trainer the trainer to add
         */
        void addTrainer(Character * trainer, Coordinate pos);
        /**
         * @brief Removes a trainer from this maps trainer map
         * 
         * @param pos the position of the trainer
         * @return true 
         * @return false 
         */
        void removeTrainer(Coordinate pos);
        /**
         * @brief Attempts to get trainer at position
         * 
         * @param pos pos
         * @return Character* 
         */
        Character * getTrainer(Coordinate pos) const;
        /**
         * @brief Checks if a trainer is located at a given position on the map
         * 
         * @param pos position to check
         * @return true 
         * @return false 
         */
        bool checkForTrainer(Coordinate pos) const;
        /**
         * @brief Checks if a trainer is located a given position and is of the given type
         * 
         * @param pos position of trainer
         * @param type type of character
         * @return true 
         * @return false 
         */
        bool checkForTrainer(Coordinate pos, CharacterType type) const;

        MinHeap * getHeap();
};

#endif