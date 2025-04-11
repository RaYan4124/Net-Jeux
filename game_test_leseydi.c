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

// ############### TEST GAME PRINT ###############
bool test_game_print() {
  game g = game_default();
  game_print(g);
  game_delete(g);
  return true;
}

// ############### TEST GAME WON ###############
bool test_game_won() {
  uint nb_rows = 5;
  uint nb_cols = 5;
  bool wrapping = true;
  game g = game_new_ext(nb_rows, nb_cols, NULL, NULL, wrapping);
  if (g == NULL) {
    fprintf(stderr, "Erreur: Impossible de créer le jeu solution\n");
    return false;
  }

  bool test = game_won(g);
  game_delete(g);

  if (!test) {
    fprintf(stderr, "Erreur: game_won a échoué pour une solution gagnante\n");
    return false;
  }

  game g2 = game_default();
  if (g2 == NULL) {
    fprintf(stderr, "Erreur: Impossible de créer le jeu par défaut\n");
    return false;
  }

  bool solution = !game_won(g2);
  game_delete(g2);

  if (!solution) {
    fprintf(stderr,
            "Erreur: game_won a retourné true pour un jeu non gagnant\n");
    return false;
  }

  return true;
}

bool test_game_get_piece_shape() {
  game g = game_default();

  game_set_piece_shape(g, 0, 0, CORNER);
  if (game_get_piece_shape(g, 0, 0) != CORNER) {
    game_delete(g);
    return false;
  }

  game_set_piece_shape(g, 0, 0, SEGMENT);
  if (game_get_piece_shape(g, 0, 0) != SEGMENT) {
    game_delete(g);
    return false;
  }

  game_set_piece_shape(g, 0, 0, TEE);
  if (game_get_piece_shape(g, 0, 0) != TEE) {
    game_delete(g);
    return false;
  }

  game_set_piece_shape(g, 0, 0, ENDPOINT);
  if (game_get_piece_shape(g, 0, 0) != ENDPOINT) {
    game_delete(g);
    return false;
  }

  game_set_piece_shape(g, 0, 0, CROSS);
  if (game_get_piece_shape(g, 0, 0) != CROSS) {
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}

bool test_game_shuffle_orientation() {
  game g = game_default();
  game g2 = game_copy(g);

  game_shuffle_orientation(g);
  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      if (g->cases[i * g->width + j].orientation < 0 ||
          g->cases[i * g->width + j].orientation >= 4) {
        printf("Invalid orientation at (%d, %d)\n", i, j);
        game_delete(g);
        game_delete(g2);
        return false;
      }
      if (game_get_piece_shape(g, i, j) != game_get_piece_shape(g2, i, j)) {
        game_delete(g);
        game_delete(g2);
        return false;
      }
    }
  }

  game_delete(g);
  game_delete(g2);
  return true;
}

bool test_game_default_solution() {
  game g = game_default_solution();

  if (game_won(g) != true) {
    game_delete(g);
    return false;
  }
  game_delete(g);
  return true;
}

bool test_game_is_well_paired() {
  // Test 1: Jeu par défaut
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr,
            "test_game_is_well_paired: Failed to create default game.\n");
    return false;
  }

  // Test 2: Ajout d'une pièce mal appariée
  game_set_piece_shape(g, 0, 0, CORNER);
  game_set_piece_orientation(g, 0, 0, NORTH);
  game_set_piece_shape(g, 0, 0,
                       SEGMENT);  // Définir une autre forme incompatible
  game_set_piece_orientation(g, 0, 0,
                             NORTH);  // Orientation incorrecte pour une paire
  if (game_is_well_paired(g) != false) {
    fprintf(stderr,
            "test_game_is_well_paired: Test 2 failed (should not be well "
            "paired).\n");
    game_delete(g);
    return false;
  }
  game_delete(g);
  // Test 3: Solution correcte
  g = game_default_solution();  // Charger une solution correcte par défaut
  if (g == NULL) {
    fprintf(
        stderr,
        "test_game_is_well_paired: Failed to create default solution game.\n");
    return false;
  }
  if (!game_is_well_paired(g)) {
    fprintf(
        stderr,
        "test_game_is_well_paired: Test 3 failed (should be well paired).\n");
    game_delete(g);
    return false;
  }

  // Test 4: Cas avec wrapping
  g->isWrapping = true;  // Activer le wrapping

  game_set_piece_shape(g, 0, game_nb_cols(g) - 1, SEGMENT);
  game_set_piece_orientation(g, 0, game_nb_cols(g) - 1,
                             EAST);  // Connecter les pièces avec wrapping

  // Vérifier si le jeu est bien apparié après modification avec wrapping
  if (!game_is_well_paired(g)) {
    fprintf(stderr,
            "test_game_is_well_paired: Test 4 failed (should be well paired "
            "with wrapping).\n");
    game_delete(g);
    return false;
  }

  // Nettoyage
  game_delete(g);
  return true;
}

bool test_game_has_half_edge() {
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr,
            "test_game_has_half_edge: Failed to create default game.\n");
    return false;
  }

  // Test avec une pièce de type CORNER
  game_set_piece_shape(g, 0, 0, CORNER);
  game_set_piece_orientation(g, 0, 0, NORTH);

  // Vérification des arêtes de la pièce CORNER
  if ((game_has_half_edge(g, 0, 0, NORTH) != true) ||
      (game_has_half_edge(g, 0, 0, EAST) != true) ||
      (game_has_half_edge(g, 0, 0, WEST) != false) ||
      (game_has_half_edge(g, 0, 0, SOUTH) != false)) {
    game_delete(g);
    return false;
  }

  // Vérification des arêtes de la pièce CROSS
  game_set_piece_shape(g, 0, 0, CROSS);
  game_set_piece_orientation(g, 0, 0, EAST);

  if ((game_has_half_edge(g, 0, 0, SOUTH) != true) ||
      (game_has_half_edge(g, 0, 0, EAST) != true) ||
      (game_has_half_edge(g, 0, 0, NORTH) != true) ||
      (game_has_half_edge(g, 0, 0, WEST) != true)) {
    game_delete(g);
    return false;
  }

  // Test avec un segment de type SEGMENT
  game_set_piece_shape(g, 0, 0, SEGMENT);
  game_set_piece_orientation(g, 0, 0, EAST);

  if ((game_has_half_edge(g, 0, 0, EAST) != true) ||
      (game_has_half_edge(g, 0, 0, WEST) != true) ||
      (game_has_half_edge(g, 0, 0, NORTH) != false) ||
      (game_has_half_edge(g, 0, 0, SOUTH) != false)) {
    game_delete(g);
    return false;
  }

  // Test avec un endpoint (aucune arête)
  game_set_piece_shape(g, 0, 0, ENDPOINT);
  game_set_piece_orientation(g, 0, 0, SOUTH);

  if ((game_has_half_edge(g, 0, 0, NORTH) != false) ||
      (game_has_half_edge(g, 0, 0, EAST) != false) ||
      (game_has_half_edge(g, 0, 0, WEST) != false) ||
      (game_has_half_edge(g, 0, 0, SOUTH) != true)) {
    game_delete(g);
    return false;
  }

  // Test de la pièce de type TEE avec CROSS (multiple directions)
  game_set_piece_shape(g, 0, 0, TEE);
  game_set_piece_orientation(g, 0, 0, WEST);

  if ((game_has_half_edge(g, 0, 0, NORTH) != true) ||
      (game_has_half_edge(g, 0, 0, SOUTH) != true) ||
      (game_has_half_edge(g, 0, 0, WEST) != true) ||
      (game_has_half_edge(g, 0, 0, EAST) != false)) {
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
  bool etat = true;
  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);

  if (strcmp("dummy", argv[1]) == 0) {
    test_dummy();
  } else if (strcmp("game_print", argv[1]) == 0) {
    etat = test_game_print();
  } else if (strcmp("game_won", argv[1]) == 0) {
    etat = test_game_won();
  } else if (strcmp("game_get_piece_shape", argv[1]) == 0) {
    etat = test_game_get_piece_shape();
  } else if (strcmp("game_shuffle_orientation", argv[1]) == 0) {
    etat = test_game_shuffle_orientation();
  } else if (strcmp("game_default_solution", argv[1]) == 0) {
    etat = test_game_default_solution();
  } else if (strcmp("game_is_well_paired", argv[1]) == 0) {
    etat = test_game_is_well_paired();
  } else if (strcmp("game_has_half_edge", argv[1]) == 0) {
    etat = test_game_has_half_edge();
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
}
