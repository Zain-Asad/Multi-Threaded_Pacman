#include "player.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int FPS = 60;

// All the mutexes.
pthread_mutex_t pacman_input;

// All the class/structure variables.
Pacman player(pacman_input);

bool keyStates[256]; // Tracks key states for all keys

void drawMap() {
    glClear(GL_COLOR_BUFFER_BIT);
    player.drawPacman();
    // Print score
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
}

void update(int value) {
    player.handleKeypress(keyStates);
    player.move();
    glutPostRedisplay();
    glutTimerFunc(5000 / FPS, update, 0);
}

void keyPressed(unsigned char key, int x, int y) {
    keyStates[key] = true;
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = false;
}

int main(int argc, char **argv) {
    pthread_mutex_init(&pacman_input, NULL);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Pacman 1.0");
    glutDisplayFunc(drawMap);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyUp);
    glutTimerFunc(1000 / FPS, update, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black

    glutMainLoop();

    pthread_mutex_destroy(&pacman_input);
    return 0;
}
