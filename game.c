#include "game.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game_aux.h"
#include "game_struct.h"
#include "queue.h"

/**
 * Fonction : game_new_empty

 * Crée un nouveau jeu vide avec les paramètres par défaut (taille, orientation,
 etc.).

 * Retour : Un nouveau jeu vide.
 */

game game_new_empty(void) {
  game new_game_empty = malloc(sizeof(struct game_s));
  if (new_game_empty == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  new_game_empty->do_queue = queue_new();
  if (new_game_empty->do_queue == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  new_game_empty->undo_queue = queue_new();
  if (new_game_empty->undo_queue == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  new_game_empty->height = DEFAULT_SIZE;
  new_game_empty->width = DEFAULT_SIZE;
  new_game_empty->isWrapping = true;

  new_game_empty->cases = malloc((DEFAULT_SIZE * DEFAULT_SIZE) * sizeof(Acase));
  if (new_game_empty->cases == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      new_game_empty->cases[i * new_game_empty->width + j].shape = EMPTY;
      new_game_empty->cases[i * new_game_empty->width + j].orientation = NORTH;
    }
  }

  return new_game_empty;
}

/**
 * Fonction : game_new

 * Crée un nouveau jeu avec les formes et orientations spécifiées, ou vide si
 NULL.

 * Paramètres :
 *  -shapes : Un tableau de formes.
 *  -orientations : Un tableau d'orientations.
 *
 * Retour : Un nouveau jeu initialisé avec les valeurs spécifiées.
 */

game game_new(shape *shapes, direction *orientations) {
  if (shapes != NULL && orientations != NULL) {
    if (shapes[DEFAULT_SIZE * DEFAULT_SIZE - 1] > NB_SHAPES) {
      fprintf(stderr, "Error: Invalid shape value.\n");
      exit(EXIT_FAILURE);
    }
    if (orientations[DEFAULT_SIZE * DEFAULT_SIZE - 1] > NB_DIRS) {
      fprintf(stderr, "Error: Invalid orientation value.\n");
      exit(EXIT_FAILURE);
    }
  }

  game new_game = game_new_empty();

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      if (shapes == NULL) {
        new_game->cases[i * new_game->width + j].shape = EMPTY;
      } else {
        new_game->cases[i * new_game->width + j].shape =
            shapes[i * new_game->width + j];
      }

      if (orientations == NULL) {
        new_game->cases[i * new_game->width + j].orientation = NORTH;
      } else {
        new_game->cases[i * new_game->width + j].orientation =
            orientations[i * new_game->width + j];
      }
    }
  }

  return new_game;
}

/**
 * Fonction : game_copy

 * Crée une copie d'un jeu existant.

 * Paramètres : g : Le jeu à copier.

 * Retour : Une copie du jeu passé en paramètre.
 */

game game_copy(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  game copy = malloc(sizeof(struct game_s));
  if (copy == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }
  copy->height = g->height;
  copy->width = g->width;

  copy->do_queue = queue_new();
  if (copy->do_queue == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  copy->undo_queue = queue_new();
  if (copy->undo_queue == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  copy->cases = malloc(sizeof(Acase) * (copy->height * copy->width));
  if (copy->cases == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      copy->cases[i * copy->width + j].shape = g->cases[i * g->width + j].shape;
      copy->cases[i * copy->width + j].orientation =
          g->cases[i * g->width + j].orientation;
    }
  }

  return copy;
}

/**
 * Fonction : game_equal

 * Compare deux jeux pour vérifier s'ils sont égaux.
 * La comparaison peut ignorer les orientations des pièces.

 * Paramètres :
 *  g1 : Le premier jeu.
 *  g2 : Le deuxième jeu.
 *  ignore_orientation : Booléen pour savoir si les orientations doivent être
 ignorées.

 * Retour : true si les jeux sont égaux, sinon false.
 */

bool game_equal(cgame g1, cgame g2, bool ignore_orientation) {
  if (g1 == NULL || g2 == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  if (g1->height != g2->height || g1->width != g2->width) {
    fprintf(stderr, "dimensions error");
    return false;
  }

  if (ignore_orientation) {
    for (int i = 0; i < g1->height; i++) {
      for (int j = 0; j < g1->width; j++) {
        if (g1->cases[i * g1->width + j].shape !=
            g2->cases[i * g2->width + j].shape) {
          fprintf(stderr, "there is a difference in i:%d, j;%d", i, j);
          return false;
        }
      }
    }
  } else {
    for (int i = 0; i < g1->height; i++) {
      for (int j = 0; j < g1->width; j++) {
        if (g1->cases[i * g1->width + j].shape !=
                g2->cases[i * g2->width + j].shape ||
            g1->cases[i * g1->width + j].orientation !=
                g2->cases[i * g2->width + j].orientation) {
          fprintf(stderr, "there is a difference in i:%d, j;%d", i, j);
          return false;
        }
      }
    }
  }
  return true;
}

/**
 * Fonction : game_delete

 * Libère la mémoire allouée pour un jeu et ses composants.

 * Paramètres : g : Le jeu à supprimer.
 */

void game_delete(game g) {
  if (g != NULL) {
    if (g->cases != NULL) {
      free(g->cases);
      g->cases = NULL;
    }
    if (g->do_queue != NULL) {
      queue_free(g->do_queue);
      g->do_queue = NULL;
    }
    if (g->undo_queue != NULL) {
      queue_free(g->undo_queue);
      g->undo_queue = NULL;
    }
    free(g);
    g = NULL;
  }
}

/**
 * Fonction : game_set_piece_shape
 *
 * Modifie la forme d'une pièce à une position donnée.
 *
 * Paramètres :
 *  g : Le jeu.
 *  i, j : Les indices de la case.
 *  s : La nouvelle forme.
 */

void game_set_piece_shape(game g, uint i, uint j, shape s) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  if (i > g->height || j > g->width) {
    fprintf(stderr, "position error");
    exit(EXIT_FAILURE);
  }

  if (s >= EMPTY && s < NB_SHAPES) {
    g->cases[i * g->width + j].shape = s;
  } else {
    fprintf(stderr, "s is not a shape");
    exit(EXIT_FAILURE);
  }
}

/**
 * Fonction : game_set_piece_orientation

 * Modifie l'orientation d'une pièce à une position donnée.

 * Paramètres :
 *  g : Le jeu.
 *  i, j : Les indices de la case.
 *  o : La nouvelle orientation.
 */

void game_set_piece_orientation(game g, uint i, uint j, direction o) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  if (i > g->height || j > g->width) {
    fprintf(stderr, "position error");
    exit(EXIT_FAILURE);
  }

  if (o == NORTH || o == EAST || o == WEST || o == SOUTH) {
    g->cases[i * g->width + j].orientation = o;
  } else {
    fprintf(stderr, "s is not an orientation");
    exit(EXIT_FAILURE);
  }
}

/**
 * Fonction : game_get_piece_shape

 * Récupère la forme d'une pièce à une position donnée.

 * Paramètres :
 *  g : Le jeu.
 *  i, j : Les indices de la case.

 * Retour : La forme de la pièce à la position donnée.
 */

shape game_get_piece_shape(cgame g, uint i, uint j) {
  if (g == NULL || i > g->height - 1 || j > g->width - 1) {
    fprintf(stderr, "Error\n");
    exit(EXIT_FAILURE);
  }
  return g->cases[i * g->width + j].shape;
}

/**
 * Fonction : game_get_piece_orientation

 * Récupère l'orientation d'une pièce à une position donnée.

 * Paramètres :
 *  g : Le jeu.
 *  i, j : Les indices de la case.

 * Retour : L'orientation de la pièce à la position donnée.
 */

direction game_get_piece_orientation(cgame g, uint i, uint j) {
  if (g == NULL || i > g->height - 1 || j > g->width - 1) {
    fprintf(stderr, "Error\n");
    exit(EXIT_FAILURE);
  }
  return g->cases[i * g->width + j].orientation;
}

/**
 * Fonction : game_play_move

 * Effectue un mouvement en tournant la pièce d'un nombre spécifié de quarts de
 tour.

 * Paramètres :
 *  g : Le jeu.
 *  i, j : Les indices de la case.
 */

void game_play_move(game g, uint i, uint j, int nb_quarter_turns) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  queue_push_head(g->do_queue, (void *)(intptr_t)i);
  queue_push_head(g->do_queue, (void *)(intptr_t)j);
  queue_push_head(g->do_queue,
                  (void *)(intptr_t)game_get_piece_orientation(g, i, j));

  int height = g->height;
  int width = g->width;

  if (i >= height || j >= width) {
    fprintf(stderr, "Error\n");
    exit(EXIT_FAILURE);
  }
  direction current_orientation = game_get_piece_orientation(g, i, j);

  int new_orientation = (current_orientation + nb_quarter_turns) % 4;
  if (new_orientation < 0) {
    new_orientation += 4;
  }

  if (new_orientation == 0) {
    game_set_piece_orientation(g, i, j, NORTH);
  } else if (new_orientation == 1) {
    game_set_piece_orientation(g, i, j, EAST);
  } else if (new_orientation == 2) {
    game_set_piece_orientation(g, i, j, SOUTH);
  } else if (new_orientation == 3) {
    game_set_piece_orientation(g, i, j, WEST);
  }

  if (!queue_is_empty(g->undo_queue)) {
    queue_clear(g->undo_queue);
  }
}

/**
 * Fonction : game_won

 * Vérifie si le jeu est gagné, c'est-à-dire si toutes les conditions de
 victoire sont remplies.

 * Paramètres : g : Le jeu.
 *
 * Retour : true si le jeu est gagné, sinon false.
 */

bool game_won(cgame g) {
  if (g == NULL) {
    return false;
  }
  if (game_is_well_paired(g) && game_is_connected(g)) {
    return true;
  }
  return false;
}

/**
 * Fonction : game_reset_orientation

 * Réinitialise toutes les orientations des pièces à l'orientation par défaut
 (NORTH).

 * Paramètres : g : Le jeu.
 */

void game_reset_orientation(game g) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      g->cases[i * g->width + j].orientation = 0;
    }
  }
  if (!queue_is_empty(g->undo_queue)) {
    queue_clear(g->undo_queue);
  }
  if (!queue_is_empty(g->do_queue)) {
    queue_clear(g->do_queue);
  }
}

/**
 * Fonction : game_shuffle_orientation

 * Mélange aléatoirement l'orientation de toutes les pièces du jeu.

 * Paramètres : g : Le jeu.
 */

void game_shuffle_orientation(game g) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      g->cases[i * g->width + j].orientation = rand() % 4;
    }
  }
  if (!queue_is_empty(g->undo_queue)) {
    queue_clear(g->undo_queue);
  }
  if (!queue_is_empty(g->do_queue)) {
    queue_clear(g->do_queue);
  }
}
