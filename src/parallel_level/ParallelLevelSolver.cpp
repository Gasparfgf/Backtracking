#include "ParallelLevelSolver.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>


bool ParallelLevelSolver::solveMaze() {
    try {
        resetMaze();

        if (!checkMazeIntegrity()) {
            std::cerr << "Erreur: Vérification du labyrinthe échouée" << std::endl;
            return false;
        }
        
        std::vector<bool> results(levels.size(), false);
        std::vector<std::vector<Position>> levelPaths(levels.size());
        
        std::vector<std::thread> threads;
        
        threads.emplace_back([this, &results, &levelPaths]() {
            try {
                results[LEVEL_0] = solveLevel(LEVEL_0, false, false, false, levelPaths[LEVEL_0]);
            } catch (const std::exception& e) {
                std::cerr << "Erreur dans le thread niveau 0: " << e.what() << std::endl;
            }
        });

        threads.emplace_back([this, &results, &levelPaths]() {
            try {
                results[LEVEL_1_MAIN] = solveLevel(LEVEL_1_MAIN, false, false, false, levelPaths[LEVEL_1_MAIN]);
            } catch (const std::exception& e) {
                std::cerr << "Erreur dans le thread niveau 1 (principal): " << e.what() << std::endl;
            }
        });

        threads.emplace_back([this, &results, &levelPaths]() {
            try {
                results[LEVEL_1_SEC] = solveLevel(LEVEL_1_SEC, false, false, false, levelPaths[LEVEL_1_SEC]);
            } catch (const std::exception& e) {
                std::cerr << "Erreur dans le thread niveau 1 (secondaire): " << e.what() << std::endl;
            }
        });

        threads.emplace_back([this, &results, &levelPaths]() {
            try {
                results[LEVEL_2] = solveLevel(LEVEL_2, true, true, true, levelPaths[LEVEL_2]);
            } catch (const std::exception& e) {
                std::cerr << "Erreur dans le thread niveau 2: " << e.what() << std::endl;
            }
        });

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        bool allSolved = true;
        for (size_t i = 0; i < results.size(); i++) {
            if (!results[i]) {
                std::cerr << "Erreur: Niveau " << i << " non résolu" << std::endl;
                allSolved = false;
            }
        }

        if (allSolved) {
            path.clear();
            for (size_t i = 0; i < levelPaths.size(); i++) {
                if (!levelPaths[i].empty()) {
                    for (const auto& pos : levelPaths[i]) {
                        if (pos.level >= 0 && pos.level < static_cast<int>(levels.size()) &&
                            pos.x >= 0 && pos.x < levels[pos.level].rows &&
                            pos.y >= 0 && pos.y < levels[pos.level].cols) {
                            path.push_back(pos);
                        }
                    }
                }
            }
            
            // protection contre segfault
            try {
                markPath();
                std::cout << "Success: Solution parallèle (par niveau) trouvée en " << path.size() << " pas" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Avertissement: Erreur lors du marquage du chemin: " << e.what() << std::endl;
                std::cout << "Solution parallèle trouvée en " << path.size() << " pas, mais erreur lors de l'affichage" << std::endl;
            }
            return true;
        } else {
            std::cout << "Erreur: Aucune solution parallèle (par niveau) trouvée" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur dans solveMaze: " << e.what() << std::endl;
        return false;
    }
}

bool ParallelLevelSolver::solveLevel(int levelIdx, bool hasB, bool hasC, bool hasE, std::vector<Position>& localPath) {
    Position start;
    try {
        switch (levelIdx) {
            case LEVEL_0:
                start = findPosition(LEVEL_0, START);
                if (start.x == -1) {
                    throw MazeException("Erreur: Point de départ introuvable dans le niveau 0");
                }
                break;
            case LEVEL_1_MAIN:
                start = findPosition(LEVEL_1_MAIN, DOOR_1);
                if (start.x == -1) {
                    throw MazeException("Erreur: Porte d'entrée '1' introuvable dans le niveau 1 (principal)");
                }
                break;
            case LEVEL_1_SEC:
                start = findPosition(LEVEL_1_SEC, TELEPORT);
                if (start.x == -1) {
                    start = findPosition(LEVEL_1_SEC, DOOR_1);
                    if (start.x == -1) {
                        throw MazeException("Erreur: Ni téléporteur ni porte d'entrée trouvés dans le niveau 1 (secondaire)");
                    }
                }
                break;
            case LEVEL_2:
                start = findPosition(LEVEL_2, DOOR_2);
                if (start.x == -1) {
                    throw MazeException("Erreur: Porte d'entrée '2' introuvable dans le niveau 2");
                }
                break;
            default:
                throw MazeException("Erreur: Index de niveau invalide: " + std::to_string(levelIdx));
        }
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            levels[levelIdx].resetVisits();
        }
        
        localPath.clear();
        bool success = solveLocalMaze(start, hasB, hasC, hasE, localPath, levelIdx);
        
        if (success) {
            std::cout << "Niveau " << levelIdx << " résolu avec " << localPath.size() << " pas" << std::endl;
            return true;
        } else {
            std::cerr << "Échec de résolution du niveau " << levelIdx << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur dans solveLevel: " << e.what() << std::endl;
        return false;
    }
}

bool ParallelLevelSolver::solveLocalMaze(Position pos, bool hasB, bool hasC, bool hasE, 
                                      std::vector<Position>& localPath, int levelIdx) {
    if (pos.level < 0 || pos.level >= static_cast<int>(levels.size()) || 
        pos.x < 0 || pos.x >= levels[pos.level].rows || 
        pos.y < 0 || pos.y >= levels[pos.level].cols ||
        levels[pos.level].maze[pos.x][pos.y] == WALL || 
        levels[pos.level].maze[pos.x][pos.y] == MONSTER) {
        return false;
    }
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (levels[pos.level].visited[pos.x][pos.y]) {
            return false;
        }
        
        levels[pos.level].visited[pos.x][pos.y] = true;
    }
    
    localPath.push_back(pos);
    
    char current = levels[pos.level].maze[pos.x][pos.y];
    
    if (current == KEY_B) hasB = true;
    if (current == KEY_C) hasC = true;
    if (current == KEY_E) hasE = true;
    
    if (levelIdx == LEVEL_0 && current == DOOR_1) {
        return true;
    }
    
    if (levelIdx == LEVEL_1_MAIN) {
        if (current == DOOR_2 || current == TELEPORT) {
            return true;
        }
    }
    
    if (levelIdx == LEVEL_1_SEC) {
        if (current == DOOR_2) {
            return true;
        }
    }
    
    if (levelIdx == LEVEL_2 && current == END) {
        if (isExitReachable(hasB, hasC, hasE)) {
            return true;
        }
    }
    
    for (int direction = 0; direction < NUMBER_OF_DIRECTIONS; direction++) {
        Position nextPos = {pos.x + dx[direction], pos.y + dy[direction], pos.level};
        if (solveLocalMaze(nextPos, hasB, hasC, hasE, localPath, levelIdx)) {
            return true;
        }
    }
    
    localPath.pop_back();
    {
        std::lock_guard<std::mutex> lock(mtx);
        levels[pos.level].visited[pos.x][pos.y] = false;
    }
    
    return false;
}