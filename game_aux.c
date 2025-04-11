#include "game_aux.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_ext.h"
#include "game_struct.h"

#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)
#define NEXT_DIR_CW(d) ((d + 1) % NB_DIRS)
#define NEXT_DIR_CCW(d) ((d + 3) % NB_DIRS)

/**
 * Fonction : game_print

 * Affiche le plateau de jeu sous forme de grille avec des indices de lignes et
 colonnes.
 * Chaque case contient un symbole représentant une pièce du jeu et son
 orientation.

 * Paramètres : g : Pointeur vers la structure du jeu (cgame) contenant la
 taille du plateau et les cases.

 * Symboles utilisés :
 *  -ENDPOINT : Flèches (`^`, `v`, `>`, `<`) pour indiquer la direction.
 *  -CORNER : Coins (`└`, `┐`, `┌`, `┘`) selon l'orientation.
 *  -SEGMENT : `|` (vertical) ou `-` (horizontal) selon l'orientation.
 *  -TEE : Formes en T (`┴`, `┬`, `├`, `┤`).

 * Notes :
 *  -Vérifie que le plateau est valide avant l'affichage.
 *  -Si une orientation invalide est trouvée, le programme s'arrête avec une
 erreur.
 */

void game_print(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  printf("   ");  // Faire un decalage
  for (uint col = 0; col < g->width; col++) {
    printf("%d ", col);
  }
  printf("\n");

  printf("   ");  // Faire un decalage
  for (uint col = 0; col < g->width; col++) {
    printf("--");
  }
  printf("\n");

  for (int i = 0; i < g->height; i++) {
    printf("%d |", i);
    for (int j = 0; j < g->width; j++) {
      switch (g->cases[i * g->width + j].shape) {
        case ENDPOINT:
          switch (g->cases[i * g->width + j].orientation) {
            case NORTH:
              printf("^ ");
              break;

            case SOUTH:
              printf("v ");
              break;

            case EAST:
              printf("> ");
              break;

            case WEST:
              printf("< ");
              break;

            case NB_DIRS:
              exit(EXIT_FAILURE);
              break;
          }
          break;

        case CORNER:

          switch (g->cases[i * g->width + j].orientation) {
            case NORTH:
              printf("└ ");
              break;

            case SOUTH:
              printf("┐ ");
              break;

            case EAST:
              printf("┌ ");
              break;

            case WEST:
              printf("┘ ");
              break;

            case NB_DIRS:
              exit(EXIT_FAILURE);
              break;
          }
          break;

        case SEGMENT:
          switch (g->cases[i * g->width + j].orientation) {
            case NORTH:
              printf("| ");
              break;

            case SOUTH:
              printf("| ");
              break;

            case EAST:
              printf("- ");
              break;

            case WEST:
              printf("- ");
              break;

            case NB_DIRS:
              exit(EXIT_FAILURE);
              break;
          }
          break;

        case TEE:
          switch (g->cases[i * g->width + j].orientation) {
            case NORTH:
              printf("┴ ");
              break;

            case SOUTH:
              printf("┬ ");
              break;

            case EAST:
              printf("├ ");
              break;

            case WEST:
              printf("┤ ");
              break;

            case NB_DIRS:
              exit(EXIT_FAILURE);
              break;
          }
          break;

        case CROSS:
          printf("+ ");
          break;
        default:
          printf("  ");  // Faire un decalage
          break;
      }
    }
    printf("|\n");
  }
  printf("   ");  // Faire un decalage
  for (uint col = 0; col < g->width; col++) {
    printf("--");
  }
  printf("\n");
}

/**
 * Fonction : game_default

 * Initialise un jeu avec un plateau de pièces prédéfini.
 * Chaque ligne et chaque colonne du plateau est remplie avec des pièces de
 formes et
 * orientations spécifiques, définies pour un état initial particulier.

 * Retour : g : Un nouvel objet `game` contenant le plateau de jeu configuré.

 * Détails de l'initialisation :
 *  -Ligne 0 : Contient des pièces de type CORNER, ENDPOINT, orientées de
 différentes façons.
 *  -Ligne 1 : Contient principalement des pièces de type TEE avec diverses
 orientations.
 *  -Ligne 2 : Mélange de ENDPOINT, TEE et SEGMENT, orientées de façon
 spécifique.
 *  -Ligne 3 : Contient des pièces ENDPOINT, TEE et CORNER avec des orientations
 variées.
 *  -Ligne 4 : Mélange de ENDPOINT et TEE avec des orientations variées.
 */

game game_default(void) {
  game g = game_new_empty();

  shape shapes[25] = {CORNER,   ENDPOINT, ENDPOINT, CORNER,   ENDPOINT,
                      TEE,      TEE,      TEE,      TEE,      TEE,
                      ENDPOINT, ENDPOINT, TEE,      ENDPOINT, SEGMENT,
                      ENDPOINT, TEE,      TEE,      CORNER,   SEGMENT,
                      ENDPOINT, TEE,      ENDPOINT, ENDPOINT, ENDPOINT};

  direction orientations[25] = {WEST,  NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,
                                NORTH, EAST,  EAST,  EAST,  NORTH, WEST,  WEST,
                                WEST,  SOUTH, SOUTH, NORTH, WEST,  NORTH, EAST,
                                WEST,  SOUTH, EAST,  SOUTH};

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      game_set_piece_shape(g, i, j, shapes[i * 5 + j]);
      game_set_piece_orientation(g, i, j, orientations[i * 5 + j]);
    }
  }

  return g;
}

/**
 * Fonction : game_default_solution

 * Initialise un jeu avec un plateau de pièces configuré pour représenter une
 solution complète.
 * Chaque ligne et chaque colonne du plateau contient des pièces de formes et
 orientations spécifiques,
 * correspondant à une solution prédéfinie.

 * Retour : g : Un nouvel objet `game` contenant le plateau de jeu configuré
 comme solution.
 */

game game_default_solution(void) {
  game g = game_new_empty();

  shape shapes[25] = {CORNER,   ENDPOINT, ENDPOINT, CORNER,   ENDPOINT,
                      TEE,      TEE,      TEE,      TEE,      TEE,
                      ENDPOINT, ENDPOINT, TEE,      ENDPOINT, SEGMENT,
                      ENDPOINT, TEE,      TEE,      CORNER,   SEGMENT,
                      ENDPOINT, TEE,      ENDPOINT, ENDPOINT, ENDPOINT};

  direction orientations[25] = {EAST,  WEST,  EAST,  SOUTH, SOUTH, EAST,  SOUTH,
                                SOUTH, NORTH, WEST,  NORTH, NORTH, EAST,  WEST,
                                SOUTH, EAST,  SOUTH, NORTH, SOUTH, NORTH, EAST,
                                NORTH, WEST,  NORTH, NORTH};

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      game_set_piece_shape(g, i, j, shapes[i * 5 + j]);
      game_set_piece_orientation(g, i, j, orientations[i * 5 + j]);
    }
  }

  return g;
}
/**
 * Fonction : game_get_adjacent_square

 * Calcule les coordonnées de la case adjacente à une case donnée (i, j) dans
 une direction donnée.

 * Paramètres :
 *  -g : Le jeu actuel (cgame).
 *  -i, j : Coordonnées de la case actuelle.
 *  -d : Direction dans laquelle chercher la case adjacente (NORTH, EAST, SOUTH,
 WEST).
 *  -pi_next : Pointeur pour stocker la coordonnée de ligne de la case
 adjacente.
 *  -pj_next : Pointeur pour stocker la coordonnée de colonne de la case
 adjacente.

 * Retour :
 *  - true si la case adjacente est valide et calculée avec succès.
 *  - false si la direction est invalide ou si la case adjacente est hors des
 limites sans mode wrapping.
 */

/* ************************************************************************** */

int DIR2OFFSET[][2] = {
    [NORTH] = {-1, 0},
    [EAST] = {0, 1},
    [SOUTH] = {1, 0},
    [WEST] = {0, -1},
};

/* ************************************************************************** */

bool game_get_ajacent_square(cgame g, uint i, uint j, direction d,  //
                             uint* pi_next, uint* pj_next) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));

  // convert direction to offset
  int i_offset = DIR2OFFSET[d][0];
  int j_offset = DIR2OFFSET[d][1];

  // move to the next square in a given direction
  int ii = i + i_offset;
  int jj = j + j_offset;

  if (game_is_wrapping(g)) {
    ii = (ii + game_nb_rows(g)) % game_nb_rows(g);
    jj = (jj + game_nb_cols(g)) % game_nb_cols(g);
  }

  // check if next square at (ii,jj) is out of grid
  if (ii < 0 || ii >= (int)game_nb_rows(g)) return false;
  if (jj < 0 || jj >= (int)game_nb_cols(g)) return false;

  *pi_next = ii;
  *pj_next = jj;

  return true;
}

/**
 * Fonction : game_has_half_edge

 * Vérifie si une case (i, j) du jeu possède une demi-arête dans une direction
 donnée.

 * Paramètres :
 *  -g : Le jeu actuel (cgame).
 *  -i, j : Coordonnées de la case à vérifier.
 *  -d : Direction à vérifier (NORTH, SOUTH, EAST, WEST).

 * Retour :
 *  -true si la case possède une demi-arête dans la direction spécifiée.
 *  -false sinon.

 * Comportement :
 *  -Gère les différentes formes de pièces (TEE, CORNER, SEGMENT, ENDPOINT).
 *  -Vérifie les orientations et les directions de chaque type de pièce.
 *  -Affiche une erreur et termine le programme si les paramètres sont
 invalides.
 */

static bool has_half_edge(shape s, direction o, direction d) {
  switch (s) {
    case EMPTY:
      return false;
    case ENDPOINT:
      return (d == o);
    case SEGMENT:
      return (d == o || d == OPPOSITE_DIR(o));
    case TEE:
      return (d != OPPOSITE_DIR(o));
    case CORNER:
      return (d == o || d == NEXT_DIR_CW(o));
    case CROSS:
      return true;
    default:
      assert(true);
      return false;
  }
}

/* ************************************************************************** */

bool game_has_half_edge(cgame g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d >= 0 && d < NB_DIRS);
  shape s = game_get_piece_shape(g, i, j);
  direction o = game_get_piece_orientation(g, i, j);
  return has_half_edge(s, o, d);
}

/**
 * Fonction : game_check_edge

 * Vérifie l'état de l'arête entre une case (i, j) et une case adjacente dans
 une direction donnée.

 * Paramètres :
 *  -g : Le jeu actuel (cgame).
 *  -i, j : Coordonnées de la case à vérifier.
 *  -d : Direction de l'arête (NORTH, SOUTH, EAST, WEST).

 * Retour :
 *  -NOEDGE : Aucune demi-arête présente sur l'arête.
 *  -MATCH : Les deux cases adjacentes ont des demi-arêtes correspondantes.
 *  -MISMATCH : Une seule des deux cases possède une demi-arête.

 * Comportement :
 *  -Vérifie si la case et la direction sont valides.
 *  -Détermine les coordonnées de la case adjacente.
 *  -Vérifie les demi-arêtes des deux cases dans la direction donnée et son
 opposé.
 */

edge_status game_check_edge(cgame g, uint i, uint j, direction d) {
  bool he = game_has_half_edge(g, i, j, d);
  uint nexti, nextj;
  bool next = game_get_ajacent_square(g, i, j, d, &nexti, &nextj);

  /* The status can be simply computed based on the number of half-hedges:
   *  - 0: no edge
   *  - 1: mismatched edge
   *  - 2: well-matched edge
   */

  edge_status status = NOEDGE;
  if (he) status += 1;
  if (next) {
    bool nexthe = game_has_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));
    if (nexthe) status += 1;
  }

  return status;
}

/**
 * Fonction : game_is_well_paired

 * Vérifie si toutes les arêtes du jeu sont correctement appariées.

 * Paramètres : g : Le jeu actuel (cgame).

 * Retour :
 *  -true si toutes les arêtes du jeu sont bien appariées (aucune arête ne
 présente de MISMATCH).
 *  -false sinon.
 *
 * Comportement :
 *  -Parcourt chaque case du plateau.
 *  -Vérifie chaque direction (NORTH, SOUTH, EAST, WEST) pour s'assurer que
 toutes les arêtes sont en état MATCH ou NOEDGE.
 *  -Retourne false dès qu'une arête est en état MISMATCH.
 */

bool game_is_well_paired(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      for (direction d = 0; d < NB_DIRS; d++) {
        if (game_check_edge(g, i, j, d) == MISMATCH) {
          return false;
        }
      }
    }
  }
  return true;
}

/**
 * Fonction : game_is_connected

 * Vérifie si le jeu est connecté, c'est-à-dire si toutes les arêtes sont bien
 appariées.

 * Paramètres : g : Le jeu actuel (cgame).

 * Retour :
 *  -true si toutes les arêtes du jeu sont correctement appariées (le jeu est
 connecté).
 *  -false sinon.
 *
 * Comportement :
 *  -Appelle la fonction game_is_well_paired pour vérifier que toutes les arêtes
 sont bien appariées.
 *  -Retourne false si des arêtes sont mal appariées, sinon true.
 */

/* square colors used in BFS algorithm */
typedef enum {
  WHITE, /* square not yet visited  */
  GRAY,  /* square partially visited */
  BLACK, /* square fully visited (included all its neighbors) */
} bfscolor;

bool game_is_connected(cgame g) {
  /* In this algorithm, we assume all pieces are well paired (no edge mismatch).
   */

  assert(g);
  uint nb_cols = game_nb_cols(g);
  uint nb_rows = game_nb_rows(g);

  // check precondition, but it should be already checked by the caller!
  if (!game_is_well_paired(g)) return false;

  /* initialize visited array */
  bfscolor visited[nb_rows][nb_cols];
  for (uint i = 0; i < nb_rows; i++)
    for (uint j = 0; j < nb_cols; j++) {
      shape s = game_get_piece_shape(g, i, j);
      visited[i][j] = WHITE;
      if (s == EMPTY) visited[i][j] = BLACK;
    }

  /* lookup for a first square to start BFS */
  bool start_found = false;
  for (uint i = 0; i < nb_rows; i++)
    for (uint j = 0; j < nb_cols; j++) {
      if (visited[i][j] == WHITE && !start_found) {
        visited[i][j] = GRAY;
        start_found = true;
      }
    }

  /* BFS Algorithm */
  bool new_visited = false;
  do {
    new_visited = false;
    for (uint i = 0; i < nb_rows; i++)
      for (uint j = 0; j < nb_cols; j++)
        /* if the square (i,j) has been already visited, then we mark all its
         * connected neighbors as visited...  */
        if (visited[i][j] == GRAY) {
          for (direction d = 0; d < NB_DIRS; d++) {
            if (!game_has_half_edge(g, i, j, d)) continue;
            uint nexti, nextj;
            bool next = game_get_ajacent_square(g, i, j, d, &nexti, &nextj);
            assert(next); /* Always true if the game is well paired! */
            edge_status es = game_check_edge(g, i, j, d);
            assert(es == MATCH); /* Always true... */

            if (visited[nexti][nextj] == WHITE) {
              visited[nexti][nextj] = GRAY;
              new_visited = true;
            }
          }
          /* now, we will not come back to this square... */
          visited[i][j] = BLACK;
        }
  } while (new_visited == true);

  // check all squares have been visited
  for (uint i = 0; i < nb_rows; i++)
    for (uint j = 0; j < nb_cols; j++) {
      if (visited[i][j] != BLACK) {
        return false;
      }
    }

  return true;
}