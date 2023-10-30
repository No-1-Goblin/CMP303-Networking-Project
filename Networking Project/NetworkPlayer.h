#pragma once
#include "PlayerBase.h"
class NetworkPlayer :
    public PlayerBase
{
public:
    NetworkPlayer();
    ~NetworkPlayer();
    void update(float) override;
    void setLatestData(MovementData);
protected:
    MovementData latest;
    MovementData prev;
    bool moving;
    sf::Vector2f moveVec;
};

