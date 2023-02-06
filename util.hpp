#ifndef UTIL_HPP
#define UTIL_HPP

#include<stdlib.h>
#include "shared.hpp"

// /**
//  * @brief Returns the largest of two numbers
//  * 
//  * @param n1 number one
//  * @param n2 number two
//  * @return int the largest number of the two
//  */
// int max(int n1, int n2);

// /**
//  * @brief Returns the smaller of two numbers
//  * 
//  * @param n1 number one
//  * @param n2 number two
//  * @return int the smaller number of the two
//  */
// int min(int n1, int n2);

class Util
{
    public:
        static Direction randdir();
        static Coordinate randpos();
        /**
         * @brief Generates a random number between a range of numbers
         * 
         * @param ubound The upper bound (inclusive)
         * @param lbound The lower bound (exclusive)
         * @return int random number between upper and lower bound (exclusive)
         */
        static int randrng(int ubound, int lbound);
        /**
         * @brief Shuffles an array
         * @author https://stackoverflow.com/questions/6127503/shuffle-array-in-c
         * 
         * @param array array to shuffle
         * @param n size of arrary
         */
        static void shuffle(int *array, size_t n);
};

#endif