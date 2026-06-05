#include "header.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <termios.h>
#include <unistd.h>
#include <iomanip>
#include <chrono>

static struct termios terminalOriginal;

void configurerTerminal()
{
    tcgetattr(STDIN_FILENO, &terminalOriginal);
    struct termios brute = terminalOriginal;
    brute.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &brute);
}

void restaurerTerminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &terminalOriginal);
}

void effacerEcran()
{
    std::cout << "\033[2J\033[1;1H";
}

int lireTouche()
{
    struct timeval tv = {0, 30000}; //attend 30ms max pour une saisie fluide
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0)
    {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == 27)
            {
                char m1, m2;
                if (read(STDIN_FILENO, &m1, 1) > 0 && read(STDIN_FILENO, &m2, 1) > 0) {
                    if (m1 == 91 && m2 == 65) return 11;   //flèche haut
                    if (m1 == 91 && m2 == 66) return 12;   //flèche basas
                }
            }
            return c;
        }
    }
    return 0;
}

void afficherMenuInteractif(const std::string& titre, const std::vector<ModeDeJeu>& options, int& selection)
{
    effacerEcran();
    std::cout << "\033[1;36m┌────────────────────────────────────────────────────────┐\033[0m\n";
    std::cout << "\033[1;36m│\033[1;33m               " << std::left << std::setw(41) << titre << "\033[1;36m│\033[0m\n";
    std::cout << "\033[1;36m├────────────────────────────────────────────────────────┤\033[0m\n";
    std::cout << "\033[1;36m│\033[0m  Navigation  : \033[1;35m↑\033[0m / \033[1;35m↓\033[0m  |   Validation : \033[1;32m[ENTREE]\033[0m        \033[1;36m│\033[0m\n";
    std::cout << "\033[1;36m└────────────────────────────────────────────────────────┘\033[0m\n\n";

    for (size_t i = 0; i < options.size(); ++i)
    {
        if (static_cast<int>(i) == selection)
        {
            std::cout << "  \033[1;42m\033[1;37m ➔  " << std::left << std::setw(14) << options[i].nom << "\033[0m \033[1;32m" << options[i].description << "\033[0m\n\n";
        } else {
            std::cout << "      \033[1;37m" << std::left << std::setw(14) << options[i].nom << "\033[0m \033[0;90m" << options[i].description << "\033[0m\n\n";
        }
    }
}

std::vector<std::string> chargerDictionnaire(const std::string& nomFichier)
{
    std::vector<std::string> mots;
    std::ifstream fichier(nomFichier);
    std::string mot;
    while (fichier >> mot) mots.push_back(mot);
    if (mots.empty()) mots = {"clavier", "terminal", "vitesse", "chute"};
    return mots;
}

