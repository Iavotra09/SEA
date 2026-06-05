import os
import time

def chargerDictionnaire(cheminFichier):
    """Charge les chaînes depuis le fichier texte."""
    if not os.path.exists(cheminFichier):
        return ["programmation", "python", "clavier", "vitesse", "code", "developpeur", "console"]
    with open(cheminFichier, "r", encoding="utf-8") as fichierOuvert:
        return [ligne.strip() for ligne in fichierOuvert if ligne.strip()]

def enregistrerHistorique(cheminFichier, modeJeu, scoreFinal, motsParMinute, precisionFinale):
    """Inscrit la performance dans le fichier de log historique."""
    horodatage = time.strftime("%d/%m/%Y %H:%M")
    with open(cheminFichier, "a", encoding="utf-8") as fichierOuvert:
        fichierOuvert.write(f"[{horodatage}] Mode: {modeJeu} | Score: {scoreFinal} | WPM: {motsParMinute} | Précision: {precisionFinale}%\n")

def lireHistorique(cheminFichier):
    """Extrait les dernières lignes enregistrées."""
    if not os.path.exists(cheminFichier):
        return ["Aucun historique disponible dans historique.dat."]
    with open(cheminFichier, "r", encoding="utf-8") as fichierOuvert:
        lignesFichier = fichierOpened = fichierOuvert.readlines()
        return [ligne.strip() for ligne in lignesFichier[-6:]]
