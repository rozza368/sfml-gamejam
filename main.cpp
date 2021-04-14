#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>


struct Weapon
{
    sf::Texture texture;
    int bulletSpeed;
    float fireRate;
};

const int PLAYER_SIZE = 50;

class CubeEntity : public sf::RectangleShape
{
    public:
        CubeEntity(sf::Vector2f size, Weapon weapon, char direction, float speed, sf::Color colour)
        {
            rect.setSize(size);
            rect.setFillColor(colour);
            entWeapon = weapon;
            facing = direction;
            container.setSize(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE));
            container.setTexture(&weapon.texture);
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
            }
            else
            {
                container.setPosition(sf::Vector2f(pos.x+PLAYER_SIZE, pos.y));
            }
        }

    private:
        sf::RectangleShape rect;
        Weapon entWeapon;
        char facing; // facing right
        sf::RectangleShape container;
        float moveSpeed;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            target.draw(rect, states);
            target.draw(container, states);
        }
};


int main()
{
    const int WIDTH = 1280,
              HEIGHT = 720;
    sf::Uint32 style = sf::Style::Titlebar | sf::Style::Close;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game Jam", style);
    window.setFramerateLimit(300);


    //// game assets
    sf::Font dialogueFont;
    if (!dialogueFont.loadFromFile("./assets/fonts/FreeMonoBold.otf"))
    {
        std::cout << "Failed to load dialogue font." << std::endl;
    }
    sf::Texture gun;
    if (!gun.loadFromFile("./assets/imgs/gun.png"))
        std::cout << "Failed to load gun texture" << std::endl;
    Weapon playerWep = {gun, 1000, 5};
    Weapon enemyWep = {gun, 200, 0.5};


    //// game vars
    int floorY = HEIGHT - 20;
    CubeEntity player(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE), playerWep, 'r',
        100, sf::Color::Red);


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
        }

        window.clear(sf::Color::Black);
        ///// start drawing

        player.updateElements();
        window.draw(player);


        if (showFps)
        {
            fpsAmt = "FPS: " + std::to_string(fps);
            fpsText.setString(fpsAmt);
            window.draw(fpsText);
        }

        // flip canvas
        window.display();
    }

    window.close();

    return 0;
}
