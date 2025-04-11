# net-b47

Net Jeux est un projet de bibliothèque en C pour faciliter le développement de jeux en réseau. Ce projet a été conçu pour offrir une base de code réutilisable, modulaire, et optimisée pour les jeux multijoueurs. 

## Auteurs
- Leyti : leyti-faye.seydi@etu.u-bordeaux.fr
- Rayan : rayan.guerroudj@etu.u-bordeaux.fr
- Pierre : pierre.pierre@etu.u-bordeaux.fr

---

## Règles du jeu

Le jeu Net est un casse-tête logique en solo.

Il se joue sur une grille de 5x5 cases contenant des pièces comparables à des câbles ou des tuyaux. Ces pièces peuvent être tournées d'un quart de tour dans le sens horaire ou antihoraire. Elles existent sous différentes formes (extrémité, segment, coin ou té) et possèdent une orientation spécifique (N, E, S, W). Chaque pièce est composée d'un ou plusieurs bras, appelés demi-arêtes, connectés au centre de la case.

Représentation des pièces

Voici la représentation des pièces en fonction de leur forme et orientation :

Dans un terminal Linux, elles sont affichées comme suit :

|          |  N  |  E  |  S  |  W  |
| -------- | --- | --- | --- | --- |
| endpoint |  ^  |  >  |  v  |  <  |
| segment  |  |  |  -  |  |  |  -  |
| corner   |  └  |  ┌  |  ┐  |  ┘  |
| tee      |  ┴  |  ├  |  ┬  |  ┤  |


## Règles de connexion

Selon la règle originale de la Simon Tatham's Portable Puzzle Collection, l'objectif est de tourner les pièces de manière à former un réseau unique connecté, sans boucles.

En tournant les pièces, il est possible d'aligner correctement les demi-arêtes de deux cases adjacentes, formant ainsi une arête.

Statut des arêtes

L'état de connexion entre une case s et une case adjacente s' dans la direction d peut être défini ainsi :

MATCH : les deux demi-arêtes sont correctement connectées.

MISMATCH : une seule demi-arête est présente.

NOEDGE : aucune demi-arête n’est présente.

Si une case est située au bord de la grille et n’a pas de voisin dans une direction donnée, elle est considérée comme connectée à une case vide.



Un jeu est résolu lorsque toutes les demi-arêtes sont correctement appariées, formant un graphe (un ensemble de sommets connectés par des arêtes).

Mise à jour des règles

Dans la version actuelle du jeu, deux modifications ont été apportées aux règles originales :

La grille peut contenir des cases vides.

Les solutions contenant des boucles sont acceptées.

Chaque case de la grille est référencée par ses coordonnées (i, j), où (0,0) correspond au coin supérieur gauche, selon la notation des matrices.

## Exemple

## Grille de départ
     0 1 2 3 4
     ----------
  0 |┘ ^ < └ v |
  1 |┬ ┤ ┴ ├ ├ |
  2 |> ^ ┤ < - |
  3 |v ┬ ┴ ┘ | |
  4 |> ┤ v > v |
     ----------

# Solution
     0 1 2 3 4
     ----------
  0 |┌ < > ┐ v |
  1 |├ ┬ ┬ ┴ ┤ |
  2 |^ ^ ├ < | |
  3 |> ┬ ┴ ┐ | |
  4 |> ┴ < ^ ^ |
     ----------


Cette représentation textuelle est utilisée dans le programme game_text via la fonction game_print().

## Architecture du projet

Le projet repose sur une bibliothèque unique, game (libgame.a), divisée en plusieurs fichiers d'en-tête :

game.h : Fonctions de base du jeu.

game_aux.h : Fonctions auxiliaires.

game_ext.h : Fonctions avancées.

Cette bibliothèque manipule un type de données opaque, la structure game, via des pointeurs ou des pointeurs constants (cgame).

Le projet comprend plusieurs programmes, notamment :

game_text : version textuelle du jeu.

game_test : programme de tests.

game_solve : solveur automatique.

game_sdl : version graphique utilisant la bibliothèque SDL.

## Gestion des erreurs

Les fonctions de la bibliothèque supposent que les appels sont effectués avec des arguments valides. Il appartient aux programmes utilisant la bibliothèque de vérifier la validité des arguments (principe garbage in, garbage out).

Pour faciliter le débogage, il est recommandé d'utiliser la macro assert(), qui permet d'activer ou de désactiver les vérifications en fonction du mode de compilation (DEBUG ou RELEASE).

## References
https://en.wikipedia.org/wiki/net
https://www.chiark.greenend.org.uk/~sgtatham/puzzles/js/net.html

# Interface Graphique

Fait à partie de SDL2,  elle nous permet de jouer au jeux en voyant les pièces et de changer leur orientation en cliquant sur celle-ci 

Caractéristiques :
    1. La grille de jeu s’affiche dynamiquement.
    2. Les pièces interagissent avec la souris pour leur rotation.
    3. L’indication au joueur est d’afficher l’état du jeu (connectée, erreur, solution).
    4. Des boutons et shortcuts permettent de faciliter la navigation.
    5. Les erreurs sont affichées lorsqu’une connexion invalide est détectée.
    6. La fenêtre est redimensionnable et en mode plein écran.


Règles du jeu :
    1. Le but du jeu est de résoudre la grille.
    2. Clic gauche de la souris pour faire tourner la pièce dans le sens contraire des aiguilles d'une montre.
    3. Clic droit de la souris pour faire tourner la pièce dans le sens des aiguilles d'une montre.
    4. Appuyez sur 'Z' pour annuler un coup et 'Y' pour refaire un coup.
    5. Appuyez sur 'R' pour réinitialiser la grille.
    6. Appuyez sur 'S' pour afficher la solution.
    7. Appuyez sur 'Q' pour quitter le jeu.
    8. Appuyer sur 'H' pour afficher l'aide.

Le fond de case des pièces initiallement blueu se transformera en blueu se transformerons en blueu lorsque le jeu sera gagné, il faudra par lasuite réinitialier le jeux pour pouvoir jouer.

Les raccourcis clavier on des boutons affichés en dessous de la grille de jeux.
