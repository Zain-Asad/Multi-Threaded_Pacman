#ifndef GHOSTS_H
#define GHOSTS_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <pthread.h>
#include <cmath>

const float GHOST_SIZE = 0.04f;
enum Direction { UP, DOWN, LEFT, RIGHT };

class Pacman; // Forward declaration

class Ghost {
private:
    float posX, posY;
    float speed;
    Direction direction;

    float targetX, targetY;

public:
    Ghost(float x, float y, float s) : posX(x), posY(y), speed(s), direction(UP) {}

    void drawGhost(sf::RenderWindow &window) {
        sf::CircleShape ghost(GHOST_SIZE * 100);
        ghost.setRadius(GHOST_SIZE * 200); // Increased size
        ghost.setFillColor(sf::Color::Red);
        ghost.setPosition((posX + 1) * window.getSize().x / 2 - ghost.getRadius(),
                          (-posY + 1) * window.getSize().y / 2 - ghost.getRadius());
        window.draw(ghost);
    }

    void move(float playerPosX, float playerPosY) {
        float dx = playerPosX - posX;
        float dy = playerPosY - posY;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 0.001f) {
            
        } else {
            posX += speed * dx / distance;
            posY += speed * dy / distance;
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
