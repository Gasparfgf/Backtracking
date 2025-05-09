#include "ParallelDirectionSolver.hpp"
#include <thread>
#include <mutex>
#include <iostream>

bool ParallelDirectionSolver::solveMaze() {
    resetMaze();
    Position start = getStartPosition();
    if (start.x == -1) {
        std::cerr << "Erreur ❌ : Pas de point de départ 'D' trouvé dans le labyrinthe !" << std::endl;
        return false;
    }
    
    bool result = solveMazeByDirectionParallel(start);
    if (result) {
        markPath();
    }
    return result;
}

bool ParallelDirectionSolver::solveMazeByDirectionParallel(Position start) {
    std::vector<std::thread> threads(NUMBER_OF_DIRECTIONS);
    std::vector<bool> results(NUMBER_OF_DIRECTIONS, false);
    std::vector<std::vector<Position>> directionPaths(NUMBER_OF_DIRECTIONS);
    
    std::mutex pathMutex;
    
    for (int dir = 0; dir < NUMBER_OF_DIRECTIONS; dir++) {
        threads[dir] = std::thread([this, dir, start, &results, &directionPaths, &pathMutex]() {
            Position nextPos = {start.x + dx[dir], start.y + dy[dir], start.level};
            
            std::vector<Level> threadLevels = levels;
            std::vector<Position> threadPath;
            
            // Résoudre à partir de cette direction
            if (nextPos.x >= 0 && nextPos.x < threadLevels[nextPos.level].rows && 
                nextPos.y >= 0 && nextPos.y < threadLevels[nextPos.level].cols && 
                threadLevels[nextPos.level].maze[nextPos.x][nextPos.y] != '#' && 
                threadLevels[nextPos.level].maze[nextPos.x][nextPos.y] != 'M' && 
                !threadLevels[nextPos.level].visited[nextPos.x][nextPos.y]) {
                    
                results[dir] = solveDirection(nextPos, false, false, false, 
                                             directionPaths[dir], threadLevels);
            }
            
            // Si une solution a été trouvée, la stocker de manière thread-safe
            if (results[dir]) {
                std::lock_guard<std::mutex> lock(pathMutex);
                if (directionPaths[dir].size() < path.size() || path.empty()) {
                    path = directionPaths[dir];
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    bool found = false;
    for (bool result : results) {
        if (result) {
            found = true;
            break;
        }
    }
    
    if (found) {
        path.insert(path.begin(), start);
    }
    
    return found;
}

bool ParallelDirectionSolver::solveDirection(Position pos, bool hasB, bool hasC, bool hasE, 
                                          std::vector<Position>& threadPath, 
                                          std::vector<Level>& threadLevels) {
    int x = pos.x;
    int y = pos.y;
    int level = pos.level;
    
    if (x < 0 || x >= threadLevels[level].rows || 
        y < 0 || y >= threadLevels[level].cols || 
        threadLevels[level].maze[x][y] == '#' || 
        threadLevels[level].maze[x][y] == 'M' || 
        threadLevels[level].visited[x][y]) {
        return false;
    }
    
    threadLevels[level].visited[x][y] = true;
    
    threadPath.push_back(pos);
    
    char current = threadLevels[level].maze[x][y];
    
    if (current == 'B') hasB = true;
    if (current == 'C') hasC = true;
    if (current == 'E') hasE = true;
    
    if (current == 'A') {
        if (hasB && hasC && hasE) {
            return true;
        } else {
            threadPath.pop_back();
            threadLevels[level].visited[x][y] = false;
            return false;
        }
    }
    
    if (current == '1') {
        if (level == LEVEL_0) {
            for (int i = 0; i < threadLevels[LEVEL_1_MAIN].rows; ++i) {
                for (int j = 0; j < threadLevels[LEVEL_1_MAIN].cols; ++j) {
                    if (threadLevels[LEVEL_1_MAIN].maze[i][j] == '1') {
                        Position doorPos = {i, j, LEVEL_1_MAIN};
                        if (solveDirection(doorPos, hasB, hasC, hasE, threadPath, threadLevels)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    else if (current == '2') {
        if (level == LEVEL_1_MAIN || level == LEVEL_1_SEC) {
            for (int i = 0; i < threadLevels[LEVEL_2].rows; ++i) {
                for (int j = 0; j < threadLevels[LEVEL_2].cols; ++j) {
                    if (threadLevels[LEVEL_2].maze[i][j] == '2') {
                        Position doorPos = {i, j, LEVEL_2};
                        if (solveDirection(doorPos, hasB, hasC, hasE, threadPath, threadLevels)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    else if (current == 'T') {
        if (level == LEVEL_1_MAIN) {
            for (int i = 0; i < threadLevels[LEVEL_1_SEC].rows; ++i) {
                for (int j = 0; j < threadLevels[LEVEL_1_SEC].cols; ++j) {
                    if (threadLevels[LEVEL_1_SEC].maze[i][j] == '1') {
                        Position doorPos = {i, j, LEVEL_1_SEC};
                        if (solveDirection(doorPos, hasB, hasC, hasE, threadPath, threadLevels)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    for (int direction = 0; direction < NUMBER_OF_DIRECTIONS; direction++) {
        Position nextPos = {x + dx[direction], y + dy[direction], level};
        if (solveDirection(nextPos, hasB, hasC, hasE, threadPath, threadLevels)) {
            return true;
        }
    }
    
    threadPath.pop_back();
    threadLevels[level].visited[x][y] = false;
    return false;
}