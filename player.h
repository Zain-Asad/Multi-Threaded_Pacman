#ifndef PLAYER_H
#define PLAYER_H

#include <GL/freeglut.h>
#include <iostream>
#include <pthread.h>
#include <cmath>

bool gameActive = true;
const float PACMAN_SIZE = 0.03f;

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

    void drawPacman() {
        glColor3f(1.0f, 1.0f, 0.0f);             
        glPushMatrix();
        glTranslatef(pacmanPosX, pacmanPosY, 0.0f);
        glutSolidSphere(pacmanRadius, 20, 20);  
        glPopMatrix();
    }

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

};

#endif
