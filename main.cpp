#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>

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


void createMap(std::ifstream& file,
    std::vector<CubeEntity>& enemyVect,
    std::vector<sf::RectangleShape>& mapVect,
    Weapon enemyWeapon)
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


int main(int argc, char* argv[])
{
    const int WIDTH = 1280,
              HEIGHT = 720;
    sf::Uint32 style = sf::Style::Titlebar | sf::Style::Close;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game Jam", style);
    // window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(240); // if no vsync


    //// game assets
    sf::Font dialogueFont;
    if (!dialogueFont.loadFromFile("./assets/fonts/FreeMonoBold.otf"))
    {
        std::cout << "Failed to load dialogue font." << std::endl;
    }
    sf::Texture gun;
    if (!gun.loadFromFile("./assets/imgs/gun.png"))
        std::cout << "Failed to load gun texture" << std::endl;

    std::ifstream mapFile;
    mapFile.open("inside.map");



    //// game vars
    Weapon playerWep = {gun, 1500, 5};
    Weapon enemyWep = {gun, 200, 0.5};
    CubeEntity player(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE), playerWep, 'r',
        100, sf::Color::Black);
    player.rect.setPosition(sf::Vector2f(600, 500));

    int gravity = 1200;
    sf::VertexArray debugLines(sf::LineStrip, 4);
    sf::Vector2i mousePos;

    // std::vector<std::unique_ptr<sf::Drawable>> mapElements;
    std::vector<sf::RectangleShape> mapElements = {};
    std::vector<CubeEntity> enemies = {};
    createMap(mapFile, enemies, mapElements, enemyWep);

    //// debug info
    bool showFps = false;
    std::string fpsAmt;
    sf::Font debugFont;
    if (!debugFont.loadFromFile("./assets/fonts/FreeSans.otf"))
    {
        std::cout << "Failed to load debug font." << std::endl;
    }
    sf::Text fpsText;
    fpsText.setFont(debugFont);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::Black);


    //// game time
    sf::Clock frameCounter;
    float fps;
    sf::Time timeElapsed;
    float delta;


    //// main loop
    bool running = true;
    sf::Clock gameTimer;
    while (running)
    {
        timeElapsed = frameCounter.restart();
        delta = timeElapsed.asSeconds();
        fps = 1000000.f / timeElapsed.asMicroseconds();

        ///// update game
        mousePos = sf::Mouse::getPosition(window);
        player.updateWeaponRotation(mousePos);


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            player.xVel('l', delta);
            player.facing = 'l';
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            player.xVel('r', delta);
            player.facing = 'r';
        }
        else player.xVel('s', delta);

        player.updatePos(delta, mapElements, enemies, gravity);
        player.updateElements();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                running = false;
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    player.shoot();
                }
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code) {
                    case sf::Keyboard::Tab:
                        showFps = !showFps;
                        break;
                    case sf::Keyboard::W:
                        player.jump();
                    default:
                        break;
                }
            }
        }


        window.clear(sf::Color(200, 200, 200));
        ///// start drawing

        for (sf::RectangleShape& obj : mapElements)
        {
            window.draw(obj);
        }
        for (CubeEntity& enemy : enemies)
        {
            window.draw(enemy);
        }
        window.draw(player);

        // debug graphics
        if (showFps)
        {
            fpsAmt = "FPS: " + std::to_string(fps) +
            "\nR: " + std::to_string(radToDeg(player.wepRotation));
            fpsText.setString(fpsAmt);
            debugLines[0].position = player.container.getPosition();
            debugLines[1].position = (sf::Vector2f)mousePos;
            debugLines[2].position = sf::Vector2f(debugLines[1].position.x, debugLines[0].position.y);
            debugLines[3].position = debugLines[0].position;
            window.draw(debugLines);
            window.draw(fpsText);
        }

        ///// flip canvas
        window.display();
    }

    window.close();

    return 0;
}
