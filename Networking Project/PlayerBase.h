#pragma once
#include <SFML/Graphics.hpp>

class PlayerBase :
    public sf::RectangleShape
{
public:
    PlayerBase();
    ~PlayerBase();
    void init(sf::Texture*, sf::Texture*);
    void setPlayerColour(sf::Color);
    void render(sf::RenderWindow*);
protected:
    sf::RectangleShape penguinBase;
    sf::RectangleShape penguinColour;
};

