#pragma once
#include "PlayerBase.h"
class PlayerControllable :
    public PlayerBase
{
public:
    PlayerControllable();
    ~PlayerControllable();
    void update(float, bool);
    bool movedThisFrame();
protected:
    bool moved;
    bool stopped;
};

