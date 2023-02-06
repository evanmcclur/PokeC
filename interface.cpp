#include "interface.hpp"
#include "ncurses.h"

const char * ErrorLogger::errors[NUM_ERROR_MESSAGES][100] = {
	{ "An error has occurred. Please try again" },
	{ "Cannot move onto impassable terrain!" },
	{ "The cell is occupied by another player or pokemon!" },
	{ "The entered key is invalid!" },
	{ "Failed to allocate memory" }
};

void ErrorLogger::log(ErrorMessage msg)
{
    mvprintw(0, 0, errors[msg][0]);
}