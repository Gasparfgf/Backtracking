#ifndef MAZE_COMMON_H
#define MAZE_COMMON_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include <thread>

const int LEVEL_0 = 0;
const int LEVEL_1_MAIN = 1;
const int LEVEL_1_SEC = 2;
const int LEVEL_2 = 3;

const char WALL = '#';
const char MONSTER = 'M';
const char PATH = '*';
const char START = 'D';
const char END = 'A';
const char KEY_B = 'B';
const char KEY_C = 'C';
const char KEY_E = 'E';
const char DOOR_1 = '1';
const char DOOR_2 = '2';
const char TELEPORT = 'T';
const char EMPTY = ' ';

const std::string COLOR_RED = "\033[31m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_BLUE = "\033[34m";
const std::string COLOR_RESET = "\033[0m";

const int NUMBER_OF_DIRECTIONS = 4;
const int dx[NUMBER_OF_DIRECTIONS] = {-1, 1, 0, 0};
const int dy[NUMBER_OF_DIRECTIONS] = {0, 0, -1, 1};

struct Position {
    int x, y;
    int level;

    Position(int x = -1, int y = -1, int level = -1) : x(x), y(y), level(level) {}
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y && level == other.level;
    }
};

struct Level {
    std::vector<std::vector<char>> maze;
    std::vector<std::vector<bool>> visited;
    int rows, cols;

    void resetVisits() {
        visited.assign(rows, std::vector<bool>(cols, false));
    }
};

class MazeException : public std::exception {
private:
    std::string message;
public:
    MazeException(const std::string& msg) : message(msg) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Classe principale pour le labyrinthe
class MazeSolver {
protected:
    std::vector<Level> levels;
    std::vector<Position> path;
    std::mutex mtx;

    bool isExitReachable(bool hasB, bool hasC, bool hasE) const;

public:
    virtual ~MazeSolver() {}
    
    void readMazeFiles(const std::vector<std::string>& fileNames);
    void resetMaze();
    
    bool isValidMove(int level, int x, int y) const;
    Position findPosition(int level, char target) const;
    bool hasAllKeys(const Position& pos, bool& hasB, bool& hasC, bool& hasE) const;
    
    void markPath();
    void printMaze(int level) const;
    void printAllLevels() const;
    
    Position getStartPosition() const;
    Position getEndPosition() const;
    
    virtual bool solveMaze() = 0;
    
    bool checkMazeIntegrity() const;
};

#endif // MAZE_COMMON_H