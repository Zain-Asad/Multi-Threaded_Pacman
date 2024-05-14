#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <iostream>

#include "player.h"

// Constant variables.
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int FPS = 60;
const int NUM_GHOSTS = 4;

sf::Clock clock_game;

//Mutex initilizations.
pthread_mutex_t pacman_input, ghost_mutex[NUM_GHOSTS];
pthread_mutex_t draw_mutex;
pthread_mutex_t coin_score;
pthread_mutex_t pellet_mutex;

//Class objects initilizations.
GameBoard game(WINDOW_WIDTH, WINDOW_HEIGHT);
Pacman player(WINDOW_WIDTH, WINDOW_HEIGHT, pacman_input, coin_score, pellet_mutex);
Ghost ghosts[NUM_GHOSTS] = {
    Ghost(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 80, 0.5f),
    Ghost(WINDOW_WIDTH/2 + 100, WINDOW_HEIGHT/2 - 80, 0.3f),
    Ghost(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 80, 0.8f),
    Ghost(WINDOW_WIDTH/2 + 100, WINDOW_HEIGHT/2 + 80, 0.1f)
};

Pellets pellet;     int player_pellet_Idx = 0, ghost_pellet_Idx = 0;

bool gameActive = true;

void drawMap(sf::RenderWindow &window) {
    pthread_mutex_lock(&draw_mutex);
    window.clear(sf::Color::Black);

    game.drawGameBoard(window);
    player.drawPacman(window);
    pellet.drawPellet(window, player_pellet_Idx, ghost_pellet_Idx);

    for (int i = 0; i < NUM_GHOSTS; ++i) {
        pthread_mutex_lock(&ghost_mutex[i]);
        ghosts[i].drawGhost(window);
        pthread_mutex_unlock(&ghost_mutex[i]);
    }

    sf::Font font;
    if (!font.loadFromFile("Assets/Samdan.ttf")) {
        std::cerr << "Error loading font\n";
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    std::string scoreStr = "Score: " + std::to_string(player.getScore()) + " Lives: " + std::to_string(player.getPacmanLives());
    text.setString(scoreStr);
    text.setPosition(25, 30);
    window.draw(text);

    if (!gameActive) {
        std::string message = "Game Over! Your score: " + std::to_string(player.getScore());
        text.setString(message);
        text.setPosition(-150, 0);
        window.draw(text);
    }

    window.display();
    pthread_mutex_unlock(&draw_mutex);
}

void update(sf::RenderWindow &window) {
    drawMap(window);
}

void keyPressed(sf::Event::KeyEvent &keyEvent) {
    player.handleKeypress();
}

void keyReleased(sf::Event::KeyEvent &keyEvent) {}

void* playerMove(void*) {
    while (gameActive) {
        pthread_mutex_lock(&pellet_mutex);
        player.move(game, pellet, player_pellet_Idx, clock_game);        
        pthread_mutex_unlock(&pellet_mutex);

        for (int i = 0; i < NUM_GHOSTS; ++i) {
            pthread_mutex_lock(&ghost_mutex[i]);        
            if (!pellet.getCollision_player(player_pellet_Idx) && player.collidesWithGhost(ghosts[i])) {
                player.decrementPacmanLives();
                player.setPacmanPosX(WINDOW_WIDTH/2);
                player.setPacmanPosY(WINDOW_HEIGHT/2);

                if (player.getPacmanLives() <= 0) {
                    gameActive = false;
                }
            }
            else if(pellet.getCollision_player(player_pellet_Idx) && player.collidesWithGhost(ghosts[i])) {
                ghosts[i].setPosX(WINDOW_WIDTH/2);      ghosts[i].setPosY(WINDOW_HEIGHT/2);      
            }

            if(player_pellet_Idx<num_player_pellets && pellet.getCollision_player(player_pellet_Idx) && clock_game.getElapsedTime().asSeconds() > 10) {
                player_pellet_Idx++;
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
        if (ghosts[ghostIndex].ghostHouse()) {
            ghosts[ghostIndex].setcanMove(true);
        }
        pthread_mutex_unlock(&ghost_mutex[ghostIndex]);

        if (ghosts[ghostIndex].getcanMove()) {
            pthread_mutex_lock(&ghost_mutex[ghostIndex]);       //pthread_mutex_lock(&pellet_mutex);
            ghosts[ghostIndex].move(game, player.getPacmanPosX(), player.getPacmanPosY(), pellet, ghost_pellet_Idx, clock_game);
            pthread_mutex_unlock(&ghost_mutex[ghostIndex]);     //pthread_mutex_unlock(&pellet_mutex);
        }

        if(ghost_pellet_Idx<num_ghost_pellets && pellet.getCollision_ghost(ghost_pellet_Idx) && clock_game.getElapsedTime().asSeconds()>5) {    ghost_pellet_Idx++; }
        if(pellet.getCollision_ghost(ghost_pellet_Idx))  { ghosts[ghostIndex].increaseSpeed(1.5); }

        usleep(10000);
    }
    return NULL;
}



int main() {
    pthread_mutex_init(&pacman_input, NULL);    pthread_mutex_init(&draw_mutex, NULL);  pthread_mutex_init(&coin_score, NULL);
    pthread_mutex_init(&pellet_mutex, NULL);
    sem_init(&Ghost::permit, 0, 2);     sem_init(&Ghost::key, 0, 2);
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pacman 1.0");
    sf::Vector2i winoffset(100,100);
    window.setPosition(winoffset);

    pthread_t playerThread;
    pthread_create(&playerThread, NULL, playerMove, NULL);
    
    pthread_t ghostThread[NUM_GHOSTS];
    int initThread = 0;
    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_mutex_init(&ghost_mutex[i], NULL);
        pthread_create(&ghostThread[i], NULL, moveGhost, (void*) &initThread);
        initThread++;
    }

    window.setFramerateLimit(FPS);

    while (window.isOpen()) {
        sf::Event event;
        bool closed = false;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed){
                closed = true;
                gameActive = false;
                window.close();
            }
            if (event.type == sf::Event::KeyPressed)
                keyPressed(event.key);
            if (event.type == sf::Event::KeyReleased)
                keyReleased(event.key);
        }
        if(closed) break;
        update(window);
    }

    pthread_mutex_destroy(&pacman_input);   pthread_mutex_destroy(&draw_mutex);     pthread_mutex_destroy(&coin_score);
    pthread_mutex_destroy(&pellet_mutex);
    
    pthread_join(playerThread, NULL);
    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_mutex_destroy(&ghost_mutex[i]);
        pthread_join(ghostThread[i], NULL);
    }

    return 0;
}