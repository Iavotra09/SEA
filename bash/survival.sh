mode_survival()
{
    local max_lives=3

    echo ""
    echo -e "${BOLD}${RED}--- Mode Survie (${max_lives} vies) ---${RESET}"
    echo -e "${DIM}  Chaque erreur coûte une vie. Game over à 0.${RESET}"
    echo ""

    local lives=$max_lives
    local words_done=0 correct=0 total_chars=0 correct_chars=0
    local streak=0
    local start_ms end_ms
    start_ms=$(now_ms)

    print_lives $lives $max_lives
    echo ""

    while (( lives > 0 )); do
        local word
        word=$(random_word)

        printf "  ${BOLD}%-15s${RESET}  " "$word"

        read_live "$word"

        local cc
        cc=$(count_correct_chars "$word" "$TYPED")
        (( words_done++ ))
        (( total_chars   += ${#word} ))
        (( correct_chars += cc ))

        if [[ "$TYPED" == "$word" ]]; then
            (( correct++ ))
            (( streak++ ))
            tput rc; tput el
            if (( streak % 5 == 0 )); then
                echo -e "${BGREEN}v Série de ${streak} !${RESET}"
            else
                echo -e "${BGREEN}v${RESET}"
            fi
        else
            (( lives-- ))
            streak=0
            tput rc; tput el
            echo -e "${RED}x  ${DIM}→ ${word}${RESET}"
            print_lives $lives $max_lives
        fi
    done

    end_ms=$(now_ms)
    local elapsed_s=$(( (end_ms - start_ms) / 1000 ))
    local wpm accuracy
    wpm=$(calc_wpm "$words_done" "$(( end_ms - start_ms ))")
    accuracy=$(calc_accuracy "$correct_chars" "$total_chars")

    echo ""
    echo -e "  ${BRED}Game Over !${RESET}"
    show_results "Survie" "$wpm" "$accuracy" "$correct" "$words_done" "$elapsed_s"
}
