#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

void test_dummy() { EXIT_SUCCESS; }

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

bool test_game_is_connected() {
  // Création d'un jeu vide
  game g = game_new_empty();
  if (g == NULL) {
    fprintf(stderr,
            "test_game_is_connected: Failed to create a new empty game.\n");
    return false;
  }

  bool result = true;

  // Test 1 : Jeu vide (doit être considéré comme connecté)
  if (!game_is_connected(g)) {
    fprintf(stderr,
            "test_game_is_connected: Test 1 failed (empty game should be "
            "connected).\n");
    result = false;
  }

  // Test 2 : Ajouter une pièce non connectée
  game_set_piece_shape(g, 0, 0, TEE);
  game_set_piece_orientation(g, 0, 0, EAST);
  if (!game_is_connected(g)) {
    fprintf(stderr,
            "test_game_is_connected: Test 2 failed (single piece should be "
            "connected).\n");
    result = false;
  }

  // Test 3 : Ajouter une pièce non connectée ailleurs
  game_set_piece_shape(g, 1, 1, SEGMENT);
  game_set_piece_orientation(g, 1, 1, NORTH);
  if (game_is_connected(g)) {
    fprintf(stderr,
            "test_game_is_connected: Test 3 failed (disconnected pieces should "
            "not be connected).\n");
    result = false;
  }

  // Test 4 : Connecter les pièces
  game_set_piece_shape(g, 1, 0, SEGMENT);
  game_set_piece_orientation(g, 1, 0, SOUTH);
  if (!game_is_connected(g)) {
    fprintf(stderr,
            "test_game_is_connected: Test 4 failed (pieces should now be "
            "connected).\n");
    result = false;
  }

  // Test 5 : Ajouter une pièce qui casse la connexion
  game_set_piece_shape(g, 0, 1, CORNER);
  game_set_piece_orientation(g, 0, 1, WEST);
  if (game_is_connected(g)) {
    fprintf(stderr,
            "test_game_is_connected: Test 5 failed (piece should break the "
            "connection).\n");
    result = false;
  }

  // Test 6 : Option wrapping
  g->isWrapping = true;  // Activer le wrapping
  game_set_piece_shape(g, 0, game_nb_cols(g) - 1, SEGMENT);
  game_set_piece_orientation(g, 0, game_nb_cols(g) - 1, EAST);
  if (!game_is_connected(g)) {
    fprintf(stderr,
            "test_game_is_connected: Test 6 failed (wrapping should make the "
            "game connected).\n");
    result = false;
  }

  // Nettoyage
  game_delete(g);

  return result;
}

bool test_game_check_edge() {
  game g = game_default();
  edge_status edgeStatus = game_check_edge(g, 0, 0, EAST);
  game_delete(g);
  return edgeStatus == MATCH;
}

bool test_game_get_ajacent_square() {
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr,
            "test_game_get_adjacent_square: Failed to create default game.\n");
    return false;
  }

  uint i, j;

  // Test 1 : Adjacent à (0,0) vers l'est
  bool adjacent_east = game_get_ajacent_square(g, 0, 0, EAST, &i, &j);
  if (!adjacent_east || i != 0 || j != 1) {
    fprintf(stderr, "test_game_get_adjacent_square: EAST direction failed.\n");
    game_delete(g);
    return false;
  }

  // Test 2 : Adjacent à (0,game_nb_cols(g)-1) vers l'est (bord droit)
  uint last_col = game_nb_cols(g) - 1;
  bool adjacent_wrap_east =
      game_get_ajacent_square(g, 0, last_col, EAST, &i, &j);
  if (game_is_wrapping(g)) {
    if (!adjacent_wrap_east || i != 0 || j != 0) {
      fprintf(stderr, "test_game_get_adjacent_square: Wrapping EAST failed.\n");
      game_delete(g);
      return false;
    }
  } else {
    if (adjacent_wrap_east) {
      fprintf(
          stderr,
          "test_game_get_adjacent_square: Non-wrapping EAST should fail.\n");
      game_delete(g);
      return false;
    }
  }

  // Test 3 : Adjacent à (0,0) vers le nord (bord supérieur)
  bool adjacent_north = game_get_ajacent_square(g, 0, 0, NORTH, &i, &j);
  if (game_is_wrapping(g)) {
    if (!adjacent_north || i != game_nb_rows(g) - 1 || j != 0) {
      fprintf(stderr,
              "test_game_get_adjacent_square: Wrapping NORTH failed.\n");
      game_delete(g);
      return false;
    }
  } else {
    if (adjacent_north) {
      fprintf(
          stderr,
          "test_game_get_adjacent_square: Non-wrapping NORTH should fail.\n");
      game_delete(g);
      return false;
    }
  }

  game_delete(g);
  return true;
}

bool test_game_default() {
  game g = game_default();
  bool res = (g != NULL);
  game_delete(g);
  return res;
}

bool test_game_reset_orientation() {
  game g = game_default();
  game_reset_orientation(g);
  direction d = 1;
  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      d = game_get_piece_orientation(g, i, j);
      if (d != 0) {
        return false;
      }
    }
  }
  game_delete(g);
  return true;
}

bool test_game_play_move() {
  game g = game_default();
  direction or = game_get_piece_orientation(g, 0, 0);
  bool test = true;

  game_play_move(g, 0, 0, 1);
  if (game_get_piece_orientation(g, 0, 0) != (or +1) % NB_DIRS) {
    printf("Test 1 échoué.\n");
    test = false;
  }

  game_play_move(g, 0, 0, 2);
  if (game_get_piece_orientation(g, 0, 0) != (or +3) % NB_DIRS) {
    printf("Test 2 échoué.\n");
    test = false;
  }

  game_play_move(g, 0, 0, -1);
  if (game_get_piece_orientation(g, 0, 0) != (or +2) % NB_DIRS) {
    printf("Test 3 échoué.\n");
    test = false;
  }

  game_delete(g);
  return test;
}

bool test_game_get_piece_orientation() {
  game g = game_default();

  game_set_piece_orientation(g, 0, 0, NORTH);
  if (game_get_piece_orientation(g, 0, 0) != NORTH) {
    game_delete(g);
    return false;
  }

  game_set_piece_orientation(g, 0, 0, EAST);
  if (game_get_piece_orientation(g, 0, 0) != EAST) {
    game_delete(g);
    return false;
  }

  game_set_piece_orientation(g, 0, 0, WEST);
  if (game_get_piece_orientation(g, 0, 0) != WEST) {
    game_delete(g);
    return false;
  }

  game_set_piece_orientation(g, 0, 0, SOUTH);
  if (game_get_piece_orientation(g, 0, 0) != SOUTH) {
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    usage(argc, argv);
  }
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);

  if (strcmp("dummy", argv[1]) == 0) {
    test_dummy();
  } else if (strcmp("game_is_connected", argv[1]) == 0) {
    test_game_is_connected();
  } else if (strcmp("game_check_edge", argv[1]) == 0) {
    test_game_check_edge();
  } else if (strcmp("game_get_ajacent_square", argv[1]) == 0) {
    test_game_get_ajacent_square();
  } else if (strcmp("game_default", argv[1]) == 0) {
    test_game_default();
  } else if (strcmp("game_reset_orientation", argv[1]) == 0) {
    test_game_reset_orientation();
  } else if (strcmp("game_play_move", argv[1]) == 0) {
    test_game_play_move();
  } else if (strcmp("game_get_piece_orientation", argv[1]) == 0) {
    test_game_get_piece_orientation();
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
