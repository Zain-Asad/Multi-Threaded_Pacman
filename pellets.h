#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

const int num_player_pellets = 4;
const int num_ghost_pellets = 2;

class Position {
private:
    int posX, posY;
public:
    Position(int x, int y) : posX(x), posY(y) {}
    int getX() const { return posX; }
    int getY() const { return posY; }

    void setX(int value) {  posX = value; }
    void setY(int value) {  posY = value; }

    friend class Pellet;
};

class Pellet {
private:
    sf::CircleShape pellet;
    Position position;
    bool eaten;
public:
    Pellet(int x, int y, float radius, sf::Color c) : position(x, y), eaten(false) {
        pellet.setRadius(radius);
        pellet.setFillColor(c);
        pellet.setPosition(x, y);
    }

    bool getEaten() { return eaten; }
    void setEaten() { eaten = true; }
    friend class Pellets;
};

class Pellets {
private:
    std::vector<Pellet> player_pellet;
    std::vector<Pellet> ghost_pellet;
public:
    Pellets() {
        // Initialize player pellets
        player_pellet.emplace_back(39*30, 13*30, 10, sf::Color::Cyan); 
        player_pellet.emplace_back(2*30, 22*30, 10, sf::Color::Cyan);
        player_pellet.emplace_back(29*30, 6*30, 10, sf::Color::Cyan);
        player_pellet.emplace_back(1*30, 9*30, 10, sf::Color::Cyan);

        // Initialize ghost pellets
        ghost_pellet.emplace_back(3*30, 6*30, 10, sf::Color::Green); 
        ghost_pellet.emplace_back(29*30, 11*30, 10, sf::Color::Green);
    }

    void drawPellet(sf::RenderWindow& window, int pIdx, int gIdx) {
        if(!player_pellet[pIdx].getEaten()) window.draw(player_pellet[pIdx].pellet); 
        if(!ghost_pellet[gIdx].getEaten()) window.draw(ghost_pellet[gIdx].pellet);
    }

    void setCollision_player(int index) {   player_pellet[index].setEaten();  }
    bool getCollision_player(int index) {   return player_pellet[index].getEaten();  }
    
    int getPosX_player(int index) { return player_pellet[index].position.getX(); } 
    int getPosY_player(int index) { return player_pellet[index].position.getY(); }
    void setPosition_player(int index) { player_pellet[index].position.setX(-1);  player_pellet[index].position.setY(-1);}

    
    void setCollision_ghost(int index) {   ghost_pellet[index].setEaten();  }
    bool getCollision_ghost(int index) {   return ghost_pellet[index].getEaten();  }
    
    int getPosX_ghost(int index) { return ghost_pellet[index].position.getX(); }
    int getPosY_ghost(int index) { return ghost_pellet[index].position.getY(); }
    
    void setPosition_ghost(int index) { ghost_pellet[index].position.setX(-1);  ghost_pellet[index].position.setY(-1);}

    
};


