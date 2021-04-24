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
#include "gameClasses.hpp"


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
