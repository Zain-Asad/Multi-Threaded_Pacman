#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <pthread.h>
#include <cmath>
#include <queue>
#include "gameboard.h"
#include <semaphore.h>

using namespace std;

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
    float speed;
    Direction direction;

    float targetX, targetY;
    sf::CircleShape ghost;

    bool canMove;
    
public:
    static sem_t permit;
    static sem_t key;
    
    Ghost(float x, float y, float s) : direction(UP), ghost(GHOST_SIZE * 100), canMove(false) {
        posX = x;
        posY = y;
        speed = s;
    }

    void drawGhost(sf::RenderWindow &window)
    {
        ghost.setRadius(GHOST_SIZE * 200);
        ghost.setFillColor(sf::Color::Red);
        ghost.setPosition(posX, posY);
        window.draw(ghost);
    }

    bool collidesWithWall(GameBoard &game, int x, int y)
    {
        return game.getBoard(y / wall_pixels, x / wall_pixels) == 1;
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

    void move(GameBoard &game, int playerPosX, int playerPosY)
    {
        vector<pair<int, int>> path = shortestPath(game, posX / wall_pixels, posY / wall_pixels, playerPosX / wall_pixels, playerPosY / wall_pixels);
        if (!path.empty())
        {
            int nextRow = path[0].first;
            int nextCol = path[0].second;
            float targetX = nextCol * wall_pixels + wall_pixels / 2;
            float targetY = nextRow * wall_pixels + wall_pixels / 2;
            float dx = targetX - posX;
            float dy = targetY - posY;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance > 1.00f)
            {
                if (!collidesWithWall(game, posX + speed * dx / distance, posY + speed * dy / distance))
                {
                    posX += speed * dx / distance;
                    posY += speed * dy / distance;
                }
            }
        }
    }

    bool ghostHouse() {
        bool permitAcquired = false;
        bool keyAcquired = false;

        if (sem_trywait(&permit) == 0) {
            permitAcquired = true;
        }
        if (sem_trywait(&key) == 0) {
            keyAcquired = true;
        }

        if (permitAcquired && keyAcquired) {
            return true;
            canMove = true;
        }

        if (permitAcquired) {
            sem_post(&permit);
        }
        if (keyAcquired) {
            sem_post(&key);
        }

        return false;
    }

    float getPosX()
    {
        return posX;
    }

    float getPosY()
    {
        return posY;
    }

    bool getcanMove() {return canMove;}
    void setcanMove(bool x) {canMove = x;}
};
sem_t Ghost::permit;
sem_t Ghost::key;
