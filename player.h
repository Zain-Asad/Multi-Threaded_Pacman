#ifndef PLAYER_H
#define PLAYER_H

#include <GL/freeglut.h>
#include <iostream>
#include <pthread.h>
#include <cmath>

bool gameActive = true;

// Define directions
enum Direction { UP, DOWN, LEFT, RIGHT };

class Pacman {
private:
    float pacmanPosX, pacmanPosY;
    float pacmanSpeed, pacmanRadius;
    int pacmanLives, score;
    pthread_mutex_t pacman_input;
    Direction direction;

public:
    Pacman(pthread_mutex_t pacman) : pacmanPosX(0.0f), pacmanPosY(0.0f),
                                     pacmanSpeed(0.02f), pacmanRadius(0.015f),
                                     pacmanLives(3), score(0),
                                     pacman_input(pacman), direction(RIGHT) {}

    // Getter for pacmanPosX
    float getPacmanPosX() {
        return pacmanPosX;
    }

    // Setter for pacmanPosX
    void setPacmanPosX(float x) {
        pacmanPosX = x;
    }

    // Getter for pacmanPosY
    float getPacmanPosY() {
        return pacmanPosY;
    }

    // Setter for pacmanPosY
    void setPacmanPosY(float y) {
        pacmanPosY = y;
    }

    // Getter for score
    int getScore() {
        return score;
    }

    // Getter for pacmanLives
    int getPacmanLives() {
        return pacmanLives;
    }

    // Function to draw Pacman
    void drawPacman() {
        glColor3f(1.0f, 1.0f, 0.0f);                // Set color to yellow
        glPushMatrix();
        glTranslatef(pacmanPosX, pacmanPosY, 0.0f);
        glutSolidSphere(pacmanRadius, 20, 20);      // Draw Pacman
        glPopMatrix();
    }

    // Function to handle keyboard input
    void handleKeypress(bool keyStates[]) {
        if (!gameActive) {
            return;
        }

        pthread_mutex_lock(&pacman_input);
        if (keyStates['w']) { direction = UP; }
        if (keyStates['s']) { direction = DOWN; }
        if (keyStates['a']) { direction = LEFT; }
        if (keyStates['d']) { direction = RIGHT; }
        pthread_mutex_unlock(&pacman_input);
    }

    // Function to move Pacman
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
};

#endif
