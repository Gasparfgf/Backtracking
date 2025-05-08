#ifndef SEQUENTIAL_SOLVER_H
#define SEQUENTIAL_SOLVER_H

#include "../common/MazeCommon.hpp"

class SequentialSolver : public MazeSolver {
public:
    bool solveMaze() override;
    
private:
    bool solveMazeSequentially(Position pos, bool hasB, bool hasC, bool hasE);
};

#endif // SEQUENTIAL_SOLVER_H