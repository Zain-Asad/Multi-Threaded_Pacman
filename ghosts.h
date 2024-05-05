#ifndef GHOSTS_H
#define GHOSTS_H

#include <GL/freeglut.h>
#include <iostream>
#include <pthread.h>
#include <cmath>
#include "player.h"

extern bool gameActive;
const float GHOST_SIZE = 0.04f;

class Ghost {
private:
    float posX, posY;
    float speed;
    Direction direction;

    float targetX, targetY;

public:
    Ghost(float x, float y, float s) : posX(x), posY(y), speed(s), direction(UP) {}

    void drawGhost() {
        glColor3f(1.0f, 0.0f, 0.0f); 
        glPushMatrix();
        glTranslatef(posX, posY, 0.0f);
        glutSolidSphere(GHOST_SIZE / 2, 20, 20); 
        glPopMatrix();
    }

    void move(float playerPosX, float playerPosY) {
        if (gameActive) {
            float dx = playerPosX - posX;
            float dy = playerPosY - posY;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 0.001f) {
                gameActive = false;
            } else {
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
