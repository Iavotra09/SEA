mode_timed()
{
    local duration=60

    echo ""
    echo -e "${BOLD}${YELLOW}--- Mode Chrono (${duration}s) ---${RESET}"
    echo -e "${DIM}  Tapez un maximum de mots en ${duration} secondes.${RESET}"
    echo ""

    local start_ms deadline
    start_ms=$(now_ms)
    deadline=$(( start_ms + duration * 1000 ))

    local words_done=0 correct=0 total_chars=0 correct_chars=0

    while true; do
        local now
        now=$(now_ms)
        (( now >= deadline )) && break

        local remaining=$(( (deadline - now) / 1000 ))
        local word
        word=$(random_word)

        printf "  ${DIM}[%3ds]${RESET}  ${BOLD}%-15s${RESET}  " "$remaining" "$word"

        read_live "$word"

        now=$(now_ms)
        (( now >= deadline )) && break

        local cc
        cc=$(count_correct_chars "$word" "$TYPED")
        (( words_done++ ))
        (( total_chars   += ${#word} ))
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

    echo ""
    echo -e "  ${BYELLOW}x Temps écoulé !${RESET}"

    local wpm accuracy
    wpm=$(calc_wpm "$words_done" "$(( duration * 1000 ))")
    accuracy=$(calc_accuracy "$correct_chars" "$total_chars")

    show_results "Chrono ${duration}s" "$wpm" "$accuracy" "$correct" "$words_done" "$duration"
}
