GAME_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$GAME_DIR/data"

mkdir -p "$DATA_DIR"

source "$GAME_DIR/lib/colors.sh"  
source "$GAME_DIR/lib/utils.sh"
source "$GAME_DIR/lib/dictionary.sh"
source "$GAME_DIR/lib/history.sh"
source "$GAME_DIR/lib/results.sh"

source "$GAME_DIR/modes/classic.sh"
source "$GAME_DIR/modes/timed.sh"
source "$GAME_DIR/modes/survival.sh"
source "$GAME_DIR/modes/falling.sh"

init_history

main_menu()
{
    while true; do
        clear
        echo ""
        echo -e "${BOLD}${CYAN}╔════════════════════════╗${RESET}"
        echo -e "${BOLD}${CYAN}║     TYPING GAME        ║${RESET}"
        echo -e "${BOLD}${CYAN}╚════════════════════════╝${RESET}"
        echo ""
        echo -e "  ${YELLOW}1${RESET}  Mode Classique"
        echo -e "  ${YELLOW}2${RESET}  Mode Chrono"
        echo -e "  ${YELLOW}3${RESET}  Mode Survie"
        echo -e "  ${YELLOW}4${RESET}  Mode Chute Libre"
        echo -e "  ${YELLOW}5${RESET}  Historique"
        echo -e "  ${YELLOW}q${RESET}  Quitter"
        echo ""
        printf "  Votre choix : "
        read -rsn1 choice
        echo ""

        case "$choice" in
            1) mode_classic  ;;
            2) mode_timed    ;;
            3) mode_survival ;;
            4) mode_falling  ;;
            5)
                show_history
                printf "  Appuyez sur Entrée pour revenir au menu..."
                read -r
                ;;
            q|Q)
                echo -e "  ${GREEN}À bientôt !${RESET}"
                echo ""
                exit 0
                ;;
            *)
                echo -e "  ${DIM}Choix invalide.${RESET}"
                sleep 0.8
                ;;
        esac
    done
}

main_menu
