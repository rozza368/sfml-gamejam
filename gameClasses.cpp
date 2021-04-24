#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include "gameClasses.hpp"
#include "funcs.hpp"



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


void Bullet::updatePos(float delta)
{
    move(delta * sf::Vector2f(dx, dy));
}


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
    std::cout << "attempted to jump: ";
    if (canJump)
    {
        dy = -jumpAmt;
        rect.move(0, -5);
        std::cout << "jumped" << std::endl;
        canJump = false;
    } else std::cout << "failed" << std::endl;
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
    std::vector<CubeEntity>& enemies,
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

    // update bullets
    for (int b = 0; b < bullets.size(); b++)
    {
        int index = 0;
        bullets[b].updatePos(delta);
        index = bullets[b].checkCollision(enemies);
        if (bullets[b].checkCollision(objects) > -1)
        {
            std::cout << "shot world\n";
            bullets.erase(bullets.begin()+b);
        }
        else if (index > -1)
        {
            std::cout << "shot enemy\n";
            bullets.erase(bullets.begin()+b);
            enemies.erase(enemies.begin()+index);
        }
    }
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
