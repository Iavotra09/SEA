mode_classic()
{
    local count=15

    echo ""
    echo -e "${BOLD}${CYAN}--- Mode Classique (${count} mots) ---${RESET}"
    echo -e "${DIM}  Tapez chaque mot et appuyez sur Entrée.${RESET}"
    echo ""

    local correct=0 total_chars=0 correct_chars=0
    local start_ms end_ms
    start_ms=$(now_ms)

    for (( i=1; i<=count; i++ )); do
        local word
        word=$(random_word)

        printf "  [%2d/%d]  ${BOLD}%-15s${RESET}  " "$i" "$count" "$word"

        read_live "$word"

        local cc
        cc=$(count_correct_chars "$word" "$TYPED")
        (( total_chars  += ${#word} ))
        (( correct_chars += cc ))

        if [[ "$TYPED" == "$word" ]]; then
            (( correct++ ))
            tput rc; tput el
            echo -e "${BGREEN}v${RESET}"
        else
            tput rc; tput el
            echo -e "${RED}x  ${DIM}→ ${word}${RESET}"
        fi
    done

    end_ms=$(now_ms)
    local elapsed_s=$(( (end_ms - start_ms) / 1000 ))
    local wpm accuracy
    wpm=$(calc_wpm "$count" "$(( end_ms - start_ms ))")
    accuracy=$(calc_accuracy "$correct_chars" "$total_chars")

    show_results "Classique" "$wpm" "$accuracy" "$correct" "$count" "$elapsed_s"
}
