#ifndef GHOSTS_H
#define GHOSTS_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <pthread.h>
#include <cmath>

#include "gameboard.h"

const float GHOST_SIZE = 0.04f;
enum Direction { UP, DOWN, LEFT, RIGHT };

class Pacman; // Forward declaration

class Ghost {
private:
    float posX, posY;
    float speed;
    Direction direction;

    float targetX, targetY;
    sf::CircleShape ghost;

public:
    Ghost(float x, float y, float s) : posX(x), posY(y), speed(s), direction(UP),
                                       ghost(GHOST_SIZE * 100) {}

    void drawGhost(sf::RenderWindow &window) {
        ghost.setRadius(GHOST_SIZE * 200); // Increased size
        ghost.setFillColor(sf::Color::Red);
        // ghost.setPosition((posX + 1) * window.getSize().x / 2 - ghost.getRadius(),
        //                   (-posY + 1) * window.getSize().y / 2 - ghost.getRadius());
        ghost.setPosition(posX, posY);
        window.draw(ghost);
    }

    bool collidesWithWall(GameBoard& game, int x, int y) {
        return game.getBoard(y/wall_pixels, x/wall_pixels) == 1;
    }

    void move(GameBoard& game, int playerPosX, float playerPosY) {
        float dx = playerPosX - posX;
        float dy = playerPosY - posY;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 1.00f) {
            //Do nothing.
        } else {
            if(!collidesWithWall(game, posX + speed * dx / distance, posY + speed * dy / distance)) {
                posX += speed * dx / distance;
                posY += speed * dy / distance;
            }
        }
    }

    float getPosX(){
        return posX;
    }
    float getPosY(){
        return posY;
    }
};

#endif
