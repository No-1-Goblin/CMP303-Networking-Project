#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.h"

class PlayerBase :
    public sf::RectangleShape
{
public:
    PlayerBase();
    ~PlayerBase();
    void init(sf::Texture*, sf::Texture*, PlayerData);
    void update(float);
    void render(sf::RenderWindow*);
protected:
    sf::RectangleShape penguinBase;
    sf::RectangleShape penguinColour;
    sf::Text usernameText;
    sf::Font font;
    PlayerData data;
};

