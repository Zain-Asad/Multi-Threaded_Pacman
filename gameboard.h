#pragma once

#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

static const int wall_pixels = 30;
static const int coin_pixels = 8;

class GameBoard {
private:
    static const int upper_limit = 4;
    static const int coin_radius = 0.04f;

    int boardXsize, boardYsize;
    vector<vector<int>> board;

    sf::RectangleShape wall;
    sf::Texture water;

    sf::RectangleShape coin;

public:
    GameBoard(int WINDOW_WIDTH, int WINDOW_HEIGHT) : boardXsize(WINDOW_WIDTH/wall_pixels), boardYsize(WINDOW_HEIGHT/wall_pixels), board(boardYsize, vector<int>(boardXsize)),
                                                     wall (sf::Vector2f(wall_pixels, wall_pixels)), coin(sf::Vector2f(coin_pixels, coin_pixels))
    { 
        water.loadFromFile("Assets/level1_block.png");
        board = { 
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, 
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, 
            {1,0,0,0,2,2,2,2,2,2,0,0,0,0,1,1,1,1,1,2,2, 2,2,1,1,1,1,1,0,0,0,0,2,2,2,2,2,2,0,0,0,1}, 
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, 
            {1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,2,2,2, 2,2,2,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,1}, 
            {1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1}, 
            {1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1}, 
            {1,0,0,0,1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,1,0,0,0,1}, 
        
            {1,0,0,0,1,0,0,0,0,1,0,2,0,0,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,0,0,2,0,1,0,0,0,0,1,0,0,0,1}, 
            {1,0,0,0,1,2,2,2,2,1,0,2,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,2,0,1,2,2,2,2,1,0,0,0,1}, 
            {1,0,0,0,1,0,0,0,0,1,0,2,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,2,0,1,0,0,0,0,1,0,0,0,1}, 
            {1,0,0,0,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,1}, 
            {1,0,0,0,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,1}, 
            {1,0,0,1,1,1,1,1,0,0,0,2,0,0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,0,2,0,0,0,1,1,1,1,1,0,0,1}, 
            {1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1}, 
            {1,0,0,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,0,0,1}, 
            {1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,0, 0,0,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1}, 
            {1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1}, 
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, 
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} 
        }; 
    }
    
    vector<pair<int, int>> getAdjacentCells(int row, int col) {
        vector<pair<int, int>> adjCells;
        if (row > 0)
            adjCells.push_back(make_pair(row - 1, col));     // Up
        if (row < boardYsize - 1)
            adjCells.push_back(make_pair(row + 1, col));     // Down
        if (col > 0)
            adjCells.push_back(make_pair(row, col - 1));     // Left
        if (col < boardXsize - 1)
            adjCells.push_back(make_pair(row, col + 1));     // Right
        return adjCells;
    }
    
    int getBoard(int row, int column) { 
        if(row>=0 && row<boardYsize && column>=0 && column<boardXsize)
            return board[row][column];  
        else
            return -1;
    }

    void setBoard(int row, int column, int value) {
        board[row][column] = value;
    }

    int getBoardXSize() {   return boardXsize;  }
    int getBoardYSize() {   return boardYsize;  }

    void drawGameBoard(sf::RenderWindow& window) {
        int count=0;
        for(int row=upper_limit; row<boardYsize; row++) {
            for(int col=0; col<boardXsize; col++) {
                if(board[row][col] == 1) {
                    wall.setPosition(col * wall_pixels, row * wall_pixels);
                    wall.setTexture(&water);
                    window.draw(wall);
                }
                 if(board[row][col] == 2) {
                    coin.setFillColor(sf::Color::Yellow);
                    coin.setPosition(col*wall_pixels, row*wall_pixels);
                    window.draw(coin);
                }
            }
        }
    }


};