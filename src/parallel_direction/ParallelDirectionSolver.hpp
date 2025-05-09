#ifndef PARALLEL_DIRECTION_SOLVER_H
#define PARALLEL_DIRECTION_SOLVER_H

#include "../common/MazeCommon.hpp"
#include <thread>
#include <mutex>
#include <vector>

/**
 * @class ParallelDirectionSolver
 * @brief Classe qui implémente la résolution parallèle par direction du labyrinthe
 *
 * Cette classe utilise un algorithme de backtracking parallèle par direction
 * où chaque direction est explorée en parallèle à partir du point de départ.
 */
class ParallelDirectionSolver : public MazeSolver {
public:
    /**
     * @brief Résout le labyrinthe en parallèle par direction
     * @return true si une solution a été trouvée, false sinon
     */
    bool solveMaze() override;

protected:
    /**
     * @brief Exécute la résolution parallèle par direction à partir du point de départ
     * @param start Position de départ
     * @return true si une solution a été trouvée, false sinon
     */
    bool solveMazeByDirectionParallel(Position start);
    
    /**
     * @brief Résout le labyrinthe dans une direction spécifique
     * @param pos Position actuelle
     * @param hasB Indique si la clé B a été trouvée
     * @param hasC Indique si la clé C a été trouvée
     * @param hasE Indique si la clé E a été trouvée
     * @param threadPath Vecteur pour stocker le chemin trouvé par ce thread
     * @param threadLevels Copie des niveaux du labyrinthe pour ce thread
     * @return true si une solution a été trouvée à partir de cette position
     */
    bool solveDirection(Position pos, bool hasB, bool hasC, bool hasE, 
                       std::vector<Position>& threadPath, 
                       std::vector<Level>& threadLevels);
};

#endif // PARALLEL_DIRECTION_SOLVER_H