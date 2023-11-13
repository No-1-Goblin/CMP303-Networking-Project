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
    virtual void update(float);
    void render(sf::RenderWindow*);
    void setPos(float, float);
    void updatePos();
    void setColour(sf::Color);
    std::string getName();
    sf::Vector2f getPos();
    sf::Color getColour();

protected:
    float speed;
    sf::RectangleShape penguinBase;
    sf::RectangleShape penguinColour;
    sf::Text usernameText;
    sf::Font font;
    PlayerData data;
};

