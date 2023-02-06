#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "shared.hpp"

class Character
{
    private:
        char icon;
        int time;
        Coordinate pos;
        MovementType mtype;
        CharacterType ctype;
    public:
        Character(char icon, Coordinate loc, CharacterType ct, MovementType mt);
        virtual ~Character() = 0;
        char getIcon() const;
        int getTime() const;
        Coordinate getPos() const;
        void setPos(Coordinate new_pos);
        CharacterType getType() const;
        MovementType getMovementType() const;
        void increaseTime(int n);
        void setMovementType(MovementType t);
};

class Player : public Character
{
    public:
    Player(char icon, Coordinate pos, CharacterType ct, MovementType mt) : Character(icon, pos, ct, mt)
    {
        // empty
    }
};

class NPC : public Character
{
    private: 
        bool defeated;
        Direction direction;
    public:
        NPC(char icon, Coordinate pos, Direction dir, CharacterType ct, MovementType mt) : Character(icon, pos, ct, mt)
        {
            defeated = false;
            direction = dir;
        }
        bool isDefeated() const;
        void setDefeated(bool defeat);
        Direction getDirection() const;
        void setDirection(Direction dir);
};

#endif