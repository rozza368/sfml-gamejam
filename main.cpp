#include <SFML/Graphics.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>

#define PI 3.14159265359

struct Weapon
{
    sf::Texture texture;
    float bulletSpeed;
    float fireRate;
};

float radToDeg(float rads)
{
    return rads * 360.f / (2*PI);
}

bool boxCollide(sf::Vector2f point1, sf::Vector2f size1, sf::Vector2f point2, sf::Vector2f size2)
{
    bool xCol = (point2.x <= point1.x + size1.x && point1.x + size1.x <= point2.x + size2.x);
    bool yCol = (point2.y <= point1.y + size1.y && point1.y + size1.y <= point2.y + size2.y);
    return (xCol && yCol);
}

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
        // change direction based on direction
        if (reverse) mod = -1;
        dx = mod*speed * std::cos(rotation);
        dy = mod*speed * std::sin(rotation);
    }
    void updatePos(float delta)
    {
        move(delta * sf::Vector2f(dx, dy));
    }

    private:
    float dx, dy;
};


const int PLAYER_SIZE = 50;

class CubeEntity : public sf::RectangleShape
{
    public:
        CubeEntity(sf::Vector2f size, Weapon& weapon, char direction, float speed, sf::Color colour, sf::Vector2f pos=sf::Vector2f())
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
            moveSpeed = speed;
        }

        void dialogue(std::string text)
        {
            // to add
        }

        void updateElements()
        {
            sf::Vector2f pos = rect.getPosition();
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
            // std::cout << "new rotation: " << rotation << std::endl;
            container.setRotation(radToDeg(wepRotation));
        }

        float dx = 0;
        float dy = 0;
        float speed = 500;
        float maxFall = 200;
        char facing;
        float wepRotation;
        void updatePos(float delta,
            std::vector<sf::RectangleShape>& objects,
            float gravity)
        {
            if (dy < maxFall)
                dy += gravity * delta;
            for (auto& obj : objects)
            {
                if (boxCollide(rect.getPosition(), rect.getSize(), obj.getPosition(), obj.getSize()))
                    dy = 0;
            }
            rect.move(delta * sf::Vector2f(dx, dy));
            for (Bullet& b: bullets)
            {
                b.updatePos(delta);
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
        float moveSpeed;
        float maxVelocity = 500;
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
    // std::vector<std::unique_ptr<sf::Drawable>>& vect,
    std::vector<sf::RectangleShape>& vect,
    Weapon enemyWeapon)
{
    if (file.is_open()) {

    float gridSize = 10;
    std::string line;
    while (getline(file, line))
    {
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
            vect.push_back(CubeEntity(
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
                vect.push_back(rect);
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


int main()
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
    int floorY = HEIGHT - 20; // unused?
    Weapon playerWep = {gun, 1500, 5};
    Weapon enemyWep = {gun, 200, 0.5};
    CubeEntity player(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE), playerWep, 'r',
        100, sf::Color::Black);
    player.rect.setPosition(sf::Vector2f(600, 500));

    int gravity = 50;
    sf::VertexArray debugLines(sf::LineStrip, 4);
    sf::Vector2i mousePos;

    // std::vector<std::unique_ptr<sf::Drawable>> mapElements;
    std::vector<sf::RectangleShape> mapElements;
    createMap(mapFile, mapElements, enemyWep);

    //// debug info
    bool showFps = true;
    std::string fpsAmt;
    sf::Font debugFont;
    if (!debugFont.loadFromFile("./assets/fonts/FreeSans.otf"))
    {
        std::cout << "Failed to load debug font." << std::endl;
    }
    sf::Text fpsText;
    fpsText.setFont(debugFont);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);


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
            else if (event.type == sf::Event::MouseMoved)
            {
                mousePos = sf::Mouse::getPosition(window);
                player.updateWeaponRotation(mousePos);
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code) {
                    case sf::Keyboard::Tab:
                        showFps = !showFps;
                        break;
                    default:
                        break;
                }
            }
            
        }

        ///// update game
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            player.dx = -player.speed;
            player.facing = 'l';
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            player.dx = player.speed;
            player.facing = 'r';
        }
        else
            player.dx = 0;

        player.updatePos(delta, mapElements, gravity);
        player.updateElements();


        window.clear(sf::Color(200, 200, 200));
        ///// start drawing

        for (sf::RectangleShape& obj : mapElements)
        {
            window.draw(obj);
        }
        window.draw(player);

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

        // flip canvas
        window.display();
    }

    window.close();

    return 0;
}
