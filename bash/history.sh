HISTORY_FILE="$DATA_DIR/history.dat"

init_history()
{
    [[ ! -f "$HISTORY_FILE" ]] && touch "$HISTORY_FILE"
}

save_history()
{
    local mode="$1"
    local wpm="$2"
    local accuracy="$3"
    local correct="$4"
    local total="$5"
    local duration="$6"

    local date heure
    date=$(date '+%d/%m/%Y')
    heure=$(date '+%H:%M')

    echo "${date}|${heure}|${mode}|${wpm}|${accuracy}|${correct}|${total}|${duration}" \
        >> "$HISTORY_FILE"
}

show_history()
{
    echo ""
    echo -e "${BOLD}${CYAN}--- Historique des parties ---${RESET}"
    echo ""

    if [[ ! -s "$HISTORY_FILE" ]]; then
        echo -e "  ${DIM}Aucune partie jouée pour l'instant.${RESET}"
        echo ""
        return
    fi

    printf "  ${DIM}%-12s %-6s %-15s %6s %6s %10s %6s${RESET}\n" \
        "Date" "Heure" "Mode" "WPM" "Préc." "Mots" "Durée"
    printf "  ${DIM}%s${RESET}\n" "------------------------------------------------------"

    tail -20 "$HISTORY_FILE" | \
    while IFS='|' read -r date heure mode wpm accuracy correct total duration; do
        printf "  %-12s %-6s ${CYAN}%-15s${RESET} ${BOLD}%4d${RESET} WPM  ${GREEN}%3d%%${RESET}  %4s/%-4s mots  %3ds\n" \
            "$date" "$heure" "$mode" "$wpm" "$accuracy" "$correct" "$total" "$duration"
    done

    echo ""
}
