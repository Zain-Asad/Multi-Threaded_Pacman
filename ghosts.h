#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <pthread.h>
#include <cmath>
#include <queue>
#include <semaphore.h>
#include <iostream>

#include "gameboard.h"
#include "pellets.h"

using namespace std;

bool ghost_pellet_Intersect(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance < r1 + r2 + 20;
}

const float GHOST_SIZE = 0.04f;
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Pacman;

class Ghost
{
private:
    float posX, posY;
    float homeX, homeY;
    float speed;
    float basespeed;
    Direction direction;

    float targetX, targetY;
    sf::CircleShape ghost;
    sf::Texture ghost_texture;

    bool canMove;

public:
    static sem_t permit;
    static sem_t key;
    static pthread_mutex_t homeMutex;
    bool boost;
    bool deathstate;

    Ghost(float x, float y, float s) : direction(UP), ghost(GHOST_SIZE * 200), canMove(false)
    {
        posX = x;
        posY = y;
        homeX = x;
        homeY = y;
        speed = s;
        boost = false;
        deathstate = false;
        basespeed = speed;
    }
    void ghostinit(string filename) {
        posX = homeX;
        posY = homeY;
        boost = false;
        deathstate = false;
        direction = UP;
        speed = basespeed;
        ghost_texture.loadFromFile(filename);

    }
    sf::CircleShape getGhostShape() { return ghost; }

    void drawGhost(sf::RenderWindow &window)
    {
        ghost.setRadius(GHOST_SIZE * 200);
        ghost.setPosition(posX, posY);
        ghost.setTexture(&ghost_texture);
        ghost.setScale(1.5f, 1.5f);
        window.draw(ghost);
    }

    bool collidesWithWall(GameBoard &game, int x, int y)
    {
        return game.getBoard(y / wall_pixels, x / wall_pixels) == 1;
    }

    bool collidesWithPellet(Pellets &pellet, int index, int ghostX, int ghostY)
    {
        if (pellet.getCollision_ghost(index))
            return false;
        sf::CircleShape pellet_shape;
        pellet_shape.setRadius(10);
        pellet_shape.setPosition((pellet.getPosX_ghost(index) + 1), (-pellet.getPosY_ghost(index) + 1));

        return ghost_pellet_Intersect(ghostX, ghostY, GHOST_SIZE * 200, pellet.getPosX_ghost(index), pellet.getPosY_ghost(index), 10);
    }

    vector<pair<int, int>> shortestPath(GameBoard &game, int startX, int startY, int endX, int endY)
    {
        vector<vector<int>> dist(game.getBoardYSize(), vector<int>(game.getBoardXSize(), INT_MAX));
        vector<vector<pair<int, int>>> parent(game.getBoardYSize(), vector<pair<int, int>>(game.getBoardXSize(), make_pair(-1, -1)));
        queue<pair<int, int>> q;
        q.push(make_pair(startY, startX));
        dist[startY][startX] = 0;

        while (!q.empty())
        {
            int row = q.front().first;
            int col = q.front().second;
            q.pop();
            vector<pair<int, int>> adjCells = game.getAdjacentCells(row, col);
            for (const auto &cell : adjCells)
            {
                int adjRow = cell.first;
                int adjCol = cell.second;
                if (dist[adjRow][adjCol] == INT_MAX && !game.getBoard(adjRow, adjCol))
                {
                    dist[adjRow][adjCol] = dist[row][col] + 1;
                    parent[adjRow][adjCol] = make_pair(row, col);
                    q.push(make_pair(adjRow, adjCol));
                }
            }
        }

        vector<pair<int, int>> path;
        int row = endY, col = endX;
        while (parent[row][col] != make_pair(-1, -1))
        {
            path.push_back(make_pair(row, col));
            int newRow = parent[row][col].first;
            int newCol = parent[row][col].second;
            row = newRow;
            col = newCol;
        }
        reverse(path.begin(), path.end());
        return path;
    }

    int shortestPathDistance(GameBoard &game, int startX, int startY, int endX, int endY)
    {
        if (startX == endX && startY == endY)
        {
            return 0;
        }

        int boardYSize = game.getBoardYSize();
        int boardXSize = game.getBoardXSize();
        vector<vector<int>> dist(boardYSize, vector<int>(boardXSize, INT_MAX));
        queue<pair<int, int>> q;

        dist[startY][startX] = 0;
        q.push({startY, startX});

        while (!q.empty())
        {
            auto [row, col] = q.front();
            q.pop();

            vector<pair<int, int>> adjCells = game.getAdjacentCells(row, col);
            for (const auto &[adjRow, adjCol] : adjCells)
            {
                if (dist[adjRow][adjCol] == INT_MAX && game.getBoard(adjRow, adjCol) == 0)
                {
                    dist[adjRow][adjCol] = dist[row][col] + 1;
                    q.push({adjRow, adjCol});

                    if (adjRow == endY && adjCol == endX)
                    {
                        return dist[adjRow][adjCol];
                    }
                }
            }
        }
        return dist[endY][endX] == INT_MAX ? -1 : dist[endY][endX];
    }

    void move(GameBoard &game, int playerPosX, int playerPosY, Pellets &pellet, int index, sf::Clock &c, pthread_mutex_t &ghostpellet_mutex)
    {
        if (!deathstate)
        {
            int distance_btw_pellet = INT_MAX;
            pthread_mutex_lock(&ghostpellet_mutex);
            if (index < num_ghost_pellets)
                if (!pellet.getCollision_ghost(index))
                    distance_btw_pellet = shortestPathDistance(game, posX / wall_pixels, posY / wall_pixels, pellet.getPosX_ghost(index) / wall_pixels, pellet.getPosY_ghost(index) / wall_pixels);
            pthread_mutex_unlock(&ghostpellet_mutex);
            int distance_btw_player = shortestPathDistance(game, posX / wall_pixels, posY / wall_pixels, playerPosX / wall_pixels, playerPosY / wall_pixels);

            float minX, minY;

            if (distance_btw_pellet <= distance_btw_player)
            {
                minX = pellet.getPosX_ghost(index);
                minY = pellet.getPosY_ghost(index);
            }
            else
            {
                minX = playerPosX;
                minY = playerPosY;
            }
            vector<pair<int, int>> path = shortestPath(game, posX / wall_pixels, posY / wall_pixels, minX / wall_pixels, minY / wall_pixels);
            if (!path.empty())
            {
                int nextRow = path[0].first;
                int nextCol = path[0].second;
                float targetX = nextCol * wall_pixels + wall_pixels / 2;
                float targetY = nextRow * wall_pixels + wall_pixels / 2;
                float dx = targetX - posX;
                float dy = targetY - posY;
                float distance = sqrt(dx * dx + dy * dy);

                if (!collidesWithWall(game, posX + speed * dx / distance, posY + speed * dy / distance))
                {
                    posX += speed * dx / distance;
                    posY += speed * dy / distance;
                }
            }
            if (collidesWithPellet(pellet, index, posX + speed, posY + speed))
            {
                pellet.setCollision_ghost(index, true);
                pellet.setPosition_ghost(index);
                increaseSpeed(0.5);
                boost = true;
                c.restart();
            }
        }
        else{
            if(!((abs(posX - homeX) <= 30) && (abs(posY - homeY) <= 30))){
                vector<pair<int, int>> path = shortestPath(game, posX / wall_pixels, posY / wall_pixels, homeX / wall_pixels, homeY / wall_pixels);
                if (!path.empty())
                {
                    int nextRow = path[0].first;
                    int nextCol = path[0].second;
                    float targetX = nextCol * wall_pixels + wall_pixels / 2;
                    float targetY = nextRow * wall_pixels + wall_pixels / 2;
                    float dx = targetX - posX;
                    float dy = targetY - posY;
                    float distance = sqrt(dx * dx + dy * dy);

                    if (!collidesWithWall(game, posX + speed * dx / distance, posY + speed * dy / distance))
                    {
                        posX += speed * dx / distance;
                        posY += speed * dy / distance;
                    }
                }
            }
            else{
                arrivesatHome();
                cout << "Reached Home\n";
                usleep(100);
            }
        }
    }

    bool ghostHouse(int index)
    {
        bool permitAcquired = false;
        bool keyAcquired = false;

        if (sem_trywait(&permit) == 0)
        {
            permitAcquired = true;
            cout << index << " Acquired Permit\n";
        }
        if (sem_trywait(&key) == 0)
        {
            keyAcquired = true;
            cout << index << " Acquired Key\n";
        }

        if (permitAcquired && keyAcquired)
        {
            return true;
        }

        if (permitAcquired)
        {
            sem_post(&permit);
            cout << index << " Relinquished Permit\n";
        }
        if (keyAcquired)
        {
            sem_post(&key);
            cout << index << " Relinquished Key\n";
        }

        return false;
    }

    void increaseSpeed(float value)
    {
        speed = basespeed + value;
    }
    void decreaseSpeed(float value)
    {
        speed = basespeed - value;
    }

    float getPosX() { return posX; }
    float getPosY() { return posY; }

    void setPosX(float value) { posX = value; }
    void setPosY(float value) { posY = value; }

    bool getcanMove() { return canMove; }
    void setcanMove(bool x) { canMove = x; }

    void death()
    {
        deathstate = true;
    }
    void arrivesatHome()
    {
        pthread_mutex_lock(&homeMutex);
        sem_post(&permit);
        cout << "Posting Permit\n";
        sem_post(&key);
        cout << "Posting Key\n";
        pthread_mutex_unlock(&homeMutex);
        deathstate = false;
        canMove = false;
        usleep(1000);
    }

    void setTexture(string filename){
        ghost_texture.loadFromFile(filename);
    }
};
sem_t Ghost::permit;
sem_t Ghost::key;
pthread_mutex_t Ghost::homeMutex;
