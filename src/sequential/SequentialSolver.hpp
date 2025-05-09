#ifndef SEQUENTIAL_SOLVER_H
#define SEQUENTIAL_SOLVER_H

#include "../common/MazeCommon.hpp"

/**
 * @class SequentialSolver
 * @brief Classe qui implémente la résolution séquentielle du labyrinthe
 * 
 * Cette classe utilise un algorithme de backtracking séquentiel
 * pour trouver un chemin du point de départ au point d'arrivée.
 */
class SequentialSolver : public MazeSolver {
public:
    /**
     * @brief Résout le labyrinthe de manière séquentielle
     * @return true si une solution a été trouvée, false sinon
     */
    bool solveMaze() override;

protected:
    /**
     * @brief Méthode récursive de backtracking pour la résolution séquentielle
     * @param pos Position actuelle
     * @param hasB Indique si la clé B a été trouvée
     * @param hasC Indique si la clé C a été trouvée
     * @param hasE Indique si la clé E a été trouvée
     * @return true si une solution a été trouvée à partir de cette position
     */
    bool solveMazeSequentially(Position pos, bool hasB, bool hasC, bool hasE);
};

#endif // SEQUENTIAL_SOLVER_H