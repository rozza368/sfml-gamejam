#pragma once

// #include "gameClasses.hpp"

float radToDeg(float rads);

bool boxCollide(sf::Vector2f point1, sf::Vector2f size1,
    sf::Vector2f point2, sf::Vector2f size2,
    sf::Vector2f offset1=sf::Vector2f(), sf::Vector2f offset2=sf::Vector2f());

bool boxCollide(sf::RectangleShape first, sf::RectangleShape second);

