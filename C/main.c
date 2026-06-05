#define _DEFAULT_SOURCE
#include "jeu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <termios.h>

char cheminHistorique[512];

static void afficherHistorique(void)
{
    printf("\n\033[1m\033[0;36m--- Historique des parties ---\033[0m\n\n");

    FILE *f = fopen(cheminHistorique, "r");
    if (!f) { printf("  \033[2mAucune partie jouée pour l'instant.\033[0m\n\n"); return; }

    char lignes[20][256]; int nb = 0; char buf[256];  //garder les 20 deernières lignes
    while (fgets(buf, sizeof(buf), f))
    {
        buf[strcspn(buf, "\n")] = '\0';
        if (!buf[0])
            continue;
        if (nb < 20)
            snprintf(lignes[nb++], 256, "%s", buf);
        else
        {
            for (int i = 0; i < 19; i++) memcpy(lignes[i], lignes[i+1], 256);
            snprintf(lignes[19], 256, "%s", buf);
        }
    }
    fclose(f);

    if (!nb)
    {
        printf("  \033[2mAucune partie jouée pour l'instant.\033[0m\n\n");
        return;
    }

    printf("  \033[2m%-12s %-6s %-14s %5s %6s %10s %6s\033[0m\n",
           "Date","Heure","Mode","WPM","Préc.","Mots","Durée");
    printf("  \033[2m────────────────────────────────────────────────────────\033[0m\n");

    for (int i = 0; i < nb; i++)
    {
        char date[16], heure[8], mode[32]; int wpm, precis, ok, tot, dur;
        if (sscanf(lignes[i], "%15[^|]|%7[^|]|%31[^|]|%d|%d|%d|%d|%d", date, heure, mode, &wpm, &precis, &ok, &tot, &dur) != 8)
            continue;
        printf("  %-12s %-6s \033[0;36m%-14s\033[0m \033[1m%4d\033[0m WPM"
               "  \033[0;32m%3d%%\033[0m  %3d/%-3d mots  %3ds\n", date, heure, mode, wpm, precis, ok, tot, dur);
    }
    printf("\n");
}

int main(void)
{
    srand((unsigned int)time(NULL));

    char base[460] = ".";
#ifdef __linux__   //cherche le chemin absolu de l'executable  
    ssize_t n = readlink("/proc/self/exe", base, sizeof(base)-1);  
    if (n > 0)
    {
        base[n] = '\0'; char *s = strrchr(base, '/');  //enlève le dernier / -> garde seulement le nom du dossier
        if (s)
            *s = '\0';
    }
#endif
    char dirData[480]; snprintf(dirData, sizeof(dirData), "%s/data", base);
    mkdir(dirData, 0755);
    snprintf(cheminHistorique, sizeof(cheminHistorique), "%s/historique.dat", dirData);

    chargerDico(base);

    while (1)
    {
        printf("\033[2J\033[H"); //=clear
        printf("\n");
        printf("\033[1m\033[0;36m╔══════════════════════════╗\033[0m\n");
        printf("\033[1m\033[0;36m║       TYPING GAME        ║\033[0m\n");
        printf("\033[1m\033[0;36m╚══════════════════════════╝\033[0m\n\n");
        printf("  \033[1;33m1\033[0m  Mode Classique\n");
        printf("  \033[1;33m2\033[0m  Mode Chrono\n");
        printf("  \033[1;33m3\033[0m  Mode Survie\n");
        printf("  \033[1;33m4\033[0m  Mode Chute Libre\n");
        printf("  \033[1;33m5\033[0m  Historique\n");
        printf("  \033[1;33mq\033[0m  Quitter\n\n");
        printf("  Votre choix : "); fflush(stdout);

        struct termios orig, raw;  //lire un caractère sans Entrée
        tcgetattr(STDIN_FILENO, &orig);
        raw = orig; raw.c_lflag &= ~(unsigned)(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        char c = 0; while (read(STDIN_FILENO, &c, 1) != 1);
        tcsetattr(STDIN_FILENO, TCSANOW, &orig);
        printf("\n");

        switch (c)
        {
            case '1': modeClassique();  break;
            case '2': modeChrono();     break;
            case '3': modeSurvie();     break;
            case '4': modeChuteLibre(); break;
            case '5':
                afficherHistorique();
                printf("  Appuyez sur Entrée pour revenir..."); fflush(stdout);
                tcsetattr(STDIN_FILENO, TCSANOW, &raw);
                while (read(STDIN_FILENO, &c, 1) != 1 || (c != '\n' && c != '\r'));
                tcsetattr(STDIN_FILENO, TCSANOW, &orig);
                printf("\n");
                break;
            case 'q': case 'Q':
                printf("  \033[0;32mÀ bientôt !\033[0m\n\n");
                return 0;
            default:
                printf("  \033[2mChoix invalide.\033[0m\n");
                usleep(700000);
        }
    }
}
