#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <cmath>
#include "ghosts.h"

const float PACMAN_SIZE = 0.04f;

class Ghost; // Forward declaration

class Pacman {
private:
    float pacmanPosX, pacmanPosY;
    float pacmanSpeed, pacmanRadius;
    int pacmanLives, score;
    pthread_mutex_t pacman_input;
    Direction direction;

public:
    Pacman(pthread_mutex_t pacman) : pacmanPosX(0.0f), pacmanPosY(0.0f),
                                     pacmanSpeed(0.003f), pacmanRadius(PACMAN_SIZE / 2),
                                     pacmanLives(3), score(0),
                                     pacman_input(pacman), direction(RIGHT) {}

    float getPacmanPosX() {
        return pacmanPosX;
    }

    void setPacmanPosX(float x) {
        pacmanPosX = x;
    }

    float getPacmanPosY() {
        return pacmanPosY;
    }

    void setPacmanPosY(float y) {
        pacmanPosY = y;
    }

    int getScore() {
        return score;
    }

    int getPacmanLives() {
        return pacmanLives;
    }

    void drawPacman(sf::RenderWindow &window) {
        sf::CircleShape pacman(pacmanRadius * 100);
        pacman.setRadius(pacmanRadius * 200); // Increased size
        pacman.setFillColor(sf::Color::Yellow);
        pacman.setPosition((pacmanPosX + 1) * window.getSize().x / 2 - pacman.getRadius(),
                           (-pacmanPosY + 1) * window.getSize().y / 2 - pacman.getRadius());
        window.draw(pacman);
    }

    void handleKeypress() {
        pthread_mutex_lock(&pacman_input);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { direction = UP; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { direction = DOWN; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { direction = LEFT; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { direction = RIGHT; }
        pthread_mutex_unlock(&pacman_input);
    }

    void move() {
        pthread_mutex_lock(&pacman_input);
        switch (direction) {
            case UP:
                pacmanPosY += pacmanSpeed;
                break;
            case DOWN:
                pacmanPosY -= pacmanSpeed;
                break;
            case LEFT:
                pacmanPosX -= pacmanSpeed;
                break;
            case RIGHT:
                pacmanPosX += pacmanSpeed;
                break;
        }
        pthread_mutex_unlock(&pacman_input);
    }

    void decrementPacmanLives(){
        pacmanLives--;
    }

    bool collidesWithGhost(Ghost& ghost) {
        sf::CircleShape pacman(pacmanRadius * 100);
        pacman.setRadius(pacmanRadius * 200); // Increased size
        pacman.setOrigin(pacmanRadius * 100, pacmanRadius * 100);
        pacman.setPosition((pacmanPosX + 1) * 640, (-pacmanPosY + 1) * 360);

        sf::CircleShape ghostShape(GHOST_SIZE * 100);
        ghostShape.setRadius(GHOST_SIZE * 200); // Increased size
        ghostShape.setOrigin(GHOST_SIZE * 100, GHOST_SIZE * 100);
        ghostShape.setPosition((ghost.getPosX() + 1) * 640, (-ghost.getPosY() + 1) * 360);

        return pacman.getGlobalBounds().intersects(ghostShape.getGlobalBounds());
    }
};

#endif
