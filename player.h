#pragma once

#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <cmath>

#include "ghosts.h"

class Ghost; // Forward declaration
class GameBoard;
class Pellets;

// Function to check if two circles intersect
bool circleIntersect(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance < r1 + r2;
}

class Pacman
{
private:
    const int window_width, window_height;
    int pacmanPosX, pacmanPosY;
    float pacmanSpeed, pacmanRadius;
    int pacmanLives, score;

    sf::CircleShape pacman;
    sf::Texture pacman_tex;
    Direction direction;

    pthread_mutex_t pacman_input;
    pthread_mutex_t coin_score;
    pthread_mutex_t pellet_player;

public:
    bool power = false;
    Pacman(const int width, const int height, pthread_mutex_t pacman, pthread_mutex_t coin, pthread_mutex_t p) : window_width(width), window_height(height),
                                                                                                                 pacmanPosX((6 * 30)), pacmanPosY((6 * 30)),
                                                                                                                 pacmanSpeed(2.00f), pacmanRadius(10),
                                                                                                                 pacmanLives(3), score(0),
                                                                                                                 pacman_input(pacman), coin_score(coin), pellet_player(p),
                                                                                                                 direction(RIGHT), pacman(pacmanRadius)
    {
        pacman_tex.loadFromFile("Assets/player1.png");
    }

    void pacmaninit(string filename) {
        pacmanPosX = (6 * 30);
        pacmanPosY = (6 * 30);
        pacmanSpeed = 2.00f;
        pacmanRadius = 10;
        pacmanLives = 3;
        score = 0;
        direction = RIGHT;
        pacman_tex.loadFromFile(filename);
    }


    float getPacmanPosX()
    {
        return pacmanPosX;
    }

    void setPacmanPosX(float x)
    {
        pacmanPosX = x;
    }

    float getPacmanPosY()
    {
        return pacmanPosY;
    }

    void setPacmanPosY(float y)
    {
        pacmanPosY = y;
    }

    int getScore()
    {
        return score;
    }

    int getPacmanLives()
    {
        return pacmanLives;
    }

    void decrementPacmanLives()
    {
        pacmanLives--;
    }

    void drawPacman(sf::RenderWindow &window)
    {
        pacman.setRadius(pacmanRadius);
        pacman.setTexture(&pacman_tex);
        pacman.setPosition(pacmanPosX, pacmanPosY);
        window.draw(pacman);
    }

    void handleKeypress()
    {
        pthread_mutex_lock(&pacman_input);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            direction = UP;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            direction = DOWN;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            direction = LEFT;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            direction = RIGHT;
        }
        pthread_mutex_unlock(&pacman_input);
    }

    bool collideWithWall(GameBoard &game, int posX, int posY)
    {
        return game.getBoard(posY / wall_pixels, posX / wall_pixels) == 1;
    }

    bool collideWithCoin(GameBoard &game, int posX, int posY)
    {
        if (game.getBoard(posY / wall_pixels, posX / wall_pixels) == 2)
        {
            game.setBoard(posY / wall_pixels, posX / wall_pixels, 0);
            return true;
        }

        return false;
    }

    bool collidesWithPellet(Pellets &pellet, int index, int pacmanX, int pacmanY)
    {
        sf::CircleShape pellet_shape;
        pellet_shape.setRadius(10); // Increased size
        pellet_shape.setPosition((pellet.getPosX_player(index) + 1), (-pellet.getPosY_player(index) + 1));
        power = true;
        return circleIntersect(pacmanX, pacmanY, pacmanRadius, pellet.getPosX_player(index), pellet.getPosY_player(index), 10);
    }

    void update_score()
    {
        pthread_mutex_lock(&coin_score);
        score += 10;
        pthread_mutex_unlock(&coin_score);
    }

    void move(GameBoard &game, Pellets &pellet, int index, sf::Clock &c)
    {
        pthread_mutex_lock(&pacman_input);
        switch (direction)
        {
        case UP:
            if (collidesWithPellet(pellet, index, pacmanPosX, pacmanPosY - pacmanSpeed))
            {
                pellet.setCollision_player(index);
                pellet.setPosition_player(index);
                c.restart();
            }
            if (collideWithCoin(game, pacmanPosX, pacmanPosY - pacmanSpeed))
            {
                update_score();
            }
            if (!collideWithWall(game, pacmanPosX, pacmanPosY - pacmanSpeed - 3))
            {
                pacmanPosY -= pacmanSpeed;
            }
            break;
        case DOWN:
            if (collidesWithPellet(pellet, index, pacmanPosX, pacmanPosY + pacmanSpeed))
            {
                pellet.setCollision_player(index);
                pellet.setPosition_player(index);
                c.restart();
            }
            if (collideWithCoin(game, pacmanPosX, pacmanPosY + pacmanSpeed))
            {
                update_score();
            }
            if (!collideWithWall(game, pacmanPosX, pacmanPosY + pacmanSpeed + 20))
            {
                pacmanPosY += pacmanSpeed;
            }
            break;
        case LEFT:
            if (collidesWithPellet(pellet, index, pacmanPosX - pacmanSpeed, pacmanPosY))
            {
                pellet.setCollision_player(index);
                pellet.setPosition_player(index);
                c.restart();
            }
            if (collideWithCoin(game, pacmanPosX - pacmanSpeed, pacmanPosY))
            {
                update_score();
            }
            if (!collideWithWall(game, pacmanPosX - pacmanSpeed - 3, pacmanPosY))
            {
                pacmanPosX -= pacmanSpeed;
            }
            break;
        case RIGHT:
            if (collidesWithPellet(pellet, index, pacmanPosX + pacmanSpeed, pacmanPosY))
            {
                pellet.setCollision_player(index);
                pellet.setPosition_player(index);
                c.restart();
            }
            if (collideWithCoin(game, pacmanPosX + pacmanSpeed, pacmanPosY))
            {
                update_score();
            }
            if (!collideWithWall(game, pacmanPosX + pacmanSpeed + 20, pacmanPosY))
            {
                pacmanPosX += pacmanSpeed;
            }
            break;
        }
        pthread_mutex_unlock(&pacman_input);
    }

    bool collidesWithGhost(Ghost &ghost)
    {
        sf::CircleShape ghostShape(GHOST_SIZE * 100);
        ghostShape.setRadius(GHOST_SIZE * 200); // Increased size
        ghostShape.setOrigin(GHOST_SIZE * 100, GHOST_SIZE * 100);
        ghostShape.setPosition((ghost.getPosX() + 1), (-ghost.getPosY() + 1));
        return ghost_pellet_Intersect(pacmanPosX, pacmanPosY, pacmanRadius, ghost.getPosX(), ghost.getPosY(), 0.04f * 200);
    }
};
