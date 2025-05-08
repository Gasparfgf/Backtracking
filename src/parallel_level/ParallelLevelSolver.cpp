#include "ParallelLevelSolver.hpp"

bool ParallelLevelSolver::solveMaze() {
    resetMaze();
    
    std::vector<std::thread> threads(levels.size());
    std::vector<bool> results(levels.size(), false);
    std::vector<std::vector<Position>> levelPaths(levels.size());
    
    threads[LEVEL_0] = std::thread([this, &results, &levelPaths]() {
        results[LEVEL_0] = solveLevel(LEVEL_0, false, false, false, levelPaths[LEVEL_0]);
    });
    
    threads[LEVEL_1_MAIN] = std::thread([this, &results, &levelPaths]() {
        results[LEVEL_1_MAIN] = solveLevel(LEVEL_1_MAIN, false, false, true, levelPaths[LEVEL_1_MAIN]);
    });
    
    threads[LEVEL_1_SEC] = std::thread([this, &results, &levelPaths]() {
        results[LEVEL_1_SEC] = solveLevel(LEVEL_1_SEC, false, false, true, levelPaths[LEVEL_1_SEC]);
    });
    
    threads[LEVEL_2] = std::thread([this, &results, &levelPaths]() {
        results[LEVEL_2] = solveLevel(LEVEL_2, true, false, true, levelPaths[LEVEL_2]);
    });
    
    for (auto& t : threads) {
        t.join();
    }
    
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
        return true;
    }
    
    return false;
}

bool ParallelLevelSolver::solveLevel(int levelIdx, bool hasB, bool hasC, bool hasE, std::vector<Position>& localPath) {
    Position start;
    switch (levelIdx) {
        case LEVEL_0:
            start = findPosition(LEVEL_0, 'D');
            break;
        case LEVEL_1_MAIN:
            start = findPosition(LEVEL_1_MAIN, '1');
            break;
        case LEVEL_1_SEC:
            start = findPosition(LEVEL_1_SEC, '1');
            break;
        case LEVEL_2:
            start = findPosition(LEVEL_2, '2');
            break;
    }
    
    if (start.x != -1) {
        return solveLocalMaze(start, hasB, hasC, hasE, localPath, levelIdx);
    }
    
    return false;
}

bool ParallelLevelSolver::solveLocalMaze(Position pos, bool hasB, bool hasC, bool hasE, std::vector<Position>& localPath, int levelIdx) {
    int x = pos.x;
    int y = pos.y;
    int level = pos.level;
    
    if (!isValidMove(level, x, y)) return false;
    
    levels[level].visited[x][y] = true;
    
    localPath.push_back(pos);
    
    char current = levels[level].maze[x][y];
    
    if (current == 'B') hasB = true;
    if (current == 'C') hasC = true;
    if (current == 'E') hasE = true;
    
    if (levelIdx == LEVEL_2 && current == 'A') {
        if (hasB && hasC && hasE) {
            return true;
        }
    }
    
    if ((levelIdx == LEVEL_0 && current == '1') || 
        ((levelIdx == LEVEL_1_MAIN || levelIdx == LEVEL_1_SEC) && current == '2')) {
        return true;
    }
    
    if (levelIdx == LEVEL_1_MAIN && current == 'T') {
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