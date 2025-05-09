#include "SequentialSolver.hpp"
#include <iostream>

bool SequentialSolver::solveMaze() {
    try {
        resetMaze();
        
        if (!checkMazeIntegrity()) {
            std::cerr << "❌ Erreur: Vérification du labyrinthe échouée" << std::endl;
            return false;
        }
        
        Position start = getStartPosition();
        bool result = solveMazeSequentially(start, false, false, false);
        
        if (result) {
            markPath();
            std::cout << "✅ Solution séquentielle trouvée en " << path.size() << " pas" << std::endl;
        } else {
            std::cout << "❌ Aucune solution séquentielle trouvée" << std::endl;
        }
        
        return result;
    } catch (const MazeException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erreur: Erreur inattendue -> " << e.what() << std::endl;
        return false;
    }
}

bool SequentialSolver::solveMazeSequentially(Position pos, bool hasB, bool hasC, bool hasE) {
    int x = pos.x;
    int y = pos.y;
    int level = pos.level;
    
    if (!isValidMove(level, x, y)) return false;
    
    levels[level].visited[x][y] = true;
    path.push_back(pos);
    
    char current = levels[level].maze[x][y];
    if (current == KEY_B) hasB = true;
    if (current == KEY_C) hasC = true;
    if (current == KEY_E) hasE = true;
    
    if (current == END) {
        if (isExitReachable(hasB, hasC, hasE)) {
            return true;
        } else {
            path.pop_back();
            levels[level].visited[x][y] = false;
            return false;
        }
    }
    
    if (current == DOOR_1) {
        if (level == LEVEL_0) {
            Position doorPos = findPosition(LEVEL_1_MAIN, DOOR_1);
            if (doorPos.x != -1) {
                if (solveMazeSequentially(doorPos, hasB, hasC, hasE)) {
                    return true;
                }
            }
        }
    }
    else if (current == DOOR_2) {
        if (level == LEVEL_1_MAIN || level == LEVEL_1_SEC) {
            Position doorPos = findPosition(LEVEL_2, DOOR_2);
            if (doorPos.x != -1) {
                if (solveMazeSequentially(doorPos, hasB, hasC, hasE)) {
                    return true;
                }
            }
        }
    }
    else if (current == TELEPORT) {
        if (level == LEVEL_1_MAIN) {
            Position doorPos = findPosition(LEVEL_1_SEC, DOOR_1);
            if (doorPos.x != -1) {
                if (solveMazeSequentially(doorPos, hasB, hasC, hasE)) {
                    return true;
                }
            }
        }
    }
    
    for (int direction = 0; direction < NUMBER_OF_DIRECTIONS; direction++) {
        Position nextPos = {x + dx[direction], y + dy[direction], level};
        if (solveMazeSequentially(nextPos, hasB, hasC, hasE)) {
            return true;
        }
    }
    
    path.pop_back();
    levels[level].visited[x][y] = false;
    return false;
}