now_ms()
{
    date +%s%3N 2>/dev/null || echo $(( $(date +%s) * 1000 ))
}

calc_wpm()
{
    local words=$1 ms=$2
    (( ms <= 0 )) && echo 0 && return
    echo $(( words * 60000 / ms ))
}

calc_accuracy()
{
    local correct=$1 total=$2
    (( total <= 0 )) && echo 100 && return
    echo $(( correct * 100 / total ))
}

count_correct_chars()
{
    local expected="$1" typed="$2"
    local correct=0
    local len=$(( ${#expected} < ${#typed} ? ${#expected} : ${#typed} ))
    for (( i=0; i<len; i++ )); do
        [[ "${expected:$i:1}" == "${typed:$i:1}" ]] && (( correct++ ))
    done
    echo "$correct"
}

read_live()
{
    local expected="$1"
    local typed=""
    local char

    tput sc  # sauvegarde position curseur

    while true; do
        tput rc; tput el  # revenir à la position et effacer la ligne

        local i   #lettre vrai -> vert, faux -> rouge, reste -> gris
        for (( i=0; i<${#typed}; i++ )); do
            local ct="${typed:$i:1}" ce="${expected:$i:1}"
            [[ "$ct" == "$ce" ]] \
                && printf "${GREEN}%s${RESET}" "$ct" \
                || printf "${RED}%s${RESET}"   "$ct"
        done
        printf "${DIM}%s${RESET}" "${expected:${#typed}}"

        IFS= read -rsn1 char

        if [[ -z "$char" ]]; then     #entrée
            break
        elif [[ "$char" == $'\x7f' || "$char" == $'\b' ]]; then     #touche Backspace
            [[ -n "$typed" ]] && typed="${typed:0:-1}"
        else
            typed+="$char"
        fi
    done

    TYPED="$typed"
}

print_lives()
{
    local current=$1 max=$2
    printf "  Vies : "
    for (( i=0; i<current; i++ )); do printf "${RED}♥ ${RESET}"; done
    for (( i=current; i<max; i++ )); do printf "${DIM}♡ ${RESET}"; done
    printf "\n"
}
