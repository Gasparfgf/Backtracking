#ifndef PARALLEL_LEVEL_SOLVER_H
#define PARALLEL_LEVEL_SOLVER_H

#include "../common/MazeCommon.hpp"
#include <functional>

class ParallelLevelSolver : public MazeSolver {
public:
    bool solveMaze() override;
    
private:
    bool solveLevel(int levelIdx, bool hasB, bool hasC, bool hasE, std::vector<Position>& localPath);
    
    bool solveLocalMaze(Position pos, bool hasB, bool hasC, bool hasE, std::vector<Position>& localPath, int levelIdx);
};

#endif // PARALLEL_LEVEL_SOLVER_H