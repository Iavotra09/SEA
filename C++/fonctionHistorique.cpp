#include "header.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <termios.h>
#include <unistd.h>
#include <iomanip>
#include <chrono>

void enregistrerHistorique(const ResultatPartie& r)
{
    std::ofstream fichier("historique.dat", std::ios::app);
    if (fichier.is_open()) {
        fichier << r.mode << "," << r.motsReussis << "," << r.totalMots << "," 
                << r.fautes << "," << r.motsParMinute << "," << r.precision << "\n";
    }
}

void afficherHistorique()
{
    effacerEcran();
    std::cout << "\033[1;33m┌────────────────────────────────────────────────────────┐\033[0m\n";
    std::cout << "\033[1;33m│                  HISTORIQUE DES SCORES                 │\033[0m\n";
    std::cout << "\033[1;33m└────────────────────────────────────────────────────────┘\033[0m\n\n";
    std::cout << std::left << std::setw(14) << "MODE" << std::setw(10) << "SCORE" << std::setw(8) << "FAUTES" << std::setw(10) << "VITESSE" << "PRECISION\n";
    std::cout << "\033[0;90m──────────────────────────────────────────────────────────\033[0m\n";

    std::ifstream fichier("historique.dat");
    std::string ligne;
    while (std::getline(fichier, ligne))
    {
        size_t pos = 0;
        std::vector<std::string> valeurs;
        while ((pos = ligne.find(',')) != std::string::npos)
        {
            valeurs.push_back(ligne.substr(0, pos));
            ligne.erase(0, pos + 1);
        }
        valeurs.push_back(ligne);

        if (valeurs.size() == 6)
        {
            std::cout << std::left << std::setw(14) << valeurs[0]
                      << valeurs[1] << "/" << std::setw(7) << valeurs[2]
                      << std::setw(8) << valeurs[3]
                      << std::setw(4) << valeurs[4] << " MPM   "
                      << valeurs[5] << " %\n";
        }
    }
    std::cout << "\n\033[0;90m──────────────────────────────────────────────────────────\033[0m\n";
    std::cout << "Appuyez sur une touche pour revenir au menu...";
    while(lireTouche() == 0);
}
