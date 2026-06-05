show_results()
{
    local mode="$1"
    local wpm="$2"
    local accuracy="$3"
    local correct="$4"
    local total="$5"
    local duration="$6"

    echo ""
    echo -e "${BOLD}${YELLOW}--- Résultats ---${RESET}"
    echo -e "  Mode      : ${CYAN}${mode}${RESET}"
    echo -e "  Vitesse   : ${BOLD}${wpm} WPM${RESET}"
    echo -e "  Précision : ${BOLD}${accuracy}%${RESET}"
    echo -e "  Mots      : ${correct}/${total} corrects"
    echo -e "  Durée     : ${duration}s"
    echo ""

    if   (( wpm >= 80 && accuracy >= 95 )); then
        echo -e "  ${BGREEN}*** Expert - Performance exceptionnelle! ***${RESET}"
    elif (( wpm >= 50 && accuracy >= 80 )); then
        echo -e "  ${GREEN}*** Avancé - Très bonne frappe! ***${RESET}"
    elif (( wpm >= 30 && accuracy >= 70 )); then
        echo -e "  ${YELLOW}*** Intermédiaire — Continue comme ça! ***${RESET}"
    else
        echo -e "  ${DIM}*** Débutant — Pratique recommandée ***.${RESET}"
    fi

    save_history "$mode" "$wpm" "$accuracy" "$correct" "$total" "$duration"

    echo ""
    printf "  Appuyez sur Entrée pour revenir au menu..."
    read -r
}
