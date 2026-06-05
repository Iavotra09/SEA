#ifndef JEU_H
#define JEU_H

extern char cheminHistorique[512];

void chargerDico(const char *base);

void modeClassique(void);
void modeChrono(void);
void modeSurvie(void);
void modeChuteLibre(void);

#endif
