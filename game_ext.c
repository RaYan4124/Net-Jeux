#include "game_ext.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
#include "queue.h"

/**
 * Fonction : game_new_empty_ext

 * Crée un nouveau jeu vide avec un nombre spécifié de lignes et de colonnes,
 * et un paramètre définissant si le jeu est en mode wrapping (enroulement).
 * Le jeu est initialisé avec des pièces vides et orientées vers le Nord.

 * Paramètres :
 *  nb_rows : Le nombre de lignes du jeu.
 *  nb_cols : Le nombre de colonnes du jeu.
 *  wrapping : Indique si le jeu utilise l'enroulement (wrapping).

 * Retour : Un nouveau jeu vide, ou NULL en cas d'erreur.
 */

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping) {
  if (nb_cols < 1 || nb_cols > 10 || nb_rows < 1 || nb_rows > 10) {
    fprintf(stderr, "Error sur les paramètres\n");
    return NULL;
  }

  game g = malloc(sizeof(struct game_s));
  if (g == NULL) {
    fprintf(stderr, "Error d'allocation\n");
    return NULL;
  }

  g->width = nb_cols;
  g->height = nb_rows;
  g->cases = malloc(nb_rows * nb_cols * sizeof(Acase));
  if (g->cases == NULL) {
    return NULL;
  }
  g->isWrapping = wrapping;

  g->do_queue = queue_new();
  g->undo_queue = queue_new();
  if (g->do_queue == NULL || g->undo_queue == NULL) {
    return NULL;
  }
  // assigner les shapes et les orientations
  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      g->cases[i * g->width + j].shape = EMPTY;
      g->cases[i * g->width + j].orientation = NORTH;
    }
  }
  return g;
}

/**
 * Fonction : game_new_ext

 * Crée un nouveau jeu avec un nombre spécifié de lignes et de colonnes,
 * en initialisant les cases avec des formes et orientations données.
 * Si les tableaux de formes et orientations sont NULL, ils seront initialisés
 par défaut.

 * Paramètres :
 *  nb_rows : Le nombre de lignes du jeu.
 *  nb_cols : Le nombre de colonnes du jeu.
 *  shapes : Un tableau de formes pour chaque case du jeu.
 *  orientations : Un tableau d'orientations pour chaque case du jeu.
 *  wrapping : Indique si le jeu utilise l'enroulement (wrapping).

 * Retour : Un nouveau jeu avec les paramètres spécifiés, ou NULL en cas
 d'erreur.
 */

game game_new_ext(uint nb_rows, uint nb_cols, shape *shapes,
                  direction *orientations, bool wrapping) {
  if (nb_cols < 1 || nb_cols > 10 || nb_rows < 1 || nb_rows > 10) {
    return NULL;
  }

  if (shapes != NULL) {
    for (int i = 0; i < nb_rows * nb_cols; i++) {
      if (shapes[i] < 0 || shapes[i] >= NB_SHAPES) {
        return NULL;
      }
    }
  }

  if (orientations != NULL) {
    for (int i = 0; i < nb_rows * nb_cols; i++) {
      if (orientations[i] < 0 || orientations[i] >= NB_DIRS) {
        return NULL;
      }
    }
  }

  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (g == NULL) {
    return NULL;
  }

  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      if (shapes == NULL) {
        g->cases[i * g->width + j].shape = EMPTY;
      } else {
        g->cases[i * g->width + j].shape = shapes[i * g->width + j];
      }

      if (orientations == NULL) {
        g->cases[i * g->width + j].orientation = NORTH;
      } else {
        g->cases[i * g->width + j].orientation = orientations[i * g->width + j];
      }
    }
  }

  return g;
}

uint game_nb_rows(cgame g) {
  if (g == NULL) {
    return 0;
  }
  return g->height;
}

uint game_nb_cols(cgame g) {
  if (g == NULL) {
    return 0;
  }
  return g->width;
}

/**
 * Fonction : game_is_wrapping

 * Vérifie si le jeu utilise le mode wrapping (enroulement).

 * Paramètres : g : Le jeu à vérifier.

 * Retour : true si le jeu est en mode wrapping, sinon false.
 */

bool game_is_wrapping(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "game_is_wrapping: g is not a valid pointer\n");
    return false;
  }
  return g->isWrapping;
}

/**
 * Fonction : game_undo

 * Annule la dernière action réalisée dans le jeu.
 * La direction de la dernière action est restaurée.

 * Paramètres : g : Le jeu sur lequel effectuer l'annulation.
 */

void game_undo(game g) {
  if (g == NULL) {
    return;
  }

  if (queue_is_empty(g->do_queue)) return;

  direction dir = (direction)(intptr_t)queue_pop_head(
      g->do_queue);  // Extraire la direction
  uint j = (uint)(intptr_t)queue_pop_head(g->do_queue);
  uint i = (uint)(intptr_t)queue_pop_head(g->do_queue);

  queue_push_head(g->undo_queue, (void *)(intptr_t)i);
  queue_push_head(g->undo_queue, (void *)(intptr_t)j);
  queue_push_head(g->undo_queue,
                  (void *)(intptr_t)game_get_piece_orientation(g, i, j));

  game_set_piece_orientation(g, i, j, dir);
}

/**
 * Fonction : game_redo

 * Restaure la dernière action annulée dans le jeu.
 * La direction de la dernière action annulée est réappliquée.

 * Paramètres : g : Le jeu sur lequel effectuer la restauration.
 */

void game_redo(game g) {
  if (g == NULL) {
    return;
  }

  if (queue_is_empty(g->undo_queue)) return;

  direction dir = (direction)(intptr_t)queue_pop_head(g->undo_queue);
  uint j = (uint)(intptr_t)queue_pop_head(g->undo_queue);
  uint i = (uint)(intptr_t)queue_pop_head(g->undo_queue);

  queue_push_head(g->do_queue, (void *)(intptr_t)i);
  queue_push_head(g->do_queue, (void *)(intptr_t)j);
  queue_push_head(
      g->do_queue,
      (void *)(void *)(intptr_t)game_get_piece_orientation(g, i, j));

  game_set_piece_orientation(g, i, j, dir);
}
