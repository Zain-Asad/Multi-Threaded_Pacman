#pragma once
#include <SFML/Graphics.hpp>

sf::Sprite gameOver;     sf::Texture gameOver_texture;

namespace MenuSystem
{
    enum GameState
    {
        MAIN_MENU,
        PLAYING,
        PAUSED,
        GAME_OVER
    };

    void drawMainMenu(sf::RenderWindow &window)
    {
        sf::Font font;
        if (!font.loadFromFile("Assets/Pixel.ttf"))
        {
            std::cerr << "Error loading font\n";
        }

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(48);
        text.setFillColor(sf::Color::White);
        text.setString("Pacman\nPress Enter to Start");
        text.setPosition(window.getSize().x / 2.0f - text.getLocalBounds().width / 2.0f, window.getSize().y / 2.0f - text.getLocalBounds().height / 2.0f);

        window.clear(sf::Color::Black);
        window.draw(text);
        window.display();
    }

    void drawPauseMenu(sf::RenderWindow &window)
    {
        sf::Font font;
        if (!font.loadFromFile("Assets/Pixel.ttf"))
        {
            std::cerr << "Error loading font\n";
        }

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(48);
        text.setFillColor(sf::Color::White);
        text.setString("Paused\nPress Enter to Resume\nPress Escape for Main Menu");
        text.setPosition(window.getSize().x / 2.0f - text.getLocalBounds().width / 2.0f, window.getSize().y / 2.0f - text.getLocalBounds().height / 2.0f);

        window.clear(sf::Color::Black);
        window.draw(text);
        window.display();
    }

    void drawGameOverScreen(sf::RenderWindow &window, int score)
    {
        sf::Font font;
        if (!font.loadFromFile("Assets/Pixel.ttf"))
        {
            std::cerr << "Error loading font\n";
        }

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(28);
        text.setFillColor(sf::Color::White);
        text.setString("Game Over\nYour score: " + std::to_string(score) + "\nPress Enter for Main Menu");
        text.setPosition(10 , 260);

        window.clear(sf::Color::Black);
        window.draw(text);
        gameOver_texture.loadFromFile("Assets/gameover.jpg");
        gameOver.setPosition((window.getSize().x - gameOver.getLocalBounds().width) / 2,
                       (window.getSize().y - gameOver.getLocalBounds().height) / 2);
        gameOver.setTexture(gameOver_texture);
        window.draw(gameOver);
        window.display();
    }
}
