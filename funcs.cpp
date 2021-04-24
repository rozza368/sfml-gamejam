#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#define PI 3.14159265359


float radToDeg(float rads)
{
    return rads * 360.f / (2*PI);
}

bool boxCollide(sf::Vector2f point1, sf::Vector2f size1,
    sf::Vector2f point2, sf::Vector2f size2,
    sf::Vector2f offset1=sf::Vector2f(), sf::Vector2f offset2=sf::Vector2f())
{
    bool xCol = (point2.x <= point1.x + size1.x && point1.x + size1.x <= point2.x + size2.x ||
        point2.x <= point1.x && point1.x <= point2.x + size2.x);
    bool yCol = (point2.y <= point1.y + size1.y && point1.y + size1.y <= point2.y + size2.y ||
        point2.y <= point1.y && point1.y <= point2.y + size2.y);
    return (xCol && yCol);
}

bool boxCollide(sf::RectangleShape first, sf::RectangleShape second)
{
    sf::Vector2f point1 = first.getPosition() - first.getOrigin();
    sf::Vector2f point2 = second.getPosition() - second.getOrigin();
    sf::Vector2f size1 = first.getSize();
    sf::Vector2f size2 = second.getSize();
    bool xCol = (point2.x <= point1.x + size1.x && point1.x + size1.x <= point2.x + size2.x ||
        point2.x <= point1.x && point1.x <= point2.x + size2.x);
    bool yCol = (point2.y <= point1.y + size1.y && point1.y + size1.y <= point2.y + size2.y ||
        point2.y <= point1.y && point1.y <= point2.y + size2.y);
    return (xCol && yCol);
}