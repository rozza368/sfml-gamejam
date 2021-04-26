#pragma once

#include <SFML/Graphics.hpp>
// #include <cmath>
// #include <iostream>
#include "funcs.hpp"

const int PLAYER_SIZE = 50;

struct Weapon
{
    sf::Texture texture;
    float bulletSpeed;
    float fireRate;
};

class Bullet : public sf::RectangleShape
{
    public:
    Bullet(sf::Vector2f pos, float speed, float rotation, bool reverse=false);
    void updatePos(float delta);
    template<typename T>
    int checkCollision(T &objects)
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

    private:
    float dx, dy;
};


class CubeEntity : public sf::RectangleShape
{
    public:
        CubeEntity(
            sf::Vector2f size, Weapon& weapon, char direction,
            float speed=100, sf::Color colour=sf::Color::Green,
            sf::Vector2f pos=sf::Vector2f()
        );
        void dialogue(std::string text);
        void updateElements();
        void updateWeaponRotation(sf::Vector2i mousePos);
        void jump();
        void xVel(char option, float delta);
        void updatePos(float delta,
            std::vector<sf::RectangleShape>& objects,
            // std::vector<Enemy>& enemies,
            float gravity);
        // int updateBullets();
        void shoot();

        float dx = 0;
        float dy = 0;
        float maxFall = 1000;
        float jumpAmt = 600;
        char facing;
        float wepRotation;
        sf::RectangleShape container;
        sf::RectangleShape rect;

    // private:
        std::vector<Bullet> bullets;
        Weapon entWeapon;
        float maxVelocity = 600;
        float speed = maxVelocity * 3;
        bool canJump;
        sf::Clock shootTimer;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};


class Player : public CubeEntity
{
    using CubeEntity::CubeEntity;
    public:
        template<typename T>
        int updateBullets(
            float delta,
            std::vector<sf::RectangleShape>& mapElements,
            std::vector<T>& enemies
        )
        {
            // std::vector<Bullet> bullets = this->bullets;
            for (int b = 0; b < bullets.size(); b++)
            {
                bullets[b].updatePos(delta);
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
};


class Enemy : public CubeEntity
{
    using CubeEntity::CubeEntity;
    public:
        void targetPlayer(sf::Vector2f playerPos);
        int updateBullets(
            float delta,
            std::vector<sf::RectangleShape>& mapElements,
            Player& player
        );
        // void shoot();
        // float weaponCooldown = 2;
};


void createMap(std::ifstream& file,
    std::vector<Enemy>& enemyVect,
    std::vector<sf::RectangleShape>& mapVect,
    Weapon& enemyWeapon);