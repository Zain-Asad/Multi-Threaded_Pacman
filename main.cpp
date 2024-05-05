#include "player.h"
#include "ghosts.h"
#include <unistd.h>
#include <iostream>
#include <pthread.h>

using namespace std;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int FPS = 60;
const int NUM_GHOSTS = 4;

pthread_mutex_t pacman_input;
pthread_mutex_t draw_mutex;
pthread_mutex_t ghost_mutex[NUM_GHOSTS];

Pacman player(pacman_input);
Ghost ghosts[NUM_GHOSTS] = {
    Ghost(-0.2f, 0.5f, 0.002f),
    Ghost(0.2f, 0.5f, 0.002f),
    Ghost(-0.2f, -0.5f, 0.002f),
    Ghost(0.2f, -0.5f, 0.002f)
};

bool keyStates[256];

void drawMap() {
    pthread_mutex_lock(&draw_mutex);
    glClear(GL_COLOR_BUFFER_BIT);

    player.drawPacman();

    for (int i = 0; i < NUM_GHOSTS; ++i) {
        pthread_mutex_lock(&ghost_mutex[i]);
        ghosts[i].drawGhost();
        pthread_mutex_unlock(&ghost_mutex[i]);
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.9, 0.9);
    std::string scoreStr = "Score: " + std::to_string(player.getScore()) + " Lives: " + std::to_string(player.getPacmanLives());
    for (char &c : scoreStr) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    if (!gameActive) {
        std::string message = "Game Over! Your score: " + std::to_string(player.getScore()) + ". Press r to restart or q to quit.";
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(-0.2, 0.0);
        for (char &c : message) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        }
    }

    glFlush();
    pthread_mutex_unlock(&draw_mutex);
}

void update(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000 / FPS, update, 0);
}

void keyPressed(unsigned char key, int x, int y) {
    keyStates[key] = true;
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = false;
}
bool collidesWithGhost(int i) {
    float dx = player.getPacmanPosX() - ghosts[i].getPosX();
    float dy = player.getPacmanPosY() - ghosts[i].getPosY();
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (PACMAN_SIZE + GHOST_SIZE / 2);
}

void* playerMove(void*) {
    while (gameActive) {
        player.handleKeypress(keyStates);
        player.move();

        float posX = player.getPacmanPosX();
        float posY = player.getPacmanPosY();
        posX = std::min(1.0f - PACMAN_SIZE, std::max(-1.0f + PACMAN_SIZE, posX));
        posY = std::min(0.85f - PACMAN_SIZE, std::max(-1.0f + PACMAN_SIZE, posY));
        player.setPacmanPosX(posX);
        player.setPacmanPosY(posY);

        for (int i = 0; i < NUM_GHOSTS; ++i) {
            pthread_mutex_lock(&ghost_mutex[i]);
            if (collidesWithGhost(i)) {
                player.decrementPacmanLives();
                player.setPacmanPosX(0.0f);
                player.setPacmanPosY(0.0f);

                if (player.getPacmanLives() <= 0) {
                    gameActive = false;
                }
            }
            pthread_mutex_unlock(&ghost_mutex[i]);
        }

        usleep(10000);
    }
    return NULL;
}

void* moveGhost(void* arg) {
    int ghostIndex = *((int*)arg) - 1;
    while (gameActive) {
        pthread_mutex_lock(&ghost_mutex[ghostIndex]);
        ghosts[ghostIndex].move(player.getPacmanPosX(), player.getPacmanPosY());
        pthread_mutex_unlock(&ghost_mutex[ghostIndex]);
        usleep(10000);
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_mutex_init(&pacman_input, NULL);
    pthread_mutex_init(&draw_mutex, NULL);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Pacman 1.0");
    glutDisplayFunc(drawMap);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyUp);
    glutTimerFunc(1000 / FPS, update, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black

    pthread_t playerThread;
    pthread_create(&playerThread, NULL, playerMove, NULL);
    pthread_t ghostThread[NUM_GHOSTS];

    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_mutex_init(&ghost_mutex[i], NULL);
        pthread_create(&ghostThread[i], NULL, moveGhost, (void*)&i);
    }

    glutMainLoop();

    pthread_mutex_destroy(&pacman_input);
    pthread_mutex_destroy(&draw_mutex);
    pthread_join(playerThread, NULL);
    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_mutex_destroy(&ghost_mutex[i]);
        pthread_join(ghostThread[i], NULL);
    }

    return 0;
}
