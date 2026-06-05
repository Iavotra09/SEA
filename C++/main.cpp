#include "header.h"
#include <iostream>
#include <vector>

int main()
{
    configurerTerminal();
    std::vector<std::string> dictionnaire = chargerDictionnaire("dictionnaire.txt");

    std::vector<ModeDeJeu> listeModes =
    {
        {"Classique",   "Ecrire 15 mots le plus vite possible."},
        {"Chrono",      "Un sprint de 60 secondes maximum."},
        {"Survie",      "Le jeu s'arrête a la 3eme erreur."},
        {"Chute libre", "Tapez le mot avant qu'il ne touche le sol !"},
        {"Historique",  "Consulter vos scores précédents."},
        {"Quitter",     "Fermer l'application."}
    };

    int selection = 0;
    bool enCours = true;

    while (enCours)
    {
        afficherMenuInteractif("TYPING GAME", listeModes, selection);
        
        int touche = lireTouche();
        if (touche == 0) continue; //ignore si aucune touche n'est pressée

        switch (touche)
        {
            case 11: //fleche haut
                selection = (selection - 1 + listeModes.size()) % listeModes.size();
                break;
            case 12: //flecha bas
                selection = (selection + 1) % listeModes.size();
                break;
            case 10: //touche Entrée
            case 13:
                if (listeModes[selection].nom == "Quitter")
                {
                    enCours = false;
                }
                else if (listeModes[selection].nom == "Historique")
                {
                    afficherHistorique();
                }
                else
                {
                    executerModeJeu(listeModes[selection].nom, dictionnaire);
                }
                break;
            default:
                break;
        }
    }

    restaurerTerminal();
    effacerEcran();
    std::cout << "\033[1;33mMerci d'avoir joué! A bientôt.\n";
    return 0;
}
