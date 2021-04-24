#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include "gameClasses.hpp"
#define PI 3.14159265359


float radToDeg(float rads)
{
    return rads * 360.f / (2*PI);
}


bool boxCollide(sf::Vector2f point1, sf::Vector2f size1,
    sf::Vector2f point2, sf::Vector2f size2,
    sf::Vector2f offset1=sf::Vector2f(), sf::Vector2f offset2=sf::Vector2f())
{
    bool xCol = ( (point2.x <= point1.x + size1.x && point1.x + size1.x <= point2.x + size2.x) ||
        (point2.x <= point1.x && point1.x <= point2.x + size2.x) );
    bool yCol = ( (point2.y <= point1.y + size1.y && point1.y + size1.y <= point2.y + size2.y) ||
        (point2.y <= point1.y && point1.y <= point2.y + size2.y) );
    return (xCol && yCol);
}


bool boxCollide(sf::RectangleShape first, sf::RectangleShape second)
{
    sf::Vector2f point1 = first.getPosition() - first.getOrigin();
    sf::Vector2f point2 = second.getPosition() - second.getOrigin();
    sf::Vector2f size1 = first.getSize();
    sf::Vector2f size2 = second.getSize();
    bool xCol = ( (point2.x <= point1.x + size1.x && point1.x + size1.x <= point2.x + size2.x) ||
        (point2.x <= point1.x && point1.x <= point2.x + size2.x) );
    bool yCol = ( (point2.y <= point1.y + size1.y && point1.y + size1.y <= point2.y + size2.y) ||
        (point2.y <= point1.y && point1.y <= point2.y + size2.y) );
    return (xCol && yCol);
}


void createMap(std::ifstream& file,
    std::vector<CubeEntity>& enemyVect,
    std::vector<sf::RectangleShape>& mapVect,
    Weapon& enemyWeapon)
{
    if (file.is_open()) {

    float gridSize = 10;
    std::string line;
    while (getline(file, line))
    {
    if (line.length() < 1)
        continue;
    char action = line[0];
    std::vector<int> args;
    if (action != '#') // else it's a comment
    {
        /// get arguments
        size_t currentPos = 2; // miss first char
        size_t seekPos;
        for (seekPos = currentPos; seekPos < line.length(); seekPos++)
        {
            if (line[seekPos] == ' ')
            {
                args.push_back(
                    std::stoi(line.substr(currentPos, seekPos))
                );
                currentPos = seekPos;
                continue;
            }
        }
        args.push_back(std::stoi(line.substr(currentPos, line.length())));

        // add elements
        switch (action)
        {
        case 'E': // enemy
            {
            sf::Vector2f position(args.at(0), args.at(1));
            position *= gridSize;
            enemyVect.push_back(CubeEntity(
                sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE), enemyWeapon, 'l', 0,
                sf::Color::Red, position
            ));
            std::cout << "created enemy at X:" << position.x << " Y:" << position.y << std::endl;
            }
            break;
        case 'w': // wall
            {
                sf::Vector2f position(args.at(0), args.at(1));
                sf::Vector2f size(args.at(2), args.at(3));
                position *= gridSize; size *= gridSize;
                sf::RectangleShape rect(size);
                rect.setPosition(position);
                mapVect.push_back(rect);
                std::cout << "created floor at X:" << position.x <<
                    " Y:" << position.y << " with width:" << size.x <<
                    " and height:" << size.y << std::endl;
            }
            break;
        default:
            std::cout << "Unrecognised entity in map." << std::endl;
            break;
        }
    }
    }
    }
    file.close();
}