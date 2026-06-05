WORDS=(
    chat chien oiseau lapin tigre lion ours aigle serpent poisson

    soleil pluie vent neige forêt rivière montagne étoile ciel nuage
    plage désert jungle vallée colline falaise brume aurore rosée torrent

    maison porte fenêtre toit mur jardin escalier couloir chambre cuisine
    chaise table lampe livre stylo clavier souris écran bureau téléphone

    courir marcher sauter lire écrire parler chanter danser sourire rire
    voler grimper plonger nager peindre cuisiner voyager construire réparer

    vite lent fort doux chaud froid haut bas grand petit
    brave joyeux calme sage libre curieux noble fier doux vif

    bonjour merci bonsoir demain matin soir nuit jour semaine mois
    rouge bleu vert jaune violet orange rose noir blanc gris
)

random_word()
{
    echo "${WORDS[RANDOM % ${#WORDS[@]}]}"
}
