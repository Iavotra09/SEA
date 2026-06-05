#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <vector>

struct ModeDeJeu
{
    std::string nom;
    std::string description;
};

struct ResultatPartie
{
    std::string mode;
    int motsReussis = 0;
    int totalMots = 0;
    int fautes = 0;
    double motsParMinute = 0.0;
    double precision = 0.0;
};

const int DUREE_MODE_CHRONO = 60;
const int VIES_MODE_SURVIE = 3;
const int LIMITATION_MOTS_CLASSIQUE = 15;

void effacerEcran();
void configurerTerminal();
void restaurerTerminal();
int lireTouche();
void afficherMenuInteractif(const std::string& titre, const std::vector<ModeDeJeu>& options, int& selection);

std::vector<std::string> chargerDictionnaire(const std::string& nomFichier);
void executerModeJeu(const std::string& nomMode, const std::vector<std::string>& dictionnaire);
void afficherResultats(const ResultatPartie& resultats);

void enregistrerHistorique(const ResultatPartie& resultats);
void afficherHistorique();

#endif
