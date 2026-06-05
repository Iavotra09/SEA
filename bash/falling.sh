mode_falling()
{
    local time_limit=5
    local max_lives=3

    echo ""
    echo -e "${BOLD}${CYAN}--- Mode Chute Libre ---${RESET}"
    echo -e "${DIM}  Tapez chaque mot avant la fin du compte à rebours (${time_limit}s).${RESET}"
    echo -e "${DIM}  Le mot suivant apparaît dès que vous tapez correctement.${RESET}"
    echo ""

    local lives=$max_lives
    local words_done=0 correct=0
    local start_ms end_ms
    start_ms=$(now_ms)

    print_lives $lives $max_lives
    echo ""

    read_falling()    #-> 0=succès, 1=timeout, 2=entrée sur mauvais mot  
    {
        local expected="$1"
        local typed=""
        local char
        local deadline=$(( $(now_ms) + time_limit * 1000 ))
        tput sc

        while true; do
            local now remaining_ms remaining_s
            now=$(now_ms)
            remaining_ms=$(( deadline - now ))
            (( remaining_ms <= 0 )) && { TYPED="$typed"; return 1; }
            remaining_s=$(( remaining_ms / 1000 ))

            tput rc; tput el

            local tcol="${GREEN}"   #coloration timer
            (( remaining_s <= 2 )) && tcol="${BRED}"
            (( remaining_s == 3 )) && tcol="${YELLOW}"
            printf "${tcol}[%ds]${RESET} " "$remaining_s"

            local i    #coloration lettre
            for (( i=0; i<${#typed}; i++ )); do
                local ct="${typed:$i:1}" ce="${expected:$i:1}"
                [[ "$ct" == "$ce" ]] \
                    && printf "${GREEN}%s${RESET}" "$ct" \
                    || printf "${RED}%s${RESET}"   "$ct"
            done
            printf "${DIM}%s${RESET}" "${expected:${#typed}}"

            IFS= read -rsn1 -t 0.25 char 2>/dev/null || char=""

            if [[ -n "$char" ]]; then
                if [[ "$char" == $'\x7f' || "$char" == $'\b' ]]; then
                    [[ -n "$typed" ]] && typed="${typed:0:-1}"
                elif [[ -z "$char" ]]; then
                    TYPED="$typed"; return 2
                else
                    typed+="$char"
                    
                    if [[ "$typed" == "$expected" ]]; then
                        tput rc; tput el
                        printf "${BGREEN}v ${expected}${RESET}\n"
                        TYPED="$typed"; return 0
                    fi
                fi
            fi
        done
    }

    while (( lives > 0 )); do
        local word
        word=$(random_word)
        (( words_done++ ))

        printf "  ${BOLD}%-15s${RESET}  " "$word"

        read_falling "$word"
        local result=$?

        case $result in
            0)  (( correct++ )) ;;
            1)
                tput rc; tput el
                echo -e "${RED}x Timeout !  ${DIM}→ ${word}${RESET}"
                (( lives-- ))
                print_lives $lives $max_lives
                ;;
            2)
                tput rc; tput el
                echo -e "${RED}x  ${DIM}→ ${word}${RESET}"
                (( lives-- ))
                print_lives $lives $max_lives
                ;;
        esac
    done

    end_ms=$(now_ms)
    local elapsed_s=$(( (end_ms - start_ms) / 1000 ))
    local wpm accuracy
    wpm=$(calc_wpm "$words_done" "$(( end_ms - start_ms ))")
    accuracy=$(calc_accuracy "$correct" "$words_done")

    echo ""
    echo -e "  ${BRED}Game Over !${RESET}"
    show_results "Chute Libre" "$wpm" "$accuracy" "$correct" "$words_done" "$elapsed_s"
}
