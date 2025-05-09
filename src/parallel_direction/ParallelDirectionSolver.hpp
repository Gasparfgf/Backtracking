#ifndef PARALLEL_DIRECTION_SOLVER_H
#define PARALLEL_DIRECTION_SOLVER_H

#include "../common/MazeCommon.hpp"
#include <functional>

class ParallelDirectionSolver : public MazeSolver {
public:
    bool solveMaze() override;

private:
    bool solveMazeByDirectionParallel(Position start);
    bool solveDirection(Position pos, bool hasB, bool hasC, bool hasE, 
                       std::vector<Position>& threadPath, 
                       std::vector<Level>& threadLevels);
};

#endif // PARALLEL_DIRECTION_SOLVER_H