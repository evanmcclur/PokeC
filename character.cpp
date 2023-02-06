#include "character.hpp"

/**************************************************/
/*********   Character Class Definitions   ********/
/**************************************************/

Character::Character(char cicon, Coordinate loc, CharacterType ct, MovementType mt)
{
    icon = cicon;
    pos = loc;
    ctype = ct;
    mtype = mt;
    time = 0;
}

Character::~Character()
{
    // do nothing as of now
}

char Character::getIcon() const
{
    return icon;
}

int Character::getTime() const
{
    return time;
}

Coordinate Character::getPos() const
{
    return pos;
}

void Character::setPos(Coordinate new_pos)
{
    pos = new_pos;
}

CharacterType Character::getType() const
{
    return ctype;
}

MovementType Character::getMovementType() const
{
    return mtype;
}

void Character::increaseTime(int n)
{
    time += n;
}

void Character::setMovementType(MovementType move)
{
    mtype = move;
}


/***********************************************/
/*********   Player Class Definitions   ********/
/***********************************************/



/********************************************/
/*********   NPC Class Definitions   ********/
/********************************************/

bool NPC::isDefeated() const 
{
    return defeated;
}

void NPC::setDefeated(bool defeat) 
{
    defeated = defeat;
}

Direction NPC::getDirection() const 
{
    return direction;
}

void NPC::setDirection(Direction dir)
{
    direction = dir;
}