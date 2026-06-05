import pygame
import random
import sys
import time
from constantes import *
import gestionnaireDonnees

class JeuFrappe:
    def __init__(self):
        pygame.init()
        pygame.font.init()
        
        self.ecranGraphique = pygame.display.set_mode((fenetreLargeur, fenetreHauteur))
        pygame.display.set_caption("**** TYPING GAME ****")
        self.horlogeInterne = pygame.time.Clock()
        
        self.policeMot = pygame.font.SysFont("Consolas", 24, bold=True)
        self.policeUi = pygame.font.SysFont("Arial", 18, bold=True)
        self.policeTitre = pygame.font.SysFont("Arial", 44, bold=True)
        
        self.fichierDictionnaire = "dictionnaire.txt"
        self.fichierHistorique = "historique.dat"
        self.listeMotsDictionnaire = gestionnaireDonnees.chargerDictionnaire(self.fichierDictionnaire)
        
        self.etatActuel = etatMenu
        self.modeActuel = modeClassique
        self.applicationEnCours = True
        
        self.motsActifs = []
        self.saisieUtilisateur = ""
        self.scoreActuel = 0
        self.nombreVies = 3
        
        self.tempsDebutPartie = 0.0
        self.frappesTotales = 0
        self.frappesReussies = 0
        self.motsParMinute = 0.0
        self.precisionFinale = 100.0
        
        self.tempsDernierMot = 0
        self.intervalleApparition = 2000

    def initialiserPartie(self, modeSelectionne):
        """Configure les variables de jeu avant de lancer la partie."""
        self.modeActuel = modeSelectionne
        self.etatActuel = etatJeu
        self.motsActifs = []
        self.saisieUtilisateur = ""
        self.scoreActuel = 0
        self.frappesTotales = 0
        self.frappesReussies = 0
        self.motsParMinute = 0.0
        self.precisionFinale = 100.0
        self.tempsDebutPartie = time.time()
        self.tempsDernierMot = pygame.time.get_ticks()
        
        if self.modeActuel == modeChuteRapide:
            self.nombreVies = 3
            self.intervalleApparition = 1000
        elif self.modeActuel == modeChrono:
            self.nombreVies = 999
            self.intervalleApparition = 1600
        else:
            self.nombreVies = 3
            self.intervalleApparition = 2000

    def ajouterNouveauMot(self):
        """Génère un élément textuel sur une coordonnée horizontale aléatoire."""
        if not self.listeMotsDictionnaire: 
            return
            
        texteSelectionne = random.choice(self.listeMotsDictionnaire)
        largeurTexte, _ = self.policeMot.size(texteSelectionne)
        
        limiteDroite = fenetreLargeur - largeurTexte - 20
        coordonneeX = random.randint(20, max(20, limiteDroite))
        
        vitesseDeBase = 1.6 if self.modeActuel != modeChuteRapide else 3.2
        vitesseCalculee = vitesseDeBase + (self.scoreActuel // 10) * 0.25
        
        self.motsActifs.append({
            'texte': texteSelectionne, 
            'x': coordonneeX, 
            'y': 0.0, 
            'vitesse': vitesseCalculee
        })

    def gererEvenements(self):
        """Capte et redistribue les interactions utilisateur."""
        for evenementDetecte in pygame.event.get():
            if evenementDetecte.type == pygame.QUIT:
                self.applicationEnCours = False
            elif evenementDetecte.type == pygame.MOUSEBUTTONDOWN and evenementDetecte.button == 1:
                self.gererClicsSouris(pygame.mouse.get_pos())
            elif evenementDetecte.type == pygame.KEYDOWN:
                if evenementDetecte.key == pygame.K_ESCAPE:
                    self.etatActuel = etatMenu
                elif self.etatActuel == etatJeu:
                    self.gererClavierJeu(evenementDetecte)

    def gererClicsSouris(self, positionSouris):
        """Routage des clics sur l'ensemble de l'interface graphique."""
        positionX, positionY = positionSouris
        if self.etatActuel == etatMenu:
            if 250 <= positionX <= 550 and 180 <= positionY <= 230: self.initialiserPartie(modeClassique)
            elif 250 <= positionX <= 550 and 250 <= positionY <= 300: self.initialiserPartie(modeChuteRapide)
            elif 250 <= positionX <= 550 and 320 <= positionY <= 370: self.initialiserPartie(modeChrono)
            elif 250 <= positionX <= 550 and 410 <= positionY <= 460: self.etatActuel = etatHistorique
            elif 250 <= positionX <= 550 and 480 <= positionY <= 530: self.applicationEnCours = False
        elif self.etatActuel in [etatHistorique, etatFin]:
            if 250 <= positionX <= 550 and 510 <= positionY <= 560: self.etatActuel = etatMenu

    def gererClavierJeu(self, evenementClavier):
        """Met à jour le tampon d'écriture lors de la frappe."""
        if evenementClavier.key == pygame.K_BACKSPACE:
            self.saisieUtilisateur = self.saisieUtilisateur[:-1]
        elif evenementClavier.key in [pygame.K_RETURN, pygame.K_SPACE]:
            self.verifierMotSaisi()
        else:
            if evenementClavier.unicode and evenementClavier.unicode.isprintable():
                self.saisieUtilisateur += evenementClavier.unicode
                self.frappesTotales += 1

    def verifierMotSaisi(self):
        """Compare la saisie utilisateur avec les entités à l'écran."""
        motEclate = self.saisieUtilisateur.strip()
        for motActif in self.motsActifs:
            if motActif['texte'] == motEclate:
                self.motsActifs.remove(motActif)
                self.scoreActuel += len(motEclate)
                self.frappesReussies += len(motEclate)
                break
        self.saisieUtilisateur = ""

    def mettreAJour(self):
        """Met à jour la physique et les boucles de contrôle de session."""
        if self.etatActuel != etatJeu: 
            return
        
        tempsPresent = pygame.time.get_ticks()
        dureeEcoulee = time.time() - self.tempsDebutPartie
        
        if self.modeActuel == modeChrono and dureeEcoulee >= 60:
            self.cloreLaPartie()
            return

        if tempsPresent - self.tempsDernierMot > self.intervalleApparition:
            self.ajouterNouveauMot()
            self.tempsDernierMot = tempsPresent

        for motActif in self.motsActifs[:]:
            motActif['y'] += motActif['vitesse']
            if motActif['y'] > fenetreHauteur - 80:
                self.motsActifs.remove(motActif)
                if self.modeActuel != modeChrono:
                    self.nombreVies -= 1
                    if self.nombreVies <= 0:
                        self.cloreLaPartie()

    def cloreLaPartie(self):
        """Calcule les statistiques et effectue l'archivage persistant."""
        minutesEcoulees = (time.time() - self.tempsDebutPartie) / 60.0
        if minutesEcoulees <= 0: 
            minutesEcoulees = 0.01
        
        self.motsParMinute = round((self.frappesReussies / 5) / minutesEcoulees, 1)
        self.precisionFinale = round((self.frappesReussies / self.frappesTotales) * 100, 1) if self.frappesTotales > 0 else 0.0
        
        gestionnaireDonnees.enregistrerHistorique(
            self.fichierHistorique, self.modeActuel, self.scoreActuel, self.motsParMinute, self.precisionFinale
        )
        self.etatActuel = etatFin

    def dessinerBoutonInteractif(self, texteBouton, axeX, axeY):
        """Affiche un bouton doté d'une détection de survol dynamique."""
        positionSouris = pygame.mouse.get_pos()
        rectangleBouton = pygame.Rect(axeX - 150, axeY - 25, 300, 50)
        couleurRemplissage = couleurSurvol if rectangleBouton.collidepoint(positionSouris) else couleurBouton
        
        pygame.draw.rect(self.ecranGraphique, couleurRemplissage, rectangleBouton, border_radius=8)
        pygame.draw.rect(self.ecranGraphique, couleurInterface, rectangleBouton, width=2, border_radius=8)
        
        surfaceDuTexte = self.policeUi.render(texteBouton, True, couleurMot)
        self.ecranGraphique.blit(surfaceDuTexte, surfaceDuTexte.get_rect(center=(axeX, axeY)))

    def dessiner(self):
        """Moteur graphique de rafraîchissement d'affichage."""
        self.ecranGraphique.fill(couleurFond)
        
        if self.etatActuel == etatMenu:
            surfaceTitre = self.policeTitre.render("JEU DE FRAPPE", True, couleurSaisie)
            self.ecranGraphique.blit(surfaceTitre, surfaceTitre.get_rect(center=(fenetreLargeur//2, 80)))
            self.dessinerBoutonInteractif(f"Mode {modeClassique}", fenetreLargeur//2, 205)
            self.dessinerBoutonInteractif(f"Mode {modeChuteRapide}", fenetreLargeur//2, 275)
            self.dessinerBoutonInteractif(f"Mode {modeChrono}", fenetreLargeur//2, 345)
            self.dessinerBoutonInteractif("Consulter l'Historique", fenetreLargeur//2, 435)
            self.dessinerBoutonInteractif("Quitter", fenetreLargeur//2, 505)
            
        elif self.etatActuel == etatJeu:
            for motActif in self.motsActifs:
                surfaceMot = self.policeMot.render(motActif['texte'], True, couleurMot)
                self.ecranGraphique.blit(surfaceMot, (motActif['x'], int(motActif['y'])))
                
            pygame.draw.rect(self.ecranGraphique, (30, 41, 59), (0, fenetreHauteur - 70, fenetreLargeur, 70))
            pygame.draw.line(self.ecranGraphique, couleurInterface, (0, fenetreHauteur - 70), (fenetreLargeur, fenetreHauteur - 70), 2)
            
            affichageChronoVies = f"TEMPS : {int(60 - (time.time() - self.tempsDebutPartie))}s" if self.modeActuel == modeChrono else f"VIES : {'♥ ' * self.nombreVies}"
            surfaceScore = self.policeUi.render(f"SCORE : {self.scoreActuel}", True, couleurInterface)
            surfaceVies = self.policeUi.render(affichageChronoVies, True, couleurAlerte)
            
            self.ecranGraphique.blit(surfaceScore, (20, fenetreHauteur - 45))
            self.ecranGraphique.blit(surfaceVies, (fenetreLargeur - 180, fenetreHauteur - 45))
            
            surfaceSaisie = self.policeMot.render(self.saisieUtilisateur + "|", True, couleurSaisie)
            self.ecranGraphique.blit(surfaceSaisie, surfaceSaisie.get_rect(center=(fenetreLargeur // 2, fenetreHauteur - 35)))
            
        elif self.etatActuel == etatHistorique:
            surfaceTitre = self.policeTitre.render("ANCIENS RÉSULTATS", True, couleurSaisie)
            self.ecranGraphique.blit(surfaceTitre, surfaceTitre.get_rect(center=(fenetreLargeur//2, 80)))
            
            lignesLog = gestionnaireDonnees.lireHistorique(self.fichierHistorique)
            ecartY = 180
            for ligneLog in lignesLog:
                surfaceLigne = self.policeUi.render(ligneLog, True, couleurMot)
                self.ecranGraphique.blit(surfaceLigne, surfaceLigne.get_rect(center=(fenetreLargeur//2, ecartY)))
                ecartY += 45
            self.dessinerBoutonInteractif("Retour au Menu", fenetreLargeur//2, 530)
            
        elif self.etatActuel == etatFin:
            surfaceTitre = self.policeTitre.render("FIN DE PARTIE", True, couleurAlerte)
            self.ecranGraphique.blit(surfaceTitre, surfaceTitre.get_rect(center=(fenetreLargeur//2, 100)))
            
            tableauMetriques = [
                f"Mode joué : {self.modeActuel}",
                f"Score cumulé : {self.scoreActuel}",
                f"Vitesse de frappe : {self.motsParMinute} WPM",
                f"Niveau de Précision : {self.precisionFinale}%"
            ]
            ecartY = 210
            for ligneMetrique in tableauMetriques:
                couleurMetrique = couleurValide if ("WPM" in ligneMetrique or "Précision" in ligneMetrique) else couleurMot
                surfaceMetrique = self.policeUi.render(ligneMetrique, True, couleurMetrique)
                self.ecranGraphique.blit(surfaceMetrique, surfaceMetrique.get_rect(center=(fenetreLargeur//2, ecartY)))
                ecartY += 50
            self.dessinerBoutonInteractif("Retour au Menu", fenetreLargeur//2, 510)
            
        pygame.display.flip()

    def executer(self):
        """Boucle d'exécution infinie cadencée par le framerate."""
        while self.applicationEnCours:
            self.gererEvenements()
            self.mettreAJour()
            self.dessiner()
            self.horlogeInterne.tick(imagesParSeconde)
        pygame.quit()
        sys.exit()

