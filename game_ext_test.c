#include "game_ext.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
#include "queue.h"

bool test_game_new_ext() {
  game g = game_new_ext(5, 6, NULL, NULL, false);
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  if (game_nb_rows(g) != 5 || game_nb_cols(g) != 6 || game_is_wrapping(g)) {
    game_delete(g);
    return false;
  }

  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY ||
          game_get_piece_orientation(g, i, j) != NORTH) {
        fprintf(stderr, "piece shape (%d, %d) is different then NULL\n", i, j);
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);

  uint nb_rows = 3, nb_cols = 3;
  shape shapes[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                    EMPTY, EMPTY, EMPTY, EMPTY};
  direction orientations[] = {NORTH, NORTH, NORTH, NORTH, NORTH,
                              NORTH, NORTH, NORTH, NORTH};
  bool wrapping = true;

  game g1 = game_new_ext(nb_rows, nb_cols, shapes, orientations, wrapping);

  if (g1 == NULL) {
    fprintf(stderr,
            "test_game_new_ext_with_shapes_and_orientations: Failed to create "
            "game.\n");
    return false;
  }

  if (game_nb_rows(g1) != nb_rows || game_nb_cols(g1) != nb_cols ||
      !game_is_wrapping(g1)) {
    fprintf(stderr,
            "test_game_new_ext_with_shapes_and_orientations: Game properties "
            "mismatch.\n");
    game_delete(g1);
    return false;
  }

  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      if (game_get_piece_shape(g1, i, j) != shapes[i * nb_cols + j]) {
        fprintf(stderr,
                "test_game_new_ext_with_shapes_and_orientations: Invalid shape "
                "at (%u, %u).\n",
                i, j);
        game_delete(g1);
        return false;
      }
      if (game_get_piece_orientation(g1, i, j) !=
          orientations[i * nb_cols + j]) {
        fprintf(stderr,
                "test_game_new_ext_with_shapes_and_orientations: Invalid "
                "orientation at (%u, %u).\n",
                i, j);
        game_delete(g1);
        return false;
      }
    }
  }

  game_delete(g1);
  return true;
}

bool test_game_new_empty_ext() {
  uint nb_rows = 5, nb_cols = 6;
  bool wrapping = false;
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);

  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  if (game_nb_rows(g) == nb_rows && game_nb_cols(g) == nb_cols &&
      !game_is_wrapping(g)) {
    for (uint i = 0; i < nb_rows; i++) {
      for (uint j = 0; j < nb_cols; j++) {
        if (game_get_piece_shape(g, i, j) != EMPTY) {
          game_delete(g);
          return false;
        }
        if (game_get_piece_orientation(g, i, j) != NORTH) {
          game_delete(g);
          return false;
        }
      }
    }
    game_delete(g);
    return true;
  }
  game_delete(g);
  return false;
}

bool test_game_nb_rows() {
  uint nb_rows = 5, nb_cols = 7;
  game g = game_new_empty_ext(nb_rows, nb_cols, false);

  if (game_nb_rows(g) == nb_rows) {
    game_delete(g);
    return true;
  }

  game_delete(g);
  return true;
}

bool test_game_nb_cols() {
  uint nb_rows = 5, nb_cols = 7;
  game g = game_new_empty_ext(nb_rows, nb_cols, false);

  if (game_nb_cols(g) == nb_cols) {
    game_delete(g);
    return true;
  }

  game_delete(g);
  return false;
}

bool test_game_is_wrapping() {
  uint nb_rows = 5;
  uint nb_cols = 3;

  game g = game_new_empty_ext(nb_rows, nb_cols, true);
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  if (!game_is_wrapping(g)) {
    fprintf(stderr,
            "Erreur : wrapping activé mais game_is_wrapping retourne false.\n");
    game_delete(g);
    return false;
  }

  game g_no_wrap = game_new_empty_ext(nb_rows, nb_cols, false);
  if (g_no_wrap == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  if (game_is_wrapping(g_no_wrap)) {
    fprintf(
        stderr,
        "Erreur : wrapping désactivé mais game_is_wrapping retourne true.\n");
    game_delete(g_no_wrap);
    game_delete(g);
    return false;
  }

  if (game_is_wrapping(NULL)) {
    fprintf(stderr, "Erreur : pointeur NULL devrait retourner false.\n");
    game_delete(g_no_wrap);
    game_delete(g);
    return false;
  }
  game_delete(g);
  game_delete(g_no_wrap);

  return true;
}

bool test_game_undo() {
  game g = game_default();
  game_play_move(g, 0, 0, 1);

  game_undo(g);
  if (game_get_piece_orientation(g, 0, 0) != WEST ||
      queue_is_empty(g->undo_queue) || !queue_is_empty(g->do_queue)) {
    game_delete(g);
    return false;
  }

  game_play_move(g, 0, 0, 1);
  game_play_move(g, 0, 1, 1);
  game_undo(g);
  game_undo(g);
  if (game_get_piece_orientation(g, 0, 0) != WEST ||
      queue_is_empty(g->undo_queue) || !queue_is_empty(g->do_queue)) {
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}

bool test_game_redo() {
  game g = game_default();
  game_play_move(g, 0, 0, 1);

  game_undo(g);
  game_redo(g);
  if (game_get_piece_orientation(g, 0, 0) == WEST ||
      !queue_is_empty(g->undo_queue) || queue_is_empty(g->do_queue)) {
    game_delete(g);
    return false;
  }

  game_play_move(g, 0, 0, 1);
  game_play_move(g, 0, 1, 1);
  game_undo(g);
  game_undo(g);
  game_redo(g);
  game_redo(g);
  if (game_get_piece_orientation(g, 0, 0) == WEST ||
      !queue_is_empty(g->undo_queue) || queue_is_empty(g->do_queue)) {
    game_delete(g);
    return false;
  }
  game_delete(g);
  return true;
}

bool test_cross_piece() {
  game g = game_new_empty_ext(3, 3, false);
  game_set_piece_shape(g, 1, 1, CROSS);
  if (!game_has_half_edge(g, 1, 1, NORTH)) {
    game_delete(g);
    return false;
  }
  if (!game_has_half_edge(g, 1, 1, EAST)) {
    game_delete(g);
    return false;
  }
  if (!game_has_half_edge(g, 1, 1, SOUTH)) {
    game_delete(g);
    return false;
  }
  if (!game_has_half_edge(g, 1, 1, WEST)) {
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    usage(argc, argv);
  }

  bool etat = true;
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);

  if (strcmp("game_new_ext", argv[1]) == 0) {
    etat = test_game_new_ext();
  } else if (strcmp("game_new_empty_ext", argv[1]) == 0) {
    etat = test_game_new_empty_ext();
  } else if (strcmp("game_nb_rows", argv[1]) == 0) {
    etat = test_game_nb_rows();
  } else if (strcmp("game_nb_cols", argv[1]) == 0) {
    etat = test_game_nb_cols();
  } else if (strcmp("game_is_wrapping", argv[1]) == 0) {
    etat = test_game_is_wrapping();
  } else if (strcmp("game_undo", argv[1]) == 0) {
    etat = test_game_undo();
  } else if (strcmp("game_redo", argv[1]) == 0) {
    etat = test_game_redo();
  } else if (strcmp("cross_piece", argv[1]) == 0) {
    etat = test_cross_piece();
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }

  // print test result
  if (etat) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
