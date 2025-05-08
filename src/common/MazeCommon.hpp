#ifndef MAZE_COMMON_H
#define MAZE_COMMON_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include <thread>

const int LEVEL_0 = 0;      // Étage 0
const int LEVEL_1_MAIN = 1; // Étage 1, pièce principale
const int LEVEL_1_SEC = 2;  // Étage 1, pièce secondaire
const int LEVEL_2 = 3;      // Étage 2

// Directions possibles (haut, bas, gauche, droite)
const int NUMBER_OF_DIRECTIONS = 4;
const int dx[NUMBER_OF_DIRECTIONS] = {-1, 1, 0, 0};
const int dy[NUMBER_OF_DIRECTIONS] = {0, 0, -1, 1};

struct Position {
    int x, y;
    int level;
};

struct Level {
    std::vector<std::vector<char>> maze;
    std::vector<std::vector<bool>> visited;
    int rows, cols;
};

// Classe principale pour le labyrinthe
class MazeSolver {
protected:
    std::vector<Level> levels;
    std::vector<Position> path;
    std::mutex mtx;

public:
    void readMazeFiles(const std::vector<std::string>& fileNames);
    
    void resetMaze();
    
    bool isValidMove(int level, int x, int y) const;
    
    Position findPosition(int level, char target) const;
    
    void markPath();
    
    void printMaze(int level) const;
    
    void printAllLevels() const;
    
    Position getStartPosition() const;
    
    // Fonction virtuelle car elle sera implémentée dans les classes filles
    virtual bool solveMaze() = 0;
    
    // Destructeur virtuel
    virtual ~MazeSolver() {}
};

#endif // MAZE_COMMON_H