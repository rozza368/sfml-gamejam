#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "funcs.hpp"

struct Weapon
{
    sf::Texture texture;
    float bulletSpeed;
    float fireRate;
};


class Bullet : public sf::RectangleShape
{
    public:
    Bullet(sf::Vector2f pos, float speed, float rotation, bool reverse=false)
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
    ~Bullet() = 0;
    void updatePos(float delta)
    {
        move(delta * sf::Vector2f(dx, dy));
    }

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


const int PLAYER_SIZE = 50;

class CubeEntity : public sf::RectangleShape
{
    public:
        CubeEntity(sf::Vector2f size, Weapon& weapon, char direction,
            float speed=100, sf::Color colour=sf::Color::Green,
            sf::Vector2f pos=sf::Vector2f())
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
            // container.setOutlineThickness(2);
            // container.setOutlineColor(sf::Color::Blue);
        }

        // fix compiler error
        ~CubeEntity() = 0;

        void dialogue(std::string text)
        {
            // to add
        }

        void updateElements()
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

        void updateWeaponRotation(sf::Vector2i mousePos)
        {
            sf::Vector2f containerPos = container.getPosition();
            wepRotation = std::atan(
                (float)(containerPos.y - mousePos.y) / (float)(containerPos.x - mousePos.x)
            );
            container.setRotation(radToDeg(wepRotation));
        }

        void jump()
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
        void xVel(char option, float delta)
        {
            int mod;
            switch (option) {
            case 'l': // left
                mod = -1;
                break;
            case 'r': // right
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

        float dx = 0;
        float dy = 0;
        float maxFall = 1000;
        float jumpAmt = 600;
        char facing;
        float wepRotation;

        void updatePos(float delta,
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

        void shoot()
        {
            if (shootTimer.getElapsedTime().asSeconds() >= 1.f / entWeapon.fireRate)
            {
                bullets.push_back(Bullet(container.getPosition(), entWeapon.bulletSpeed, wepRotation, (facing == 'l')));
                shootTimer.restart();
            }
        }
        sf::RectangleShape container;
        sf::RectangleShape rect;

    private:
        std::vector<Bullet> bullets;
        Weapon entWeapon;
        // movement
        float maxVelocity = 600;
        float speed = maxVelocity * 3;
        bool canJump;
        sf::Clock shootTimer;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            for (Bullet b : bullets)
                target.draw(b, states);
            target.draw(rect, states);
            target.draw(container, states);
        }
};
