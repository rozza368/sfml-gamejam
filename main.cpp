#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "gameClasses.hpp"
#include "funcs.hpp"
#include "cutscene.hpp"

int main(int argc, char* argv[])
{
    const int WIDTH = 1280,
              HEIGHT = 720;
    sf::Uint32 style = sf::Style::Titlebar | sf::Style::Close;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game Jam", style);
    // window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(240); // if no vsync
    sf::Image icon;
    icon.loadFromFile("./assets/imgs/gun.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    bool running = doCutscene(window);


    //// game assets
    sf::Font dialogueFont;
    if (!dialogueFont.loadFromFile("./assets/fonts/FreeMonoBold.otf"))
    {
        std::cout << "Failed to load dialogue font." << std::endl;
    }

    sf::Texture gun;
    if (!gun.loadFromFile("assets/imgs/gun.png"))
        std::cout << "Failed to load gun texture." << std::endl;
    std::ifstream mapFile;
    mapFile.open("inside.map");



    //// game vars
    Weapon playerWep = {gun, 1500, 5};
    Weapon enemyWep = {gun, 500, 0.5};
    Player player(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE), playerWep, 'r',
        100, sf::Color::Black);
    player.rect.setPosition(sf::Vector2f(200, 500));

    int gravity = 1200;
    sf::VertexArray debugLines(sf::LineStrip, 4);
    sf::Vector2i mousePos;
    sf::View gameView(sf::FloatRect(0, 0, WIDTH, HEIGHT));
    window.setView(gameView);

    // std::vector<std::unique_ptr<sf::Drawable>> mapElements;
    std::vector<sf::RectangleShape> mapElements = {};
    std::vector<Enemy> enemies = {};
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
    sf::Clock gameTimer;
    std::cout << "started game loop." << std::endl;
    while (running)
    {
        timeElapsed = frameCounter.restart();
        delta = timeElapsed.asSeconds();
        fps = 1000000.f / timeElapsed.asMicroseconds();

        ///// update game
        mousePos = (sf::Vector2i)((sf::Vector2f)sf::Mouse::getPosition(window) + gameView.getCenter() - sf::Vector2f(WIDTH / 2, HEIGHT / 2));
        player.updateWeaponRotation(mousePos);


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            player.xVel('l', delta);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            player.xVel('r', delta);
        }
        else player.xVel('s', delta);

        player.updatePos(delta, mapElements, gravity);
        gameView.setCenter(player.rect.getPosition() - sf::Vector2f(0, 100));
        window.setView(gameView);
        player.updateElements();
        player.updateBullets(delta, mapElements, enemies);

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
        for (Enemy& enemy : enemies)
        {
            enemy.targetPlayer(player.rect.getPosition());
            enemy.updatePos(delta, mapElements, gravity);
            enemy.updateBullets(delta, mapElements, player);
            enemy.updateWeaponRotation((sf::Vector2i)player.rect.getPosition());
            enemy.updateElements();
            enemy.shoot();
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
    std::cout << "Game closed" << std::endl;

    return 0;
}
