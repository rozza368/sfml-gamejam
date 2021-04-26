#include <SFML/Graphics.hpp>
#include <iostream>
#define WIDTH 1280
#define HEIGHT 720
#include "cutscene.hpp"

bool doCutscene(sf::RenderWindow& win)
{
    sf::Font cutFont;
    cutFont.loadFromFile("assets/fonts/FreeMonoBold.otf");

    sf::Text cutText;
    cutText.setFont(cutFont);
    cutText.setCharacterSize(32);
    cutText.setFillColor(sf::Color::Black);
    cutText.setString("Click to continue...");
    cutText.setPosition(sf::Vector2f(WIDTH - 500, HEIGHT - 50));

    sf::Sprite cutSprite;

    sf::Texture cutsceneImgs[5];
    for (int i = 0; i < 5; i++)
    {
        sf::Texture cutImg;
        if (!cutImg.loadFromFile("assets/imgs/cut"+std::to_string(i)+".png"))
        {
            std::cout << "A cutscene image failed to load." << std::endl;
            continue;
        }
        cutsceneImgs[i] = cutImg;
    }
    std::cout << "Loaded cutscene images." << std::endl;

    bool cutscene = true;
    // cutscene
    int currentImg = 0;
    cutSprite.setTexture(cutsceneImgs[currentImg]);
    while (cutscene)
    {
        sf::Event evt;
        while (win.pollEvent(evt))
        {
            if (evt.type == sf::Event::Closed)
            {
                cutscene = false;
                return false;
            }
            else if (evt.type == sf::Event::MouseButtonPressed && evt.mouseButton.button == sf::Mouse::Left)
            {
                currentImg++;
                if (currentImg > 4) cutscene = false;
                cutSprite.setTexture(cutsceneImgs[currentImg]);
            }
        }

        win.clear(sf::Color::White);
        win.draw(cutSprite);
        win.draw(cutText);
        win.display();
    }
    return true;
}