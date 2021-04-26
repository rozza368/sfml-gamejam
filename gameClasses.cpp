#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include <vector>
#include <fstream>

#include "funcs.hpp"
#include "gameClasses.hpp"


////////////////////////
///// Bullet class functions
///////////////////////
Bullet::Bullet(sf::Vector2f pos, float speed, float rotation, bool reverse)
{
    setSize(sf::Vector2f(20, 5));
    setPosition(pos);
    setRotation(radToDeg(rotation));
    setFillColor(sf::Color::Yellow);
    short mod = 1;
    // change bullet direction based on player direction
    if (reverse) mod = -1;
    dx = mod*speed * std::cos(rotation);
    dy = mod*speed * std::sin(rotation);
}

#include "gameClasses.hpp"

void Bullet::updatePos(float delta)
{
    move(delta * sf::Vector2f(dx, dy));
}

/*
template<typename T>
int Bullet::checkCollision(T &objects)
{
    for (int o = 0; o < objects.size(); o++)
    {
        if (boxCollide(getPosition(), sf::Vector2f(), objects[o].getPosition(), objects[o].getSize()))
        {
            return o;
        }
    }
    return -1;
}
*/


////////////////////////
///// CubeEntity class functions
///////////////////////
CubeEntity::CubeEntity(sf::Vector2f size, Weapon& weapon, char direction,
    float speed, sf::Color colour,
    sf::Vector2f pos)
{
    rect.setSize(size);
    rect.setFillColor(colour);
    rect.setOrigin(PLAYER_SIZE/2.f, PLAYER_SIZE/2.f);
    rect.setPosition(pos);
    entWeapon = weapon;
    facing = direction;
    container.setSize(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    container.setTexture(&weapon.texture);
    container.setOrigin(container.getSize()/2.f);
}


void CubeEntity::dialogue(std::string text)
{
    // to add
}


void CubeEntity::updateElements()
{
    sf::Vector2f pos = rect.getPosition();
    // flip gun texture depending on direction
    if (facing == 'l')
    {
        container.setPosition(sf::Vector2f(pos.x-PLAYER_SIZE, pos.y));
        container.setScale(-1, 1);
    }
    else
    {
        container.setPosition(sf::Vector2f(pos.x+PLAYER_SIZE, pos.y));
        container.setScale(1, 1);
    }
}


void CubeEntity::updateWeaponRotation(sf::Vector2i mousePos)
{
    sf::Vector2f containerPos = container.getPosition();
    wepRotation = std::atan(
        (float)(containerPos.y - mousePos.y) / (float)(containerPos.x - mousePos.x)
    );
    container.setRotation(radToDeg(wepRotation));
}


void CubeEntity::jump()
{
    // std::cout << "attempted to jump: ";
    if (canJump)
    {
        dy = -jumpAmt;
        rect.move(0, -5);
        // std::cout << "jumped" << std::endl;
        canJump = false;
    } // else std::cout << "failed" << std::endl;
}


// update dx in a way that makes movement smoother
void CubeEntity::xVel(char option, float delta)
{
    int mod;
    switch (option) {
    case 'l': // left
        facing = 'l';
        mod = -1;
        break;
    case 'r': // right
        facing = 'r';
        mod = 1;
        break;
    case 's': // slow down
        if (dx < 0)
            mod = 1;
        else if (dx > 0)
            mod = -1;
        // stops small velocities remaining
        if (dx > -delta*speed && dx < delta*speed)
        {
            dx = 0;
            mod = 0;
        }
        break;
    }
    dx += mod * speed * delta;

    if (dx > maxVelocity)
        dx = maxVelocity;
    else if (dx < -maxVelocity)
        dx = -maxVelocity;
}


void CubeEntity::updatePos(float delta,
    std::vector<sf::RectangleShape>& objects,
    // std::vector<Enemy>& enemies,
    float gravity)
{
    if (dy < maxFall)
        dy += gravity * delta;
    for (auto& obj : objects)
    {
        if (boxCollide(rect, obj))
        { // player is colliding with something
            dy = 0;
            canJump = true;
            break;
        } else canJump = false;
    }
    rect.move(delta * sf::Vector2f(dx, dy));
}

/*
template<typename T>
int Player::updateBullets(
            float delta,
            std::vector<sf::RectangleShape>& mapElements,
            std::vector<T>& enemies
        )
{
    std::vector<Bullet> bullets = this->bullets;
    for (int b = 0; b < bullets.size(); b++)
    {
        int index = 0;
        bullets[b].updatePos(delta);
        index = bullets[b].checkCollision(enemies);
        if (bullets[b].checkCollision(mapElements) > -1)
        {
            std::cout << "shot world\n";
            bullets.erase(bullets.begin()+b);
        }
        else if (index > -1)
        {
            std::cout << "shot enemy\n";
            bullets.erase(bullets.begin()+b);
            enemies.erase(enemies.begin()+index);
            return index;
        }
    }
    return -1;
}
*/

int Enemy::updateBullets(
            float delta,
            std::vector<sf::RectangleShape>& mapElements,
            Player& player
        )
{
    // std::vector<Bullet> bullets = this->bullets;
    for (int b = 0; b < bullets.size(); b++)
    {
        bullets[b].updatePos(delta);
        if (bullets[b].checkCollision(mapElements) > -1)
        {
            std::cout << "shot world\n";
            bullets.erase(bullets.begin()+b);
        }
        else if (boxCollide(bullets[b].getPosition(), sf::Vector2f(), player.rect.getPosition(), player.rect.getSize(), sf::Vector2f(), player.rect.getOrigin()))
        {
            std::cout << "shot player\n";
            return 1;
        }
    }
    return 0;
}


void CubeEntity::shoot()
{
    if (shootTimer.getElapsedTime().asSeconds() >= 1.f / entWeapon.fireRate)
    {
        bullets.push_back(Bullet(container.getPosition(), entWeapon.bulletSpeed, wepRotation, (facing == 'l')));
        shootTimer.restart();
    }
}


void CubeEntity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (Bullet b : bullets)
        target.draw(b, states);
    target.draw(rect, states);
    target.draw(container, states);
}



////////////////////////
///// Enemy class functions
///////////////////////
void Enemy::targetPlayer(sf::Vector2f playerPos)
{
    if (playerPos.x < getPosition().x)
        facing = 'l';
    else
        facing = 'r';
}


void createMap(std::ifstream& file,
    std::vector<Enemy>& enemyVect,
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
            enemyVect.push_back(Enemy(
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