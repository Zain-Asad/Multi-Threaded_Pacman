#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <cmath>

#include "ghosts.h"

const float PACMAN_SIZE = 0.04f;

class Ghost; // Forward declaration
class GameBoard;

class Pacman {
private:
    const int window_width, window_height;
    int pacmanPosX, pacmanPosY;
    float pacmanSpeed, pacmanRadius;
    int pacmanLives, score;
    
    sf::CircleShape pacman;

    pthread_mutex_t pacman_input;
    Direction direction;

public:
    Pacman(const int width, const int height, pthread_mutex_t pacman) : 
                                    window_width(width), window_height(height),
                                    pacmanPosX((window_width/2)), pacmanPosY((window_height/2)),
                                    pacmanSpeed(2.00f), pacmanRadius(PACMAN_SIZE / 2),
                                    pacmanLives(3), score(0),
                                    pacman_input(pacman), direction(RIGHT),
                                    pacman(pacmanRadius * 100) {}

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

    // void drawPacman(sf::RenderWindow &window) {
    //     pacman.setRadius(pacmanRadius * 200); // Increased size
    //     pacman.setFillColor(sf::Color::Yellow);
    //     pacman.setPosition((pacmanPosX + 1) * window.getSize().x / 2 - pacman.getRadius(),
    //                        (-pacmanPosY + 1) * window.getSize().y / 2 - pacman.getRadius());
    //     window.draw(pacman);
    // }

    void drawPacman(sf::RenderWindow &window) {
        pacman.setRadius(pacmanRadius * 200); // Increased size
        pacman.setFillColor(sf::Color::Yellow);
        pacman.setPosition(pacmanPosX, pacmanPosY);
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

    bool collideWithWall(GameBoard& game, int posX, int posY) {
        return game.getBoard(posY/wall_pixels, posX/wall_pixels) == 1;
    }

    // void move(GameBoard& game) {
    //     pthread_mutex_lock(&pacman_input);
    //     switch (direction) {
    //         case UP:
    //             //if(!collideWithWall(game))
    //                 pacmanPosY += pacmanSpeed;   
    //             break;
    //         case DOWN:
    //             //if(!collideWithWall(game))
    //                 pacmanPosY -= pacmanSpeed;
    //             break;
    //         case LEFT:
    //             //if(!collideWithWall(game))
    //                 pacmanPosX -= pacmanSpeed;
    //             break;
    //         case RIGHT:
    //             //if(!collideWithWall(game))
    //                 pacmanPosX += pacmanSpeed;
    //             break;
    //     }
    //     pthread_mutex_unlock(&pacman_input);
    // }

    void move(GameBoard& game) {
        pthread_mutex_lock(&pacman_input);
        switch (direction) {
            case UP:
                if(!collideWithWall(game, pacmanPosX, pacmanPosY-pacmanSpeed-3)) 
                    pacmanPosY -= pacmanSpeed;   
                break;
            case DOWN:
                if(!collideWithWall(game, pacmanPosX, pacmanPosY+pacmanSpeed+3)) 
                    pacmanPosY += pacmanSpeed;
                break;
            case LEFT:
                if(!collideWithWall(game, pacmanPosX-pacmanSpeed-3, pacmanPosY)) 
                    pacmanPosX -= pacmanSpeed;
                break;
            case RIGHT:
                if(!collideWithWall(game, pacmanPosX+pacmanSpeed+3, pacmanPosY)) 
                    pacmanPosX += pacmanSpeed;
                break;
        }
        pthread_mutex_unlock(&pacman_input);
    }
    

    void decrementPacmanLives(){
        pacmanLives--;
    }

    bool collidesWithGhost(Ghost& ghost) {
        pacman.setOrigin(pacmanRadius * 100, pacmanRadius * 100);
        pacman.setPosition((pacmanPosX + 1), (-pacmanPosY + 1));

        sf::CircleShape ghostShape(GHOST_SIZE * 100);
        ghostShape.setRadius(GHOST_SIZE * 200); // Increased size
        ghostShape.setOrigin(GHOST_SIZE * 100, GHOST_SIZE * 100);
        ghostShape.setPosition((ghost.getPosX() + 1), (-ghost.getPosY() + 1));

        return pacman.getGlobalBounds().intersects(ghostShape.getGlobalBounds());
    }
};

#endif
