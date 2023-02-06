#include "util.hpp"

// int max(n1, n2)
// {
//     if (n1 > n2)
//     {
//         return n1;
//     }
//     return n2;
// }

// int min(n1, n2)
// {
//     if (n1 < n2)
//     {
//         return n1;
//     }
//     return n2;
// }

Direction Util::randdir()
{
    int d = rand() & 0x7;    
    return (Direction)d;
}

Coordinate Util::randpos()
{
    Coordinate pos;
	pos.x = (rand() % (MAP_X - 2)) + 1;
	pos.y = (rand() % (MAP_Y - 2)) + 1;
	return pos;
}

Coordinate randPos()
{
	Coordinate pos;
	pos.x = (rand() % (MAP_X - 2)) + 1;
	pos.y = (rand() % (MAP_Y - 2)) + 1;
	return pos;
}

int Util::randrng(int ubound, int lbound)
{
    return ((rand() % (ubound - lbound) + 1) + lbound);
}

void Util::shuffle(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

