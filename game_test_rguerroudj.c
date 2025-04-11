#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

void test_dummy() { EXIT_SUCCESS; }

bool test_game_new_empty() {
  return false;
  game g = game_new_empty();
  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      if ((game_get_piece_shape(g, i, j) != EMPTY) ||
          (game_get_piece_orientation(g, i, j) != NORTH)) {
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);
  return true;
}
bool test_game_new() {
  game g = game_new(NULL, NULL);

  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY ||
          game_get_piece_orientation(g, i, j) != NORTH) {
        fprintf(stderr, "piece shape (%d, %d) is different then NULL\n", i, j);
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);
  return true;
}

bool test_game_copy() {
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr, "test_game_copy: Failed to create default game.\n");
    return false;
  }

  game g_copy = game_copy(g);
  if (g_copy == NULL) {
    fprintf(stderr, "test_game_copy: Failed to copy game (memory error).\n");
    game_delete(g);
    return false;
  }

  if (!game_equal(g, g_copy, false) || g->width != g_copy->width ||
      g->height != g_copy->height || g->undo_queue != g_copy->undo_queue ||
      g->do_queue != g_copy->do_queue) {
    fprintf(stderr,
            "test_game_copy: Original game and copied game are not equal.\n");
    game_delete(g);
    game_delete(g_copy);
    return false;
  }

  if (g == g_copy) {
    fprintf(stderr,
            "test_game_copy: Original game and copied game share the same "
            "memory.\n");
    game_delete(g);
    game_delete(g_copy);
    return false;
  }

  game_delete(g);
  game_delete(g_copy);

  return true;
}

bool test_game_equal() {
  game g_1 = game_default();
  game_set_piece_shape(g_1, 0, 0, CORNER);
  game_set_piece_orientation(g_1, 0, 0, SOUTH);

  game g_2 = game_default();
  game_set_piece_shape(g_2, 0, 0, ENDPOINT);
  game_set_piece_orientation(g_2, 0, 0, SOUTH);

  if (game_equal(g_1, g_2, true)) {
    fprintf(stderr, "this function return uncorrect output (same shapes)\n");
    game_delete(g_1);
    game_delete(g_2);
    return false;
  }

  game_set_piece_shape(g_1, 0, 0, CORNER);
  game_set_piece_orientation(g_1, 0, 0, SOUTH);

  game_set_piece_shape(g_2, 0, 0, CORNER);
  game_set_piece_orientation(g_2, 0, 0, NORTH);

  if (game_equal(g_1, g_2, false)) {
    fprintf(stderr,
            "this function return uncorrect output (same oriantations)\n");
    game_delete(g_1);
    game_delete(g_2);
    return false;
  }

  for (int i = 0; i < g_1->height; i++) {
    for (int j = 0; j < g_1->height; j++) {
      game_set_piece_shape(g_1, i, j, TEE);
      game_set_piece_shape(g_2, i, j, TEE);
      game_set_piece_orientation(g_1, i, j, WEST);
      game_set_piece_orientation(g_2, i, j, WEST);
    }
  }

  if (!game_equal(g_1, g_2, true)) {
    fprintf(stderr,
            "this function return uncorrect output (different shapes)\n");
    game_delete(g_1);
    game_delete(g_2);
    return false;
  } else if (!game_equal(g_1, g_2, false)) {
    fprintf(stderr,
            "this function return uncorrect output (different orientations)\n");
    game_delete(g_1);
    game_delete(g_2);
    return false;
  }

  game_delete(g_1);
  game_delete(g_2);
  return true;
}

bool test_game_delete() {
  game g = game_default();
  game_delete(g);
  g = NULL;
  if (g != NULL) {
    fprintf(stderr, "the game has not been deleted correctly\n");
    return false;
  }
  return true;
}

bool test_game_set_piece_shape() {
  game g = game_default();
  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      game_set_piece_shape(g, i, j, CORNER);
    }
  }

  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      if (game_get_piece_shape(g, i, j) != CORNER) {
        fprintf(stderr, "uncorrect output\n");
        game_delete(g);
        return false;
      }
    }
  }

  game_delete(g);
  return true;
}

bool test_game_set_piece_orientaion() {
  game g = game_default();
  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      game_set_piece_orientation(g, i, j, EAST);
    }
  }

  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      if (game_get_piece_orientation(g, i, j) != EAST) {
        fprintf(stderr, "uncorrect output\n");
        game_delete(g);
        return false;
      }
    }
  }

  game_delete(g);
  return true;
}

bool usage(int argc, char const *argv[]) {
  fprintf(stderr, "Usage %s <testname>\n", argv[0]);
  exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]) {
  if (argc == 1) {
    usage(argc, argv);
  }

  bool ok = false;

  if (strcmp("dummy", argv[1]) == 0) {
    test_dummy();
  } else if (strcmp("game_new_empty", argv[1]) == 0) {
    ok = test_game_new_empty();
  } else if (strcmp("game_new", argv[1]) == 0) {
    ok = test_game_new();
  } else if (strcmp("game_copy", argv[1]) == 0) {
    ok = test_game_copy();
  } else if (strcmp("game_equal", argv[1]) == 0) {
    ok = test_game_equal();
  } else if (strcmp("game_delete", argv[1]) == 0) {
    ok = test_game_delete();
  } else if (strcmp("game_set_piece_shape", argv[1]) == 0) {
    ok = test_game_set_piece_shape();
  } else if (strcmp("game_set_piece_orientation", argv[1]) == 0) {
    ok = test_game_set_piece_orientaion();
  } else {
    fprintf(stderr, "Test \"%s\" not found\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (ok) {
    printf("Test \"%s\" finished : SUCCES\n", argv[0]);
    EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished : FAILURE\n", argv[0]);
    EXIT_FAILURE;
  }
}
