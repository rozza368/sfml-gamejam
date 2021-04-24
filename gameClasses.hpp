#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
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
    ~Bullet();
    void updatePos(float delta);
    template<typename T> int checkCollision(T &objects);

    private:
    float dx, dy;
};


class CubeEntity : public sf::RectangleShape
{
    public:
        CubeEntity(sf::Vector2f size, Weapon& weapon, char direction,
            float speed=100, sf::Color colour=sf::Color::Green,
            sf::Vector2f pos=sf::Vector2f());
        ~CubeEntity();
        void dialogue(std::string text);
        void updateElements();
        void updateWeaponRotation(sf::Vector2i mousePos);
        void jump();
        void xVel(char option, float delta);
        void updatePos(float delta,
            std::vector<sf::RectangleShape>& objects,
            std::vector<CubeEntity>& enemies,
            float gravity);
        void shoot();

        float dx = 0;
        float dy = 0;
        float maxFall = 1000;
        float jumpAmt = 600;
        char facing;
        float wepRotation;
        sf::RectangleShape container;
        sf::RectangleShape rect;

    private:
        std::vector<Bullet> bullets;
        Weapon entWeapon;
        float maxVelocity = 600;
        float speed = maxVelocity * 3;
        bool canJump;
        sf::Clock shootTimer;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

