#include "ParallelLevelSolver.hpp"
#include <iostream>
#include <thread>
#include <mutex>

bool ParallelLevelSolver::solveMaze() {
    try {
        resetMaze();
        if (!checkMazeIntegrity()) {
            std::cerr << "❌ Erreur: Vérification du labyrinthe échouée" << std::endl;
            return false;
        }
        
        std::vector<std::thread> threads(levels.size());
        std::vector<bool> results(levels.size(), false);
        std::vector<std::vector<Position>> levelPaths(levels.size());
        
        threads[LEVEL_0] = std::thread([this, &results, &levelPaths]() {
            try {
                results[LEVEL_0] = solveLevel(LEVEL_0, false, false, false, levelPaths[LEVEL_0]);
            } catch (const MazeException& e) {
                std::cerr << "❌ Erreur: Erreur dans le thread niveau 0 -> " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Erreur: Exception dans le thread niveau 0 -> " << e.what() << std::endl;
            }
        });

        threads[LEVEL_1_MAIN] = std::thread([this, &results, &levelPaths]() {
            try {
                results[LEVEL_1_MAIN] = solveLevel(LEVEL_1_MAIN, false, false, true, levelPaths[LEVEL_1_MAIN]);
            } catch (const MazeException& e) {
                std::cerr << "❌ Erreur: Erreur dans le thread niveau 1 (principal) -> " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Exception dans le thread niveau 1 (principal) -> " << e.what() << std::endl;
            }
        });

        threads[LEVEL_1_SEC] = std::thread([this, &results, &levelPaths]() {
            try {
                results[LEVEL_1_SEC] = solveLevel(LEVEL_1_SEC, false, false, true, levelPaths[LEVEL_1_SEC]);
            } catch (const MazeException& e) {
                std::cerr << "❌ Erreur: Erreur dans le thread niveau 1 (secondaire) -> " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Erreur: Exception dans le thread niveau 1 (secondaire) -> " << e.what() << std::endl;
            }
        });

        threads[LEVEL_2] = std::thread([this, &results, &levelPaths]() {
            try {
                results[LEVEL_2] = solveLevel(LEVEL_2, true, false, true, levelPaths[LEVEL_2]);
            } catch (const MazeException& e) {
                std::cerr << "❌ Erreur: Erreur dans le thread niveau 2 -> " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Exception dans le thread niveau 2 -> " << e.what() << std::endl;
            }
        });

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        // Vérifier si tous les niveaux ont été résolus
        bool allSolved = true;
        for (bool result : results) {
            if (!result) {
                allSolved = false;
                break;
            }
        }

        if (allSolved) {
            path.clear();
            for (const auto& levelPath : levelPaths) {
                path.insert(path.end(), levelPath.begin(), levelPath.end());
            }
            markPath();
            std::cout << "✅ Success: Solution parallèle (par niveau) trouvée en " << path.size() << " pas" << std::endl;
            return true;
        } else {
            std::cout << "❌ Erreur: Aucune solution parallèle (par niveau) trouvée" << std::endl;
            return false;
        }
    } catch (const MazeException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erreur: Erreur inattendue -> " << e.what() << std::endl;
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
                    throw MazeException("❌ Erreur: Point de départ introuvable dans le niveau 0");
                }
                break;
            case LEVEL_1_MAIN:
                start = findPosition(LEVEL_1_MAIN, DOOR_1);
                if (start.x == -1) {
                    throw MazeException("❌ Erreur: Porte d'entrée '1' introuvable dans le niveau 1 (principal)");
                }
                break;
            case LEVEL_1_SEC:
                start = findPosition(LEVEL_1_SEC, DOOR_1);
                if (start.x == -1) {
                    throw MazeException("❌ Erreur: Porte d'entrée '1' introuvable dans le niveau 1 (secondaire)");
                }
                break;
            case LEVEL_2:
                start = findPosition(LEVEL_2, DOOR_2);
                if (start.x == -1) {
                    throw MazeException("❌ Erreur: Porte d'entrée '2' introuvable dans le niveau 2");
                }
                break;
            default:
                throw MazeException("❌ Erreur: Index de niveau invalide: " + std::to_string(levelIdx));
        }
        
        return solveLocalMaze(start, hasB, hasC, hasE, localPath, levelIdx);
    } catch (const MazeException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erreur: Erreur inattendue dans solveLevel -> " << e.what() << std::endl;
        return false;
    }
}

bool ParallelLevelSolver::solveLocalMaze(Position pos, bool hasB, bool hasC, bool hasE, 
                                      std::vector<Position>& localPath, int levelIdx) {
    int x = pos.x;
    int y = pos.y;
    int level = pos.level;
    
    if (!isValidMove(level, x, y)) {
        return false;
    }
    
    // Marquer comme visité et ajouter au chemin
    std::lock_guard<std::mutex> lock(mtx); // Protéger l'accès concurrent
    levels[level].visited[x][y] = true;
    localPath.push_back(pos);
    
    char current = levels[level].maze[x][y];
    
    if (current == KEY_B) hasB = true;
    if (current == KEY_C) hasC = true;
    if (current == KEY_E) hasE = true;
    
    if (levelIdx == LEVEL_2 && current == END) {
        if (isExitReachable(hasB, hasC, hasE)) {
            return true;
        } else {
            localPath.pop_back();
            levels[level].visited[x][y] = false;
            return false;
        }
    }
    
    // Conditions de succès pour les transitions entre niveaux
    if ((levelIdx == LEVEL_0 && current == DOOR_1) ||
        ((levelIdx == LEVEL_1_MAIN || levelIdx == LEVEL_1_SEC) && current == DOOR_2)) {
        return true;
    }
    
    if (levelIdx == LEVEL_1_MAIN && current == TELEPORT) {
        return true;
    }
    
    for (int direction = 0; direction < NUMBER_OF_DIRECTIONS; direction++) {
        Position nextPos = {x + dx[direction], y + dy[direction], level};
        if (solveLocalMaze(nextPos, hasB, hasC, hasE, localPath, levelIdx)) {
            return true;
        }
    }
    
    localPath.pop_back();
    levels[level].visited[x][y] = false;
    return false;
}