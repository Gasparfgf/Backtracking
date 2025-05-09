#include "ParallelDirectionSolver.hpp"
#include <iostream>

bool ParallelDirectionSolver::solveMaze() {
    try {
        resetMaze();
        
        if (!checkMazeIntegrity()) {
            std::cerr << "Erreur: Vérification du labyrinthe échouée" << std::endl;
            return false;
        }
        
        Position start = getStartPosition();
        if (start.x == -1) {
            std::cerr << "Erreur: Pas de point de départ 'D' trouvé dans le labyrinthe !" << std::endl;
            return false;
        }
        
        bool result = solveMazeByDirectionParallel(start);
        if (result) {
            markPath();
            std::cout << "Solution parallèle (par direction) trouvée en " << path.size() << " pas" << std::endl;
        } else {
            std::cout << "Erreur: Aucune solution parallèle (par direction) trouvée" << std::endl;
        }
        return result;
    } catch (const MazeException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Erreur: Erreur inattendue -> " << e.what() << std::endl;
        return false;
    }
}

bool ParallelDirectionSolver::solveMazeByDirectionParallel(Position start) {
    std::vector<std::thread> threads(NUMBER_OF_DIRECTIONS);
    std::vector<bool> results(NUMBER_OF_DIRECTIONS, false);
    std::vector<std::vector<Position>> directionPaths(NUMBER_OF_DIRECTIONS);
    
    std::mutex pathMutex;
    
    for (int dir = 0; dir < NUMBER_OF_DIRECTIONS; dir++) {
        threads[dir] = std::thread([this, dir, start, &results, &directionPaths, &pathMutex]() {
            try {
                Position nextPos = {start.x + dx[dir], start.y + dy[dir], start.level};
                
                std::vector<Level> threadLevels = levels;
                std::vector<Position> threadPath;
                threadPath.push_back(start);
                
                if (nextPos.x >= 0 && nextPos.x < threadLevels[nextPos.level].rows && 
                    nextPos.y >= 0 && nextPos.y < threadLevels[nextPos.level].cols && 
                    threadLevels[nextPos.level].maze[nextPos.x][nextPos.y] != WALL && 
                    threadLevels[nextPos.level].maze[nextPos.x][nextPos.y] != MONSTER && 
                    !threadLevels[nextPos.level].visited[nextPos.x][nextPos.y]) {
                    
                    results[dir] = solveDirection(nextPos, false, false, false, 
                                                threadPath, threadLevels);
                }
                
                if (results[dir]) {
                    std::lock_guard<std::mutex> lock(pathMutex);
                    if (path.empty() || threadPath.size() < path.size()) {
                        path = threadPath;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Erreur: Erreur dans le thread direction " << dir << " : " << e.what() << std::endl;
            }
        });
    }
    
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    bool found = false;
    for (bool result : results) {
        if (result) {
            found = true;
            break;
        }
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
        threadLevels[level].maze[x][y] == WALL || 
        threadLevels[level].maze[x][y] == MONSTER || 
        threadLevels[level].visited[x][y]) {
        return false;
    }
    
    threadLevels[level].visited[x][y] = true;
    threadPath.push_back(pos);
    
    char current = threadLevels[level].maze[x][y];
    
    if (current == KEY_B) hasB = true;
    if (current == KEY_C) hasC = true;
    if (current == KEY_E) hasE = true;
    
    if (current == END) {
        if (hasB && hasC && hasE) {
            return true;
        } else {
            threadPath.pop_back();
            threadLevels[level].visited[x][y] = false;
            return false;
        }
    }
    
    if (current == DOOR_1) {
        if (level == LEVEL_0) {
            Position doorPos = {-1, -1, LEVEL_1_MAIN};
            for (int i = 0; i < threadLevels[LEVEL_1_MAIN].rows; ++i) {
                for (int j = 0; j < threadLevels[LEVEL_1_MAIN].cols; ++j) {
                    if (threadLevels[LEVEL_1_MAIN].maze[i][j] == DOOR_1) {
                        doorPos.x = i;
                        doorPos.y = j;
                        break;
                    }
                }
                if (doorPos.x != -1) break;
            }
            
            if (doorPos.x != -1) {
                if (solveDirection(doorPos, hasB, hasC, hasE, threadPath, threadLevels)) {
                    return true;
                }
            }
        }
    }
    else if (current == DOOR_2) {
        if (level == LEVEL_1_MAIN || level == LEVEL_1_SEC) {
            Position doorPos = {-1, -1, LEVEL_2};
            for (int i = 0; i < threadLevels[LEVEL_2].rows; ++i) {
                for (int j = 0; j < threadLevels[LEVEL_2].cols; ++j) {
                    if (threadLevels[LEVEL_2].maze[i][j] == DOOR_2) {
                        doorPos.x = i;
                        doorPos.y = j;
                        break;
                    }
                }
                if (doorPos.x != -1) break;
            }
            
            if (doorPos.x != -1) {
                if (solveDirection(doorPos, hasB, hasC, hasE, threadPath, threadLevels)) {
                    return true;
                }
            }
        }
    }
    else if (current == TELEPORT) {
        if (level == LEVEL_1_MAIN) {
            Position doorPos = {-1, -1, LEVEL_1_SEC};
            for (int i = 0; i < threadLevels[LEVEL_1_SEC].rows; ++i) {
                for (int j = 0; j < threadLevels[LEVEL_1_SEC].cols; ++j) {
                    if (threadLevels[LEVEL_1_SEC].maze[i][j] == DOOR_1) {
                        doorPos.x = i;
                        doorPos.y = j;
                        break;
                    }
                }
                if (doorPos.x != -1) break;
            }
            
            if (doorPos.x != -1) {
                if (solveDirection(doorPos, hasB, hasC, hasE, threadPath, threadLevels)) {
                    return true;
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