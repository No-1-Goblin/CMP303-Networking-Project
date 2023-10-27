#pragma once
#include "PlayerBase.h"
class PlayerControllable :
    public PlayerBase
{
public:
    PlayerControllable();
    ~PlayerControllable();
    void update(float) override;
protected:
    float speed;
};

