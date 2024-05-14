#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include "soundsystem.h"
#include "player.h"
#include "menus.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int FPS = 60;
const int NUM_GHOSTS = 4;

sf::Clock clock_game;
sf::Clock clock_game1;

pthread_mutex_t pacman_input, ghost_mutex[NUM_GHOSTS];
pthread_mutex_t draw_mutex;
pthread_mutex_t coin_score;
pthread_mutex_t pellet_mutex;
pthread_mutex_t ghostpellet_mutex;

sf::Sprite background;
sf::Texture bg_texture;

GameBoard game(WINDOW_WIDTH, WINDOW_HEIGHT);
Pellets pellet;

Pacman player(WINDOW_WIDTH, WINDOW_HEIGHT, pacman_input, coin_score, pellet_mutex);
Ghost ghosts[NUM_GHOSTS] = {
    Ghost(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 80, 0.5f),
    Ghost(WINDOW_WIDTH / 2 + 50, WINDOW_HEIGHT / 2 + 80, 0.7f),
    Ghost(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 80, 0.9f),
    Ghost(WINDOW_WIDTH / 2 + 100, WINDOW_HEIGHT / 2 + 80, 1.0f)};

int player_pellet_Idx = 0, ghost_pellet_Idx = 0;

MenuSystem::GameState gameState = MenuSystem::MAIN_MENU;
bool gameActive = true;

void drawMap(sf::RenderWindow &window) {
    pthread_mutex_lock(&draw_mutex);
    window.clear(sf::Color::Black);
    window.draw(background);

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
    if (gameState == MenuSystem::PLAYING) {
        drawMap(window);
    } else if (gameState == MenuSystem::MAIN_MENU) {
        MenuSystem::drawMainMenu(window);
    } else if (gameState == MenuSystem::PAUSED) {
        MenuSystem::drawPauseMenu(window);
    } else if (gameState == MenuSystem::GAME_OVER) {
        MenuSystem::drawGameOverScreen(window, player.getScore());
    }
}

void keyPressed(sf::Event::KeyEvent &keyEvent) {
    if (gameState == MenuSystem::MAIN_MENU && keyEvent.code == sf::Keyboard::Enter) {
        gameState = MenuSystem::PLAYING;
    } else if (gameState == MenuSystem::PAUSED && keyEvent.code == sf::Keyboard::Enter) {
        gameState = MenuSystem::PLAYING;
    } else if (gameState == MenuSystem::PAUSED && keyEvent.code == sf::Keyboard::Escape) {
        gameState = MenuSystem::MAIN_MENU;
    } else if (gameState == MenuSystem::GAME_OVER && keyEvent.code == sf::Keyboard::Enter) {
        gameState = MenuSystem::MAIN_MENU;
    } else if (gameState == MenuSystem::PLAYING && keyEvent.code == sf::Keyboard::Escape) {
        gameState = MenuSystem::PAUSED;
    } else if (gameState == MenuSystem::PLAYING) {
        player.handleKeypress();
    }
}

void keyReleased(sf::Event::KeyEvent &keyEvent) {}

void *playerMove(void *) {
    while (gameActive) {
        if (gameState == MenuSystem::PLAYING) {
            pthread_mutex_lock(&pellet_mutex);
            player.move(game, pellet, player_pellet_Idx, clock_game1);
            pthread_mutex_unlock(&pellet_mutex);

            for (int i = 0; i < NUM_GHOSTS; ++i) {
                pthread_mutex_lock(&ghost_mutex[i]);
                pthread_mutex_lock(&pellet_mutex);
                if (!ghosts[i].deathstate) {
                    if (!pellet.getCollision_player(player_pellet_Idx) && player.collidesWithGhost(ghosts[i])) {
                        player.decrementPacmanLives();
                        player.setPacmanPosX(WINDOW_WIDTH / 2);
                        player.setPacmanPosY(WINDOW_HEIGHT / 2);

                        if (player.getPacmanLives() <= 0) {
                            gameActive = false;
                            gameState = MenuSystem::GAME_OVER;
                        }
                    } else if (player.power && player.collidesWithGhost(ghosts[i])) {
                        ghosts[i].death();
                    }
                }
                if (player_pellet_Idx < num_player_pellets && pellet.getCollision_player(player_pellet_Idx) && clock_game1.getElapsedTime().asSeconds() > 5) {
                    player.power = false;
                    player_pellet_Idx++;
                    if (player_pellet_Idx >= num_player_pellets)
                        player_pellet_Idx--;
                    clock_game1.restart();
                }
                pthread_mutex_unlock(&pellet_mutex);
                pthread_mutex_unlock(&ghost_mutex[i]);
            }

            usleep(10000);
        }
    }
    return NULL;
}

void *moveGhost(void *arg) {
    int ghostIndex = *((int *)arg);
    while (gameActive) {
        if (gameState == MenuSystem::PLAYING) {
            pthread_mutex_lock(&ghost_mutex[ghostIndex]);
            bool canMove = ghosts[ghostIndex].getcanMove();
            if (!canMove) {
                if (ghosts[ghostIndex].ghostHouse(ghostIndex)) {
                    ghosts[ghostIndex].setcanMove(true);
                }
            }
            pthread_mutex_unlock(&ghost_mutex[ghostIndex]);

            if (canMove) {
                pthread_mutex_lock(&ghostpellet_mutex);
                if (ghost_pellet_Idx < num_ghost_pellets && pellet.getCollision_ghost(ghost_pellet_Idx) && clock_game.getElapsedTime().asSeconds() > 2) {
                    pthread_mutex_lock(&ghost_mutex[ghostIndex]);
                    if (ghosts[ghostIndex].boost) {
                        clock_game.restart();
                        ghosts[ghostIndex].decreaseSpeed(0.5);
                        ghosts[ghostIndex].boost = false;
                    }
                    pthread_mutex_unlock(&ghost_mutex[ghostIndex]);
                    ghost_pellet_Idx++;
                    if (ghost_pellet_Idx >= num_ghost_pellets)
                        ghost_pellet_Idx--;
                }
                pthread_mutex_unlock(&ghostpellet_mutex);

                pthread_mutex_lock(&ghost_mutex[ghostIndex]);
                ghosts[ghostIndex].move(game, player.getPacmanPosX(), player.getPacmanPosY(), pellet, ghost_pellet_Idx, clock_game, ghostpellet_mutex);
                pthread_mutex_unlock(&ghost_mutex[ghostIndex]);
            }

            usleep(10000);
        }
    }
    return NULL;
}

void initGameObjects(int level) {
        
    switch (level) {
        case 1:
            player.pacmaninit("Assets/player1.png");
            ghosts[0].ghostinit("Assets/ghost1_1.png");
            ghosts[1].ghostinit("Assets/ghost1_2.png");
            ghosts[2].ghostinit("Assets/ghost1_3.png");
            ghosts[3].ghostinit("Assets/ghost1_4.png");
            pellet.pelletinit();
            game.gameinit("Assets/level1_block.png");
            bg_texture.loadFromFile("Assets/level1_bg.png");
            background.setTexture(bg_texture);
            break;
        case 2:
            player.pacmaninit("Assets/player2.png");
            ghosts[0].ghostinit("Assets/ghost2_1.png");
            ghosts[1].ghostinit("Assets/ghost2_2.png");
            ghosts[2].ghostinit("Assets/ghost2_3.png");
            ghosts[3].ghostinit("Assets/ghost2_4.png");
            pellet.pelletinit();
            game.gameinit("Assets/level2_block.png");
            bg_texture.loadFromFile("Assets/level2_bg.png");
            background.setTexture(bg_texture);
            break;
        default:
            std::cerr << "Invalid level\n";
            exit(EXIT_FAILURE);
    }
}

int main() {

    int level;
    cout << "Enter Level {1 ,2 ,3}: ";
    cin>>level;
    initGameObjects(level);

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pacman 1.0");
    sf::Vector2i winoffset(100, 100);
    window.setPosition(winoffset);

    pthread_mutex_init(&pacman_input, NULL);
    pthread_mutex_init(&draw_mutex, NULL);
    pthread_mutex_init(&coin_score, NULL);
    pthread_mutex_init(&pellet_mutex, NULL);
    pthread_mutex_init(&ghostpellet_mutex, NULL);
    sem_init(&Ghost::permit, 0, 2);
    sem_init(&Ghost::key, 0, 2);
    pthread_mutex_init(&Ghost::homeMutex, NULL);

    pthread_t playerThread;
    pthread_create(&playerThread, NULL, playerMove, NULL);

    pthread_t ghostThread[NUM_GHOSTS];
    int initThread[NUM_GHOSTS];
    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_mutex_init(&ghost_mutex[i], NULL);
        initThread[i] = i;
        pthread_create(&ghostThread[i], NULL, moveGhost, (void *)&initThread[i]);
    }

    window.setFramerateLimit(FPS);
    while (window.isOpen()) {
        sf::Event event;
        bool closed = false;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                closed = true;
                gameActive = false;
                window.close();
            }
            if (event.type == sf::Event::KeyPressed)
                keyPressed(event.key);
            if (event.type == sf::Event::KeyReleased)
                keyReleased(event.key);
        }
        if (closed)
            break;
        update(window);
    }

    pthread_mutex_destroy(&pacman_input);
    pthread_mutex_destroy(&draw_mutex);
    pthread_mutex_destroy(&coin_score);
    pthread_mutex_destroy(&pellet_mutex);

    pthread_join(playerThread, NULL);
    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_mutex_destroy(&ghost_mutex[i]);
        pthread_join(ghostThread[i], NULL);
    }

    return 0;
}
