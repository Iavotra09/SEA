#include "header.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <termios.h>
#include <unistd.h>
#include <iomanip>
#include <chrono>

void afficherResultats(const ResultatPartie& r)
{
    effacerEcran();
    std::cout << "\n\033[1;35m╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   SCORE DE LA PARTIE                   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\033[0m\n\n";
    
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "  ➔  Mots valides : \033[1;32m" << r.motsReussis << " / " << r.totalMots << "\033[0m\n";
    std::cout << "  ➔  Erreurs      : \033[1;31m" << r.fautes << "\033[0m\n";
    std::cout << "  ➔  Precision     : \033[1;33m" << r.precision << " %\033[0m\n";
    std::cout << "  ➔  Vitesse       : \033[1;36m" << r.motsParMinute << " MPM\033[0m\n\n";
    
    std::cout << "\033[0;90m──────────────────────────────────────────────────────────\033[0m\n";
    std::cout << "Score enregistre. Appuyez sur une touche pour continuer...";
    while(lireTouche() == 0);
}

void executerModeJeu(const std::string& nomMode, const std::vector<std::string>& dictionnaire)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, dictionnaire.size() - 1);

    ResultatPartie statistiques;
    statistiques.mode = nomMode;
    auto tempsDebut = std::chrono::steady_clock::now();
    int viesRestantes = VIES_MODE_SURVIE;
    bool jeuEnCours = true;

    while (jeuEnCours)
    {
        auto tempsActuel = std::chrono::steady_clock::now();
        double secondesEcoulees = std::chrono::duration_cast<std::chrono::seconds>(tempsActuel - tempsDebut).count();

        if (nomMode == "Chrono" && secondesEcoulees >= DUREE_MODE_CHRONO)
            break;  //condition d'arrêt global
        if (nomMode == "Classique" && statistiques.totalMots >= LIMITATION_MOTS_CLASSIQUE)
            break;

        std::string motCible = dictionnaire[dis(gen)];
        statistiques.totalMots++;

        std::string saisieUtilisateur = "";
        bool motFini = false;
        
        int hauteurMot = 0; 
        auto tempsDerniereChute = std::chrono::steady_clock::now();

        while (!motFini)
        {
            effacerEcran();
            
            if (nomMode == "Chute libre")
            {
                auto maintenant = std::chrono::steady_clock::now();  //chute toute les 1s
                if (std::chrono::duration_cast<std::chrono::milliseconds>(maintenant - tempsDerniereChute).count() > 1000)
                {
                    hauteurMot++;
                    tempsDerniereChute = maintenant;
                    if (hauteurMot > 8)   //touche le sol
                    {
                        statistiques.fautes++;
                        viesRestantes--;
                        if (viesRestantes <= 0)
                            jeuEnCours = false;
                        motFini = true;
                        break;
                    }
                }
            }

            // --------------- INTERFACE DE JEU --------------------------
            std::cout << "\033[1;34m MODE : " << std::left << std::setw(13) << nomMode << "\033[0m | ";
            if (nomMode == "Chrono")
            {
                std::cout << "\033[1;33mTemps: " << (DUREE_MODE_CHRONO - (int)secondesEcoulees) << "s\033[0m | ";
            }
            else if (nomMode == "Survie" || nomMode == "Chute libre")
            {
                std::cout << "\033[1;31mVies: ";
                for(int v=0; v<viesRestantes; v++) std::cout << "♥ ";
                std::cout << "\033[0m | ";
            }
            else
            {
                std::cout << "\033[1;37mProgression: " << statistiques.totalMots << "/" << LIMITATION_MOTS_CLASSIQUE << "\033[0m | ";
            }
            std::cout << "\033[1;32mScore: " << statistiques.motsReussis << "\033[0m\n";
            std::cout << "\033[0;90m──────────────────────────────────────────────────────────\033[0m\n";

            // ------------ zone mot -------------------------
            for(int h=0; h<hauteurMot; h++)
                std::cout << "\n";
            std::cout << "  Mot cible :  \033[1;35m" << motCible << "\033[0m\n";
            for(int h=hauteurMot; h<8; h++)
                std::cout << "\n";   ///vérrouille l'espace vertical

            std::cout << "\033[0;90m──────────────────────────────────────────────────────────\033[0m\n";
            std::cout << "  Saisie    :  \033[1;32m" << saisieUtilisateur << "\033[1;5;32m█\033[0m\n";

            // ------------- GESTION DU CLAVIER ----------------
            int caractereSaisi = lireTouche();
            if (caractereSaisi == 10 || caractereSaisi == 13 || caractereSaisi == ' ')
            {
                if(!saisieUtilisateur.empty())
                    motFini = true;
            }
            else if (caractereSaisi == 127 || caractereSaisi == 8)
            {
                if (!saisieUtilisateur.empty())
                    saisieUtilisateur.pop_back();
            }
            else if (caractereSaisi >= 32 && caractereSaisi <= 126)
            {
                saisieUtilisateur += static_cast<char>(caractereSaisi);
            }

            if (nomMode == "Chrono")   //vérif temps réel pour chrono
            {
                auto tA = std::chrono::steady_clock::now();
                secondesEcoulees = std::chrono::duration_cast<std::chrono::seconds>(tA - tempsDebut).count();
                if (secondesEcoulees >= DUREE_MODE_CHRONO)
                {
                    jeuEnCours = false;
                    break;
                }
            }
        }

        if (!jeuEnCours)
            break;

        if (hauteurMot <= 8)  //vérif mot tapé
        {
            if (saisieUtilisateur == motCible)
            {
                statistiques.motsReussis++;
            }
            else
            {
                statistiques.fautes++;
                if (nomMode == "Survie" || nomMode == "Chute libre")
                {
                    viesRestantes--;
                    if (viesRestantes <= 0) jeuEnCours = false;
                }
            }
        }
    }

    //--------------------- Calcul --------------------
    auto tempsFin = std::chrono::steady_clock::now();
    double dureeMinutes = std::chrono::duration_cast<std::chrono::milliseconds>(tempsFin - tempsDebut).count() / 60000.0;
    if (dureeMinutes < 0.01) dureeMinutes = 0.01; //évite la division par zéro

    statistiques.motsParMinute = statistiques.motsReussis / dureeMinutes;
    if (statistiques.totalMots > 0)
    {
        statistiques.precision = (static_cast<double>(statistiques.motsReussis) / statistiques.totalMots) * 100.0;
    }

    enregistrerHistorique(statistiques);  //sauvegarde automatique et affichage du tableau des scores
    afficherResultats(statistiques);
}
