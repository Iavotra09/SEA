#define _DEFAULT_SOURCE
#include "jeu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define ROUGE "\033[0;31m"
#define VERT "\033[0;32m"
#define JAUNE "\033[0;33m"
#define CYAN "\033[0;36m"
#define BROUGE "\033[1;31m"
#define BVERT "\033[1;32m"
#define BJAUNE "\033[1;33m"

#define MAX_MOTS     1024
#define MAX_MOT_LEN  64

static char  dicoBuf[MAX_MOTS][MAX_MOT_LEN];
static int   nbMots = 0;

static struct termios termOriginal;

static void modeRaw(int activer)
{
    if (activer)
    {
        struct termios t;
        tcgetattr(STDIN_FILENO, &termOriginal);
        t = termOriginal;
        t.c_lflag &= ~(unsigned)(ICANON | ECHO);
        t.c_cc[VMIN] = 0; t.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &termOriginal);
    }
}

static int lireChar(long long timeoutMs, char *c)   //lit un caractère avec un timeout en ms -> 1 si lu, 0 sinon
{
    struct timeval tv = { timeoutMs / 1000, (timeoutMs % 1000) * 1000 };
    fd_set fds; FD_ZERO(&fds); FD_SET(STDIN_FILENO, &fds);
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0)
    {
        unsigned char buf;
        if (read(STDIN_FILENO, &buf, 1) == 1)
        {
            *c = (char)buf; return 1;
        }
    }
    return 0;
}

static long long maintenant_ms(void)
{
    struct timeval tv; gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
}

void chargerDico(const char *base)
{
    char chemin[512];
    snprintf(chemin, sizeof(chemin), "%s/dico.txt", base);

    FILE *f = fopen(chemin, "r");
    if (!f)
    {
        fprintf(stderr, "Impossible d'ouvrir %s\n", chemin);
        exit(1);
    }

    char ligne[MAX_MOT_LEN];
    while (nbMots < MAX_MOTS && fgets(ligne, sizeof(ligne), f))
    {
        ligne[strcspn(ligne, "\r\n")] = '\0';   /* retire \n / \r\n */
        if (ligne[0] == '\0') continue;          /* ignore lignes vides */
        strncpy(dicoBuf[nbMots], ligne, MAX_MOT_LEN - 1);
        dicoBuf[nbMots][MAX_MOT_LEN - 1] = '\0';
        nbMots++;
    }
    fclose(f);

    if (nbMots == 0)
    {
        fprintf(stderr, "dico.txt est vide !\n");
        exit(1);
    }
}

static const char *motAleatoire(void) { return dicoBuf[rand() % nbMots]; }

static void afficherMot(const char *attendu, const char *tape, int timer, int restant) {
    printf("\033[u\033[2K"); /* restaurer curseur + effacer ligne */
    if (timer) {
        const char *col = restant <= 2 ? BROUGE : restant == 3 ? BJAUNE : VERT;
        printf("%s[%ds]" RESET " ", col, restant);
    }
    int lenT = strlen(tape), lenA = strlen(attendu);
    for (int i = 0; i < lenT; i++)
        printf(i < lenA && tape[i] == attendu[i] ? VERT "%c" RESET : ROUGE "%c" RESET, tape[i]);
    if (lenT < lenA) printf(DIM "%s" RESET, attendu + lenT);
    fflush(stdout);
}

static char *saisirMot(const char *attendu)  //saisie simple (sans timer). Retourne le mot tapé (à free() par l'appelant).
{
    char buf[256] = {0}; int len = 0; char c;
    modeRaw(1);
    printf("\033[s"); //sauvegarder position curseur
    while (1) {
        afficherMot(attendu, buf, 0, 0);
        while (!lireChar(100, &c));
        if (c == '\n' || c == '\r')          break;
        else if (c == 127 || c == '\b')      { if (len > 0) buf[--len] = '\0'; }
        else if ((unsigned char)c >= 32 && len < 254) buf[len++] = c, buf[len] = '\0';
    }
    modeRaw(0);
    printf("\n");
    char *res = malloc(len + 1); if (res) memcpy(res, buf, len + 1);
    return res;
}

static int saisirMotTimer(const char *attendu, int limiteSec)  //saisie timer  -> 0=succès, 1=timeout, 2=Entrée sur mauvais mot
{
    char buf[256] = {0};
    int len = 0;
    char c;
    long long deadline = maintenant_ms() + (long long)limiteSec * 1000LL;
    modeRaw(1);
    printf("\033[s");
    int retour = 1;

    while (1)
    {
        long long restantMs = deadline - maintenant_ms();
        if (restantMs <= 0) break;
        afficherMot(attendu, buf, 1, (int)(restantMs / 1000));
        long long fenetre = restantMs < 100 ? restantMs : 100;

        if (!lireChar(fenetre, &c))
            continue;
        if (c == '\n' || c == '\r')
        {
            retour = 2;
            break;
        }
        else if (c == 127 || c == '\b')
        {
            if (len > 0) buf[--len] = '\0';
        }
        else if ((unsigned char)c >= 32 && len < 254)
        {
            buf[len++] = c; buf[len] = '\0';
            if (strcmp(buf, attendu) == 0)
            {
                printf("\033[u\033[2K" BVERT "V %s" RESET "\n", attendu);
                fflush(stdout); retour = 0; break;
            }
        }
    }
    modeRaw(0);
    if (retour != 0) printf("\n");
    return retour;
}

static int calcWpm(int mots, long long ms)
{
    return ms > 0 ? (int)((long long)mots * 60000LL / ms) : 0;
}

static int calcPrecision(int corrects, int total)
{
    return total > 0 ? corrects * 100 / total : 100;
}

static int charsCorrects(const char *a, const char *b)
{
    int n = 0, lim = strlen(a) < strlen(b) ? strlen(a) : strlen(b);
    for (int i = 0; i < lim; i++)
        if (a[i] == b[i])
            n++;
    return n;
}

static void afficherVies(int vies, int max)
{
    printf("  Vies : ");
    for (int i = 0; i < vies; i++)
        printf(ROUGE "♥ " RESET);
    for (int i = vies; i < max; i++)
        printf(DIM "♡ " RESET);
    printf("\n"); fflush(stdout);
}

static void afficherResultats(const char *mode, int wpm, int precis, int corrects, int total, int dureeS) {
    printf("\n" BOLD BJAUNE "── Résultats ──" RESET "\n");
    printf("  Mode      : " CYAN "%s" RESET "\n", mode);
    printf("  Vitesse   : " BOLD "%d WPM" RESET "\n", wpm);
    printf("  Précision : " BOLD "%d%%" RESET "\n", precis);
    printf("  Mots      : %d/%d corrects\n", corrects, total);
    printf("  Durée     : %ds\n\n", dureeS);

    if      (wpm >= 80 && precis >= 95) printf("  " BVERT  "★★★ Expert — Performance exceptionnelle !" RESET "\n");
    else if (wpm >= 50 && precis >= 80) printf("  " VERT   "★★☆ Avancé — Très bonne frappe !"         RESET "\n");
    else if (wpm >= 30 && precis >= 70) printf("  " BJAUNE "★☆☆ Intermédiaire — Continue comme ça !"  RESET "\n");
    else                                printf("  " DIM    "☆☆☆ Débutant — Pratique recommandée."      RESET "\n");

    FILE *f = fopen(cheminHistorique, "a");  //sauvegarde historique
    if (f)
    {
        time_t t = time(NULL); struct tm *tm = localtime(&t);
        fprintf(f, "%02d/%02d/%04d|%02d:%02d|%s|%d|%d|%d|%d|%d\n", tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min, mode, wpm, precis, corrects, total, dureeS);
        fclose(f);
    }

    printf("\n  Appuyez sur Entrée pour continuer..."); fflush(stdout);
    modeRaw(1);
    char c; while (read(STDIN_FILENO, &c, 1) != 1 || (c != '\n' && c != '\r'));
    modeRaw(0); printf("\n");
}

void modeClassique(void)
{
    const int NB = 15;
    printf("\n" BOLD CYAN "--- Mode Classique (%d mots) ---" RESET "\n", NB);
    printf(DIM "  Tapez chaque mot et appuyez sur Entrée.\n\n" RESET);

    int corrects = 0, totChars = 0, okChars = 0;
    long long debut = maintenant_ms();

    for (int i = 1; i <= NB; i++)
    {
        const char *mot = motAleatoire();
        printf("  [%2d/%d]  " BOLD "%-15s" RESET "  ", i, NB, mot);
        fflush(stdout);

        char *tape = saisirMot(mot);
        okChars += charsCorrects(mot, tape ? tape : "");
        totChars += strlen(mot);

        if (tape && strcmp(tape, mot) == 0)
        {
            corrects++;
            printf("  -> " BVERT "v" RESET "\n");
        }
        else
        {
            printf("  -> " ROUGE "x  " DIM "(%s)" RESET "\n", mot);
        }
        free(tape);
    }

    long long fin = maintenant_ms();
    afficherResultats("Classique", calcWpm(NB, fin - debut), calcPrecision(okChars, totChars), corrects, NB, (int)((fin - debut) / 1000));
}

void modeChrono(void)
{
    const int DUREE = 60;
    printf("\n" BOLD BJAUNE "--- Mode Chrono (%ds) ---" RESET "\n", DUREE);
    printf(DIM "  Tapez un maximum de mots en %d secondes.\n\n" RESET, DUREE);

    long long debut = maintenant_ms();
    long long deadline = debut + (long long)DUREE * 1000LL;
    int mots = 0, corrects = 0, totChars = 0, okChars = 0;

    while (maintenant_ms() < deadline)
    {
        int restant = (int)((deadline - maintenant_ms()) / 1000);
        const char *mot = motAleatoire();
        printf("  " DIM "[%3ds]" RESET "  " BOLD "%-15s" RESET "  ", restant, mot);
        fflush(stdout);

        char *tape = saisirMot(mot);
        if (maintenant_ms() >= deadline)
        {
            free(tape);
            break;
        }

        okChars += charsCorrects(mot, tape ? tape : "");
        totChars += strlen(mot); mots++;

        if (tape && strcmp(tape, mot) == 0)
        {
            corrects++;
            printf("  -> " BVERT "v" RESET "\n");
        }
        else                                
        {
            printf("  -> " ROUGE "x  " DIM "(%s)" RESET "\n", mot);
        }
        free(tape);
    }

    printf("\n  " BJAUNE "⏱ Temps écoulé !" RESET "\n");
    afficherResultats("Chrono 60s", calcWpm(mots, (long long)DUREE * 1000LL), calcPrecision(okChars, totChars), corrects, mots, DUREE);
}

void modeSurvie(void)
{
    const int MAX_VIES = 3;
    printf("\n" BOLD BROUGE "--- Mode Survie (%d vies) ---" RESET "\n", MAX_VIES);
    printf(DIM "  Chaque erreur coûte une vie. Game over à 0.\n\n" RESET);

    int vies = MAX_VIES, mots = 0, corrects = 0, totChars = 0, okChars = 0, serie = 0;
    long long debut = maintenant_ms();

    afficherVies(vies, MAX_VIES); printf("\n");

    while (vies > 0)
    {
        const char *mot = motAleatoire();
        printf("  " BOLD "%-15s" RESET "  ", mot); fflush(stdout);

        char *tape = saisirMot(mot);
        okChars += charsCorrects(mot, tape ? tape : "");
        totChars += strlen(mot); mots++;

        if (tape && strcmp(tape, mot) == 0)
        {
            corrects++;
            serie++;
            if (serie % 5 == 0)
                printf("  → " BVERT "v Série de %d !" RESET "\n", serie);
            else                
                printf("  → " BVERT "v" RESET "\n");
        }
        else
        {
            vies--; serie = 0;
            printf("  -> " ROUGE "x  " DIM "(%s)" RESET "\n", mot);
            afficherVies(vies, MAX_VIES);
        }
        free(tape);
    }

    long long fin = maintenant_ms();
    printf("\n  " BROUGE "Game Over !" RESET "\n");
    afficherResultats("Survie", calcWpm(mots, fin - debut), calcPrecision(okChars, totChars), corrects, mots, (int)((fin - debut) / 1000));
}

void modeChuteLibre(void)
{
    const int LIMITE = 5, MAX_VIES = 3;
    printf("\n" BOLD CYAN "--- Mode Chute Libre ---" RESET "\n");
    printf(DIM "  Tapez chaque mot avant la fin du compte à rebours (%ds).\n\n" RESET, LIMITE);

    int vies = MAX_VIES, mots = 0, corrects = 0;
    long long debut = maintenant_ms();

    afficherVies(vies, MAX_VIES); printf("\n");

    while (vies > 0)
    {
        const char *mot = motAleatoire();
        mots++;
        printf("  " BOLD "%-15s" RESET "  ", mot); fflush(stdout);

        int res = saisirMotTimer(mot, LIMITE);
        if (res == 0)
        {
            corrects++;
        }
        else
        {
            if (res == 1) printf("  -> " ROUGE "x Timeout !  " DIM "(%s)" RESET "\n", mot);
            else          printf("  -> " ROUGE "x  " DIM "(%s)" RESET "\n", mot);
            vies--; afficherVies(vies, MAX_VIES);
        }
    }

    long long fin = maintenant_ms();
    printf("\n  " BROUGE "Game Over !" RESET "\n");
    afficherResultats("Chute Libre", calcWpm(mots, fin - debut), calcPrecision(corrects, mots), corrects, mots, (int)((fin - debut) / 1000));
}
