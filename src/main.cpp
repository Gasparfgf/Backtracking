#include <iostream>
#include <memory>
#include "common/MazeCommon.hpp"
#include "sequential/SequentialSolver.hpp"
#include "parallel_level/ParallelLevelSolver.hpp"

using namespace std;

int showMenu() {
    int choice;
    cout << "\n=== RÉSOLUTION DE LABYRINTHE MULTI-NIVEAUX ===\n";
    cout << "1. Résolution par backtracking séquentiel\n";
    cout << "2. Résolution par backtracking parallèle par niveau\n";
    cout << "0. Quitter\n";
    cout << "Votre choix : ";
    cin >> choice;
    return choice;
}

int main() {
    vector<string> files = {"maze0.txt", "maze1-1.txt", "maze1-2.txt", "maze2.txt"};
    
    // Créer un solveur initial (sera remplacé selon le choix)
    unique_ptr<MazeSolver> solver;
    
    int choice;
    do {
        choice = showMenu();
        
        switch (choice) {
            case 1: {
                cout << "\n=== Résolution par backtracking séquentiel ===\n";
                solver = make_unique<SequentialSolver>();
                solver->readMazeFiles(files);
                
                Position start = solver->getStartPosition();
                if (start.x == -1) {
                    cerr << "Erreur ❌ : Pas de point de départ 'D' trouvé dans le labyrinthe !" << endl;
                    break;
                }
                
                bool success = solver->solveMaze();
                if (success) {
                    cout << "Solution trouvée ✅\n";
                    cout << "Chemin final :\n";
                    solver->printAllLevels();
                } else {
                    cout << "Aucune solution trouvée ❌\n";
                }
                break;
            }
            case 2: {
                cout << "\n=== Résolution par backtracking parallèle par niveau ===\n";
                solver = make_unique<ParallelLevelSolver>();
                solver->readMazeFiles(files);
                
                bool success = solver->solveMaze();
                if (success) {
                    cout << "Solution trouvée ✅\n";
                    cout << "Chemin final :\n";
                    solver->printAllLevels();
                } else {
                    cout << "Aucune solution trouvée ❌\n";
                }
                break;
            }
            case 0:
                cout << "Au revoir !\n";
                break;
            default:
                cout << "Choix invalide. Veuillez réessayer.\n";
        }
        
    } while (choice != 0);
    
    return 0;
}