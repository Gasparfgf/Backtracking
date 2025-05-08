#include "SequentialSolver.hpp"

bool SequentialSolver::solveMaze() {
    resetMaze();
    Position start = getStartPosition();
    if (start.x == -1) {
        std::cerr << "Erreur ❌ : Pas de point de départ 'D' trouvé dans le labyrinthe !" << std::endl;
        return false;
    }
    
    bool result = solveMazeSequentially(start, false, false, false);
    if (result) {
        markPath();
    }
    return result;
}

bool SequentialSolver::solveMazeSequentially(Position pos, bool hasB, bool hasC, bool hasE) {
    int x = pos.x;
    int y = pos.y;
    int level = pos.level;
    
    if (!isValidMove(level, x, y)) return false;
    
    levels[level].visited[x][y] = true;
    
    path.push_back(pos);
    
    char current = levels[level].maze[x][y];
    
    if (current == 'B') hasB = true;
    if (current == 'C') hasC = true;
    if (current == 'E') hasE = true;
    
    if (current == 'A') {
        if (hasB && hasC && hasE) {
            return true;
        } else {
            path.pop_back();
            levels[level].visited[x][y] = false;
            return false;
        }
    }
    
    // Gérer les portes et téléporteurs
    if (current == '1') {
        if (level == LEVEL_0) {
            Position doorPos = findPosition(LEVEL_1_MAIN, '1');
            if (doorPos.x != -1) {
                if (solveMazeSequentially(doorPos, hasB, hasC, hasE)) {
                    return true;
                }
            }
        }
    }
    else if (current == '2') {
        if (level == LEVEL_1_MAIN || level == LEVEL_1_SEC) {
            Position doorPos = findPosition(LEVEL_2, '2');
            if (doorPos.x != -1) {
                if (solveMazeSequentially(doorPos, hasB, hasC, hasE)) {
                    return true;
                }
            }
        }
    }
    else if (current == 'T') {
        if (level == LEVEL_1_MAIN) {
            Position doorPos = findPosition(LEVEL_1_SEC, '1');
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