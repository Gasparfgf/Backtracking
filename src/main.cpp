#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "common/MazeCommon.hpp"
#include "sequential/SequentialSolver.hpp"
#include "parallel_level/ParallelLevelSolver.hpp"
#include "parallel_direction/ParallelDirectionSolver.hpp"

using namespace std;

int showMenu() {
    int choice;
    cout << "\n=== RÉSOLUTION DE LABYRINTHE MULTI-NIVEAUX ===\n\n";
    cout << "1. Résolution par backtracking séquentiel\n";
    cout << "2. Résolution par backtracking parallèle par niveau\n";
    cout << "3. Résolution par backtracking parallèle par directions\n";
    cout << "0. Quitter\n";
    cout << "Votre choix : ";
    cin >> choice;
    return choice;
}

int main(int argc, char* argv[]) {
    vector<string> files;
    
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            files.push_back(argv[i]);
        }
    } else {
        files = {"labyrinthe/maze0.txt", "labyrinthe/maze1-1.txt", "labyrinthe/maze1-2.txt", "labyrinthe/maze2.txt"};
    }
    
    unique_ptr<MazeSolver> solver;
    
    int choice;
    do {
        choice = showMenu();
        switch (choice) {
            case 1: {
                cout << "\n=== Résolution par backtracking séquentiel ===\n";
                solver = unique_ptr<MazeSolver>(new SequentialSolver());
                
                try {
                    solver->readMazeFiles(files);
                    Position start = solver->getStartPosition();
                    
                    cout << "Lecture des fichiers réussie. Résolution en cours..." << endl;
                    
                    bool success = solver->solveMaze();
                    if (success) {
                        cout << "\nSolution trouvée\n";
                        cout << "Chemin final :\n";
                        solver->printAllLevels();
                    } else {
                        cout << "\nAucune solution trouvée\n";
                    }
                } catch (const MazeException& e) {
                    cerr << e.what() << endl;
                } catch (const std::exception& e) {
                    cerr << "Erreur inattendue : " << e.what() << endl;
                }
                break;
            }
            case 2: {
                cout << "\n=== Résolution par backtracking parallèle par niveau ===\n";
                solver = unique_ptr<MazeSolver>(new ParallelLevelSolver());
                
                try {
                    solver->readMazeFiles(files);
                    cout << "Lecture des fichiers réussie. Résolution en cours..." << endl;
                    
                    bool success = solver->solveMaze();
                    if (success) {
                        cout << "\nSolution trouvée\n";
                        cout << "Chemin final :\n";
                        solver->printAllLevels();
                    } else {
                        cout << "\nAucune solution trouvée\n";
                    }
                } catch (const MazeException& e) {
                    cerr << e.what() << endl;
                } catch (const std::exception& e) {
                    cerr << "Erreur: Erreur inattendue -> " << e.what() << endl;
                }
                break;
            }
            case 3: {
                cout << "\n=== Résolution par backtracking parallèle par directions ===\n";
                solver = unique_ptr<MazeSolver>(new ParallelDirectionSolver());
                
                try {
                    solver->readMazeFiles(files);
                    cout << "Lecture des fichiers réussie. Résolution en cours..." << endl;
                    
                    bool success = solver->solveMaze();
                    if (success) {
                        cout << "\nSolution trouvée\n";
                        cout << "Chemin final :\n";
                        solver->printAllLevels();
                    } else {
                        cout << "\nAucune solution trouvée\n";
                    }
                } catch (const MazeException& e) {
                    cerr << e.what() << endl;
                } catch (const std::exception& e) {
                    cerr << "Erreur: Erreur inattendue -> " << e.what() << endl;
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